# Artifact evaluation

The first step is to get the source code for SafePM:
```
git clone git@github.com:mstniy/safepm.git
```

### Benchmarks run

For convience we created an evaluation script (run.sh) that will first build SafePM and then run all evaluation experiments from the paper.
It gets as input a `path-to-pm` which should be a file system directory that is mounted with the `dax` mount option ([hint](https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/7/html/storage_administration_guide/configuring-persistent-memory-for-file-system-direct-access-dax)).
Usage:
``` 
./run.sh <path-to-pm>
```
`path-to-pm` is the directory where benchmarks will create the persistent memory pools.

### Figures

After the benchmark run is over and you have collected all the results you can reproduce each of the paper figures by running the respective script:
```
./figure_x.sh
```
This script will create a `plots` directory and will place the respective plot there, both in .png and in .pdf format.

To obtain all the plots, we provide the `safepm_figures.sh`. 
Simply run:
```
./safepm_figures.sh
```

To see the RIPE benchmark results in a human readable format, we provide the `table_4.sh` script.
This script will print the results in the standard output as a table, similar to the one in the paper.
Simply run:
```
./table_4.sh
```

### Reproduce bugs

To reproduce the bugs found with SafePM ([[1]](https://github.com/pmem/pmdk/issues/5333),[[2]](https://github.com/pmem/pmdk/issues/5334)), run:
``` 
./reproduce_bugs.sh <path-to-pm>
```
`path-to-pm` is the directory where benchmarks will create the persistent memory pools.

This execution will show the bugs in the `standard output` with the appropriate error messages (error message report from ASan for the first bug and a failed assertion for an invalid free for the second one).

### Hardware configuration

To reproduce the results from the paper, the machine should preferably be equipped with a physical persistent memory module (e.g., Intel Optane DC) with at least 64 GB available space. The persistent memory module should be mounted using a DAX-enabled file system (e.g. EXT4-DAX), as also described in A.3. Additionally, we recommend running the \href{https://github.com/pmem/pmemkv}{pmemkv} experiments on a machine with at least 24 cores, as they are configured to run with up to 24 threads. The testbed, used to conduct our experiments, is equipped with Intel(R) Xeon(R) Gold 6212U CPU with 24 cores and 768GB (6 channels×128 GB/DIMM) Intel Optane DC DIMMs.

### Software dependecies

We require the following software configuration to reproduce our experimental results:
1. Linux (tested in Ubuntu 20.04.3 LTS with kernel version 5.4.0)
2. Docker (tested with Docker version 20.10.7): Each experiment comes with its pre-configured Dockerfile. We provide scripts that automatically build the images containing the required software dependencies.
3. gcc and cmake (tested with gcc 9.3.0 and cmake 3.16.3): To build our unit tests.
4. Python 3.7 or newer (tested with Python 3.8.10): To execute our parsing and plotting scripts for the figures.

To use SafePM in a native environment the required software dependencies are the same as those of [PMDK v1.9.2](https://github.com/pmem/pmdk/tree/1.9.2#dependencies) and can also be found in the respective Dockerfiles in our benchmarks' directories.
