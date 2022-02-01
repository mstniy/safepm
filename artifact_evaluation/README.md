# Artifact evaluation

## Set-up
To reproduce the results of SafePM, a machine equipped with a physical persistent memory module is required. SafePM requires a DAX-enabled file system, such as EXT4-DAX. To format and mount the drive (assuming the device name is `/dev/pmem0`), follow the instructions below:
```
$ sudo mkdir /mnt/pmem0
$ sudo mkfs.ext4 /dev/pmem0
$ sudo mount -t ext4 -o dax /dev/pmem0 /mnt/pmem0
$ sudo chmod -R 777 /mnt/pmem0
 ```
In case you do not have a machine with persistent memory, you can execute the experiments by [emulating](https://pmem.io/blog/2016/02/how-to-emulate-persistent-memory/) it with DRAM, e.g., with the `/dev/shm/` directory. Note that, this results in considerable differences in the final performance measurements.

Further, you need to get the source code of SafePM. It can be found here(permanent url) or by simply cloning the current repository:
```
$ git clone git@github.com:mstniy/safepm.git
```

## Experiments execution
Our automated scripts create the docker images, run the containers and carry out each experiment with the appropriate configuration.
To execute our experiments, navigate to the directory of cloned/downloaded source code and run:
```
$ cd safepm/artifact_evaluation
```
The artifact_evaluation folder is where our automated scripts are placed. 
In the following sections, we assume `/mnt/pmem0/` as the directory where PM is mounted.

### pmembench, pmemkv & RIPE benchmark (approx. 8-10hrs)
To ease the execution of our experiments, we provide a script that collectively runs all the benchmarks (`pmembench`, `pmemkv` and `ripe benchmark`) with their configurations:
``` 
$ ./run_all.sh /mnt/pmem0/
```
These benchmarks can also be executed separately. To this end, we provide three additional scripts:
```
$ ./run_pmembench /mnt/pmem0/
$ ./run_pmemkv /mnt/pmem0/
$ ./run_ripe /mnt/pmem0/
```
### Crash consistency (approx. 1.5-2hrs)
This experiment runs `memcheck` and `pmemcheck` on the described configurations in Section 6.5 of the paper to prove SafePM's crash consistency. The `valgrind` output is redirected to files for later inspection.
To execute the experiment:
```
$ ./run_crash_consistency.sh /mnt/pmem0/
```
### Bug reproduction ( < 10mins )
To reproduce the bugs found with SafePM ([[1]](https://github.com/pmem/pmdk/issues/5333),[[2]](https://github.com/pmem/pmdk/issues/5334)), run:
``` 
$ ./reproduce_bugs.sh /mnt/pmem0/
```
This execution will show the bugs in the `standard output` with the appropriate error messages (error message report from ASan for the first bug and a failed assertion for an invalid free for the second one).

## Analysis of the results
### Performance Figures
After the `pmembench` and `pmemkv` benchmark runs are over, the results are collected in `.txt` files in the benchmark folders. Then, the results can be analysed to reproduce each of the paper figures by running the respective script:
```
$ ./figure_x.sh
```
where `x` is the figure number inside the paper.
This script will place the respective plot in the `plots` folder inside the artifact evaluation folder, both in .png and in .pdf format.

To obtain all the plots at once, we provide the `safepm_figures.sh` script. 
Simply execute:
```
$ ./safepm_figures.sh
```
### Ripe results
The following script prints the `RIPE benchmark` results in the `standard output` in a table format similar to Table 4 in the paper.
```
$ ./table_4.sh
```
### Valgrind reports
The following script prints in the `standard output` the `valgrind` summaries to prove SafePM's crash consistency claims.
```
$ ./valgrind_summary.sh
```

## Hardware configuration
To reproduce the results from the paper, the machine should preferably be equipped with a physical persistent memory module (e.g., Intel Optane DC) with at least 64 GB available space. The persistent memory module should be mounted using a DAX-enabled file system (e.g. EXT4-DAX)
Additionally, we recommend running the [pmemkv](https://github.com/pmem/pmemkv) experiments on a machine with at least 24 cores, as they are configured to run with up to 24 threads. 
The testbed, used to conduct our experiments, is equipped with Intel(R) Xeon(R) Gold 6212U CPU with 24 cores and 768GB (6 channels×128 GB/DIMM) Intel Optane DC DIMMs.

## Software dependecies
We require the following software configuration to reproduce our experimental results:
1. Linux (tested in Ubuntu 20.04.3 LTS with kernel version 5.4.0)
2. Docker (tested with Docker version 20.10.7): Each experiment comes with its pre-configured Dockerfile. We provide scripts that automatically build the images containing the required software dependencies.
3. gcc and cmake (tested with gcc 9.3.0 and cmake 3.16.3): To build our unit tests.
4. Python 3.7 or newer (tested with Python 3.8.10): To execute our parsing and plotting scripts for the figures.

To use SafePM in a native environment the required software dependencies are the same as those of [PMDK v1.9.2](https://github.com/pmem/pmdk/tree/1.9.2#dependencies) and can also be found in the respective Dockerfiles in our benchmarks' directories.

## SafePM code structure
For more information about the source code structure please see [here](https://github.com/mstniy/safepm/blob/master/Readme.md#safepm-source-code-organization).
