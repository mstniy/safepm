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

### Reproduce bugs

To reproduce the bugs found with SafePM ([[1]](https://github.com/pmem/pmdk/issues/5333),[[2]](https://github.com/pmem/pmdk/issues/5334)), run:
``` 
./reproduce_bugs.sh <path-to-pm>
```
`path-to-pm` is the directory where benchmarks will create the persistent memory pools.

This execution will show the bugs in the `standard output` with the appropriate error messages (error message report from ASan for the first bug and a failed assertion for an invalid free for the second one).

### Hardware configuration

The machine we use for our evaluation has the following hardware configuration:

TODO

### Software dependecies

TODO