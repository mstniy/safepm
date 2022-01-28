import os
import numpy as np
import matplotlib.pyplot as plt
from analyse import metrics, bench_info
from plotters import pmembench_tx_plot_single, pmembench_tx_plot_dual, pmembench_open_create_plot,\
                    pmembench_open_create_plot_time, pmembench_open_create_plot_time_log,pmembench_map_plot,\
                    pmembench_map_partial_cov_plot, pmemkv_plot, pmemkv_plot_overhead,\
                    pmembench_tx_plot_single_vertical

def plot(plot_config, x_values, y_values, info):
    plot_type = plot_config['plot']['plot_type']
    plot_folder = plot_config['plot']['plot_folder']
    x_axis_label = plot_config['plot']['x_axis']
    y_axis_label = plot_config['plot']['y_axis']
    overhead_annot = plot_config['plot']['overhead_annot']

    if plot_type=="pmembench_tx_single":
        plot_fun = pmembench_tx_plot_single
    elif plot_type=="pmembench_tx_dual":
        plot_fun = pmembench_tx_plot_dual
    elif plot_type=="pmembench_tx_single_vertical":
        plot_fun = pmembench_tx_plot_single_vertical
    elif plot_type=="pmembench_open_create":
        plot_fun = pmembench_open_create_plot
    elif plot_type=="pmembench_open_create_time":
        plot_fun = pmembench_open_create_plot_time
    elif plot_type=="pmembench_open_create_time_log":
        plot_fun = pmembench_open_create_plot_time_log
    elif plot_type=="pmembench_map":
        plot_fun = pmembench_map_plot
    elif plot_type=="pmembench_map_partial_cov":
        plot_fun = pmembench_map_partial_cov_plot
    elif plot_type=="pmemkv":
        plot_fun = pmemkv_plot
    elif plot_type=="pmemkv_overhead":
        plot_fun = pmemkv_plot_overhead
    else:
        print("invalid plot type")
        exit()

    plot_x = {}
    plot_y = {}
    plot_info = {}
    
    for benchmark in x_values:
        plot_x[benchmark] = {}
        plot_y[benchmark] = {}
        plot_info[benchmark] = {}
        for lib_version in x_values[benchmark]:
            for variant in x_values[benchmark][lib_version]:
                if (variant not in plot_x[benchmark]):
                    plot_x[benchmark][variant] = {}
                    plot_y[benchmark][variant] = {}
                    plot_info[benchmark][variant] = {}
                plot_x[benchmark][variant][lib_version] = x_values[benchmark][lib_version][variant]
                plot_y[benchmark][variant][lib_version] = y_values[benchmark][lib_version][variant]
                plot_info[benchmark][variant][lib_version] = info[benchmark][lib_version][variant]
    
    plot_fun(plot_x, plot_y, plot_info, x_axis_label, y_axis_label, overhead_annot, plot_folder)

    return