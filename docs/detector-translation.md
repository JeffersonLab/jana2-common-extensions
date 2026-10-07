# Detector Translation

## Purpose

Detector translation maps a DAQ address `(rocid, slot, channel)` to a detector
name and detector-specific integer channel fields. It does not decode hardware,
calibrate measurements, or provide detector geometry.

## Main Flow

1. On the first table request, `JEventService_TranslationTable` loads the root detector catalog, each
   detector's run-range manifest, and the referenced mapping files.
2. During that synchronized first request, the service builds immutable combined tables for
   every resulting run interval and publishes the table selected for each run.
3. A module-parser hit type opts into translation by providing
   `DAQAddress getDAQAddress(const HitType&)`, which normalizes its hardware
   address field names.
4. Registering a translator also registers a type-erased scanner for that raw
   hit type. The central processor asks the registry to scan the event and
   performs one immutable lookup per hit using only `getDAQAddress()`.
5. `JEventService_DetectorTranslatorsMap` selects a translator by raw-hit C++
   type and detector name.
6. The translator returns its concrete typed DigiHit; core inserts it into the
   current event and records the producing route.

## Expected Behavior

- Detector-specific fields remain generic name/value pairs.
- Unmapped and deliberately excluded reference channels return `nullptr`.
- A mapping file applies one detector name to every channel it contains.
- Each setup plugin installs detector mappings under its own configuration
  namespace, for example
  `config/<namespace>/<setup-plugin>/detector_mappings/` in its own prefix.
- Setup plugins register named mapping-catalog directories in their registration service's
  `Init()`, before event processing. Catalogs may contribute different detectors and are merged
  into the same run-aware translation tables.
- `TRANSLATION:DIRECTORY` bypasses registered providers and loads only the
  specified mapping directory.
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
- All referenced mapping files and combined tables are loaded once on the first
  table request, after all registration services initialize. Concurrent first
  requests synchronize loading; subsequent lookups perform no configuration
  file I/O or table construction.
- Combined tables with the same selected mapping-file set share one immutable
  table instance.
- Mapping changes are expected only between runs.
- Translation runs in the processor's parallel event callback.
- The processor is registered by `detector_translation` and inserts DigiHits before
  downstream processors consume the physics event.
- Unmapped channels and channels belonging to other detectors are skipped.
- Routes are keyed by raw-hit C++ type and detector name.
- Each setup plugin owns its route registrations. The registry freezes on the
  first event translation, after plugin initialization is complete.
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
- DigiHits contain copied digitized values. Experiment plugins own detector
  identity validation, required fields, output schemas, and conversion policy.
  JCE provides no experiment-specific DigiHit types or routes.
- TI scaler and helicity decoder records do not participate in detector
  translation and intentionally do not satisfy `DAQAddressable`.
- Channel-addressed diagnostic CSVs keep `rocid`, `slot`, and `channel`
  contiguous before module and detector fields so mapping inputs can be
  compared directly with translated outputs.
- Each route registered with `JEventService_DetectorTranslatorsMap` requires
  exactly one typed CSV row-writing function and a nonempty header. Its conversion returns one
  DigiHit; core publishes it to the normal untagged event collection.
- CSV header constants and row-writing functions live beside their
  detector/raw-family translators. `DetectorTranslationDump` owns dump metadata,
  generated paths, collision checks, and event-writing callbacks. The translator
  service owns this helper; there is no separate writer service.
- Core keeps event-owned non-owning references to each route's DigiHits, so
  routes sharing an output type do not mix rows. All per-event grouping and
  provenance state is local to the event; shared routes and formatting callbacks remain immutable.
- Setup plugins register routes in their service initialization before the
  first event freezes the registry. Every translator has one writer.
- The dump plugin depends only on generic translation and does not load HMS.
  Load the selected setup plugin before the dump plugin.
- Registration opens no files. The first dump event freezes registration and
  creates one file per route. Jobs with no events create no files. Missing route outputs are skipped; routes without
  hits leave header-only files. With no routes, no files are created.
- Duplicate routes, missing row writers or headers, empty detector names or conversions,
  generated output path collisions, late registration, and output I/O errors
  fail. Dumping truncates existing outputs on each run.
- Output paths are `<detector-key>/<raw-hit-C++-type>.csv`, using reversible
  percent encoding for bytes other than letters, digits, underscores and hyphens.
  For example, `MY_DETECTOR/MyRawHit.csv` identifies one route.

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

- `src/plugins/detector_translation/detector_mapping/`
- Address overloads beside participating common raw-hit types under
  `src/plugins/evio_common_modules/module_parsers/`
- `src/plugins/detector_translation/services/JEventService_TranslationTable.*`
- `src/plugins/detector_translation/services/JEventService_DetectorMappingCatalogs.h`
- `src/plugins/detector_translation/services/JEventService_DetectorTranslatorsMap.h`
- `src/plugins/detector_translation/JEventProcessor_DetectorDigiHits.*`

## Verification

The `translation_table_tests` CTest loads the dependency-free demo HMS mapping
and verifies its known lookup, duplicate insertion rejection, and an
unknown-address lookup.

The `translation_table_service_tests` CTest forces table-service initialization
before catalog registration and verifies that the first table request includes
the later registration and rejects registration after loading. It also verifies
that the service combines
multiple detectors, selects different HMS mappings across a run boundary, and
preserves the applicable BCAL mapping in both tables. It also rejects a mapping
file whose declared detector differs from its root-catalog entry, plus absolute
and escaping paths at both manifest levels. Run-coverage checks verify global
gap failures, detector-specific omission, cached lookups across failures and
repeated range switches, cross-detector DAQ-address collision rejection,
missing referenced files, empty catalogs and manifests, reversed ranges, and
rejection of `max` as a lower bound.

The `detector_translators_map_tests` CTest verifies duplicate-route rejection
and registry immutability after initialization.

The `daq_address_tests` CTest verifies the participating module-parser hit
families, including normal channel names, VFTDC's `channel_num`, MPD's
`apv_channel`, and the FADC scaler board-level sentinel. It also verifies that
TI scaler and helicity decoder records are not `DAQAddressable`.

The `translation_table_tests` CTest also verifies that `none` maps to
`DAQAddress::UnspecifiedChannel`.

For EVIO integration checks, load an experiment setup plugin before
`detector_translation_dump`, for example
`-Pplugins=my_detector_setup,detector_translation_dump`. The experiment repository
owns its concrete translators, writer tests, and installed mapping catalog.
Generic core tests retain synthetic HMS/BCAL fixtures solely as test data;
these are not installed experiment defaults.

The `detector_translators_map_tests` verifies required writers, duplicate routes,
frozen registration, generated naming, missing outputs, and isolation across
raw types and detectors which publish the same DigiHit type. It also verifies
that unrelated objects in that DigiHit collection are excluded from dumping
and processing another event preserves earlier event provenance.
Concrete detector translation and CSV regression tests belong beside their
implementations in the owning experiment repository.
