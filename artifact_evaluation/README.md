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

Further, you need to get the source code of SafePM. It can be found by simply cloning the current repository:
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

## Benchmark configuration

### Dockerfiles
For each of our benchmarks we provide Dockerfile(s). They include the software dependencies required to run each experiment. Additionally, the Dockerfile includes the setup of the variants for the experiments. They clone and install the appropriate PMDK fork(s) and run patches, where applicable. For more information about SafePM's PMDK forks, please see [here](https://github.com/mstniy/safepm#pmdk-submodule).

### pmembench
`pmembench` is a benchmark driver shipped with PMDK. Its source code can be found in `$(PMDK_root)/src/benchmarks/pmembench.cpp`. It is accompanied with a set of configurations files that define the parameters of each benchmark run. These configurations are placed in `$(PMDK_root)/src/benchmarks/` folder.

For our experiments, we use `pmembench` with the following configurations:
1. `pmembench_map.cfg` : Configuration file containing the parameters for experiments on PM indices (paper Figure 3).
2. `pmembench_tx_safepm.cfg` : Configuration file containing the parameters for the experiments on alloc/realloc/free PMDK operations (paper Figure 5).
3. `pmembench_obj_gen_safepm.cfg` : Configuration file containing the parameters for the experiments pool_open/pool_create PMDK operaitions (paper Figure 6).
4. `pmembench_map_partial_cov.cfg` : Configuration file containing the parameters for the partial coverage experiments on PMDK's hashmap (paper Figure 7).

### pmemkv
[`pmemkv`](https://github.com/pmem/pmemkv) is a local/embedded key-value datastore optimized for persistent memory. For our experiments we use the [`pmemkv_bench`](https://github.com/pmem/pmemkv-bench) utility which provides some standard read, write & remove benchmarks. 
More specifically, to install `pmemkv` we use the following open-source libraries:
1. `memkind`: https://github.com/memkind/memkind.git tag: v1.11.0
2. `libpmemobj-cpp` : https://github.com/pmem/libpmemobj-cpp.git tag: stable-1.12
3. `pmemkv` : https://github.com/pmem/pmemkv.git tag: 1.4
4. `pmemkv_bench` :  https://github.com/pmem/pmemkv-bench.git commit: 32d94c0

The instructions to clone and install the above libraries can be found in the Dockerfiles.

The configurations for `pmemkv_bench` are in the `$(SafePM_root)/benchmarks/pmemkv/run-all.sh` script. Further parameters for the `pmemkv` benchmarks, such as the thread count and benchmark type, can be found in each of the `$(SafePM_root)/benchmarks/pmemkv/run-pmemkv-bench-$(variant).sh` scripts

### RIPE benchmark
The source code for the RIPE benchmark and our port to PM are placed in `$(SafePM_root)/benchmarks/ripe/source` directory. The `ripe_tester.py` script acts as a driver for the RIPE executable. The attack parameters are defined in lists starting from [here](https://github.com/mstniy/safepm/blob/master/benchmarks/ripe/ripe_tester.py#L22). The RIPE benchmark variants and the parameters we provide to `ripe_tester.py` in our experiments can be found in the [`$(SafePM_root)/benchmarks/ripe/run-variants.sh`](https://github.com/mstniy/safepm/blob/0fb7c269b927abc65b51a992ec0c805fa4c08a62/benchmarks/ripe/run-variants.sh) script.

Note that for `valgrind`, we used [this version](https://github.com/pmem/valgrind/tree/06f15d69237501852dd29883940e18da4179830a).

### Crash consistency benchmark
For our experiments on crash consistency we run `pmembench` with the `pmembench_map.cfg` and `pmembench_tx_safepm.cfg` configuration files, as described in `pmembench` section above. We execute the `change_pmembench_file_path.sh` script to lessen the operations in these configuration files to keep the runtime reasonable, because of the large performance overhead introduced by valgrind. Note that for `valgrind`, we used [this version](https://github.com/pmem/valgrind/tree/06f15d69237501852dd29883940e18da4179830a) to include `pmemcheck`. The executed `valgrind` commands inside the container and the output files of the experiment can be found in `$(SafePM_root)/benchmarks/crash_consistency/inner_run.sh`.

### Bug reproduction
To reproduce the bugs, we define in our PMDK fork's branch `pmasan-1.9.2` two distinct configuration files for `pmembench`, namely `pmembench_tx_bug.cfg` and `pmembench_map_bug.cfg`. These configuration files lead to the reported memory safety violations ([[1]](https://github.com/pmem/pmdk/issues/5333),[[2]](https://github.com/pmem/pmdk/issues/5334)) that we discovered using SafePM. Note that the configuration file `pmembench_map_bug.cfg` simply tries to benchmark the btree index, which triggers the off-by-one violation mentioned in section 6.7 in the paper. The second configuration file includes just the `obj_tx_realloc_sizes_abort` benchmark as defined in the upstream repository in the file `pmembench_tx.cfg`. In summary, both of these configuration files were taken from the upstream repository and modified to trigger the discovered bugs and anomalies as quickly as possible.

## Hardware configuration
To reproduce the results from the paper, the machine should preferably be equipped with a physical persistent memory module (e.g., Intel Optane DC) with at least 64 GB available space. The persistent memory module should be mounted using a DAX-enabled file system (e.g. EXT4-DAX).
Additionally, we recommend running the [pmemkv](https://github.com/pmem/pmemkv) experiments on a machine with at least 24 cores, as they are configured to run with up to 24 threads. 
The testbed, used to conduct our experiments, is equipped with:
1. Intel(R) Xeon(R) Gold 6212U CPU @ 2.40GHz (24 cores)
2. L1d cache: 768 KiB, L1i cache: 768 KiB, L2 cache: 24 MiB, L3 cache: 35.8 MiB
3. 192 GB (6 channels×32 GB/DIMM) DRAM 
4. 768 GB (6 channels×128 GB/DIMM) Intel Optane DC DIMMs.

## Software dependencies
We require the following software configuration to reproduce our experimental results:
1. Linux (tested in Ubuntu 20.04.3 LTS with kernel version 5.4.0)
2. Docker (tested with Docker version 20.10.7): Each experiment comes with its pre-configured Dockerfile. We provide scripts that automatically build the images containing the required software dependencies.
3. gcc and cmake (tested with gcc 9.3.0 and cmake 3.16.3): To build our unit tests.
4. Python 3.7 or newer (tested with Python 3.8.10): To execute our parsing and plotting scripts for the figures.

To use SafePM in a native environment the required software dependencies are the same as those of [PMDK v1.9.2](https://github.com/pmem/pmdk/tree/1.9.2#dependencies) and can also be found in the respective Dockerfiles in our benchmarks' directories.

## SafePM code structure
For more information about the source code structure please see [here](https://github.com/mstniy/safepm/blob/master/Readme.md#safepm-source-code-organization).
