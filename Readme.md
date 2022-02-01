# SafePM: Memory Safety for Persistent Memory

Memory safety violation is a major root cause of reliability and security issues in software systems. Byte-addressable persistent memory (PM), just like its volatile counterpart, is also susceptible to memory safety violations, e.g., object overflows and use-after-free bugs. While there is a couple of decades of work in ensuring memory safety for programs based on volatile memory, there exists no memory safety mechanism for PM — the existing approaches are incompatible since the PM programming model introduces a persistent pointer representation for persistent memory objects and allocators, where it is imperative to design a crash consistent safety mechanism. We introduce SafePM, a memory safety mechanism that transparently and comprehensively detects both spatial and temporal memory safety violations for PM-based applications. SafePM’s design builds on a shadow memory approach, and augments it with crash consistent data structures and system operations to ensure memory safety even across system reboots and crashes. We implement SafePM based on the [AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html) compiler pass, and integrate it with the PM development kit ([PMDK](https://github.com/pmem/pmdk)) runtime library. We evaluate SafePM across three dimensions: overheads, effectiveness, and crash consistency. SafePM overall incurs reasonable overheads while providing comprehensive memory safety, and has uncovered real-world bugs in the widely-used PMDK library.

## Artifact evaluation

SafePM was published in Eurosys 2022.
The artifact evaluation folder contains the required scripts to reproduce the results and the figures from our Paper.
For more information please look at our [Artifact Evaluation folder](https://github.com/mstniy/safepm/tree/master/artifact_evaluation).

## Installation

Initially clone the repository:
```
git clone git@github.com:mstniy/safepm.git
```

This repository includes the SafePM's PMDK fork as a submodule, so make sure you initialize it first:  
```
git submodule update --init
```

By default, the submodule contains the wrappers of SafePM and includes the ASan flags for the compilation of the test applications (persistent indices and benchmarks) shipped with pmdk. 
It is recommended that you uninstall `libpmem`/`libpmemobj`, in case it conflicts with the one this repo builds from source.
The dependencies of SafePM are the same with those of [PMDK](https://github.com/pmem/pmdk#dependencies).
To install the modified SafePM's libpmemobj library system-wide:
```
cd pmdk
make
make install
```

## SafePM source code organization
### PMDK submodule
This submodule is our modified PMDK fork. Our branches of interest are the following:
1. `pmasan-1.9.2`: Modified PMDK that includes the SafePM's wrappers (referred as `SafePM` in the paper). This branch also contains a patch to disable ASan. When it is applied, this PMDK variant will only include the SafePM's wrappers but will omit the ASan's instrumentation (referred as `SafePM w/o Asan` in the paper).
2. `asan_only-1.9.2`: Configured PMDK to be builT with ASan (referred as `ASan` in the paper).
3. `vanilla-1.9.2`: Unmodified PMDK (referred as `native` in the paper).

### Benchmarks
This folder contains the benchmark drivers and scripts used for the evaluation of SafePM. For more details and configuration information, see the folder [`benchmarks`](https://github.com/mstniy/safepm/tree/master/benchmarks).
### Utils
Currently, there is only one utility program: `pool_overhead`. It gets built together with the tests. Set the environment variable `POOL_SIZE` and run it to get an estimate of the user-available area (excluding e.g. the PMDK metadata and the SafePM persistent shadow memory).
### Tests
This folder contains various unit tests for SafePM. The tests make sure SafePM provides ASan-level protection even for memory violations that happen within the PMDK-controlled persistent heap. To run the tests, use:  
```
./test.sh
```
This will build SafePM and the tests, and run the tests. Most tests contain deliberate memory violations, and are meant to crash. `test.sh` makes sure that the tests that are meant to crash do indeed crash, in the way they are supposed to. Note that the last test, `zalloc`, takes some time to complete.

To better understand how SafePM provides extra memory safety over PMDK, when used with ASan, you can checkout to the upstream branch within the pmdk submodule, and run the tests again:  
```
cd pmdk
git checkout 1.9.2
make clobber
cd ..
./test.sh
```
### Plot utils
Scripts used for parsing and analysing the benchmark results, and creating the plots in the paper.
### vscode_init.sh
Builds the repo with `bear` to enable the vscode extension `clangd` to do auto-completion, jumps etc. My experience with Intellisense has been very poor.