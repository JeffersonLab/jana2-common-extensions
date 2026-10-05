# Detector Translation Dump Plugin

This diagnostic plugin groups published DigiHits by detector subdirectory and
writes each translation route to its own CSV. Each file contains only columns
applicable to that type. Waveform samples and scaler counts use `|` as the
separator within their CSV field. Channel-addressed files keep the DAQ address
columns together as `rocid,slot,channel` before `module_id` and detector fields.
Load the experiment setup plugin before this diagnostic plugin so translation
runs before CSV writing. The dump plugin requests generic `detector_translation`
but does not load any experiment setup plugin:

```tcsh
"${JCE_HOME}/scripts/jce.sh" \
  -Pplugins=my_detector_setup,detector_translation_dump \
  -PTRANSLATION:DIRECTORY=/path/to/detector_mappings \
  -Pdetector_translation_dump:OUTPUT_DIRECTORY=detector_translation_dump \
  /path/to/input.evio
```

Install your experiment setup plugin and its mapping catalog separately,
and add its plugin directory to `JANA_PLUGIN_PATH`. Output follows the route
identity, for example:

```text
MY_DETECTOR/
└── MyRawHit.csv
```

A file containing only its header means the input produced no matching
translated hits of that type.

For candidate configuration, boundary checks, and common failure causes, see
the [detector mapping guide](../detector_translation/DETECTOR_MAPPINGS.md).
Setup plugins register conversion and formatting together with the single
`JEventService_DetectorTranslatorsMap` service, exported by `detector_mapping_api`:

```cpp
translators.addTranslator<MyRawHit, MyDigiHit>(
    "MY_DETECTOR", makeMyDigiHit,
    MyCSVHeader, writeMyCSVRow);
```

The conversion accepts `(const MyRawHit&, const DetectorAddress&)` and returns
one `MyDigiHit`. Core inserts that result into the ordinary untagged DigiHit
collection. The CSV header and row-writing function live beside the translator:

```cpp
extern const std::string MyCSVHeader;
void writeMyCSVRow(std::ostream&, std::uint64_t event_number,
                   const MyDigiHit&);
```

Define both in the detector/raw-family's dump-writer `.cc` file. `writeMyCSVRow`
writes a complete row including its newline, and must not retain event data.
No writer subclasses are needed. The service owns a `DetectorTranslationDump`
helper that handles output descriptions, naming, collision checks, and
route-specific event-writing callbacks. The processor owns the files.
One non-null row writer and nonempty CSV header are required per route. Missing
formatting, empty detector names or conversions, duplicate route keys, and
registration after the registry freezes fail.

Core derives the directory from the route's detector key and the filename from
the full demangled raw-hit C++ type plus `.csv`. Letters, digits, underscores,
and hyphens are preserved; other bytes are percent-encoded, preserving identity
while preventing path traversal. Type-name demangling uses the GCC/Clang C++ ABI
available on the supported Linux/macOS stack. Distinct C++ identities whose
displayed names generate the same path are rejected.

This intentionally replaces the previous lowercase HMS folder and short
filenames. Headers and row formatting are preserved. Callers do not supply
output paths or repeat detector names in formatting functions.

Each event owns route-specific references to the DigiHits produced by that
route. Writers read those references rather than the entire DigiHit collection,
so different raw types or detectors producing the same DigiHit type remain
separate in their CSVs. These references add one pointer per translated hit and
one collection per active route; they own no DigiHits and perform no file I/O.

Register during plugin loading after requesting generic translation.
Registration opens no files and does not enable dumping. The dump processor
freezes the translator registry in `Init()`, opens each route's file, writes its
header, and invokes writers in its sequential event callback. Missing route
outputs are skipped without requesting factory creation. With no routes, it
creates no files. Routes without hits produce header-only CSVs.

Files are truncated on each run; output errors fail the job. Only enable one
dump processor per output directory. CSV headers and row functions remain
experiment-owned beside each detector/raw-family translator.
Keep `rocid,slot,channel` contiguous and escape CSV fields when needed.

Focused verification on a configured test-enabled build:

```tcsh
cmake --build build --target detector_translators_map_tests
ctest --test-dir build -R '^detector_translators_map_tests$' --output-on-failure
```

For a superbuild, the inner build directory is `build-super/jce-build`.
Enable `JCE_SUPERBUILD_BUILD_TESTING=ON` and use that directory with the same
CTest expression, or run the superbuild `check` target for all tests.
The registry test verifies required writers, duplicate routes, frozen
registration, generated naming, and isolation when multiple routes produce
the same DigiHit type. Experiment-owned route tests should check generated paths, headers, distinct
field values, and array formatting.
An EVIO run using the command above additionally checks processor ordering
and file creation. Compare CSV content with the previous version using the
new filenames.
