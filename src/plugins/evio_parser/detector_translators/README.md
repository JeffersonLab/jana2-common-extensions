# Adding Detector Translation

Detector translation converts hardware-oriented raw hits into typed,
uncalibrated detector DigiHits. This guide covers the C++ work. For mapping
DAQ addresses and run ranges, see the
[detector mapping guide](../../../../config/evio_parser/detector_mappings/README.md).

## Before You Start

Do not start a detector translator until the hardware input already works.
You need:

- a module parser that decodes the EVIO words;
- a typed raw-hit object inserted into the physics-level `JEvent`;
- stable ROC, slot, and channel information for that raw hit; and
- representative raw-hit values and DAQ addresses for tests.

If any item is missing, follow
[Adding a New Module Parser](../README.md#adding-a-new-module-parser) first.
Translation does not decode EVIO words, calibrate measurements, reconstruct
physics quantities, or provide geometry.

The complete path is:

```text
EVIO words
  -> module parser
  -> typed raw hit
  -> getDAQAddress(raw hit)
  -> run-specific mapping lookup
  -> detector translator
  -> typed DigiHit
  -> downstream processor or detector_translation_dump CSV
```

## Decide Whether Configuration Is Enough

Before writing C++, ask the detector experts:

1. Does an existing module parser publish the needed raw-hit family?
2. Does an existing translator publish every DigiHit type the detector needs?
3. Do those DigiHits contain the required digitized measurements?
4. Does the detector identity helper require the correct mapping fields?

If all answers are yes, only mapping configuration may need to change. Mapping
files can change DAQ addresses and detector-field values; they cannot add a
raw-hit route, change a DigiHit schema, or perform calibration.

## Directory and Naming Rules

Use one directory per detector and one route directory per raw-hit family:

```text
detector_translators/
└── MyDetector/
    ├── CMakeLists.txt
    ├── MyDetectorIdentity.h
    ├── MyDetectorIdentity.cc
    ├── InitMyDetectorTranslators.h
    ├── InitMyDetectorTranslators.cc
    └── MyRawHitFamily/
        ├── CMakeLists.txt
        ├── MyRawHitFamilyTranslator.h
        ├── MyRawHitFamilyTranslator.cc
        ├── data_objects/
        │   └── MyRawHitFamilyDigiHit.h
        └── tests/
            └── MyRawHitFamilyTranslatorTests.cc
```

Keep tests in `tests/`, even when there is only one test file. Public headers
are installed flat, so exported C++ types must be globally detector-qualified,
for example `MyDetectorMyRawHitFamilyDigiHit`.

Use one exact detector key everywhere, conventionally uppercase with
underscores:

```text
MY_DETECTOR
```

The same key must appear in the root mapping catalog, mapping-file `detector`
declaration, identity validation, and translator registration.

## Step 1: Make the Raw Hit Addressable

Define `getDAQAddress()` beside the raw-hit type so argument-dependent lookup
can find it:

```cpp
#include "DAQAddress.h"

inline DAQAddress getDAQAddress(const MyRawHit& hit) {
    return {hit.rocid, hit.slot, hit.channel};
}
```

The overload must return `DAQAddress` exactly. This makes the type satisfy the
`DAQAddressable` C++20 concept.

If the native channel member has another name, such as `chan`, normalize it to
`DAQAddress::channel`. For a board-level record that truly has no channel, use:

```cpp
return {hit.rocid, hit.slot, DAQAddress::UnspecifiedChannel};
```

That sentinel is written as `none` in mapping files. It is not a wildcard.
Add the raw-hit type to `daq_address_tests` so the normalization is checked.

## Step 2: Add the Raw-Hit Family to the Central Scan

`JEventProcessor_DetectorDigiHits` scans each supported raw-hit collection once.
A new addressable type is not routed automatically.

In `processors/detector_digi_hits/JEventProcessor_DetectorDigiHits.h`:

```cpp
#include "MyRawHit.h"
```

In `ProcessParallel()` add:

```cpp
routeHits<MyRawHit>(
    event.Get<MyRawHit>("", false), table, translator_map, event);
```

Keep `event.Get<Hit>("", false)` in this parallel callback. Do not replace it
with processor `Input<T>`; `Input<T>` is populated for the sequential callback
and may be stale here.

## Step 3: Define the Detector Identity

Mapping fields are generic name/value pairs. Convert them once into a detector
identity shared by all routes for that detector:

```cpp
struct MyDetectorIdentity {
    std::int32_t layer;
    std::int32_t element;
    std::int32_t signal;
};

MyDetectorIdentity getMyDetectorIdentity(const DetectorAddress& address);
```

The implementation must:

- reject a detector key other than `MY_DETECTOR`;
- require every field used by the DigiHits; and
- throw a clear `JException` naming a missing field.

Do not silently default missing detector fields. Use
`HMSHodoscope/HMSHodoscopeIdentity.*` as the current concrete example.

## Step 4: Define the DigiHit

Create a plain typed data object under the route's `data_objects/` directory.
Include:

- the flattened detector identity needed by consumers;
- copied raw digitized measurements;
- useful raw identifiers such as ROC, slot, channel, or module ID; and
- no calibration or geometry results.

Name the exported type with the detector prefix:

```cpp
struct MyDetectorMyRawHitFamilyDigiHit {
    std::int32_t layer;
    std::int32_t element;
    std::int32_t signal;
    std::uint32_t rocid;
    std::uint32_t slot;
    std::uint32_t channel;
    // Copied digitized measurements...
};
```

The generic `DetectorAddress` does not need to be retained after its values
have been validated and copied.

## Step 5: Implement the Translator

Keep object construction separate from insertion so it can be unit tested
without running a full JANA pipeline:

```cpp
MyDetectorMyRawHitFamilyDigiHit makeMyDetectorMyRawHitFamilyDigiHit(
    const MyRawHit& raw,
    const DetectorAddress& address);

void translateMyDetectorMyRawHitFamilyHit(
    const MyRawHit& raw,
    const DetectorAddress& address,
    const JEvent& event);
```

The `make...` function should obtain the validated identity, copy the detector
fields and raw payload, and return the typed DigiHit. The `translate...`
function inserts that result with the default empty tag:

```cpp
event.Insert(new MyDetectorMyRawHitFamilyDigiHit(
    makeMyDetectorMyRawHitFamilyDigiHit(raw, address)));
```

Use the HMS Hodoscope FADC and FADC-scaler routes as working references. Their
current mappings are demonstration data, not physics-approved configuration.

## Step 6: Register the Route

Create `InitMyDetectorTranslators.*` in the detector directory and register
each raw-hit type separately:

```cpp
void InitMyDetectorTranslators(
    JEventService_DetectorTranslatorsMap& translators) {
    translators.addTranslator<MyRawHit>(
        "MY_DETECTOR",
        translateMyDetectorMyRawHitFamilyHit);
}
```

Then call that detector initializer from
`detector_translators/InitDetectorTranslators.cc` before `freeze()`:

```cpp
#include "InitMyDetectorTranslators.h"

InitMyDetectorTranslators(*translators);
```

The registry key is `(raw-hit C++ type, detector key)`. Duplicate keys and
registration after `freeze()` fail during initialization. A mapped detector
with no translator for the current raw-hit type is skipped.

## Step 7: Wire CMake and Public Headers

Follow the existing three-level aggregation:

1. The route `CMakeLists.txt` builds one static translator library, links the
   raw-hit parser, detector mapping, and detector identity libraries, and
   exports its include directories and public DigiHit headers to its parent.
2. The detector `CMakeLists.txt` adds each route, builds the detector
   initializer, and exports `<DETECTOR>_TRANSLATOR_LIBS`,
   `<DETECTOR>_TRANSLATOR_INCLUDE_DIRS`, and `<DETECTOR>_TRANSLATOR_HEADERS`.
3. `detector_translators/CMakeLists.txt` adds the detector and appends only
   those detector-level variables to `DETECTOR_TRANSLATOR_*`.

Do not name individual route targets in the top-level translator CMake file.
Public DigiHit headers must be included in the exported header list so
downstream plugins receive them through `evio_parser_data_types`.

Route `MyDetector/MyRawHitFamily/CMakeLists.txt`:

```cmake
add_library(my_detector_my_raw_hit_family_translator STATIC
    MyRawHitFamilyTranslator.cc
)
target_include_directories(my_detector_my_raw_hit_family_translator
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/data_objects
)
target_link_libraries(my_detector_my_raw_hit_family_translator
    PUBLIC
        detector_mapping
        my_raw_hit_parser
        my_detector_identity
)

set(MY_DETECTOR_MY_RAW_HIT_FAMILY_TRANSLATOR_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/data_objects
    PARENT_SCOPE
)
set(MY_DETECTOR_MY_RAW_HIT_FAMILY_TRANSLATOR_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/data_objects/MyRawHitFamilyDigiHit.h
    PARENT_SCOPE
)

if(BUILD_TESTING)
    add_executable(my_detector_my_raw_hit_family_translator_tests
        tests/MyRawHitFamilyTranslatorTests.cc
    )
    target_link_libraries(my_detector_my_raw_hit_family_translator_tests
        PRIVATE my_detector_my_raw_hit_family_translator
    )
    add_test(
        NAME my_detector_my_raw_hit_family_translator_tests
        COMMAND my_detector_my_raw_hit_family_translator_tests
    )
endif()
```

Replace `my_raw_hit_parser` with the actual module-parser target that exports
the raw-hit header.

Detector `MyDetector/CMakeLists.txt`:

```cmake
add_library(my_detector_identity STATIC
    MyDetectorIdentity.cc
)
target_include_directories(my_detector_identity
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${JANA_INCLUDE_DIRS}
)
target_link_libraries(my_detector_identity
    PUBLIC detector_mapping
    PRIVATE ${JANA_LIBRARY}
)

add_subdirectory(MyRawHitFamily)

add_library(my_detector_translators_init STATIC
    InitMyDetectorTranslators.cc
)
target_include_directories(my_detector_translators_init
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(my_detector_translators_init
    PUBLIC my_detector_my_raw_hit_family_translator
)

set(MY_DETECTOR_TRANSLATOR_LIBS
    my_detector_translators_init
    PARENT_SCOPE
)
set(MY_DETECTOR_TRANSLATOR_INCLUDE_DIRS
    ${MY_DETECTOR_MY_RAW_HIT_FAMILY_TRANSLATOR_INCLUDE_DIRS}
    PARENT_SCOPE
)
set(MY_DETECTOR_TRANSLATOR_HEADERS
    ${MY_DETECTOR_MY_RAW_HIT_FAMILY_TRANSLATOR_PUBLIC_HEADERS}
    PARENT_SCOPE
)
```

Finally, append the detector-level variables in
`detector_translators/CMakeLists.txt`:

```cmake
add_subdirectory(MyDetector)

set(DETECTOR_TRANSLATOR_LIBS
    ${HMS_TRANSLATOR_LIBS}
    ${MY_DETECTOR_TRANSLATOR_LIBS}
    PARENT_SCOPE
)
set(DETECTOR_TRANSLATOR_INCLUDE_DIRS
    ${HMS_TRANSLATOR_INCLUDE_DIRS}
    ${MY_DETECTOR_TRANSLATOR_INCLUDE_DIRS}
    PARENT_SCOPE
)
set(DETECTOR_TRANSLATOR_HEADERS
    ${HMS_TRANSLATOR_HEADERS}
    ${MY_DETECTOR_TRANSLATOR_HEADERS}
    PARENT_SCOPE
)
```

Keep the focused test target in the route's existing `CMakeLists.txt` and link
the production translator target rather than recompiling its `.cc` file.

## Step 8: Add Diagnostic CSV Output

Every new DigiHit type must remain observable through
`detector_translation_dump`. Update:

- `JEventProcessor_DetectorTranslationDump.h`: include the DigiHit header, add
  an optional `Input<DigiHit>`, and add an output stream;
- its constructor: call `SetOptional(true)` for the new input;
- `Init()`: create a detector-named directory and open a type-specific CSV with
  an explicit header;
- `ProcessSequential()`: write one row per DigiHit; and
- `Finish()`: close the stream.

Keep `rocid,slot,channel` contiguous for channel-addressed records. Use only
columns applicable to that DigiHit type. Update the dump plugin README and run
the CSV check in Step 11.

## Step 9: Add Mapping Configuration

Add the detector to the root catalog, create its run-range manifest, and add
at least one mapping file. The mapping field names must match the fields
required by the detector identity helper. Follow the
[mapping authoring guide](../../../../config/evio_parser/detector_mappings/README.md)
for exact formats and validation rules.

## Step 10: Add Focused Tests

Construct raw hits directly; unit tests should not depend on an EVIO file.
Cover at least:

- every emitted DigiHit type;
- every copied detector and raw measurement field;
- rejection of the wrong detector key;
- rejection of every missing required detector field; and
- insertion of the concrete DigiHit type into a `JEvent`.

Also update `daq_address_tests` for a new raw-hit address overload and
`detector_translators_map_tests` only when registry behavior changes.

## Step 11: Build and Verify CSV Output

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Then run representative data through the diagnostic plugin:

```bash
jana \
  -Pplugins=evio_parser,detector_translation_dump \
  -PTRANSLATION:DIRECTORY=/path/to/candidate/detector_mappings \
  -Pdetector_translation_dump:OUTPUT_DIRECTORY=detector_translation_dump \
  /path/to/input.evio
```

Inspect the detector's CSVs. Check known DAQ addresses, detector fields, raw
measurements, record counts, and runs at each mapping boundary. A header-only
CSV means no matching DigiHits of that type were published.

## Completion Checklist

- [ ] The module parser and typed raw hit already work.
- [ ] `getDAQAddress()` returns the correct normalized address.
- [ ] The raw-hit family is listed in the central parallel scan.
- [ ] Detector identity and required mapping fields are defined.
- [ ] DigiHit types contain the intended uncalibrated measurements.
- [ ] Construction and insertion translators are implemented.
- [ ] Every `(raw-hit type, detector)` route is registered before `freeze()`.
- [ ] Route, detector, and top-level CMake aggregation is complete.
- [ ] Public DigiHit headers are exported.
- [ ] `detector_translation_dump` handles every new DigiHit type.
- [ ] Root catalog, detector manifest, and mapping files are present.
- [ ] Focused unit tests and the complete CTest suite pass.
- [ ] Representative CSV output is reviewed with detector experts.
