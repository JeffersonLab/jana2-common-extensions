# Detector Translation

## Purpose

Detector translation maps a DAQ address `(rocid, slot, channel)` to a detector
name and detector-specific integer channel fields. It does not decode hardware,
calibrate measurements, or provide detector geometry.

## Main Flow

1. `JEventService_TranslationTable` loads every `*.map` file from the configured
   detector mapping directory.
2. The service publishes it as `shared_ptr<const TranslationTable>`.
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
- Mapping files are loaded in deterministic filename order into one table.
- Until run-range selection is added, every run receives the same table
  instance.
- Mapping changes are expected only between runs.
- Translation runs in the processor's parallel event callback.
- The processor is registered by `evio_parser` and inserts DigiHits before
  downstream processors consume the physics event.
- Unmapped channels and channels belonging to other detectors are skipped.
- Routes are keyed by raw-hit C++ type and detector name.
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
- DigiHits contain copied digitized FADC values; consumers do not need to
  traverse back to raw hits for normal use.

## Failure Behavior

Loading throws when the directory cannot be read, contains no `*.map` files, a
file cannot be read, declarations or channel rows are invalid, a numeric field
is out of range, or a DAQ address is duplicated within or across files. The
keyword `none` is accepted only in the DAQ channel column.

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

The `translation_table_service_tests` CTest verifies that the service loads
multiple detector files into one immutable table and currently reuses it for
different run numbers.

The `hms_hodoscope_translator_tests` CTest verifies conversion of detector
coordinates and uncalibrated FADC pulse values and typed insertion into a
`JEvent`.

The `detector_translators_map_tests` CTest verifies duplicate-route rejection
and registry immutability after initialization.

The `daq_address_tests` CTest verifies that every current module-parser hit
family satisfies `DAQAddressable`, including normal channel names, VFTDC's
`channel_num`, MPD's `apv_channel`, and the board-level sentinel.

The `translation_table_tests` CTest also verifies that `none` maps to
`DAQAddress::UnspecifiedChannel`.

For EVIO integration checks, load `detector_translation_dump` after
`evio_parser`. It writes translated HMS hodoscope DigiHits to CSV; the current
demo mapping expects DAQ address `(rocid=1, slot=3, channel=0)`.
