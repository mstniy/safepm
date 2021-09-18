import os
from collections import namedtuple

def parse(lib_version, res_file):

    results = {} #dictionary for the results of all the benchmarks
    line_category = {    
        "info": True,
        "labels": False,
        "values": 0
    }

    print("Parsing starts for " + res_file )
    result_file = open(res_file, 'r')
    for line in result_file: 
        line = line.replace('\n','')
        #print(line)
        if (line_category["info"]): #case of benchmark info line
            splitted_line = line.split()
            name = splitted_line[0][:-1] #remove ':'
            category = splitted_line[1]
            number_of_experiments = splitted_line[2][1:-1] #remove '[]'
            group = splitted_line[4][:-1] #remove ']'
            results[name] = []
            results[name].append([lib_version, category, number_of_experiments, group])
            line_category["info"] = False
            line_category["labels"] = True
        elif (line_category["labels"]): #case of label line
            labels = line.split(';')
            results[name].append(labels)
            line_category["labels"] = False
            results[name].append([]) #init the list of the values      
        elif (int(line_category["values"]) < int(number_of_experiments)): #number of lines to be read for values
            values = line.split(';')
            results[name][2].append(values)
            line_category["values"] = int(line_category["values"]) + 1
            if (int(line_category["values"]) == int(number_of_experiments)): #new benchmark in the next line
                line_category["info"] = True
                line_category["labels"] = False
                line_category["values"] = 0
        
    return results
