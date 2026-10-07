FROM ubuntu:24.04

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential ca-certificates cmake git libboost-all-dev liblz4-dev libpugixml-dev \
    gdb time valgrind \
    && rm -rf /var/lib/apt/lists/*

ENV JCE_HOME=/workspace/jana2-common-extensions/jce-stack \
    JANA_HOME=/workspace/jana2-common-extensions/jce-stack \
    CMAKE_PREFIX_PATH=/workspace/jana2-common-extensions/jce-stack \
    JANA_PLUGIN_PATH=/workspace/jana2-common-extensions/jce-stack/lib/plugins \
    LD_LIBRARY_PATH=/workspace/jana2-common-extensions/jce-stack/lib \
    PATH=/workspace/jana2-common-extensions/jce-stack/bin:${PATH}

WORKDIR /workspace/jana2-common-extensions
CMD ["bash"]
