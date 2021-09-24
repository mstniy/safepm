import subprocess
import os
import sys
import yaml
from parser import parse
from analyse import filter_out, analyse
from plot import plot

#Input checking
#If config file is not given return message with the usage
def input_check() -> None:
    if (len(sys.argv) < 2):
        print("Usage: python3 generate_plots.py /path/to/config_file.yml")
        exit()
    else:
        return sys.argv[1]

#Parse the config file and create the list with the files that will be parsed
def result_files_finder(config) -> None:
    result_folder = config[0]['results']['folder']
    lib_versions = config[0]['results']['lib_versions']
    result_files = {}
    for version in lib_versions:
        pre_res = [result_folder + '/' + version + '/' + sub + '_result' for sub in lib_versions[version]]
        result_files[version] = pre_res

    return result_files

#Parse the config file and create the list with the plot setups
def plot_config_setup(config) -> None:
    plot_config = []
    for elem in config[1:]:
        plot_config.append(elem)
    return plot_config

def main() -> None:
    cfg_file = input_check()
    config = yaml.safe_load(open(cfg_file))

    #fine the files of the results
    files_to_parse = result_files_finder(config)

    #parse the files and get the results
    accumulated_results = []
    for lib_version in files_to_parse:
        for file_idx in range(len(files_to_parse[lib_version])):
            accumulated_results.append(parse(lib_version, files_to_parse[lib_version][file_idx]))
    
    #indices where the info, labels and values are stored in the results dictionary
    info_idx = 0 #[category, number_of_experiments, group]
    labels_idx = 1 #labels list
    values_idx = 2 #values starting from idx 2

    plot_configs = plot_config_setup(config)
    for plot_config in plot_configs:
        filtered_results = filter_out(accumulated_results, plot_config)
        x_values, y_values, info = analyse(filtered_results, plot_config, info_idx, labels_idx, values_idx)
        plot(plot_config, x_values, y_values, info)

if __name__ == "__main__":
    main()
