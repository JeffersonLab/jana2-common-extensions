# Docker environment

From the repository root, build the image and run the superbuild:

```sh
docker compose -f docker/compose.yaml run --build --rm dev
```

This mounts the repository's parent folder read/write at `/workspace`, so JCE,
`hallc_recon_jce`, `HallA-compton-jana2`, and any data or future repositories in
that folder are available directly under `/workspace`. Nothing is copied into
the image. Commands start in `/workspace/jana2-common-extensions`.

Choose another common folder with `WORKSPACE_DIR` (it must contain a
`jana2-common-extensions` checkout):

```sh
WORKSPACE_DIR=/absolute/path/to/common-folder \
  docker compose -f docker/compose.yaml run --build --rm dev
```

The default command configures JCE's `build-super/` with
`RelWithDebInfo` and tests enabled, then builds and installs JANA2, EVIO, and
JCE into `jce-stack/` and runs the JCE tests. Repeat the command after edits.
The install prefix is explicitly set to
`/workspace/jana2-common-extensions/jce-stack`.
Dependencies use the existing superbuild's pinned versions. ROOT is disabled.
The first run needs internet access to download dependencies.

To run commands yourself in the same environment:

```sh
docker compose -f docker/compose.yaml run --rm dev bash
```

The image includes GCC, CMake, Boost, LZ4, PugiXML, GDB, GNU time, and Valgrind.
`JCE_HOME`, `JANA_HOME`, library and plugin paths point at the installed stack.
Builds and installations remain in the checkout after the container exits.
Use either the container or the system for a given build directory; switching
requires a fresh build directory and installation.

For single-worker timing, put input in the common folder’s `data/` and run `jana` directly:

```sh
docker compose -f docker/compose.yaml run --rm \
  dev /usr/bin/time -v jana -Pplugins=evio_parser,evio_common_modules \
    -Pnthreads=1 /workspace/data/run.evio
```

Translation additionally needs the downstream translator plugin and mappings,
compiled/configured inside this Linux environment. Use Valgrind Callgrind for
instruction profiling; measure throughput separately without instrumentation.
Docker Desktop uses a VM, so verify final gains on the remote machine.

To work on a sibling repository, open the shell above and use
`cd /workspace/hallc_recon_jce` or `cd /workspace/HallA-compton-jana2`.
`CMAKE_PREFIX_PATH` points at JCE's installed stack. Follow each repository's
build/test instructions; the default command only builds and tests JCE.

Existing build caches from the previous `/workspace` layout contain old
absolute paths. Preserve or remove those build/install directories and start
fresh before using the new layout.

See the [environment contract](../docs/docker-environment.md).

## Separate ROOT environment (amd64)

Use the standalone Compose file; do not combine it with `compose.yaml`:

```sh
docker compose -f docker/compose.root.yaml run --build --rm dev-root
```

`Dockerfile.root-dev` uses the official `rootproject/root:6.34.00-ubuntu24.04`
image. The service explicitly selects `linux/amd64`; Docker Desktop on ARM
uses emulation. Its separate Compose project is `jce-root`.
The default command enables `evio_processor`, builds and installs the pinned
JANA2/EVIO/JCE dependencies, and runs JCE tests. It uses only
`build-root-super/` and `jce-root-stack/` for build and install output.
The original Dockerfile, Compose service, `build-super/`, and `jce-stack/`
are unchanged. Both environments share the same source checkout.
ROOT stays in the image at `/opt/root`; it is passed explicitly to CMake.
JANA2 keeps the existing superbuild's `USE_ROOT=OFF`; JCE links ROOT directly
for `evio_processor`. This environment does not change the processor's
current sequential writing behavior.

Open a shell after building:

```sh
docker compose -f docker/compose.root.yaml run --rm dev-root bash
root-config --version
jana -Pplugins=evio_parser,evio_common_modules,evio_processor \
  -Pnthreads=4 /workspace/data/run.evio
```

The same `WORKSPACE_DIR` override works here. Environment paths select
`jce-root-stack/` and `/opt/root`. Use separate build/install directories
for downstream repositories built against this ROOT stack too. Emulation
can slow builds and is unsuitable for final throughput measurements.

See the [ROOT environment contract](../docs/docker-root-environment.md).
