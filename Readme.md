# SafePM: Memory Safety for Persistent Memory

### Installation

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

### Tests

The tests make sure SafePM provides ASan-level protection even for memory violations that happen within the PMDK-controlled persistent heap. To run the tests, use:  
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

### Utilities

Currently, there is only one utility program: `pool_overhead`. It gets built together with the tests. Set the environment variable `POOL_SIZE` and run it to get an estimate of the user-available area (excluding e.g. the PMDK metadata and the SafePM persistent shadow memory).

### Benchmarks

For various performance benchmark drivers and scripts, see the folder `benchmarks`.

### Plot utilities

Scripts used for creating the plots in the paper.

### ``vscode_init.sh`

Builds the repo with `bear` to enable the vscode extension `clangd` to do auto-completion, jumps etc. My experience with Intellisense has been very poor.

### Artifact evaluation

This folder contains the scripts required to reproduce the results and the figures from our Paper.
For more information please look at our [Artifact Evaluation folder](https://github.com/mstniy/safepm/tree/master/artifact_evaluation).