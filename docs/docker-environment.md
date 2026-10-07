# Docker environment

## Purpose

Run the normal superbuild and tests in a Linux container using the current
checkout.

## Expected behavior

- Compose mounts the checkout read/write at `/workspace`.
- The default command configures `build-super/` with RelWithDebInfo and tests
  enabled, builds and installs the pinned dependency stack into `jce-stack/`,
  then runs JCE tests. ROOT is excluded.
- Source edits, build output, and installed files persist on the host.
- Overriding the command with `bash` opens a shell instead of building.
- A build directory is used by either the container or the host system;
  switching environments requires fresh build and install directories.

## Failure behavior

Image creation and first-time dependency builds need network access. Configure,
build, and test failures return a nonzero exit code.

## Verification

Run `docker compose -f docker/compose.yaml config -q`, then
`docker compose -f docker/compose.yaml run --build --rm dev`.
