# Detector Translation

## Purpose

Detector translation maps a DAQ address `(rocid, slot, channel)` to a detector
name and detector-specific integer channel fields. It does not decode hardware,
calibrate measurements, or provide detector geometry.

## Main Flow

1. Load one detector mapping file into a `TranslationTable`.
2. Reject malformed input and duplicate DAQ addresses.
3. Perform immutable lookups by DAQ address after loading.

## Expected Behavior

- Detector-specific fields remain generic name/value pairs.
- Unmapped and deliberately excluded reference channels return `nullptr`.
- A mapping file applies one detector name to every channel it contains.
- Detector mapping files are installed under
  `config/<namespace>/evio_parser/detector_mappings/`.
- Mapping changes are expected only between runs.

## Failure Behavior

Loading throws `std::runtime_error` when the file cannot be read, declarations
or channel rows are invalid, an integer is out of range, or a DAQ address is
duplicated.

## Key Components

- `config/evio_parser/detector_mappings/`
- `src/plugins/evio_parser/core/detector_mapping_objects/`

## Verification

The `translation_table_tests` CTest loads the dependency-free demo HMS mapping
and verifies its known lookup, duplicate insertion rejection, and an
unknown-address lookup.
