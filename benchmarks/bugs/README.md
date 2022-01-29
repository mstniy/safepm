## Bugs found by SafePM
To reproduce the bugs found with SafePM ([[1]](https://github.com/pmem/pmdk/issues/5333),[[2]](https://github.com/pmem/pmdk/issues/5334)), run:
``` 
./safepm_bugs.sh <path-to-pm>
```
`path-to-pm` is the directory where benchmarks will create the persistent memory pools.

This execution will show the bugs in the `standard output` with the appropriate error messages (error message report from ASan for the first bug and a failed assertion for an invalid free for the second one).