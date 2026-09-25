# EVIO Extension Architecture and Migration Guide

## Goal

`jana2-common-extensions` should know the EVIO and CODA structure and reusable
electronics formats. It should not know the meaning of an experiment-owned
bank or standalone user event. A DAQ repository such as `compton` should link
the public libraries at build time and load the shared plugins at runtime,
then register only its own decoders.

This separation covers three distinct extension points:

1. A reusable electronics bank inside a physics block, such as FADC250.
2. A setup-owned bank inside a physics block, such as Compton bank `0xace`.
3. A setup-owned top-level EVIO event with no trigger bank.

The third case must be classified before physics parsing. It is not a malformed
physics event and must never be sent to `parseTriggerBank()`.

## Libraries and Plugins in JANA2

A C++ library is linked while building. It provides headers, data types, parser
base classes, registries, and implementation code. Loading a library does not
register anything with `JApplication`.

A JANA plugin is a runtime shared library exporting
`extern "C" void InitPlugin(JApplication*)`. Loading it adds event sources,
services, factories, processors, or other plugins to one application.

The resulting components are:

| Component | Kind | Responsibility |
|---|---|---|
| `evio_parser_api` | CMake interface library | `BankParser`, `TopLevelEventDecoder`, raw event types, and registration services |
| `evio_common_modules_data_types` | CMake interface library | Reusable electronics raw-hit headers |
| `detector_mapping_api` | CMake interface library | DAQ/detector addresses, translation table, and translator registry |
| `hms_detector_translation_data_types` | CMake interface library | HMS DigiHit headers |
| `evio_parser` | JANA plugin | EVIO source, event classification, physics parser, block-to-physics unfolder, registries |
| `evio_common_modules` | JANA plugin | Registers reusable CAEN1190, FADC, scaler, helicity, MPD, and VFTDC parsers |
| `detector_translation` | JANA plugin | Run-aware mapping service and generic raw-hit translation processor |
| `hms_detector_translation` | JANA plugin | Optional HMS-specific translator registrations and DigiHit types |
| `compton` | External JANA plugin | Compton-owned bank/event decoders, objects, mappings, and translator routes |

Reusable parser implementations and their raw-hit objects live in
`src/plugins/evio_common_modules/module_parsers/`. The EVIO source and public
registration interfaces live in `src/plugins/evio_parser/`. The original
`evio_parser_data_types` target remains an umbrella for older consumers; new
consumers should link the specific data-types target they use.

Plugins may request dependencies with `app->AddPlugin()`. Registration itself
is performed from a small `JService::Init()` callback. This matters because
calling `GetService()` directly from `InitPlugin()` depends on plugin load order,
whereas JANA service dependencies establish the initialization order.

## Event Classification

`JEventSource_EVIO::Emit()` retains a raw EVIO event while changing JANA event
levels. It classifies in this order:

1. CODA run-control tags `0xffd0` through `0xffdf` become `Run` events.
2. Physics tags `0xff50`, `0xff58`, `0xff70`, and `0xff78` become `Block`
   events and follow trigger-bank parsing and unfolding.
3. A registered `TopLevelEventSelector` becomes the level chosen by the setup,
   normally `SlowControls` for asynchronous user/configuration data.
4. An unrecognized event is skipped and logged at debug level.

A selector can match the 16-bit tag with a mask and can additionally constrain
the EVIO header number and data type. The most specific match wins; equally
specific matches are rejected as ambiguous. The registry becomes immutable on
its first lookup.

CODA data-bank tags combine a four-bit status with a 12-bit description:

```text
full tag:     ssss dddd dddd dddd
status:       (tag >> 12) & 0x0f
description: tag & 0x0fff
```

Bank `0xace` is therefore selected using the description, independently of its
status nibble. `BankContext` supplies both values to the decoder.

## Embedded User Bank

An embedded user bank is part of a normal physics block. The outer block has a
trigger bank, so the common parser can determine the contained physics event
numbers. The setup parser must attach its decoded objects to the appropriate
`PhysicsEvent` entries by returning an `EventHits` implementation, just as an
electronics parser does.

The setup registration service adds both the bank route and parser:

```cpp
class ComptonRegistration final : public JService {
public:
    Service<JEventService_BankToModuleMap> banks {this};
    Service<JEventService_ModuleParsersMap> parsers {this};

    void Init() override {
        constexpr int parser_id = 0x100ace; // private setup namespace
        banks->addRoute(0xace, parser_id);

        auto parser = std::make_shared<ComptonAceParser>();
        parsers->addParser(parser_id, std::move(parser));
    }
};
```

New parsers should derive from `BankParser` and override the context-rich
overload. The legacy `(bank, rocid, ... )` overload remains available for
electronics parsers during migration.

```cpp
class ComptonAceParser final : public BankParser {
public:
    void parse(
        std::shared_ptr<evio::BaseStructure> bank,
        const BankContext& context,
        std::vector<PhysicsEvent*>& events,
        TriggerData& trigger) override {
        // Decode bank->getUIntData(), then add EventHits objects to events.
        // context.description == 0xace; context.status is separate.
    }
};
```

## Standalone User Event

A standalone user event has no trigger bank and is decoded directly into its
own JANA event. It normally uses `SlowControls`; `Run` is also supported when
the record changes run-scoped state. Its decoder inserts setup-owned JObjects
into the output event:

```cpp
class ComptonConfigDecoder final : public TopLevelEventDecoder {
public:
    std::string_view name() const noexcept override {
        return "compton-config";
    }

    void decode(
        const std::shared_ptr<evio::EvioEvent>& raw,
        const TopLevelEventContext& context,
        JEvent& output) const override {
        auto* config = new ComptonConfig;
        // Decode strings or words from raw and populate config.
        output.Insert(config);
    }
};
```

Register it from the same setup service:

```cpp
Service<JEventService_TopLevelEventDecoders> events {this};

events->addDecoder(
    "compton-config",
    TopLevelEventSelector {
        .tag_value = 0x0071,
        .tag_mask = 0xffff,
        .number = 0,
        .data_type = std::nullopt,
    },
    JEventLevel::SlowControls,
    std::make_shared<ComptonConfigDecoder>());
```

Use the actual Compton header values in place of the example selector. Prefer
the narrowest stable selector. A broad selector can accidentally claim an
event owned by another setup component.

## Detector Translation

Hardware decoding and channel translation are separate stages:

```text
EVIO words -> typed raw hit -> DAQAddress -> DetectorAddress -> typed DigiHit
```

An addressable raw-hit type supplies `getDAQAddress(const RawHit&)`. A setup
plugin registers a translator keyed by raw-hit C++ type and detector name:

```cpp
Service<JEventService_DetectorTranslatorsMap> translators {this};

translators->addTranslator<FADC250PulseHit>(
    "COMPTON_DETECTOR",
    translateComptonFADCPulse);
```

The generic processor is type-erased: it scans every registered raw-hit type,
looks up the run-specific `TranslationTable`, and invokes the matching route.
Adding a setup-defined raw-hit type therefore requires no edit to the shared
processor. Translation files and manifests remain configuration owned by the
DAQ setup; calibration and geometry remain later reconstruction stages.

Load `detector_translation` only when this stage is wanted. A setup translation
plugin should request it with `app->AddPlugin("detector_translation")`.

## External Compton Plugin

A minimal external build links APIs and the module library it actually uses:

```cmake
find_package(jana2_common_extensions REQUIRED)

add_jana_plugin(compton
    SOURCES InitPlugin.cc ComptonAceParser.cc ComptonConfigDecoder.cc)

target_link_libraries(compton PRIVATE
    jana2_common_extensions::evio_parser_api
    jana2_common_extensions::evio_common_modules_data_types
    jana2_common_extensions::detector_mapping_api)
```

Its entry point requests common dependencies and provides the deferred
registration service:

```cpp
extern "C" void InitPlugin(JApplication* app) {
    InitJANAPlugin(app);
    app->AddPlugin("evio_common_modules");
    app->ProvideService(std::make_shared<ComptonRegistration>());
}
```

Run with both plugin directories on `JANA_PLUGIN_PATH` and load `compton`.
Dependency plugins are requested by its entry point.

## Migration Plan

1. Derive current and new parsers from `BankParser`. The `ModuleParser.h`
   compatibility alias has been removed; downstream repositories must update
   their includes and base classes.
2. Change deployments from `evio_parser` alone to
   `evio_parser,evio_common_modules`. The default plugin file already does so.
3. Move experiment-owned parser source and hit objects to the experiment repo.
   Register bank routes programmatically instead of editing shared
   `mapping.db`.
4. Register every triggerless top-level event with a precise selector and test
   that it is emitted without trigger-bank parsing.
5. Move detector-specific route registration and mapping configuration to the
   setup repo. Load `detector_translation` only for jobs producing DigiHits.
6. Rename the remaining module-oriented service methods in a later major
   release, after downstream callers have migrated.

## Required Tests for a Setup Plugin

- Decode a known `0xace` bank with nonzero status bits and verify description
  masking.
- Verify the embedded bank contributes to the correct unfolded physics event.
- Decode a standalone event containing words and one containing a string.
- Verify the standalone decoder runs with no trigger bank present.
- Reject duplicate bank routes, parser IDs, decoder keys, and translator routes.
- Verify mapping selection on both sides of each run boundary.
- Run an EVIO integration fixture containing run-control, physics, embedded
  user-bank, and standalone user events in their real stream order.
