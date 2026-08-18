# Detector Translation

## Purpose

Detector translation maps a DAQ address `(rocid, slot, channel)` to a detector
name and detector-specific integer channel fields. It does not decode hardware,
calibrate measurements, or provide detector geometry.

## Main Flow

1. `JEventService_TranslationTable` loads the root detector catalog, each
   detector's run-range manifest, and the referenced mapping files.
2. During initialization, the service builds immutable combined tables for
   every resulting run interval and publishes the table selected for each run.
3. A module-parser hit type opts into translation by providing
   `DAQAddress getDAQAddress(const HitType&)`, which normalizes its hardware
   address field names.
4. One central event processor acquires the table for each event, scans each
   addressable raw-hit collection once, and performs one immutable lookup per
   hit using only `getDAQAddress()`.
5. `JEventService_DetectorTranslatorsMap` selects a translator by raw-hit C++
   type and detector name.
6. The translator inserts its concrete typed DigiHit into the current event.

## Expected Behavior

- Detector-specific fields remain generic name/value pairs.
- Unmapped and deliberately excluded reference channels return `nullptr`.
- A mapping file applies one detector name to every channel it contains.
- Detector mapping files are installed under
  `config/<namespace>/evio_parser/detector_mappings/`.
- `TRANSLATION:DIRECTORY` overrides the mapping directory.
- The root `manifest.map` lists authoritative detector names and
  detector-manifest paths. Every mapping file selected through a detector's
  manifest must declare that same detector name.
- Detector-manifest paths are relative to the root mapping directory, and
  mapping-file paths are relative to their detector-manifest directory.
  Absolute paths, `..` traversal, and resolved paths outside those owning
  directories are rejected.
- Each detector manifest contains inclusive `run_min run_max mapping_file`
  rows; `max` is accepted as an open-ended upper bound.
- Detector ranges may have gaps. A detector without a range for a run is absent
  from that combined table; a run with no applicable detector mapping fails.
- All referenced mapping files and combined tables are loaded during service
  initialization. Event processing performs no configuration file I/O or table
  construction.
- Combined tables with the same selected mapping-file set share one immutable
  table instance.
- Mapping changes are expected only between runs.
- Translation runs in the processor's parallel event callback.
- The processor is registered by `evio_parser` and inserts DigiHits before
  downstream processors consume the physics event.
- Unmapped channels and channels belonging to other detectors are skipped.
- Routes are keyed by raw-hit C++ type and detector name.
- Each detector directory owns its route registrations. The central
  initializer composes detector initializers and freezes the completed registry.
- Addressable raw-hit types satisfy the `DAQAddressable` C++20 concept.
- Each addressable module-parser hit family provides a `getDAQAddress()`
  overload beside its hit type; mapping lookup code must not access raw address
  members such as `chan` or `channel_num` directly. Translators may still copy
  raw fields into their typed DigiHits.
- Channel-addressed hit families normalize their native member (`chan`,
  `channel_num`, or `apv_channel`) to `DAQAddress::channel`.
- Board-level hit families without a channel use
  `DAQAddress::UnspecifiedChannel`; mapping files represent that exact sentinel
  with the keyword `none` in the channel column.
- `none` means “this record has no channel.” It is not a wildcard and does not
  match ordinary numbered channels.
- Duplicate route keys fail during plugin initialization; the registry is
  immutable during event processing.
- A mapped detector without a registered route for that raw-hit type is
  skipped.
- DigiHits contain copied digitized values; consumers do not need to traverse
  back to raw hits for normal use. The HMS FADC scaler route copies one
  complete 16-counter board-level record into one
  `HMSHodoscopeFADCScalerDigiHit`.
- `DetectorAddress` is authoritative for HMS detector identity. Every HMS FADC
  format and the FADC scaler route validate the detector name and require
  `plane`, `bar`, and `signal`, then copy those values into flat fields on their
  typed DigiHits. The generic address is not retained by any DigiHit.
- Waveform, pulse, pulse-integral, pulse-time, and pulse-peak FADC records
  produce distinct DigiHit types. Translation preserves each hardware payload
  and does not combine Hall-B pulse fragments into reconstructed pulses.
- TI scaler and helicity decoder records do not participate in detector
  translation and intentionally do not satisfy `DAQAddressable`.
- Channel-addressed diagnostic CSVs keep `rocid`, `slot`, and `channel`
  contiguous before module and detector fields so mapping inputs can be
  compared directly with translated outputs.
- Diagnostic outputs are grouped under a detector-named subdirectory; files
  inside it are named only for their raw-hit family.

## Failure Behavior

Loading throws when the catalog or a manifest cannot be read, is empty, or has
malformed or duplicate entries; when detector run ranges overlap or are
reversed; when a referenced mapping file cannot be read or declares a detector
different from its root-catalog entry; when declarations or channel rows are
invalid; when a referenced path is absolute, traverses through `..`, or resolves
outside its owning configuration directory; or when a DAQ address is duplicated
within a combined table. The
keyword `none` is accepted only in the DAQ channel column. A table request
throws when no configured detector mapping applies to that run.

## Key Components

- `config/evio_parser/detector_mappings/`
- `src/plugins/evio_parser/core/detector_mapping_objects/`
- Address overloads beside participating hit types under
  `src/plugins/evio_parser/module_parsers/`
- `src/plugins/evio_parser/services/JEventService_TranslationTable.*`
- `src/plugins/evio_parser/services/JEventService_DetectorTranslatorsMap.h`
- `src/plugins/evio_parser/detector_translators/`
- `src/plugins/evio_parser/processors/detector_digi_hits/`

## Verification

The `translation_table_tests` CTest loads the dependency-free demo HMS mapping
and verifies its known lookup, duplicate insertion rejection, and an
unknown-address lookup.

The `translation_table_service_tests` CTest verifies that the service combines
multiple detectors, selects different HMS mappings across a run boundary, and
preserves the applicable BCAL mapping in both tables. It also rejects a mapping
file whose declared detector differs from its root-catalog entry, plus absolute
and escaping paths at both manifest levels. Run-coverage checks verify global
gap failures, detector-specific omission, cached lookups across failures and
repeated range switches, and cross-detector DAQ-address collision rejection.

The `hms_hodoscope_fadc_translator_tests` CTest verifies detector identity and
hardware measurements for every emitted FADC format, rejection of invalid
detector metadata, and typed insertion into a `JEvent`.

The `hms_hodoscope_fadc_scaler_translator_tests` CTest verifies that all 16
FADC scaler counters and the mapped detector identity are copied into a typed
HMS board-level DigiHit and inserted into the event.

The `detector_translators_map_tests` CTest verifies duplicate-route rejection
and registry immutability after initialization.

The `daq_address_tests` CTest verifies the participating module-parser hit
families, including normal channel names, VFTDC's `channel_num`, MPD's
`apv_channel`, and the FADC scaler board-level sentinel. It also verifies that
TI scaler and helicity decoder records are not `DAQAddressable`.

The `translation_table_tests` CTest also verifies that `none` maps to
`DAQAddress::UnspecifiedChannel`.

For EVIO integration checks, load `detector_translation_dump` after
`evio_parser`. It writes each published translated DigiHit type to a dedicated
CSV under its detector subdirectory; the current demo mapping expects DAQ
address `(rocid=1, slot=3, channel=0)`.

Production FADC scaler translation additionally requires an HMS mapping row
for the real board address in the form `rocid slot none ...`; no hardware
address is guessed by the source configuration.
