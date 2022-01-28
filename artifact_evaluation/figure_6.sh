set -e

bash ../plot_utils/import_pmembench.sh ../benchmarks/pmembench/results/
bash ../plot_utils/import_pmemkv.sh ../benchmarks/pmemkv/results/

python3 ../plot_utils/generate_plots.py ../plot_utils/plot_config_open_create.yml

mv ./plots/pmembench_open_create/pmembench_open_create_plot_time_log.pdf ./plots/figure6.pdf
mv ./plots/pmembench_open_create/pmembench_open_create_plot_time_log.png ./plots/figure6.png

rm -rf asan pmdk safepm wrappers_only plots/pmembench_open_create