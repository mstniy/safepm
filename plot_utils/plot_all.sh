set -e

if [ "$#" -ne 2 ]
then
  echo "Usage: plot_all.sh <pmembench results location> <pmemkv results location>" 2>&1
  exit 1
fi

bash import_pmembench.sh ${1}
bash import_pmemkv.sh ${2}

python3 generate_plots.py plot_config_maps.yml
python3 generate_plots.py plot_config_pmembench_tx.yml
python3 generate_plots.py plot_config_partial_coverage.yml
python3 generate_plots.py plot_config_open_create.yml
python3 generate_plots.py plot_config_pmemkv.yml

rm -rf asan pmdk safepm wrappers_only