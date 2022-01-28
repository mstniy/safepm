set -e

declare -a variants_plotter=("pmdk" "asan" "safepm")
declare -a variants_results=("vanilla" "asan" "safepm")

if [ "$#" -ne 1 ]
then
  echo "Usage: import_pmemkv.sh <results location>" 2>&1
  exit 1
fi

for (( i=0; i<${#variants_plotter[@]}; i++))
do
  variant_plotter="${variants_plotter[i]}"
  mkdir -p "$variant_plotter"
  variant_results="${variants_results[i]}"
  if [ -e ${variant_plotter}/pmemkv_result ]
  then
    j=1
    while true
    do
      if ! [ -e ${variant_plotter}/pmemkv_result_${j} ]
      then
        break
      fi
      j=$((j+1))
    done
    
    mv ${variant_plotter}/pmemkv_result ${variant_plotter}/pmemkv_result_${j}
  fi
  echo "delete_random: delete_random [6] [group: pmemobj]" > ${variant_plotter}/pmemkv_result
  cat $1/${variant_results}/delete/deleterandom_threads.csv | sed 's/,/;/g' >> ${variant_plotter}/pmemkv_result
  echo "delete_seq: delete_seq [6] [group: pmemobj]" >> ${variant_plotter}/pmemkv_result
  cat $1/${variant_results}/delete/deleteseq_threads.csv | sed 's/,/;/g' >> ${variant_plotter}/pmemkv_result
  echo "randomreadwrite50: randomreadwrite50 [6] [group: pmemobj]" >> ${variant_plotter}/pmemkv_result
  cat $1/${variant_results}/randomreadwrite/50_readpercentage/readrandomwriterandom_threads.csv | sed 's/,/;/g' >> ${variant_plotter}/pmemkv_result
  echo "randomreadwrite95: randomreadwrite95 [6] [group: pmemobj]" >> ${variant_plotter}/pmemkv_result
  cat $1/${variant_results}/randomreadwrite/95_readpercentage/readrandomwriterandom_threads.csv | sed 's/,/;/g' >> ${variant_plotter}/pmemkv_result
  echo "read_random: read_random [6] [group: pmemobj]" >> ${variant_plotter}/pmemkv_result
  cat $1/${variant_results}/read/readrandom_threads.csv | sed 's/,/;/g' >> ${variant_plotter}/pmemkv_result
  echo "read_seq: read_seq [6] [group: pmemobj]" >> ${variant_plotter}/pmemkv_result
  cat $1/${variant_results}/read/readseq_threads.csv | sed 's/,/;/g' >> ${variant_plotter}/pmemkv_result
done
