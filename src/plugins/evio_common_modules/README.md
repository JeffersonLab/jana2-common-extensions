# Common EVIO Module Parsers

`evio_common_modules` registers reusable electronics-bank parsers with the
registries provided by `evio_parser`. It requests `evio_parser` automatically.

Load it for CAEN1190, FADC, FADC scaler, TI scaler, helicity, MPD, and VFTDC
banks. Experiment-owned formats should be built and registered by an
experiment plugin instead of being added here.

The reusable parsers, raw-hit types, tests, and registration service live in
`module_parsers/`. A new shared electronics format belongs in its own
`module_parsers/<module>/` directory. Add it to
`module_parsers/CMakeLists.txt`, then register its parser ID in
`module_parsers/InitModuleParsers.cc`.

Downstream CMake projects link the installed header target:

```cmake
target_link_libraries(my_processor PRIVATE
    jana2_common_extensions::evio_common_modules_data_types)
```

`evio_parser_api` supplies the shared parser interfaces. A DAQ-specific bank
parser belongs in the DAQ repository and links that API directly.
