# Configuration Layers

## Purpose

Append default plugins from multiple configuration directories without copying
core defaults. This checkpoint covers wrappers only; mapping, filtering, and
translation loaders remain unchanged.

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
- Existing C++ config loaders do not yet support the directory list. Multi-root
  runs must pass `BANKMAP:FILE` and `FILTER:FILE` (when filtering is enabled).

## Key Components

- `scripts/jce.sh`, `scripts/jce.csh`

## Verification

Run `python3 scripts/tests/test_plugin_layers.py`; it exercises production wrappers
with a fake JANA executable. It checks ordering, deduplication, core overrides,
missing paths, fallback, and arguments containing spaces. tcsh is tested when
available.
