set -e

RESULTS="../benchmarks/ripe/results"

printf "|-----------------------------------------------|\n"
printf "|%17s|%9s|%9s|%9s|\n" "RIPE VARIANT" "Always" "Sometimes" "Never"
printf "|-----------------------------------------------|\n"
for FILE in $RESULTS/*; do 
    VARIANT="$(basename -- $FILE)"
    while read LINE; do
        set -- $LINE
        if [[ "$1" == "OK:" ]]; then
            ALWAYS=$2
            SOMETIMES=$4
            NEVER=$6
            printf "|%17s|%9d|%9d|%9d|\n" $VARIANT $ALWAYS $SOMETIMES $NEVER
        fi
    done < $FILE
done
printf "|-----------------------------------------------|\n"