# EVIO Extension Architecture

## Purpose

Keep the EVIO pipeline setup-neutral while allowing reusable hardware and
experiment plugins to register parsers and detector translators without
modifying `evio_parser`.

## Main Flow

1. `evio_parser` provides the EVIO source, unfolder, shared services, and
   installed extension APIs.
2. `evio_common_modules` requests `evio_parser` and registers JCE's reusable
   electronics parsers.
3. `detector_translation` requests `evio_parser` and provides the run-aware
   mapping service plus the generic translation processor.
4. A setup plugin such as `hms_detector_translation` requests its dependencies
   and registers detector routes and its mapping catalog with shared services.
5. The first event translation freezes the registry. Event processing then
   uses immutable type-erased scanner entries.

## Expected Behavior

- `evio_parser` does not include or link common raw-hit or detector DigiHit
  types.
- Module parsers are selected by mapped module ID. Each invocation receives a
  `BankContext` containing the source EVIO bank metadata; the parser abstraction
  remains `ModuleParser` because a parser represents a module data format, not
  a one-to-one EVIO bank type.
- Existing module parsers implementing the ROC-ID-only overload remain valid.
  New parsers may implement the context-rich overload when they need the bank
  tag, number, data type, or logger.
- Filtering, bank-to-module routing, and `BankContext` use the original 16-bit
  EVIO bank tag without masking its upper bits.
- Module parser plugins register shared parser instances during JANA service
  initialization. Null parsers, duplicate module IDs, and registration after
  the first lookup fail with `JException`.
- Module parser plugins register their bank-to-module routes during service
  initialization. `mapping.db` remains a compatibility source; duplicate bank
  IDs and registration after the first lookup fail with `JException`.
- Setup plugins register named detector-mapping catalog directories during
  plugin loading. The translation-table service freezes and merges those
  catalogs during initialization; `TRANSLATION:DIRECTORY` remains an explicit
  single-directory override.
- Registering a translator for `RawHitT` makes the generic processor scan
  `RawHitT`; the processor has no central raw-type list.
- Plugins can link the installed `evio_parser_api` or `detector_mapping_api`
  targets.
- `evio_parser_data_types` remains a compatibility umbrella for existing
  consumers.
- Loading `evio_processor` automatically loads `evio_common_modules`.
- Loading `detector_translation_dump` automatically loads the HMS translation
  chain it consumes.
- Duplicate translator routes and registration after the registry freezes
  fail with `JException`.

## Failure Behavior

- A mapped bank without a registered parser fails through the existing parser
  lookup error.
- Invalid, duplicate, or late module-parser registration fails immediately.
- Duplicate or late bank-to-module route registration fails immediately.
- Duplicate mapping-provider names, late provider registration, and detectors
  declared by multiple providers fail immediately.
- A mapped detector without a route for the raw-hit type is skipped.
- Missing or invalid detector mapping configuration fails during mapping
  service initialization.

## Key Components

- `src/plugins/evio_parser/`
- `src/plugins/evio_common_modules/`
- `src/plugins/detector_translation/`
- `src/plugins/detector_translation/examples/hms_detector_translation/`
- `src/plugins/detector_translation/services/JEventService_DetectorTranslatorsMap.h`
- `src/plugins/evio_parser/core/ModuleParser.h`

## Verification

- Build all four plugins independently in the same build.
- Run `detector_translators_map_tests`.
- Run `module_parsers_map_tests`.
- Run `bank_to_module_map_tests`.
- Install JCE and configure an external consumer using the namespaced API
  targets.
- Confirm the generic `evio_parser` target has no common-hit or HMS link
  dependency.
