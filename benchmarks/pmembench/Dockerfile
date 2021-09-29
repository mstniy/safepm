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
    libtbb-dev

# Add the shared library path
ENV LD_LIBRARY_PATH /usr/local/lib:$LD_LIBRARY_PATH

# Install PMDK
WORKDIR /
RUN git clone https://github.com/mstniy/pmdk.git /pmdk_safepm -b pmasan-1.9.2
WORKDIR /pmdk_safepm
RUN make -j7

WORKDIR /
RUN git clone https://github.com/mstniy/pmdk.git /pmdk_safepm_partial_coverage -b pmasan-1.9.2-partial_coverage
WORKDIR /pmdk_safepm_partial_coverage
RUN make -j7

RUN git clone https://github.com/mstniy/pmdk.git /pmdk_wrappers_only -b pmasan-1.9.2
WORKDIR /pmdk_wrappers_only
RUN git apply disable_asan.patch
RUN make -j7

RUN git clone https://github.com/mstniy/pmdk.git /pmdk_asan_only -b asan_only-1.9.2
WORKDIR /pmdk_asan_only
RUN make -j7

RUN git clone https://github.com/mstniy/pmdk.git /pmdk_vanilla -b vanilla-1.9.2
WORKDIR /pmdk_vanilla
RUN make -j7

COPY . /safepm-bench-scripts
WORKDIR /pmdk_safepm
RUN git apply /safepm-bench-scripts/pmembench.patch
WORKDIR /pmdk_safepm_partial_coverage
RUN git apply /safepm-bench-scripts/pmembench.patch
RUN git apply /safepm-bench-scripts/pmembench_partial_cov.patch
WORKDIR /pmdk_wrappers_only
RUN git apply /safepm-bench-scripts/pmembench.patch
WORKDIR /pmdk_asan_only
RUN git apply /safepm-bench-scripts/pmembench.patch
WORKDIR /pmdk_vanilla
RUN git apply /safepm-bench-scripts/pmembench.patch
RUN git apply /safepm-bench-scripts/pmembench_partial_cov.patch
WORKDIR /safepm-bench-scripts


# Necessary to mount the device and use full potential
#ENTRYPOINT mkdir /mnt/pmem0 && mount -o dax /dev/pmem0 /mnt/pmem0  && /bin/bash
