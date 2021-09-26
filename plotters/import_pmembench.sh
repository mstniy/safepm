set -e

declare -a variants_plotter=("pmdk" "asan" "wrappers_only" "safepm")
declare -a variants_results=("vanilla" "asan_only" "wrappers_only" "safepm")
declare -a pmembench_benchmark=("pmembench_map" "pmembench_map_partial_cov" "pmembench_open_create" "pmembench_tx")
if [ "$#" -ne 1 ]
then
  echo "Usage: import_pmembench.sh <results location>" 2>&1
  exit 1
fi

for (( i=0; i<${#variants_plotter[@]}; i++))
do
  variant_plotter="${variants_plotter[i]}"
  mkdir -p "$variant_plotter"
  variant_results="${variants_results[i]}"

    for (( k=0; k<${#pmembench_benchmark[@]}; k++))
    do
        if ! [ -e $1/"${pmembench_benchmark[k]}"/${variant_results}.txt ]
        then
            continue
        fi

        if [ -e ${variant_plotter}/"${pmembench_benchmark[k]}"_result ]
        then
            j=1
            while true
            do
            if ! [ -e ${variant_plotter}/"${pmembench_benchmark[k]}"_result_${j} ]
            then
                break
            fi
            j=$((j+1))
        done
            mv ${variant_plotter}/"${pmembench_benchmark[k]}"_result ${variant_plotter}/"${pmembench_benchmark[k]}"_result_${j}
        fi

        cp $1/"${pmembench_benchmark[k]}"/${variant_results}.txt ${variant_plotter}/"${pmembench_benchmark[k]}"_result
    done
done
