import os
import numpy as np
import matplotlib.pyplot as plt
from analyse import metrics, bench_info
from plotters import bar_plot, bar_overhead_plot

def plot(plot_config, x_values, y_values, info):
    plot_type = plot_config['plot']['plot_type']
    plot_folder = plot_config['plot']['plot_folder']
    x_axis_label = plot_config['plot']['x_axis']
    y_axis_label = plot_config['plot']['y_axis']
    overhead_annot = plot_config['plot']['overhead_annot']

    if plot_type=="bar":
        plot_fun = bar_plot
    elif plot_type=="overhead":
        plot_fun = bar_overhead_plot

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