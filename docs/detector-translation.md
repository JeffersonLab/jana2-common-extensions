# Detector Translation

## Purpose

Detector translation maps a DAQ address `(rocid, slot, channel)` to a detector
name and detector-specific integer channel fields. It does not decode hardware,
calibrate measurements, or provide detector geometry.

## Main Flow

1. `TranslationTableService` loads every `*.map` file from the configured
   detector mapping directory.
2. The service publishes it as `shared_ptr<const TranslationTable>`.
3. Factories acquire the table for a run and perform immutable DAQ-address
   lookups while processing events.

## Expected Behavior

- Detector-specific fields remain generic name/value pairs.
- Unmapped and deliberately excluded reference channels return `nullptr`.
- A mapping file applies one detector name to every channel it contains.
- Detector mapping files are installed under
  `config/<namespace>/evio_parser/detector_mappings/`.
- `TRANSLATION:DIRECTORY` overrides the mapping directory.
- Mapping files are loaded in deterministic filename order into one table.
- Until run-range selection is added, every run receives the same table
  instance.
- Mapping changes are expected only between runs.

## Failure Behavior

Loading throws when the directory cannot be read, contains no `*.map` files, a
file cannot be read, declarations or channel rows are invalid, an integer is
out of range, or a DAQ address is duplicated within or across files.

## Key Components

- `config/evio_parser/detector_mappings/`
- `src/plugins/evio_parser/core/detector_mapping_objects/`
- `src/plugins/evio_parser/services/TranslationTableService.*`

## Verification

The `translation_table_tests` CTest loads the dependency-free demo HMS mapping
and verifies its known lookup, duplicate insertion rejection, and an
unknown-address lookup.

The `translation_table_service_tests` CTest verifies that the service loads
multiple detector files into one immutable table and currently reuses it for
different run numbers.
