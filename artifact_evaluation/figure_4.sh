set -e

bash ../plot_utils/import_pmembench.sh ../benchmarks/pmembench/results/
bash ../plot_utils/import_pmemkv.sh ../benchmarks/pmemkv/results/

python3 ../plot_utils/generate_plots.py ../plot_utils/plot_config_pmemkv.yml

mv ./plots/pmemkv/pmemkv_plot_overhead.pdf ./plots/figure4.pdf
mv ./plots/pmemkv/pmemkv_plot_overhead.png ./plots/figure4.png

rm -rf asan pmdk safepm wrappers_only plots/pmemkv