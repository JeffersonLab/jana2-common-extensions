# Docker environment

## Purpose

Run the normal superbuild and tests in a Linux container using the current
checkout.

## Expected behavior

- Compose mounts `WORKSPACE_DIR` read/write at `/workspace`, defaulting to
  the JCE checkout's parent folder. The folder must already exist and contain
  `jana2-common-extensions`; sibling repositories and data appear directly
  under `/workspace` without copying or image rebuilds.
- Commands start in `/workspace/jana2-common-extensions`; environment paths
  point to that checkout's `jce-stack/`. The configure command explicitly sets
  the install prefix to this path so cached defaults cannot retain an old prefix.
- The default command configures `build-super/` with RelWithDebInfo and tests
  enabled, builds and installs the pinned dependency stack into `jce-stack/`,
  then runs JCE tests. ROOT is excluded.
- Source edits, build output, and installed files persist on the host.
- Overriding the command with `bash` opens a shell instead of building.
- A build directory is used by either the container or the host system;
  switching environments requires fresh build and install directories.

## Failure behavior

Image creation and first-time dependency builds need network access. Configure,
build, and test failures return a nonzero exit code. A missing workspace
folder is rejected instead of being created. Moving the container mount layout
requires fresh build/install directories because cached paths are absolute.

## Verification

Run `docker compose -f docker/compose.yaml config -q`, then
`docker compose -f docker/compose.yaml run --build --rm dev`.
