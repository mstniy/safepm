set -e

bash ../plot_utils/import_pmembench.sh ../benchmarks/pmembench/results/
bash ../plot_utils/import_pmemkv.sh ../benchmarks/pmemkv/results/

python3 ../plot_utils/generate_plots.py ../plot_utils/plot_config_partial_coverage.yml

mv ./plots/pmembench_map_partial_cov/pmembench_map_partial_cov_plot.pdf ./plots/figure7.pdf
mv ./plots/pmembench_map_partial_cov/pmembench_map_partial_cov_plot.png ./plots/figure7.png

rm -rf asan pmdk safepm wrappers_only plots/pmembench_map_partial_cov