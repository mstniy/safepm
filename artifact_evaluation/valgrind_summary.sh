#!/bin/bash
set -e

RESULTS="../benchmarks/crash_consistency/results"

for FILE in $RESULTS/*; do 
    VARIANT="$(basename -- $FILE)"
    printf "|---------------%s---------------|\n" $VARIANT
    tail $FILE
done