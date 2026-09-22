# EVIO Parser

`evio_parser` is the setup-neutral EVIO ingestion plugin. It reads the EVIO
stream, classifies top-level events, decodes physics block structure, and
unfolds block contents into JANA `PhysicsEvent` children. Electronics parsers,
user-defined event decoders, and detector translators are registered by other
plugins.

For the public APIs, Compton example, and migration sequence, read the
[EVIO extension architecture guide](../../../docs/evio-extension-architecture.md).

## Runtime Composition

| Plugin | Load when | Provides |
|---|---|---|
| `evio_parser` | Always for EVIO input | Source, classifier, physics parser, unfolder, registries |
| `evio_common_modules` | Standard hardware banks are present | CAEN1190, FADC, scaler, helicity, MPD, VFTDC parsers |
| `detector_translation` | Raw hits should become DigiHits | Mapping service and generic translation processor |
| `hms_detector_translation` | HMS hodoscope DigiHits are wanted | HMS translator routes |
| Setup plugin | Setup-owned data is present | Embedded banks, standalone events, objects, and routes |

Plugins request prerequisites with `JApplication::AddPlugin()`. A setup plugin
may request `evio_common_modules` and reuse FADC without duplicating its
registration code. The default job loads:

```text
evio_parser,evio_common_modules,evio_processor
```

Detector translation is opt-in because mappings and detector routes are setup
dependent.

## Data Flow

```text
EVIO record
  +-- run control ---------------------------> JANA Run event
  +-- registered standalone user event -----> JANA SlowControls event
  +-- physics block
        +-- trigger bank -> TriggerData
        +-- ROC/data banks -> BankParser registrations
        +-- PhysicsEvent containers
              +-- JEventUnfolder_EVIO -> JANA PhysicsEvent children
                    +-- optional detector_translation
```

Standalone events do not enter the trigger parser or unfolder. Embedded user
banks do because they are part of a physics block and share its trigger data.

## Public APIs

Downstream projects should use the installed target:

```cmake
find_package(jana2_common_extensions REQUIRED)
target_link_libraries(my_setup_plugin PRIVATE
    jana2_common_extensions::evio_parser_api)
```

`evio_parser_api` exposes `BankParser`, `BankContext`, the `ModuleParser`
compatibility alias, physics data objects, `TopLevelEventDecoder`, selector
types, and the bank/parser/top-level registries. Registries accept additions
during JANA service initialization and freeze on first event lookup. Duplicate
keys fail during initialization.

Common electronics parsers and their hit headers are built and installed by
`evio_common_modules`. Link
`jana2_common_extensions::evio_common_modules_data_types` to consume FADC and
other common raw-hit types. HMS DigiHits use
`jana2_common_extensions::hms_detector_translation_data_types`.
`evio_parser_data_types` remains available for existing CMake consumers.

## Event Classes

Physics tags `0xff50`, `0xff58`, `0xff70`, and `0xff78` become block-level
events. Tags `0xffd0` through `0xffdf` become run-level events; prestart
`0xffd1` updates the source run number.

All other top-level events are offered to
`JEventService_TopLevelEventDecoders`. A setup selector can constrain the tag
with a mask and optionally constrain EVIO number and data type. The most
specific match wins. `JEventLevel::SlowControls` is the normal level for
asynchronous configuration or monitoring data with no trigger bank. Unknown
events are skipped with a debug log entry.

## Embedded Banks

CODA data-bank tags combine status and description:

```text
status = (tag >> 12) & 0x0f
description = tag & 0x0fff
```

Filtering and routing use the 12-bit description. `BankContext` includes both
fields, ROC ID, EVIO number, data type, and logger. Bank `0xace` therefore has
one route regardless of its upper status nibble.

`mapping.db` is the default static bank-description to parser-ID table. A
setup should register its private route programmatically:

```cpp
void ComptonRegistration::Init() {
    constexpr int parser_id = 0x100ace;
    banks->addRoute(0xace, parser_id);

    auto parser = std::make_shared<ComptonAceParser>();
    parsers->addParser(parser_id, std::move(parser));
}
```

Parser IDs are process-local dispatch keys, not EVIO fields. Choose a private
range that cannot collide with reusable hardware IDs.

## Registration Lifecycle

Provide a small registration service instead of retrieving and mutating
services directly in `InitPlugin()`:

```cpp
class SetupRegistration final : public JService {
public:
    Service<JEventService_BankRoutes> banks {this};
    Service<JEventService_BankParsers> parsers {this};
    Service<JEventService_TopLevelEventDecoders> events {this};

    void Init() override {
        // Add setup registrations here.
    }
};

extern "C" void InitPlugin(JApplication* app) {
    InitJANAPlugin(app);
    app->AddPlugin("evio_common_modules");
    app->ProvideService(std::make_shared<SetupRegistration>());
}
```

The `Service<T>` members let JANA establish initialization order independent
of command-line plugin order.

## Configuration

| Parameter | Default | Meaning |
|---|---|---|
| `BANKMAP:FILE` | `<config>/mapping.db` | Static `parser_id bank_description` rows |
| `FILTER:ENABLE` | `false` | Enable ROC/bank allow-list filtering |
| `FILTER:FILE` | `<config>/filter.db` | Filter rows |
| `TRANSLATION:DIRECTORY` | `<config>/evio_parser/detector_mappings` | Run-aware mappings used by `detector_translation` |

`JCE_CONFIG_DIR` overrides the installed configuration directory.

## Detector Translation

The optional stage maps:

```text
typed raw hit -> DAQAddress -> run-specific DetectorAddress -> typed DigiHit
```

External translation plugins link
`jana2_common_extensions::detector_mapping_api`, request
`detector_translation`, and register routes with
`JEventService_DetectorTranslatorsMap`. The shared processor discovers raw-hit
types through that registry, so setup-defined types require no central edit.

See [Detector Translation](../../../docs/detector-translation.md) and the
[mapping format](../../../config/evio_parser/detector_mappings/README.md).

## Ownership Rules

This repository owns EVIO/CODA structure, common electronics parsers, raw-hit
types, registration APIs, and generic mapping infrastructure. A setup repo owns
user-event meanings, private parser IDs and JObjects, detector translators,
mapping data, calibration, geometry, and reconstruction.
