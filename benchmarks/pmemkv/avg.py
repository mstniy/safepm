import os, csv, argparse
from collections import defaultdict

avgValues = [
    "Median [micros/op]",
    "Open [millis/op]",
    "Percentilie P50.000000 [micros/op]",
    "Percentilie P75.000000 [micros/op]",
    "Percentilie P90.000000 [micros/op]",
    "Percentilie P99.900000 [micros/op]",
    "Percentilie P99.990000 [micros/op]",
    "RawSize [MB (estimated)]",
    "micros/op (avarage)",
    "ops/sec",
    "throughput [MB/s]"
]

def calculateAverage(files: list, resultPath: str):
    # If no files where provided exit
    if not files:
        print("No files found")
        return
    # Structure {linenumber : dictOfValues}
    avgDict = defaultdict(lambda: defaultdict(str))
    # Filename for details is: resultpath_details.csv
    detailsFileName = resultPath.replace('.csv', '_details.csv')
    # Empty file content
    open(detailsFileName, 'w').close()

    for f in files:
        # Parse csv to dict
        file = open(f, 'r')
        res = csv.DictReader(file)

        for row in res:
            for key, value in row.items():
                # Make sure operation is a float and not empty
                if key in avgValues and avgDict[res.line_num][key]:
                    try:
                        avgDict[res.line_num][key] = float(avgDict[res.line_num][key]) + float(value)
                    except ValueError as e:
                        print("Value error: "+str(key)+" "+str(value))
                        raise e
                else:
                    avgDict[res.line_num][key] = value

        # Read file and append to details file
        file.seek(0)
        content = file.read()
        filename=os.path.basename(f)
        # Write file to summary file
        with open(detailsFileName, 'a') as detailsFile:
            detailsFile.write("{}\n".format(filename))
            detailsFile.write(content)

        file.close()
        os.remove(f)
            
    # Write average file
    with open(resultPath, 'w', newline='') as csvfile:
        csvwriter = csv.writer(csvfile, delimiter=',')
        
        for i in range(1,max(avgDict.keys())+1):
            # Dict begins with line 2 because first line are headers
            if i == 1:
                # Take the keys found in the second line as headers for the file 
                csvwriter.writerow(avgDict[2].keys())
            else:
                linestr = []
                for key, value in avgDict[i].items():
                    # Float values need to be converted to correct values
                    if key in avgValues and avgDict[res.line_num][key]:
                        linestr.append("{:.6f}".format(float(value)/len(files)))
                    else:
                        linestr.append(value)
                csvwriter.writerow(linestr)


def main():
    parser = argparse.ArgumentParser(description='Writes all files into one file by calculating the average off all values')

    optional = parser._action_groups.pop()
    requiredGroup = parser.add_argument_group("required named arguments")

    requiredGroup.add_argument("-f","--files", required=True, dest="files", type=str, nargs='+', metavar="FILES",
                help="List of files for which the average values should be calculated")
    requiredGroup.add_argument("-r","--result", required=True, dest="resultFilename", type=str, metavar="FILENAME",
                help="Filename for the result of the calculated average file")

    parser._action_groups.append(optional) # Make sure optional is last
    args = parser.parse_args()

    #print("{} {}".format(args.files, args.resultFilename))

    calculateAverage(args.files, args.resultFilename)


if __name__ == '__main__':
    main()