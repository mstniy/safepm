# Artifact evaluation

### Benchmarks run

To run the experiments and gather the results, please run:
``` 
./run.sh <path-to-pm>
```
where `path-to-pm` is a file system directory that is mounted with the `dax` mount option ([hint](https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/7/html/storage_administration_guide/configuring-persistent-memory-for-file-system-direct-access-dax)).
For example :
```
./run.sh /mnt/pmem0/dir_name
```
This is the path where benchmarks will create the persistent memory pools.

### Figures

After the benchmark run is over and you have collected all the results you can reproduce each of the paper figures by running the respective script:
```
./figure_x.sh
```
This script will create a `plots` directory and will place the respective plot there both in .png and in .pdf format.

To obtain all the plots, we provide the `safepm_figures.sh`. 
Simply run:
```
./safepm_figures.sh
```