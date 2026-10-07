# Configuration Layers

## Purpose

Append default plugins from multiple configuration directories without copying
core defaults. Bank mapping and filter files use the same directory layers.
Translation catalogs retain their existing provider registration and overrides.

## Expected Behavior

- Core defaults use `${JCE_HOME}/config/default_plugins.db`.
- `-PJCE:CORE_CONFIG_DIR` overrides the core directory; `-PDEFAULT_PLUGINS:FILE`
  overrides the core plugin file and takes precedence. No core environment override.
- Optional `default_plugins.db` files from colon-separated `JCE_CONFIG_DIR`
  directories append left to right, even with an explicit core override.
- CLI `-Pplugins` appends last. Duplicate names retain first occurrence;
  `evio_parser` is always first.
- Files accept comma-separated names, whitespace, and full-line `#` comments.
- Empty directory-list entries are ignored. Missing files in appended directories
  are optional. Missing directories warn and are skipped.
- Missing or empty core files use `evio_parser,evio_common_modules,detector_translation`; missing files
  warn. Unreadable existing files fail before JANA starts.
- Warnings are yellow on a terminal; redirected diagnostics contain no ANSI codes.
- `jce.csh` forwards to `jce.sh`, sharing the implementation and preserving arguments.
- Direct JANA invocation does not read default plugin files.
- Plugin paths still use `JANA_PLUGIN_PATH` or `-Pjana:plugin_path`.
- Mapping and filter services resolve their core files at initialization, using
  `JCE:CORE_CONFIG_DIR` or the compiled installation config directory.
- `DEFAULT_PLUGINS:FILE`, `BANKMAP:FILE`, and `FILTER:FILE` each replace
  only their core file. Directory additions still apply to all three functions.
- To use only explicit mapping/filter files, clear or unset `JCE_CONFIG_DIR`.
  This also removes directory plugin additions; CLI plugins still append.
  Appended filters broaden the allow-list even with `FILTER:FILE` set.
- Required core/function files must exist and be readable; missing appended
  files are optional, and missing appended directories warn and are skipped.
- Mapping layers are keyed by bank ID. Equal routes deduplicate silently;
  a changed route in a later file overrides with a warning containing both
  module IDs and file/line locations. Conflicting routes within one file fail.
- Empty mapping files are allowed. Malformed rows, extra columns, negative IDs,
  and bank tags outside 0..65535 fail with file/line diagnostics.
- Filter layers union exact `(rocid, slot, module, bank)` rows, silently ignoring
  duplicates. Malformed rows fail with file/line diagnostics. Filtering remains
  disabled by default; disabled filtering performs no file resolution.
- Filter enforcement remains ROC/bank based. Slot/module columns remain
  informational. An empty merged filter retains the existing allow-all behavior.
- Programmatic `addRoute` duplicates and registration after first lookup remain
  errors; file-layer overrides do not weaken the plugin registration contract.
- Both direct JANA invocation and wrappers support mapping/filter layering;
  the wrapper forwards `JCE:CORE_CONFIG_DIR` to JANA.

## Key Components

- `scripts/jce.sh`, `scripts/jce.csh`
- `templates/jce_config_paths.h.in`
- `src/plugins/evio_parser/services/JEventService_BankToModuleMap.h`
- `src/plugins/evio_parser/services/JEventService_FilterDB.cc`

## Verification

Run `python3 scripts/tests/test_plugin_layers.py`; it exercises production wrappers
with a fake JANA executable. It checks ordering, deduplication, core overrides,
missing paths, fallback, and arguments containing spaces. tcsh is tested when
available.

Run CTest `config_layers_tests` and `bank_to_module_map_tests` to check ordered
routes, warnings, strict parsing, freeze behavior, filter union/deduplication,
and explicit replacement files. Tests link the production `services` target.
