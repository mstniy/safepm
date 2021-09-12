set -e

declare -a variants=("asan" "safepm" "memcheck")

for variant in "${variants[@]}"
do
  docker build -t ripe64-$variant -f=Dockerfile_$variant .
done

for variant in "${variants[@]}"
do
  echo
  echo
  echo "--------------------------"
  echo "|Running variant $variant|"
  echo "--------------------------"
  echo
  echo
  docker run -it --shm-size=2g ripe64-$variant ./ripe_tester.py both 1 gcc
done
