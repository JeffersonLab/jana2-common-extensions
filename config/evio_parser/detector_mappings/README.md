# Detector Mapping Configuration

This directory contains physicist-editable configuration that maps DAQ
addresses to detector identities for specific run ranges. Use this guide when
the required raw-hit and DigiHit types already exist. For C++ translator work,
see the [detector translator guide](../../../src/plugins/evio_parser/detector_translators/README.md).

## Current HMS Status

The HMS Hodoscope route is a working reference implementation built with
demonstration mapping data. Its current DAQ addresses and detector-field values
are not physics-approved production configuration.

Before replacing only the `.map` files, confirm with detector experts that:

- the FADC and FADC-scaler raw-hit families are the records HMS needs;
- the published DigiHit types contain the required digitized measurements; and
- `plane`, `bar`, and `signal` are the correct detector identity fields.

If those assumptions are correct, real addresses, values, and run ranges can
be supplied through configuration. If the raw-hit families, DigiHit fields, or
identity model are wrong, the C++ translator must be changed first.

## Directory Layout

```text
detector_mappings/
├── README.md
├── manifest.map                 # root detector catalog
└── hms_hodoscope/
    ├── manifest.map             # this detector's run ranges
    └── hms_hodoscope.map        # DAQ-to-detector rows
```

Use one subdirectory per detector. A detector may have multiple mapping files
when its channel assignment changes between runs.

All files support blank lines and `#` comments. Tokens are separated by
whitespace; quoting is not supported.

## 1. Root Detector Catalog

The service always starts at `detector_mappings/manifest.map`.

```text
# detector detector_manifest
HMS_HODOSCOPE hms_hodoscope/manifest.map
MY_DETECTOR my_detector/manifest.map
```

Each non-comment row contains exactly:

```text
DETECTOR_KEY relative/path/to/detector/manifest.map
```

Rules:

- Detector keys must be unique.
- The catalog key is authoritative and must exactly match the `detector`
  declaration in every mapping file reached through that entry.
- Manifest paths must be relative to the root mapping directory.
- Absolute paths, any `..` component, and symlink-resolved paths outside the
  root mapping directory are rejected.
- The catalog must contain at least one detector.

## 2. Detector Run-Range Manifest

Each detector manifest selects one mapping file by run:

```text
# run_min run_max mapping_file
0 999 commissioning.map
1000 2499 production_2026.map
2500 max current.map
```

Each non-comment row contains exactly:

```text
run_min run_max mapping_file
```

Rules:

- Bounds are inclusive unsigned integers.
- `max` is accepted only as `run_max` and means no upper limit.
- `run_min` must not exceed `run_max`.
- Ranges for one detector must not overlap.
- Rows may appear in any order; they are sorted when loaded.
- Mapping-file paths are relative to the directory containing this manifest.
- Absolute paths, any `..` component, and symlink-resolved paths outside that
  detector directory are rejected.
- The manifest must contain at least one range.

### Boundaries and Gaps

For `1000 2499`, both run 1000 and run 2499 use that mapping. A range beginning
at 2500 starts immediately after it.

Detector-specific gaps are allowed:

```text
0 99 early.map
200 max current.map
```

That detector is absent for runs 100 through 199. Other detectors with active
ranges still translate normally. If no detector has a mapping for a requested
run, translation fails with `No detector translation table for run ...`.

Use gaps only deliberately and document why the detector should be absent.

## 3. Mapping File

A mapping file declares one detector, its identity fields, and its DAQ rows:

```text
detector MY_DETECTOR
fields layer element signal

# rocid slot channel layer element signal
1 3 0 1 1 0
1 3 1 1 2 0
```

The order is mandatory:

1. One `detector DETECTOR_KEY` declaration.
2. One `fields NAME [NAME ...]` declaration.
3. Zero or more channel rows.

Every channel row contains:

```text
rocid slot channel field_value [field_value ...]
```

Rules:

- `rocid`, `slot`, and an ordinary `channel` are unsigned 32-bit integers.
- Detector field values are signed 32-bit integers.
- Field names must be unique.
- Every row must provide exactly one value for every field, in declared order.
- A DAQ address `(rocid, slot, channel)` may appear only once in a mapping file.
- Active mappings from different detectors must not claim the same DAQ address.
- The `detector` value must exactly match the authoritative root catalog key.
- A mapping file must contain valid `detector` and `fields` declarations.

The translator decides which detector fields are required and what they mean.
Changing `fields` in configuration does not change a DigiHit schema. Coordinate
field changes with the translator developer.

### Board-Level Records and `none`

Some raw records identify a board but have no channel. Write `none` only in the
DAQ channel column:

```text
detector MY_DETECTOR
fields crate_group purpose signal

# rocid slot channel crate_group purpose signal
1 3 none 2 2 0
```

`none` represents `DAQAddress::UnspecifiedChannel`. It matches only a raw-hit
type whose `getDAQAddress()` returns that sentinel. It is not a wildcard and
does not match every channel on the board.

## Adding a Detector

1. Confirm that the module parser, raw-hit type, address overload, translator,
   DigiHit types, and diagnostic CSV output already exist.
2. Choose the exact detector key used by the C++ translator registration.
3. Create `detector_mappings/<detector>/manifest.map`.
4. Create the mapping files referenced by its run ranges.
5. Add the detector and relative manifest path to the root `manifest.map`.
6. Validate representative runs and every boundary before deployment.

## Changing an Existing Detector

Do not edit a mapping file used by historical runs when only future runs should
change. Add a new file and split the inclusive ranges:

```text
# Before
0 max current.map

# After a change beginning at run 2500
0 2499 current.map
2500 max run_2500.map
```

Check that there is neither an unintended gap nor an overlap at the boundary.

## Validate Before Deployment

Use a separate candidate directory so the installed configuration remains
untouched:

```text
/path/to/candidate/detector_mappings/
├── manifest.map
└── ...
```

Run representative EVIO data with the override:

```bash
jana \
  -Pplugins=evio_parser,detector_translation_dump \
  -PTRANSLATION:DIRECTORY=/path/to/candidate/detector_mappings \
  -Pdetector_translation_dump:OUTPUT_DIRECTORY=detector_translation_dump \
  /path/to/input.evio
```

Initialization reads the complete catalog, every detector manifest, and every
referenced mapping file. It therefore rejects invalid configuration before
normal event processing. There is currently no separate mapping-validator
command.

Inspect the generated detector CSVs and verify:

- known DAQ addresses map to the expected detector fields;
- raw digitized values are preserved;
- expected channels appear once and unexpected channels do not appear;
- board-level records use the intended `none` mapping;
- runs at `run_min`, `run_max`, and both sides of every boundary behave as
  intended; and
- a header-only CSV is investigated—it means no matching DigiHits were emitted.

For repository changes, also run:

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Physicists changing only mappings are not expected to add C++ unit tests. CSV
inspection with representative data is their primary pre-deployment check.

## Common Validation Failures

| Failure | What to check |
|---|---|
| Unable to read a catalog, manifest, or mapping | Referenced file exists and its path is relative to the owning directory |
| Catalog or manifest is empty | At least one non-comment row is present |
| Malformed row | Token count and integer values match the documented format |
| Duplicate detector | Root catalog contains the detector key only once |
| Overlapping or reversed range | Inclusive bounds are ordered and do not overlap |
| Detector does not match catalog | `detector` declaration exactly matches the root catalog key |
| Missing detector field | `fields` includes every name required by the detector identity helper |
| Duplicate DAQ address | No active detector mapping claims the same `(rocid, slot, channel)` |
| No table for a run | At least one detector has an active range for that run |
| Header-only diagnostic CSV | Input lacks that raw-hit type, address is unmapped, route is missing, or detector key does not match |

## Deployment Checklist

- [ ] Detector experts approved the DigiHit types and detector identity fields.
- [ ] Root catalog detector key matches C++ registration exactly.
- [ ] All paths are relative and contained in their owning directories.
- [ ] Run ranges are inclusive, non-overlapping, and intentionally cover or
      omit every relevant run.
- [ ] Every mapping row has the correct DAQ address and field order.
- [ ] No active mappings collide on a DAQ address.
- [ ] Candidate configuration initializes successfully.
- [ ] Boundary runs and known channels were verified in diagnostic CSVs.

