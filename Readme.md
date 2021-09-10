Demo repo for the pmasan project.

Includes the pmasan fork as a submodule, so make sure you initialize it first:  
```
git submodule update --init
```

I would recommend that you uninstall `libpmem`/`libpmemobj`, in case it conflicts with the one this repo builds from the source.

### Tests

The tests make sure pmasan provides ASan-level protection even for memory violations that happen within the PMDK-controlled persistent heap. To run the tests, use:  
```
./test.sh
```

This will build pmasan and the tests, and run the tests. Note that most tests contain deliberate memory violations, and are meant to crash. `test.sh` makes sure that the tests that are meant to crash do indeed crash, in the way they are supposed to. Note that the last test, `zalloc`, takes some time to complete.

To better understand how pmasan provides extra memory safety over PMDK, when used with ASan, you can checkout to the upstream branch within the pmdk submodule, and run the tests again:  
```
cd pmdk
git checkout 1.9.2
make clobber
cd ..
./test.sh
```


### Utilities

Currently, there is only one utility program: `pool_overhead`. It gets built together with the tests. Set the environment variable `POOL_SIZE` and run it to get an estimate of the user-available area (excluding e.g. the PMDK metadata and the pmasan persistent shadow memory).

### Benchmarks

For various performance benchmark drivers and scripts, see the folder `benchmarks`.

### ``vscode_init.sh`

Builds the repo with `bear` to enable the vscode extension `clangd` to do auto-completion, jumps etc. My experience with Intellisense has been very poor.