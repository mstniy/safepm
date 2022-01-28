set -e

bash ../plot_utils/import_pmembench.sh ../benchmarks/pmembench/results/
bash ../plot_utils/import_pmemkv.sh ../benchmarks/pmemkv/results/

python3 ../plot_utils/generate_plots.py ../plot_utils/plot_config_pmembench_tx.yml

mv ./plots/pmembench_tx/pmembench_tx_plot_single_vertical.pdf ./plots/figure5.pdf
mv ./plots/pmembench_tx/pmembench_tx_plot_single_vertical.png ./plots/figure5.png

rm -rf asan pmdk safepm wrappers_only plots/pmembench_tx