The RIPE benchmark, originally developed by John Wilander and Nick Nikiforakis and
presented at the 2011 Annual Computer Security Applications Conference (ACSAC) in Orlando, Florida.

Ported to 64-bits by Hubert ROSIER.

Patched to use ASan and the PMDK's persistent heap (instead of the volatile system heap), to measure the security capabilities of the safepm project.

For further details, see our paper and [Hubert ROSIER's repo](https://github.com/hrosier/ripe64)

## How to run

```
$ ./run_variants.sh
```
