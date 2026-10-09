# ROOT Docker environment

## Purpose

Provide an isolated amd64 Linux environment for developing ROOT output in
`evio_processor`, including on ARM hosts through Docker emulation.

## Expected behavior

- `docker/compose.root.yaml` runs `dev-root` in Compose project `jce-root`,
  using `docker/Dockerfile.root-dev` and ROOT 6.34.00 on Ubuntu 24.04.
- The default command enables `JCE_SUPERBUILD_EVIO_PROCESSOR` and JCE tests,
  builds the existing pinned dependency versions, installs the stack, and
  runs the `check` target.
- Build output is confined to `build-root-super/`; installation is confined
  to `jce-root-stack/`. Both are ignored by Git. The standard Docker files,
  `build-super/`, and `jce-stack/` are untouched.
- ROOT is supplied by the image under `/opt/root`. JCE finds ROOT through
  `CMAKE_PREFIX_PATH`; JANA2 retains the standard superbuild configuration.
- The common workspace is mounted read/write exactly as in the standard
  environment; both setups share source and runtime output paths.
- Overriding the command with `bash` opens a shell with the ROOT stack's
  executable, plugin, and library paths.
- This enables development of parallel writing; it does not implement it.

## Failure behavior

Image pulls and initial dependency downloads require network access. A missing
workspace is rejected. Configure, build, or test failures exit nonzero. The host
must support amd64 containers or emulation. Keep host and container build
caches separate; downstream builds also need their own ROOT-stack prefixes.

## Verification

```sh
docker compose -f docker/compose.root.yaml config -q
docker compose -f docker/compose.root.yaml run --build --rm dev-root
docker compose -f docker/compose.root.yaml run --rm dev-root root-config --features
```

## Current build limitation

The existing `evio_processor` includes `faV3comptonAccumulatorHit.h` and
`faV3comptonHit.h`, which are absent from this repository and its exported
common hit types. The default build currently fails at the first missing
header. Those inputs need an explicit downstream dependency or a separate
processor change before the complete stack and tests can succeed.

The prebuilt ROOT image uses C++17 while JCE uses C++20; ROOT emits a standard
mismatch warning. A matching C++20 ROOT build would remove this warning.
