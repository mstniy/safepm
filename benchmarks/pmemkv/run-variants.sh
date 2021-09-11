set -e

declare -a variants=("vanilla" "asan" "safepm")

for variant in "${variants[@]}"
do
  docker build -t pmemkv-bench-$variant -f=Dockerfile_$variant .
done

REPEATS=3

mkdir -p results

for i in $(seq 1 $REPEATS)
do
  for variant in "${variants[@]}"
  do
    mkdir results/${variant}_$i
    docker run -v $(pwd)/results/${variant}_$i:/results -it --shm-size=2g pmemkv-bench-$variant ./run-all.sh randomreadwrite
  done
done
