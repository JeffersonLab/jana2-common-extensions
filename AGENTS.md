# Repository Working Agreement

## Before Making Changes

- Work from the `jana2-common-extensions` repository root.
- Read this file and the README nearest to the code being changed.
- Run `git status --short` and inspect the relevant implementation, build files,
  documentation, tests, and current diff.
- Preserve unrelated user changes. Never discard or overwrite them.
- Keep each logical step small and reviewable. If the previous step is still
  uncommitted, finish or revise that checkpoint before starting another.

## Scope and Design

- Prefer the smallest viable change. Reuse existing code, C++ standard-library
  facilities, JANA2 facilities, and already configured dependencies.
- Avoid speculative abstractions, unrelated cleanup, and new dependencies.
- Keep `evio_parser/core` hardware- and experiment-agnostic.
- Keep hardware decoding in `evio_parser/module_parsers/<module>/`.
- Keep shared JANA resources in `evio_parser/services/`.
- Keep physicist-editable runtime data under the repository-level `config/`
  directory; keep the C++ implementation under `src/`.
- Translation, calibration, and geometry are separate responsibilities.

## JANA2 and Parallelism

- Follow the existing ExpertMode source/unfolder pipeline documented in
  `src/plugins/evio_parser/README.md`.
- Perform per-event work with local state. Shared services must not expose
  mutable state on the event-processing hot path.
- Publish run-dependent shared data as immutable objects, preferably through
  `std::shared_ptr<const T>`.
- Protect initialization or cache population when needed, then keep normal
  read-only lookups lock-free.
- Do not add detector translation directly to hardware word decoders unless the
  established pipeline explicitly requires it.

## Project Conventions

- Use C++20 and the existing `.h`/`.cc` naming and formatting style.
- Build components as focused CMake targets and link existing targets instead
  of recompiling their production source files in consumers or tests.
- Follow the existing plugin layout: local `CMakeLists.txt`, `InitPlugin.cc`,
  and registration through the nearest parent `CMakeLists.txt`.
- Name concrete JANA components and their primary `.h`/`.cc` files with the
  established base-role prefix: `JEventService_<Role>`,
  `JEventProcessor_<Role>`, `JEventSource_<Role>`, and
  `JEventUnfolder_<Role>`. Keep non-component data and utility types, such as
  `TranslationTable`, free of JANA component prefixes.
- Organize detector translation routes under
  `detector_translators/<Detector>/<RawHitFamily>/`. Name conversion files
  `<RawHitFamily>Translator.h/.cc`, keep published DigiHit headers in the
  route's `data_objects/` subdirectory, and name them
  `<RawHitFamily>DigiHit.h`. Keep exported C++ DigiHit type names globally
  detector-qualified while public headers are installed flat. Each route
  directory owns its focused tests and local `CMakeLists.txt`; parent CMake
  files only add and aggregate route targets, include directories, and public
  headers. A detector parent exports `<DETECTOR>_TRANSLATOR_LIBS`,
  `<DETECTOR>_TRANSLATOR_INCLUDE_DIRS`, and `<DETECTOR>_TRANSLATOR_HEADERS`;
  the shared `detector_translators/CMakeLists.txt` aggregates only those
  detector-level variables and never names individual route targets.
- Export public hit/data headers through the existing
  `evio_parser_data_types` mechanism when downstream plugins need them.
- Resolve installed configuration through `jce_config_path()` and preserve the
  `JCE_CONFIG_DIR` override behavior.
- Use JANA logging macros and terminate each log statement with `LOG_END`.
- Update the nearest README when architecture, configuration, extension steps,
  or observable behavior changes.

## Verification

- Start with the narrowest relevant runnable check.
- A small test is appropriate for new non-trivial logic even though older code
  may not have tests. Keep it focused and use CTest without adding a test
  framework unless one becomes necessary.
- Tests must link the production library or target; do not compile a second copy
  of the implementation being tested.
- Run `git diff --check` and `git status --short` after each logical step.
- Run the full configured build or broader tests only when dependencies and
  scope make that proportional. Report checks that could not be run.

## Documentation and Handoff

- For a major feature or independently meaningful component, create or update a
  concise behavioral contract under `docs/<feature-name>.md`. Do not create one
  for trivial helpers.
- Do not commit unless the user explicitly asks.
- At each checkpoint, summarize behavior, verification, remaining risk, exact
  `git add` paths, and one Conventional Commit subject.
