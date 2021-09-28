import os
import numpy as np
import matplotlib.pyplot as plt
from analyse import metrics,bench_info

versions_map = {
        "pmdk"                  :   "PMDK",    
        "asan"                  :   "ASan",
        "wrappers_only"         :   "SafePM wo/ Asan",
        "safepm"                :   "SafePM"
    }

colour = ["white", "black", "white", "grey", "white", "white"]
hatch = ['--' , '' , '' , '', '/', '++']
markers = ['o', 's', '+', 'x', 'D', '*']

def create_dir(new_dir):
    if not(os.path.exists(new_dir)):
        try:
            os.makedirs(new_dir)
        except OSError as error:
            print ("Creation of the directory %s failed : %s" % (new_dir, error))
        else:
            print ("Successfully created the directory %s" % new_dir)

def save_plot(plot_dir, plot_file_path, fig, lgd):
    create_dir(plot_dir)
    #fig.set_size_inches(12, 5)
    print(plot_file_path)
    fig.savefig(plot_file_path + ".png", dpi=300, format='png', bbox_extra_artists=(lgd,), bbox_inches='tight')

def bar_plot(x_values, y_values, info, x_axis_label, y_axis_label, overhead_annot, plot_folder):
    print("bar plot")
    for benchmark in x_values:
        plot_title = benchmark
        for variant in x_values[benchmark]:
            experiment_params = variant.split('+')[1:]
            #number_of_bars = 3
            number_of_bars = len(x_values[benchmark][variant])
            bar_area_percentage = 0.8
            w = float(bar_area_percentage / number_of_bars) #bar width to cover 80% of the dedicated space
            x_axis_spacing = np.linspace(-bar_area_percentage/2 + w/2, bar_area_percentage/2 - w/2, num=number_of_bars)
            
            fig = plt.figure()
            ax = fig.add_subplot(111)

            #append values to the plot
            for version_lib in x_values[benchmark][variant]:     
                internal_idx = list(x_values[benchmark][variant].keys()).index(version_lib)
                x_index = np.arange(0, len(x_values[benchmark][variant][version_lib]), 1) + x_axis_spacing[internal_idx]
                rect = ax.bar(x_index, [float(i) for i in y_values[benchmark][variant][version_lib]], width = w, 
                                    color = colour[internal_idx], hatch = hatch[internal_idx], edgecolor = 'black', align='center', label=version_lib)
            
                #percentage change annotation
                if overhead_annot:
                    if (version_lib == "pmdk"):
                        reference = [float(i) for i in y_values[benchmark][variant][version_lib]]
                    else: 
                        if 'reference' in vars(): #if pmdk reference is defined
                            bar_values = [float(item) for item in y_values[benchmark][variant][version_lib]]
                            for j in range(len(rect)):
                                #print(version_lib)
                                #print(bar_values)
                                percentage_change = reference[j] / bar_values[j]
                                height = rect[j].get_height()
                                if (percentage_change != 1):
                                    ax.annotate('{:.2f}x'.format(percentage_change),
                                        xy=(rect[j].get_x() + rect[j].get_width() / 2, height),
                                        xytext=(0, 3),  # 3 points vertical offset
                                        textcoords="offset points",
                                        ha='center', va='bottom', size=4)
           
            #configure the look of the plot
            plt.xticks(range(0,len(x_values[benchmark][variant][version_lib])), x_values[benchmark][variant][version_lib])
            for tick in ax.xaxis.get_major_ticks():
                tick.label.set_fontsize(8)
            for tick in ax.yaxis.get_major_ticks():
                tick.label.set_fontsize(8)
            ax.set_ylabel("Throughput ("+ y_axis_label + ")", fontsize=10)
            ax.set_xlabel(x_axis_label, fontsize=10)
            ax.set_title(plot_title + " - " + ' '.join(map(str, experiment_params)), fontsize=10)
            handles, labels = ax.get_legend_handles_labels()
            labels = [versions_map[label] for label in labels]
            lgd = ax.legend(handles, labels, loc='upper center', bbox_to_anchor=(0.5,-0.12))      

            #save the plot
            plot_dir = plot_folder
            plot_file_path = plot_folder + "/" + benchmark
            save_plot(plot_dir, plot_file_path, fig, lgd)
            plt.close(fig) 
    return

def bar_overhead_plot(x_values, y_values, info, x_axis_label, y_axis_label, overhead_annot, plot_folder):
    print("bar overhead plot")
    for benchmark in x_values:
        plot_title = benchmark
        for variant in x_values[benchmark]:
            experiment_params = variant.split('+')[1:]
            number_of_bars = len(x_values[benchmark][variant]) - 1 #minus the first one which is the reference
            bar_area_percentage = number_of_bars * 0.2 #0.8
            w = float(bar_area_percentage / number_of_bars) #bar width to cover 80% of the dedicated space
            x_axis_spacing = np.linspace(-bar_area_percentage/2 + w/2, bar_area_percentage/2 - w/2, num=number_of_bars)
            fig = plt.figure()
            ax = fig.add_subplot(111)

            #append values to the plot
            for version_lib in x_values[benchmark][variant]:     
                if (version_lib == "pmdk"):
                    reference = [float(i) for i in y_values[benchmark][variant][version_lib]]
                else:
                    internal_idx = (list(x_values[benchmark][variant].keys()).index(version_lib))-1
                    x_index = np.arange(0, len(x_values[benchmark][variant][version_lib]), 1) + x_axis_spacing[internal_idx]
                    lib_values = [float(i) for i in y_values[benchmark][variant][version_lib]]
                    values_to_plot = [x/y for x, y in zip(reference, lib_values)]
                    rect = ax.bar(x_index, values_to_plot, width = w, 
                                        color = colour[internal_idx], hatch = hatch[internal_idx], 
                                        edgecolor = 'black', align='center', label=version_lib)

            #configure the look of the plot
            plt.xticks(range(0,len(x_values[benchmark][variant][version_lib])), x_values[benchmark][variant][version_lib])
            #plt.xticks(range(0,standard_x_ticks), x_values[benchmark][variant]["pmdk"])
            for tick in ax.xaxis.get_major_ticks():
                tick.label.set_fontsize(8)
            for tick in ax.yaxis.get_major_ticks():
                tick.label.set_fontsize(8)
            #ax.set_ylabel(metrics[y_axis_label], fontsize=10)
            ax.set_ylabel("Relative throughput overhead w.r.t. pmdk native", fontsize=10)
            ax.set_xlabel(bench_info[x_axis_label], fontsize=10)
            ax.set_title(plot_title + " - " + ' '.join(map(str, experiment_params)), fontsize=10)
            handles, labels = ax.get_legend_handles_labels()
            labels = [versions_map[label] for label in labels]
            lgd = ax.legend(handles, labels, loc='upper center', bbox_to_anchor=(0.5,-0.12))
            
            #save the plot
            plot_dir = plot_folder
            plot_file_path = plot_folder + "/" + benchmark + "_overhead"
            save_plot(plot_dir, plot_file_path, fig, lgd)
            plt.close(fig)  
    return