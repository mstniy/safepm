import os
import numpy as np
import matplotlib.pyplot as plt

#y axis metrics to consider
metrics = {
        "exec_time"     :   "total-avg[sec]",    
        #"latency-avg"   :   "latency-avg[nsec]",
        "throughput"    :   "ops-per-second[1/sec]"
    }

#x axis possible variables
bench_info = {
        "threads"           :   "threads",    
        "ops-per-thread"    :   "ops-per-thread",
        "total-ops"         :   "total-ops",
        "data-size"         :   "data-size",
        "seed"              :   "seed",
        "repeats"           :   "repeats",
        "thread-affinity"   :   "thread-affinity",
        "main-affinity"     :   "main-affinity",
        "min-exe-time"      :   "min-exe-time",
        "type-number"       :   "type-number",
        "operation"         :   "operation",
        "min-size"          :   "min-size",
        "lib"               :   "lib",
        "nestings"          :   "nestings",
        "min-rsize"         :   "min-rsize",
        "realloc-size"      :   "realloc-size",
        "changed-type"      :   "changed-type",
        "type"              :   "type",
        "keys"              :   "keys",
        "value-size"        :   "value-size",
        "read-ratio"        :   "read-ratio",
        "zipf-exp"          :   "zipf-exp"
    }

def filter_out(accumulated_results, plot_config) -> None:
    benchmarks = plot_config['plot']['benchmarks']
    filtered_results = {}
    for benchmark_name in benchmarks:
        filtered_results[benchmark_name]=[]

    for results in accumulated_results:
        for benchmark_name in results:
            if (benchmark_name in benchmarks):
                filtered_results[benchmark_name].append(results[benchmark_name])

    return filtered_results

def analyse(filtered_results, plot_config, info_idx, labels_idx, values_idx) -> None:
    x_values = []
    y_values = []
    info = []

    benchmarks = plot_config['plot']['benchmarks']
    x_axis = plot_config['plot']['x_axis']
    y_axis = plot_config['plot']['y_axis']
    variants = plot_config['plot']['variants']
    
    y_label = y_axis#metrics[y_axis]
    x_label = x_axis#bench_info[x_axis]
    y_values = {}
    x_values = {}
    info = {}

    for benchmark in filtered_results:
        y_values[benchmark] = {}
        x_values[benchmark] = {}
        info[benchmark] = {}
        y_values[benchmark] = {}
        x_values[benchmark] = {}
        info[benchmark] = {}
        for i in range (len(filtered_results[benchmark])):
            #locate variant idx
            variant_indexes = {}
            for variant in variants:
                variant_indexes[variant] = (filtered_results[benchmark][i][labels_idx].index(variant))
            #append the benchmark name
            lib_version = filtered_results[benchmark][i][info_idx][0]
            if (lib_version not in x_values[benchmark]):
                x_values[benchmark][lib_version] = {}
            if (lib_version not in y_values[benchmark]):
                y_values[benchmark][lib_version] = {}
            if (lib_version not in info[benchmark]):
                info[benchmark][lib_version] = {}

            #locate the x-axis values
            if ("realloc" in benchmark) and x_label == "data-size":
                x_values_idx = filtered_results[benchmark][i][labels_idx].index("realloc-size")
            else:
                x_values_idx = filtered_results[benchmark][i][labels_idx].index(x_label)     
              
            for j in range(len(filtered_results[benchmark][i][values_idx])):
                variant_str = ''
                for variant in variants:
                    variant_str = variant_str + '+' + variant + '=' + filtered_results[benchmark][i][values_idx][j][variant_indexes[variant]]
                if (variant_str not in x_values[benchmark][lib_version]):
                    x_values[benchmark][lib_version][variant_str] = []

                x_values[benchmark][lib_version][variant_str].append(filtered_results[benchmark][i][values_idx][j][x_values_idx])               
                    
            #locate the y-axis values
            y_values_idx = filtered_results[benchmark][i][labels_idx].index(y_label)       
            for j in range(len(filtered_results[benchmark][i][values_idx])):
                variant_str = ''
                for variant in variants:
                    variant_str = variant_str + '+' + variant + '=' + filtered_results[benchmark][i][values_idx][j][variant_indexes[variant]]
                if (variant_str not in y_values[benchmark][lib_version]):
                    y_values[benchmark][lib_version][variant_str] = []

                y_values[benchmark][lib_version][variant_str].append(filtered_results[benchmark][i][values_idx][j][y_values_idx])
              
            #locate the rest of info
            for j in range(len(filtered_results[benchmark][i][values_idx])):
                variant_str = ''
                for variant in variants:
                    variant_str = variant_str + '+' + variant + '=' + filtered_results[benchmark][i][values_idx][j][variant_indexes[variant]]
                if (variant_str not in info[benchmark][lib_version]):
                    info[benchmark][lib_version][variant_str] = []
                
                labels_list = filtered_results[benchmark][i][labels_idx][:y_values_idx] + filtered_results[benchmark][i][labels_idx][y_values_idx+1:]
                info_values_list = filtered_results[benchmark][i][values_idx][j][:y_values_idx] + filtered_results[benchmark][i][values_idx][j][y_values_idx+1:]
                info[benchmark][lib_version][variant_str].extend([labels_list, info_values_list])

    return x_values, y_values, info
