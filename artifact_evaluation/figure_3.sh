set -e

bash ../plot_utils/import_pmembench.sh ../benchmarks/pmembench/results/
bash ../plot_utils/import_pmemkv.sh ../benchmarks/pmemkv/results/

python3 ../plot_utils/generate_plots.py ../plot_utils/plot_config_maps.yml

mv ./plots/pmembench_map/pmembench_map_plot.pdf ./plots/figure3.pdf
mv ./plots/pmembench_map/pmembench_map_plot.png ./plots/figure3.png

rm -rf asan pmdk safepm wrappers_only plots/pmembench_map