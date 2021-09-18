set -e

declare -a variants_plotter=("pmdk" "asan" "wrappers_only" "safepm")
declare -a variants_results=("vanilla" "asan_only" "wrappers_only" "safepm")

if [ "$#" -ne 1 ]
then
  echo "Usage: import_pmembench_map.sh <results location>" 2>&1
  exit 1
fi

for (( i=0; i<${#variants_plotter[@]}; i++))
do
  variant_plotter="${variants_plotter[i]}"
  variant_results="${variants_results[i]}"
  if [ -e ${variant_plotter}/pmembench_map_result ]
  then
    j=1
    while true
    do
      if ! [ -e ${variant_plotter}/pmembench_map_result_${j} ]
      then
        break
      fi
      j=$((j+1))
    done
    
    mv ${variant_plotter}/pmembench_map_result ${variant_plotter}/pmembench_map_result_${j}
  fi
  cp $1/${variant_results}.txt ${variant_plotter}/pmembench_map_result
done
