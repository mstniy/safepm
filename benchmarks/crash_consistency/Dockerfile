FROM ubuntu:20.04
ARG DEBIAN_FRONTEND=noninteractive

# Install dependencies for PMDK
RUN apt-get update && apt-get -qq install -y \
    autoconf \
    automake \
    pkg-config \
    libndctl-dev \
    libdaxctl-dev \
    libglib2.0-dev \
    libncurses5-dev \
    libfuse-dev \
    libuv1-dev \
    pandoc \
    libfabric-dev \
    # Dependencies to compile PMDK
    git \
    build-essential \
    # Install dependencies for memkind
    libnuma-dev \
    libtool \
    numactl \
    unzip \
    # Install dependencies for libpmemobj-cpp
    cmake \
    # Install dependencies for pmemkv
    libtbb-dev\
    # Install dependencies for valgrind
    libc6-dbg

# Add the shared library path
ENV LD_LIBRARY_PATH /usr/local/lib:$LD_LIBRARY_PATH

# Install PMDK
RUN git clone https://github.com/mstniy/pmdk.git /pmdk_wrappers_only -b pmasan-1.9.2
WORKDIR /pmdk_wrappers_only
RUN git apply disable_asan.patch
RUN make -j7

WORKDIR /
RUN git clone https://github.com/pmem/valgrind.git
WORKDIR /valgrind
RUN ./autogen.sh
RUN ./configure
RUN make -j7
RUN make -j7 install

COPY . /safepm-bench-scripts
WORKDIR /pmdk_wrappers_only/src/benchmarks
RUN /safepm-bench-scripts/change_pmembench_file_path.sh
WORKDIR /safepm-bench-scripts


# Necessary to mount the device and use full potential
#ENTRYPOINT mkdir /mnt/pmem0 && mount -o dax /dev/pmem0 /mnt/pmem0  && /bin/bash
