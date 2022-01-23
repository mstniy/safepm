set -e

declare -a variants=("vanilla" "asan" "safepm")

for variant in "${variants[@]}"
do
  docker build -t pmemkv-bench-$variant -f=Dockerfile_$variant .
done

for variant in "${variants[@]}"
do
  mkdir -p "results/${variant}"
  # Use this line to run the bencharks on DRAM. Do not forget to modify the variable MOUNT_PM in run-all.sh accordingly.
  #docker run -v $(pwd)/results/${variant}:/results -it --shm-size=2g pmemkv-bench-$variant ./run-all.sh
  docker run -v "$BENCHMARK_PM_PATH:/mnt/pmemkv" -v "$(pwd)/results/${variant}:/results" -t --shm-size=2g pmemkv-bench-$variant ./run-all.sh
done
