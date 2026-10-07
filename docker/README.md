# Docker environment

From the repository root, build the image and run the superbuild:

```sh
docker compose -f docker/compose.yaml run --build --rm dev
```

This mounts the current checkout read/write, configures `build-super/` with
`RelWithDebInfo` and tests enabled, then builds and installs JANA2, EVIO, and
JCE into `jce-stack/` and runs the JCE tests. Repeat the command after edits.
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

For single-worker timing, mount your input and run `jana` directly:

```sh
docker compose -f docker/compose.yaml run --rm \
  -v /absolute/path/to/evio-data:/data:ro \
  dev /usr/bin/time -v jana -Pplugins=evio_parser,evio_common_modules \
    -Pnthreads=1 /data/run.evio
```

Translation additionally needs the downstream translator plugin and mappings,
compiled/configured inside this Linux environment. Use Valgrind Callgrind for
instruction profiling; measure throughput separately without instrumentation.
Docker Desktop uses a VM, so verify final gains on the remote machine.

See the [environment contract](../docs/docker-environment.md).
