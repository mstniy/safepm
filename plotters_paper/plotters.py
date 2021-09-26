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

pmembench_map = {
    "map_insert"            :   "Insert",
    "map_get"               :   "Get",
    "map_remove"            :   "Remove"
}

pmembench_open_create = {
    "obj_open_sizes_256MB_64GB"     :   "Pool open",
    "obj_create_sizes_256MB_64GB"   :   "Pool create"
}

pmembench_map_partial_cov = {
    "map_insert_partial_cov"        :   "Insert",
    "map_get_partial_cov"           :   "Get",
    "map_remove_partial_cov"        :   "Remove"
}

pmemkv = {
    "delete_random"                 :   "Random deletes",
    "delete_seq"                    :   "Sequential deletes",
    "randomreadwrite50"             :   "Random reads/writes (50%-50%)",
    "randomreadwrite95"             :   "Random reads/writes (95%-5%)",
    "read_random"                   :   "Random reads",
    "read_seq"                      :   "Sequential reads"
}

pmembench_tx = {
    "obj_atomic_alloc"              :   "Atomic alloc",
    "obj_atomic_free"               :   "Atomic free",
    "obj_atomic_realloc"            :   "Atomic realloc",
    "obj_tx_alloc"                  :   "Transactional alloc",
    "obj_tx_free"                   :   "Transactional free",
    "obj_tx_realloc"                :   "Transactional realloc"
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
    print(plot_file_path)
    fig.savefig(plot_file_path + ".pdf", dpi=300, format='pdf', bbox_extra_artists=(lgd,), bbox_inches='tight')
    fig.savefig(plot_file_path + ".png", dpi=300, format='png', bbox_extra_artists=(lgd,), bbox_inches='tight')
    
def pmembench_tx_plot_single(x_values, y_values, info, x_axis_label, y_axis_label, overhead_annot, plot_folder):
    print("pmembench_tx_plot_single")
    fig, ax = plt.subplots(1, 1)
    plot_idx = 0
    colour_local = ["white", "black", "white", "grey", "white", "lightgray"]
    hatch_local = ['' , '' , '++' , '', '/', '']
    for benchmark in x_values:
        plot_title = benchmark
        for variant in x_values[benchmark]:
            experiment_params = variant.split('+')[1:]
            number_of_bars = len(x_values) * (len(x_values[benchmark][variant]) - 1) #minus the first one which is the reference
            bar_area_percentage = 0.8
            w = float(bar_area_percentage / number_of_bars) #bar width to cover 80% of the dedicated space
            y_axis_spacing = np.linspace(-bar_area_percentage/2 + w/2, bar_area_percentage/2 - w/2, num=number_of_bars)
    
            #append values to the plot
            for version_lib in x_values[benchmark][variant]:     
                if (version_lib == "pmdk"):
                    reference = [float(i) for i in y_values[benchmark][variant][version_lib]]
                else:
                    y_index = np.arange(0, len(x_values[benchmark][variant][version_lib]), 1) + y_axis_spacing[plot_idx]
                    lib_values = [float(i) for i in y_values[benchmark][variant][version_lib]]
                    values_to_plot = [x/y for x, y in zip(reference, lib_values)]
                    rect = ax.barh(y_index, values_to_plot, height = w, 
                                        color = colour_local[plot_idx], hatch = hatch_local[plot_idx],
                                        edgecolor = 'black', align='center', label=pmembench_tx[benchmark])
                    plot_idx = plot_idx + 1
            
            #configure the look of the plot
            plt.yticks(range(0,len(x_values[benchmark][variant][version_lib])), x_values[benchmark][variant][version_lib])
            ax.yaxis.set_ticks(range(0,len(x_values[benchmark][variant][version_lib])))
            
            x_labels = x_values[benchmark][variant][version_lib]
            x_labels = [str(x) + " B" if int(x) < 1024 else str(int(int(x)/1024)) + " KB" for x in x_labels]

            ax.yaxis.set_ticklabels(x_labels)

            ax.set_xlabel("Slowdown w.r.t. native PMDK", fontsize=8)

            for tick in ax.yaxis.get_major_ticks():
                tick.label.set_fontsize(8)
            for tick in ax.xaxis.get_major_ticks():
                tick.label.set_fontsize(8)
            
            #ax.set_title(pmembench_map_partial_cov[plot_title], fontsize=10)
            handles, labels = ax.get_legend_handles_labels()
            lgd = ax.legend(handles, labels, loc='upper center', #bbox_to_anchor=(0.5,-0.12),
                                    bbox_to_anchor=(0., 1.03, 1., .102), #loc='lower left',
                                    ncol=int(len(labels)/2) , borderaxespad=0., fontsize = 6) 
       
    
    #save the plot
    plot_dir = plot_folder
    #plot_file_path = plot_folder + "/" + benchmark
    plot_file_path = plot_folder + "/" + "pmembench_tx_plot_single"
    fig.set_size_inches(3.5, 3)
    save_plot(plot_dir, plot_file_path, fig, lgd)
    plt.close(fig)       
    return

def pmembench_tx_plot_dual(x_values, y_values, info, x_axis_label, y_axis_label, overhead_annot, plot_folder):
    print("pmembench_tx_plot_dual")
    fig, ax_ = plt.subplots(1, 2)
    plot_idx_0 = 0
    plot_idx_1 = 0
    colour_local = ["white", "black", "grey"]
    for benchmark in x_values:
        plot_title = benchmark
        for variant in x_values[benchmark]:
            experiment_params = variant.split('+')[1:]
            number_of_bars = int((len(x_values) * (len(x_values[benchmark][variant]) - 1))/2) #minus the first one which is the reference
            bar_area_percentage = number_of_bars * 0.2
            w = float(bar_area_percentage / number_of_bars) #bar width to cover 80% of the dedicated space
            y_axis_spacing = np.linspace(-bar_area_percentage/2 + w/2, bar_area_percentage/2 - w/2, num=number_of_bars)

            if ("atomic" in benchmark):
                plot_idx = plot_idx_0
                ax = ax_[0]
                plot_idx_0 = plot_idx_0 + 1
            else:
                plot_idx = plot_idx_1
                ax = ax_[1]
                plot_idx_1 = plot_idx_1 + 1

            #append values to the plot
            for version_lib in x_values[benchmark][variant]:     
                if (version_lib == "pmdk"):
                    reference = [float(i) for i in y_values[benchmark][variant][version_lib]]
                else:
                    y_index = np.arange(0, len(x_values[benchmark][variant][version_lib]), 1) + y_axis_spacing[plot_idx]
                    lib_values = [float(i) for i in y_values[benchmark][variant][version_lib]]
                    values_to_plot = [x/y for x, y in zip(reference, lib_values)]
                    rect = ax.barh(y_index, values_to_plot, height = w, 
                                        color = colour_local[plot_idx],
                                        edgecolor = 'black', align='center', label=pmembench_tx[benchmark])
                    plot_idx = plot_idx + 1
            
            #configure the look of the plot
            plt.yticks(range(0,len(x_values[benchmark][variant][version_lib])), x_values[benchmark][variant][version_lib])
            
            ax.yaxis.set_ticks(range(0,len(x_values[benchmark][variant][version_lib])))
            
            x_labels = x_values[benchmark][variant][version_lib]
            x_labels = [str(x) + " B" if int(x) < 1024 else str(int(int(x)/1024)) + " KB" for x in x_labels]

            ax.yaxis.set_ticklabels(x_labels)

            #ax.set_xlabel("Slowdown w.r.t. native PMDK", fontsize=8)

            for tick in ax.yaxis.get_major_ticks():
                tick.label.set_fontsize(5)
            for tick in ax.xaxis.get_major_ticks():
                tick.label.set_fontsize(5)
            
            #ax.set_title(pmembench_map_partial_cov[plot_title], fontsize=10)
            handles, labels = ax.get_legend_handles_labels()
            lgd = ax.legend(handles, labels, loc='upper center', #bbox_to_anchor=(0.5,-0.12),
                                    bbox_to_anchor=(0., 1.22, 1., .102), #loc='lower left',
                                    ncol=int(len(labels)/2) , borderaxespad=0., fontsize = 5) 
       
    fig.text(0.5, -0.05, "Slowdown w.r.t. native PMDK", ha='center', fontsize=5)
    #save the plot
    plot_dir = plot_folder
    #plot_file_path = plot_folder + "/" + benchmark
    plot_file_path = plot_folder + "/" + "pmembench_tx_plot_dual"
    fig.set_size_inches(4.5, 1.5)
    save_plot(plot_dir, plot_file_path, fig, lgd)
    plt.close(fig)       
    return

def pmembench_open_create_plot(x_values, y_values, info, x_axis_label, y_axis_label, overhead_annot, plot_folder):
    print("pmembench_open_create_plot")
    fig1, ax1 = plt.subplots(1, 2)
    ax_idx1 = 0
    for benchmark in x_values:
        plot_title = benchmark
        for variant in x_values[benchmark]:
            ax = ax1[ax_idx1]
            experiment_params = variant.split('+')[1:]
            number_of_bars = len(x_values[benchmark][variant]) - 1 #minus the first one which is the reference
            bar_area_percentage = number_of_bars * 0.2 #0.8
            w = float(bar_area_percentage / number_of_bars) #bar width to cover 80% of the dedicated space
            x_axis_spacing = np.linspace(-bar_area_percentage/2 + w/2, bar_area_percentage/2 - w/2, num=number_of_bars)
    
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
            ax.xaxis.set_ticks(range(0,len(x_values[benchmark][variant][version_lib])))
            
            x_labels = x_values[benchmark][variant][version_lib]
            #convert objects to bytes
            #the pool size is 4 * data_size (1MB) * obj_number
            x_labels = map(lambda x: (int(x) * 4), x_labels)
            x_labels = [str(x) + " MB" if int(x) < 1024 else str(int(int(x)/1024)) + " GB" for x in x_labels]
            ax.xaxis.set_ticklabels(x_labels)

            for tick in ax.xaxis.get_major_ticks():
                tick.label.set_fontsize(8)
            for tick in ax.yaxis.get_major_ticks():
                tick.label.set_fontsize(8)
            
            ax.set_title(pmembench_open_create[plot_title], fontsize=10)
            handles, labels = ax.get_legend_handles_labels()
            labels = [versions_map[label] for label in labels]
            
            if (ax_idx1 == 0):
                ax.set_ylabel("Slowdown w.r.t. native PMDK", fontsize=10)
            elif (ax_idx1 == 1):
                lgd1 = ax.legend(handles, labels, loc='upper center', #bbox_to_anchor=(0.5,-0.12),
                                    bbox_to_anchor=(-0.6, 1.22, 1., .102), #loc='lower left',
                                    ncol=len(labels) , borderaxespad=0.) 
            
            ax_idx1 = ax_idx1 + 1         
    #save the plot
    plot_dir = plot_folder
    #plot_file_path = plot_folder + "/" + benchmark
    plot_file_path = plot_folder + "/" + "pmembench_open_create_plot"
    fig1.set_size_inches(12, 2)
    save_plot(plot_dir, plot_file_path, fig1, lgd1)
    plt.close(fig1)       
    return

def pmembench_map_plot(x_values, y_values, info, x_axis_label, y_axis_label, overhead_annot, plot_folder):
    print("pmembench_map_plot")
    fig1, ax1 = plt.subplots(1, 3)
    ax_idx1 = 0
    for benchmark in x_values:
        plot_title = benchmark
        for variant in x_values[benchmark]:
            ax = ax1[ax_idx1]
            experiment_params = variant.split('+')[1:]
            number_of_bars = len(x_values[benchmark][variant]) - 1 #minus the first one which is the reference
            bar_area_percentage = number_of_bars * 0.2 #0.8
            w = float(bar_area_percentage / number_of_bars) #bar width to cover 80% of the dedicated space
            x_axis_spacing = np.linspace(-bar_area_percentage/2 + w/2, bar_area_percentage/2 - w/2, num=number_of_bars)
    
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
            ax.xaxis.set_ticks(range(0,len(x_values[benchmark][variant][version_lib])))
            
            x_labels = x_values[benchmark][variant][version_lib]
            for i in range (len(x_labels)):
                if x_labels[i]=="hashmap_tx":
                    x_labels[i]="hashmap"

            ax.xaxis.set_ticklabels(x_labels)

            for tick in ax.xaxis.get_major_ticks():
                tick.label.set_fontsize(8)
            for tick in ax.yaxis.get_major_ticks():
                tick.label.set_fontsize(8)
            
            ax.set_title(pmembench_map[plot_title], fontsize=10)
            handles, labels = ax.get_legend_handles_labels()
            labels = [versions_map[label] for label in labels]
            
            if (ax_idx1 == 0):
                    ax.set_ylabel("Slowdown w.r.t. native PMDK", fontsize=10)
            elif (ax_idx1 == 1):
                #lgd1 = ax.legend(handles, labels, loc='upper center', bbox_to_anchor=(0.5,-0.12))
                lgd1 = ax.legend(handles, labels, loc='upper center', #bbox_to_anchor=(0.5,-0.12),
                                    bbox_to_anchor=(0., 1.22, 1., .102), #loc='lower left',
                                    ncol=len(labels) , borderaxespad=0.) 
            ax_idx1 = ax_idx1 + 1        
    
    #save the plot
    plot_dir = plot_folder
    #plot_file_path = plot_folder + "/" + benchmark
    plot_file_path = plot_folder + "/" + "pmembench_map_plot"
    fig1.set_size_inches(12, 2)
    save_plot(plot_dir, plot_file_path, fig1, lgd1)
    plt.close(fig1)       
    return

def pmembench_map_partial_cov_plot(x_values, y_values, info, x_axis_label, y_axis_label, overhead_annot, plot_folder):
    print("pmembench_map_partial_cov_plot")
    colour_local = ["white", "black", "grey"]
    fig, ax = plt.subplots(1, 1)
    plot_idx = 0
    for benchmark in x_values:
        plot_title = benchmark
        for variant in x_values[benchmark]:
            experiment_params = variant.split('+')[1:]
            number_of_bars = len(x_values) * (len(x_values[benchmark][variant]) - 1) #minus the first one which is the reference
            bar_area_percentage = number_of_bars * 0.2
            w = float(bar_area_percentage / number_of_bars) #bar width to cover 80% of the dedicated space
            x_axis_spacing = np.linspace(-bar_area_percentage/2 + w/2, bar_area_percentage/2 - w/2, num=number_of_bars)
    
            #append values to the plot
            for version_lib in x_values[benchmark][variant]:     
                if (version_lib == "pmdk"):
                    reference = [float(i) for i in y_values[benchmark][variant][version_lib]]
                else:
                    x_index = np.arange(0, len(x_values[benchmark][variant][version_lib]), 1) + x_axis_spacing[plot_idx]
                    lib_values = [float(i) for i in y_values[benchmark][variant][version_lib]]
                    values_to_plot = [x/y for x, y in zip(reference, lib_values)]
                    rect = ax.bar(x_index, values_to_plot, width = w, 
                                        color = colour_local[plot_idx],
                                        edgecolor = 'black', align='center', label=pmembench_map_partial_cov[benchmark])
                    plot_idx = plot_idx + 1
            
            #configure the look of the plot
            plt.xticks(range(0,len(x_values[benchmark][variant][version_lib])), x_values[benchmark][variant][version_lib])
            ax.xaxis.set_ticks(range(0,len(x_values[benchmark][variant][version_lib])))
            
            x_labels = x_values[benchmark][variant][version_lib]
            x_labels = [x + "%" for x in x_labels]
            
            ax.xaxis.set_ticklabels(x_labels)

            ax.set_xlabel("Unsafe objects percentage", fontsize=6)
            ax.set_ylabel("Slowdown w.r.t. native PMDK", fontsize=6)

            for tick in ax.xaxis.get_major_ticks():
                tick.label.set_fontsize(6)
            for tick in ax.yaxis.get_major_ticks():
                tick.label.set_fontsize(6)
            
            #ax.set_title(pmembench_map_partial_cov[plot_title], fontsize=10)
            handles, labels = ax.get_legend_handles_labels()
            lgd = ax.legend(handles, labels, loc='upper center', #bbox_to_anchor=(0.5,-0.12),
                                    bbox_to_anchor=(0., 1.1, 1., .102), #loc='lower left',
                                    ncol=len(labels) , borderaxespad=0., fontsize = 5) 
       
    
    #save the plot
    plot_dir = plot_folder
    #plot_file_path = plot_folder + "/" + benchmark
    plot_file_path = plot_folder + "/" + "pmembench_map_partial_cov_plot"
    fig.set_size_inches(3, 1.)
    save_plot(plot_dir, plot_file_path, fig, lgd)
    plt.close(fig)       
    return

def pmemkv_plot(x_values, y_values, info, x_axis_label, y_axis_label, overhead_annot, plot_folder):
    print("pmemkv_plot")
    fig, ax = plt.subplots(1, 4)
    ax_idx = 0
    for benchmark in x_values:
        plot_title = benchmark
        for variant in x_values[benchmark]:
            experiment_params = variant.split('+')[1:]
            #number_of_bars = 3
            number_of_bars = len(x_values[benchmark][variant])
            bar_area_percentage = 0.8
            w = float(bar_area_percentage / number_of_bars) #bar width to cover 80% of the dedicated space
            x_axis_spacing = np.linspace(-bar_area_percentage/2 + w/2, bar_area_percentage/2 - w/2, num=number_of_bars)

            #append values to the plot
            for version_lib in x_values[benchmark][variant]:     
                internal_idx = list(x_values[benchmark][variant].keys()).index(version_lib)
                x_index = np.arange(0, len(x_values[benchmark][variant][version_lib]), 1) + x_axis_spacing[internal_idx]
                rect = ax[ax_idx].bar(x_index, [float(i) for i in y_values[benchmark][variant][version_lib]], width = w, 
                                    color = colour[internal_idx], hatch = hatch[internal_idx],
                                    edgecolor = 'black', align='center', label=version_lib)

            #configure the look of the plot
            custom_x_ticks = list(map(float,x_values[benchmark][variant][version_lib]))
            custom_x_ticks = [round(a) for a in custom_x_ticks]

            ax[ax_idx].xaxis.set_ticks(range(0,len(x_values[benchmark][variant][version_lib])))
            ax[ax_idx].xaxis.set_ticklabels(custom_x_ticks)
            #plt.xticks(range(0,standard_x_ticks), x_values[benchmark][variant]["pmdk"])
            for tick in ax[ax_idx].xaxis.get_major_ticks():
                tick.label.set_fontsize(8)
            for tick in ax[ax_idx].yaxis.get_major_ticks():
                tick.label.set_fontsize(8)
            ax[ax_idx].yaxis.offsetText.set_fontsize(8)

            if (ax_idx == 0):
                if (y_axis_label == "ops/sec"):
                    ax[ax_idx].set_ylabel("Ops/sec", fontsize=10)

            ax[ax_idx].ticklabel_format(axis='y', style='sci', scilimits=(6,6)) 

            ax[ax_idx].set_title(pmemkv[plot_title], fontsize=8)
            handles, labels = ax[ax_idx].get_legend_handles_labels()
            labels = [versions_map[label] for label in labels]
            if (ax_idx == 2): #to set the legend once
                lgd = ax[ax_idx].legend(handles, labels,  #bbox_to_anchor=(0.5,-0.12),
                                        bbox_to_anchor=(-0.7, 1.32), loc='upper left', borderaxespad=0.,
                                        ncol=len(labels), fontsize=10)     
            ax_idx = ax_idx + 1
    
    fig.text(0.5, -0.025, 'Threads', ha='center', fontsize=10)

    #save the plot
    plot_dir = plot_folder
    #plot_file_path = plot_folder + "/" + benchmark
    plot_file_path = plot_folder + "/" + "pmemkv_plot"
    fig.set_size_inches(14, 2.2)
    save_plot(plot_dir, plot_file_path, fig, lgd)
    plt.close(fig)       
    return