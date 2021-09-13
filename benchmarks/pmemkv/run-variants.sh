set -e

declare -a variants=("vanilla" "asan" "safepm")

for variant in "${variants[@]}"
do
  docker build -t pmemkv-bench-$variant -f=Dockerfile_$variant .
done

mkdir -p results

for variant in "${variants[@]}"
do
  mkdir results/${variant}
  docker run -v $(pwd)/results/${variant}:/results -it --shm-size=2g pmemkv-bench-$variant ./run-all.sh
done
