set -e

declare -a variants=( "asan" "safepm" "memcheck" )

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
  
  if [ "$variant" = "memcheck" ]
  then
    # We trick valgrind into not producing (large) core dumps by creating a read-only directory and running the tests in that. But for that to work, we must run as a non-root user in the container
    docker run -it --shm-size=2g --user kartal ripe64-$variant ./ripe_tester.py both 3 gcc
  else
    docker run -it --shm-size=2g ripe64-$variant ./ripe_tester.py both 3 gcc
  fi
done
