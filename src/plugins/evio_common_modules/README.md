# Common EVIO Module Parsers

`evio_common_modules` registers reusable electronics-bank parsers with the
registries supplied by `evio_parser`. It loads `evio_parser` automatically.
The built-in parsers cover CAEN1190, FADC, FADC scaler, TI scaler, helicity,
MPD, and VFTDC. Keep experiment-specific banks in the experiment's own plugin;
see the [extension architecture guide](../../../docs/evio-extension-architecture.md).

## Adding a Reusable Module Parser

The [main-branch guide](https://github.com/JeffersonLab/jana2-common-extensions/tree/main/src/plugins/evio_parser#adding-a-new-module-parser)
still describes the basic workflow, but its `evio_parser/module_parsers`
paths, central registration function, and raw-pointer registration are from
the old layout. Use the steps below on this branch.

### 1. Choose the IDs and word format

Identify the EVIO DMA bank's 12-bit description and choose an unused parser ID.
These are separate values: the bank-to-parser route is read from `config/mapping.db`,
while the parser ID is registered in C++. For a shared MyHW example, suppose
both values are decimal `350`. Check that neither the parser ID nor the bank
description conflicts with existing registrations and routes. Document the
module's block, event, data, and trailer word layouts before decoding them.

Bank routing uses `tag & 0x0fff`; the upper nibble is status. If a parser needs
the full context (status, EVIO number, or data type), override the
`BankParser::parse(..., const BankContext&, ...)` overload instead of the
legacy `BankParser::parse(..., rocid, ...)` overload used by FADC.

### 2. Add the parser and hit types

Create `module_parsers/MyHW/` with this layout, using
[`FADC/`](module_parsers/FADC/) as a working reference:

```text
MyHW/
  CMakeLists.txt
  ModuleParser_MyHW.h
  ModuleParser_MyHW.cc
  data_objects/
    MyHWHit.h
    EventHits_MyHW.h
```

`MyHWHit` holds the decoded raw electronics fields. `EventHits_MyHW` derives
from `EventHits`, groups hits for one physics event, and publishes them in
`insertIntoEvent(JEvent&)` with `event.Insert(...)`. Follow
[`EventHits_FADC.h`](module_parsers/FADC/data_objects/EventHits_FADC.h)
for the ownership and insertion pattern. Keep detector-channel translation
out of the hardware parser.

### 3. Decode into physics events

Implement `ModuleParser_MyHW` by deriving from `BankParser`. Match the signature in
[`ModuleParser_FADC.h`](module_parsers/FADC/ModuleParser_FADC.h):

```cpp
void parse(std::shared_ptr<evio::BaseStructure> data_block,
           uint32_t rocid,
           std::vector<PhysicsEvent*>& physics_events,
           TriggerData& trigger_data) override;
```

Read words with `data_block->getUIntData()`, validate their sequence and
bounds, and decode the module's bit fields. `getBitsInRange(word, high, low)`
is available for fields narrower than 32 bits. Compute each physics event
number from `trigger_data.first_event_number` and the module's event index.
Accumulate hits in per-call local state, then append one
`new PhysicsEvent(event_number, event_hits)` for each event number. The core
parser merges contributions from different banks with the same event number.
It throws if the entire block yields no `PhysicsEvent`, so account for empty
module payloads when testing real blocks. Parsers are shared across worker
threads; do not store per-event decode state in mutable members.

### 4. Wire the library and installed headers

Add `module_parsers/MyHW/CMakeLists.txt` using the same shape as
[`FADC/CMakeLists.txt`](module_parsers/FADC/CMakeLists.txt):

```cmake
add_library(myhw_parser STATIC ModuleParser_MyHW.cc)
target_include_directories(myhw_parser PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/data_objects)
target_link_libraries(myhw_parser PUBLIC core)

set(MYHW_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data_objects PARENT_SCOPE)
file(GLOB MYHW_PUBLIC_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/data_objects/*.h)
set(MYHW_PUBLIC_HEADERS ${MYHW_PUBLIC_HEADERS} PARENT_SCOPE)
```

In [`module_parsers/CMakeLists.txt`](module_parsers/CMakeLists.txt), add
`add_subdirectory(MyHW)`, `myhw_parser` to `MODULE_PARSERS_LIBS`,
`${MYHW_INCLUDE_DIR}` to `MODULE_PARSERS_INCLUDE_DIRS`, and
`${MYHW_PUBLIC_HEADERS}` to `MODULE_PARSERS_HEADERS`. The parent
`evio_common_modules/CMakeLists.txt` consumes these lists and installs the
public hit headers; do not add the parser to `evio_parser/CMakeLists.txt`.

### 5. Register and route it

Add `#include "ModuleParser_MyHW.h"` to
[`InitModuleParsers.cc`](module_parsers/InitModuleParsers.cc), then add this
inside `JEventService_CommonModuleParsers::Init()`:

```cpp
add(350, std::make_shared<ModuleParser_MyHW>());
```

The `add()` helper registers a `shared_ptr<BankParser>` with
`JEventService_ModuleParsersMap`. Do not register it in
`evio_parser/InitPlugin.cc`, and do not use the old
`InitModuleParsers(JApplication*)` example.

Add the route to [`config/mapping.db`](../../../config/mapping.db):

```text
# parser_id  bank_description (both decimal here)
350          350
```

The file format is `parser_id bank_description`, not the reverse. To use a
different mapping file, pass `-PBANKMAP:FILE=/path/to/mapping.db` or set
`JCE_CONFIG_DIR`. A setup-specific bank description should be routed by its
own setup plugin rather than added to the shared default mapping.

### 6. Consume and verify the hits

Within this repository, link a downstream processor against
`evio_common_modules_data_types`. Outside the repository, use the installed
target and the install prefix in `CMAKE_PREFIX_PATH`:

```cmake
find_package(jana2_common_extensions REQUIRED)
target_link_libraries(my_processor PRIVATE
    jana2_common_extensions::evio_common_modules_data_types)
```

Include `MyHWHit.h` and use `Input<MyHWHit>` in a JANA processor, following
`evio_processor`'s existing inputs. Add focused tests for word boundaries,
malformed data, and event-number assignment alongside the new parser. Then
build and run the test suite:

```sh
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
cmake --install build
```

Finally, run an EVIO file containing the new bank with
`-Pplugins=evio_parser,evio_common_modules` and check that the expected
`MyHWHit` objects reach the downstream processor. The plugin name is
`evio_common_modules`; `evio_common_modules_data_types` is a CMake header
target, not a runtime plugin.
