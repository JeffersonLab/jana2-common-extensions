# JCE Superbuild

## Purpose

The in-repository superbuild provides one entry point for downloading, building,
and installing compatible JANA2, EVIO, and JCE versions into a shared prefix.
The normal JCE build remains available for developers and managed installations
that provide their own dependencies.

## Main Flow

1. Configure `superbuild/` with an installation prefix.
2. Build the generated project.
3. The superbuild downloads and installs JANA2 `v2026.03.01`, then EVIO
   `v6.1.2`, and finally builds this JCE checkout against both installations.
4. Downstream experiments use the resulting prefix through
   `CMAKE_PREFIX_PATH`.

## Expected Behavior

- All three projects install into the selected `CMAKE_INSTALL_PREFIX`.
- Git tags are pinned by default and can be overridden with
  `JCE_JANA_GIT_TAG` or `JCE_EVIO_GIT_TAG`.
- JANA examples and tests, EVIO examples, and JCE tests are disabled to keep
  the dependency bootstrap focused on installable runtime components.
- The ROOT-based `evio_processor` remains disabled by default. Set
  `JCE_SUPERBUILD_EVIO_PROCESSOR=ON` and make ROOT discoverable through the
  outer `CMAKE_PREFIX_PATH` to include it.
- EVIO is patched only in the superbuild's downloaded source tree so that its
  CMake build honors the shared installation prefix.

## Failure Behavior

- Configuration fails when Git is unavailable.
- Download or build failures stop the dependent projects; JCE is not built
  against a partial dependency installation.
- System dependencies required by EVIO, including Boost and LZ4, must already
  be discoverable by CMake.

## Key Components

- `superbuild/CMakeLists.txt`
- `superbuild/patches/evio-respect-install-prefix.patch`
- Root `CMakeLists.txt` for the normal JCE build

## Verification

Configure without building to validate the outer project:

```bash
cmake -S superbuild -B build-super \
  -DCMAKE_INSTALL_PREFIX=/path/to/jce-stack
```

Build the complete stack when network access and system dependencies are
available:

```bash
cmake --build build-super --parallel
```
