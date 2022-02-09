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

# Install PMDK
WORKDIR /
RUN git clone https://github.com/mstniy/pmdk.git /pmdk_safepm -b pmasan-1.9.2
WORKDIR /pmdk_safepm
RUN make -j7
RUN mkdir -p /safepm
RUN make -j7 install prefix=/safepm
ENV C_INCLUDE_PATH /safepm/include
ENV LIBRARY_PATH /safepm/lib
COPY . /recovery-safepm
WORKDIR /recovery-safepm
RUN mkdir -p build
RUN make clean && make

RUN git clone https://github.com/mstniy/pmdk.git /pmdk_wrappers_only -b pmasan-1.9.2
WORKDIR /pmdk_wrappers_only
RUN git apply disable_asan.patch
RUN make -j7
RUN mkdir -p /wrappers
RUN make -j7 install prefix=/wrappers
ENV C_INCLUDE_PATH /wrappers/include
ENV LIBRARY_PATH /wrappers/lib
COPY . /recovery-wrappers
WORKDIR /recovery-wrappers
RUN mkdir -p build
RUN make clean && make NO_ASAN=1

RUN git clone https://github.com/mstniy/pmdk.git /pmdk_asan_only -b asan_only-1.9.2
WORKDIR /pmdk_asan_only
RUN make -j7
RUN mkdir -p /asan
RUN make -j7 install prefix=/asan
ENV C_INCLUDE_PATH /asan/include
ENV LIBRARY_PATH /asan/lib
COPY . /recovery-asan
WORKDIR /recovery-asan
RUN mkdir -p build
RUN make clean && make

RUN git clone https://github.com/mstniy/pmdk.git /pmdk_vanilla -b vanilla-1.9.2
WORKDIR /pmdk_vanilla
RUN make -j7
RUN mkdir -p /vanilla
RUN make -j7 install prefix=/vanilla
ENV C_INCLUDE_PATH /vanilla/include
ENV LIBRARY_PATH /vanilla/lib
COPY . /recovery-vanilla
WORKDIR /recovery-vanilla
RUN mkdir -p build
RUN make clean && make NO_ASAN=1

COPY inner_run.sh /
WORKDIR /

# Necessary to mount the device and use full potential
#ENTRYPOINT mkdir /mnt/pmem0 && mount -o dax /dev/pmem0 /mnt/pmem0  && /bin/bash
