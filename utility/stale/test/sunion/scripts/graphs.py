#!/usr/bin/env python
from sys import *
import os

import Scientific.Statistics

RUNTIME=0
NB_INPUT_STREAMS=1
BENCHMARK=2
FAILURE_DURATION=3
STATE_SIZE=4
TUPLE_RATE=5
PUNCTUATION=6
MAX_DELAY=7
CHECKPOINT_INTERVAL=8
MAX_HISTORY=9
NB_BOXES=10

STAGGER=11
REPLICA_STAGGER=12

MAX_E2E=13
AVG_E2E=14
NB_ERRONOUS=15

# If exists, the number of nodes is added at the end
NB_NODES=16

true = 1
false = 0

TITLES = {}
TITLES["NONE"] = "No fault-tolerance"
TITLES["none"] = "No fault-tolerance"
TITLES["TUPLE_TYPES"] = "Tuple types"
TITLES["tuple_types"] = "Tuple types"
TITLES["UNDO_LOG"] = "Undo buffer"
TITLES["undo_log"] = "Undo buffer"
TITLES["CHECKPOINT"] = "Checkpoints"
TITLES["checkpoint"] = "Checkpoints"
TITLES["SUNION"] = "SUnion"
TITLES["sunion"] = "SUnion"
TITLES["SUNION_PUNCTUATION"] = "SUnion & punct."
TITLES["sunion_punctuation"] = "SUnion & punct."
TITLES["SOUTPUT"] = "SOutput"
TITLES["soutput"] = "SOutput"
TITLES["BLOCK"] = "Block"
TITLES["block"] = "Block"
TITLES["ALL_TOGETHER_UNDO_REDO_NO_MOVE"] = "Undo/redo"
TITLES["all_together_undo_redo_no_move"] = "Undo/redo"
TITLES["ALL_TOGETHER_CHECKPOINT_REDO_NO_MOVE"] = "Checkpoint/redo"
TITLES["all_together_checkpoint_redo_no_move"] = "Checkpoint/redo"
TITLES["ALL_TOGETHER_UNDO_REDO"] = "Undo/redo & coll."
TITLES["all_together_undo_redo"] = "Undo/redo & coll."
TITLES["ALL_TOGETHER_CHECKPOINT_REDO"] = "Checkpoint/redo & coll."
TITLES["all_together_checkpoint_redo"] = "Checkpoint/redo & coll."
TITLES["all_together_undo_redo_n"] = "Block continuously"
TITLES["ADAPTIVE"] = "Adaptive"
TITLES["adaptive"] = "Adaptive"

TITLES["all_together_checkpoint_redo00"] = "Process & process";
TITLES["all_together_checkpoint_redo01"] = "Process & delay";
TITLES["all_together_checkpoint_redo10"] = "Delay & process";
TITLES["all_together_checkpoint_redo11"] = "Delay & delay";
TITLES["all_together_checkpoint_redo_no_move00"] = "Process & suspend";
TITLES["all_together_checkpoint_redo_no_move10"] = "Delay & supsend";

# --------------------------------------------------
# x_ref_columns are the indexes of the other columns that we use
# as x value. We will find the most frequent values in these columns
# and then compute averages only for lines that contain these most frequent values
def computeAverages1(lines, y_column, x_column, x_ref_columns):
    return averages(lines,y_column,x_column,-1, -1, x_ref_columns)

# --------------------------------------------------
def computeAverages2(lines, y_column, x_column1,x_column2, x_ref_columns):
    return averages(lines,y_column,x_column1,x_column2,-1, x_ref_columns)

# --------------------------------------------------
def computeAverages3(lines, y_column, x_column1,x_column2, x_column3, x_ref_columns):
    return averages(lines,y_column,x_column1,x_column2, x_column3, x_ref_columns)


# --------------------------------------------------
def computeAveragesWithFilter(lines, y_column, x_column1, x_ref_columns, benchmark_filter):
    results = computeAverages1(lines,y_column,x_column1, x_ref_columns)

    # Keep only those benchmarks that appear in the benchmark_filter
    results_filtered = {}
    for result in results.items():
        x_value = result[0]
        benchmark_values = result[1]
        results_filtered[x_value] = {}        
        
        for benchmark in benchmark_values.items():
            benchmark_name = benchmark[0]
            value = benchmark[1]
            if benchmark_filter.has_key(benchmark_name):
                results_filtered[x_value][benchmark_name] = value
    return results_filtered


# --------------------------------------------------
def valid_line(items,x_ref_values):

    is_valid = true
    for x_ref in x_ref_values.items():
        index = x_ref[0]
        value = x_ref[1]
        if ( int(items[index]) != value):
            is_valid = false
    #print "Line %s validity is %s" % (items,is_valid)
    return is_valid

# --------------------------------------------------
def compute_ref_values(lines, x_ref_columns):

    # For each x_column
    temp_values = {}
    for x_ref in x_ref_columns:
        temp_values[x_ref] = {}

    # Compute nb of samples for each value that we see
    for line in lines:
        items = line.split()
        for x_ref in x_ref_columns:
            value = int(items[x_ref])
            if not temp_values[x_ref].has_key(value):
                temp_values[x_ref][value] = 0
            temp_values[x_ref][value] += 1

    # Returns a map that associates the most frequent value for each column
    x_ref_values = {}

    for x_ref in x_ref_columns:
        max_value_counter = -1
        max_value = 0
        for values in temp_values[x_ref].items():
            if values[1] > max_value_counter:
                max_value_counter = values[1]
                max_value = values[0]
        x_ref_values[x_ref] = max_value
        #print "Max value for column %s is %d" % (x_ref,max_value)

    return x_ref_values

# --------------------------------------------------
# Expects raw data in the form of one experiment on one line
# and columns as defined in the global variables above
# Returns a structure as follows:
# For each benchmark
#     For each x_value
#         results[x_value][benchmark] = y_value
# --------------------------------------------------
def averages(lines,y_column,x_column1,x_column2, x_column3, x_ref_columns) :

    x_ref_values = compute_ref_values(lines, x_ref_columns)
    
    results = {}
    temp_results = {}
    TOTAL = 0
    NB_MEASUREMENTS=1

    # Just compute the sum of all y_values for each benchmark and each x_value
    for line in lines:
        values = line.split()
        benchmark = values[BENCHMARK]        
        x_value = int(values[x_column1])
        if ( x_column2 != -1 ):
            x_value = int(values[x_column1])*int(values[x_column2])
        if ( x_column3 != -1 ):
            x_value = int(values[x_column1])*int(values[x_column2])*int(values[x_column3])/1000
        y_value = float(values[y_column])

        if not temp_results.has_key(x_value):            
            temp_results[x_value] = {}

        if not temp_results[x_value].has_key(benchmark):
            temp_results[x_value][benchmark] = []

        if valid_line(values,x_ref_values):
            temp_results[x_value][benchmark].append(y_value)

    # Now compute the averages of these y_values
    for x_results in temp_results.items():
        x = x_results[0] # key
        x_values = x_results[1] #value
        results[x] = {}
        for benchmark_results in x_values.items():
            benchmark = benchmark_results[0]
            benchmark_values = benchmark_results[1]
            #Eliminate best and wors and then compute average
            benchmark_values.sort()
            avg = Scientific.Statistics.mean(benchmark_values)
            stddev = 0
            if ( len(benchmark_values) > 1):
                stddev = Scientific.Statistics.standardDeviation(benchmark_values)
            results[x][benchmark] = (avg,stddev)
    return results

# --------------------------------------------------
# Returns a structure as follows:
# For each benchmark
#     For each x_value
#         results[x_value][benchmark-with-stagger] = y_value
# --------------------------------------------------
def staggeraverages(lines,y_column,x_column1,x_column2, x_column3, x_ref_columns) :

    x_ref_values = compute_ref_values(lines, x_ref_columns)
    
    results = {}
    temp_results = {}
    TOTAL = 0
    NB_MEASUREMENTS=1

    # Just compute the sum of all y_values for each benchmark and each x_value
    for line in lines:
        values = line.split()
        benchmark = values[BENCHMARK]        
        x_value = int(values[x_column1])
        if ( x_column2 != -1 ):
            x_value = int(values[x_column1])*int(values[x_column2])
        if ( x_column3 != -1 ):
            x_value = int(values[x_column1])*int(values[x_column2])*int(values[x_column3])/1000
        y_value = float(values[y_column])

        if not temp_results.has_key(x_value):            
            temp_results[x_value] = {}

        if valid_line(values,x_ref_values):
            benchmarkWithStagger = "%s%s%s" % (benchmark, values[STAGGER], values[REPLICA_STAGGER])
            if not temp_results[x_value].has_key(benchmarkWithStagger):
                temp_results[x_value][benchmarkWithStagger] = []
            temp_results[x_value][benchmarkWithStagger].append(y_value)

    # Now compute the averages of these y_values
    for x_results in temp_results.items():
        x = x_results[0] # key
        x_values = x_results[1] #value
        results[x] = {}
        for benchmark_results in x_values.items():
            benchmark = benchmark_results[0]
            benchmark_values = benchmark_results[1]
            #Eliminate best and wors and then compute average
            benchmark_values.sort()
            avg = Scientific.Statistics.mean(benchmark_values)
            stddev = 0
            if ( len(benchmark_values) > 1):
                stddev = Scientific.Statistics.standardDeviation(benchmark_values)
            results[x][benchmark] = (avg,stddev)
    return results


# --------------------------------------------------
# Expects data in format:
# x_value -> (benchmark, y_value), (benchmark, y_value)
# x_value -> (benchmark, y_value), (benchmark, y_value)
# but as maps so it is like:
# results[x_value][benchmark] -> y_value
# Returns the maximum x and y values seen
def writeResults(results,outFileName):

    max_x = 0
    max_y = 0

    lines = []
    for result in results.items():
        x_value = result[0]
        if ( max_x < x_value ):
            max_x = x_value
        y_values = result[1]
        line = []
        line.append( x_value )
        for y_value in y_values.items():
            benchmark = y_value[0]
            #y = y_value[1]
            y = y_value[1][0]
            stddev = y_value[1][1]
            if ( max_y < y ):
                max_y = y
            #line.append(" %s %s " % (benchmark,y))
            line.append(" %s %s %s" % (benchmark,y,stddev))
        lines.append(line)

    lines.sort()
    
    outFile = open(outFileName,"w")    
    for line in lines:
        for element in line:
            outFile.write(str(element))
        outFile.write("\n")
    outFile.close()
    
    #print "Max values are %d,%d " % (max_x,max_y)
    return (max_x,max_y)

#-----------------------------------------
# Data file should be in format:
# x_value plot_name y_value plot_name y_value ...
# Will plot all the y_values vs the x_value
#-----------------------------------------
def produceGnuplotFile(dataFileName, x_label, y_label,xy_max):
    
    # Total results
    max_x = xy_max[0]
    max_y = 1.5*xy_max[1]
    outGnuplotName = "%s.gnuplot" % dataFileName
    outGnuplot = open(outGnuplotName,"w")
    outGnuplot.write("set terminal postscript eps 28\n\n")
    outGnuplot.write("set output \"%s.eps\"\n" % (dataFileName))
    outGnuplot.write("set xlabel \"%s\"\n" % (x_label))
    outGnuplot.write("set ylabel \"%s\"\n" % (y_label))    
    outGnuplot.write("set xrange [-1:%d]\n" % max_x);
    outGnuplot.write("set yrange [0:%d]\n" % max_y);
    if ( max_x > 110000):
        outGnuplot.write("set xtics 30000\n");
    elif ( max_x > 75000):
        outGnuplot.write("set xtics 20000\n");
    elif ( max_x > 30000):
        outGnuplot.write("set xtics 10000\n");
    elif ( max_x > 11000):
        outGnuplot.write("set xtics 3000\n");
    elif ( max_x > 7500):
        outGnuplot.write("set xtics 2000\n");        
    elif ( max_x > 3000):
        outGnuplot.write("set xtics 1000\n");
    #outGnuplot.write("set key bottom right\n")

    dataFile = open(dataFileName)
    line = dataFile.readlines()[0]
    line_elements = line.split()
    dataFile.close()
    
    nb_plots = (len(line_elements) - 1)/3
        
    for i in range(1,nb_plots+1):

        title_column = (i*3)-1
        data_column = title_column + 1
        stddev_column = data_column + 1
        title = TITLES[line_elements[title_column - 1]]
        if i == 1 :
            outGnuplot.write("plot \"%s\" using 1:%d:%d title \"\" with errorbars" % (dataFileName,data_column,stddev_column))
            outGnuplot.write(",\"%s\" using 1:%d title \"%s\" with linespoints" % (dataFileName,data_column,title))
        else:
            outGnuplot.write(", \"%s\" using 1:%d:%d title \"\" with errorbars" % (dataFileName,data_column,stddev_column))
            outGnuplot.write(", \"%s\" using 1:%d title \"%s\" with linespoints" % (dataFileName,data_column,title))
    outGnuplot.write("\n");

    outGnuplot.close()


#-----------------------------------------
# Takes as input a raw data file from microbenchmark1 and produces graphs
def microbenchmark1():

    inFileName="microbenchmark1.dat"

    # Process raw data
    inFile = open(inFileName)    
    lines = inFile.readlines();
    inFile.close()    
    state_size_results = computeAverages1(lines, AVG_E2E, STATE_SIZE, [TUPLE_RATE])
    tuple_rate_results = computeAverages2(lines, AVG_E2E, NB_INPUT_STREAMS, TUPLE_RATE, [STATE_SIZE])

    # Write processed data to temporary files
    outFileName1 = "%s.state_size" % inFileName
    outFileName2 = "%s.tuple_rate" % inFileName    
    max_values1 = writeResults(state_size_results,outFileName1)
    max_values2 = writeResults(tuple_rate_results,outFileName2)

    # Produce the gnuplot files and the graphs
    produceGnuplotFile(outFileName1, "State size (tuple)", "Avg proc delay (msec)",max_values1)
    produceGnuplotFile(outFileName2, "Agg input rate (tuple/sec)", "Avg proc delay (msec)",max_values2)

    os.system("gnuplot %s.gnuplot" % outFileName1);
    os.system("gnuplot %s.gnuplot" % outFileName2);

    os.system("epstopdf %s.eps" % (outFileName1))
    os.system("epstopdf %s.eps" % (outFileName2))


#-----------------------------------------
# Produces graphs for second microbenchmark
def microbenchmark2():

    inFileName="microbenchmark2.dat"

    # Process raw data
    inFile = open(inFileName)    
    lines = inFile.readlines();
    inFile.close()

    benchmarks = {}
    benchmarks["checkpoint"] = true
    results_ci     = computeAveragesWithFilter(lines, AVG_E2E, CHECKPOINT_INTERVAL, [PUNCTUATION], benchmarks)
        
    benchmarks = {}
    benchmarks["sunion"] = true
    benchmarks["sunion_punctuation"] = true

    results_punct  = computeAveragesWithFilter(lines, AVG_E2E, PUNCTUATION, [CHECKPOINT_INTERVAL], benchmarks)

    # Write processed data to temporary files
    outFileName1 = "%s.ci" % inFileName
    outFileName2 = "%s.punct" % inFileName    
    max_values1 = writeResults(results_ci,outFileName1)
    max_values2 = writeResults(results_punct,outFileName2)

    # Produce the gnuplot files and the graphs
    produceGnuplotFile(outFileName1, "Checkpoint interval (msec)", "Avg proc delay (msec)", max_values1)
    produceGnuplotFile(outFileName2, "Punctuation interval (msec)", "Avg proc delay (msec)", max_values2)

    os.system("gnuplot %s.gnuplot" % outFileName1);
    os.system("gnuplot %s.gnuplot" % outFileName2);
    os.system("epstopdf %s.eps" % (outFileName1))
    os.system("epstopdf %s.eps" % (outFileName2))

#-----------------------------------------
# Produces graphs for third microbenchmark
# These are in a different format. No averages needed
# We're going to read files ConsistencyMngr.log.micro3 and SJoinQBox.log.micro3
# and merge them into a single file.
def microbenchmark3():

    BENCHMARK_INDEX = 0
    STATE_SIZE_INDEX = 2 # X_values
    MEDIAN_DURATION = 4 # Y_values
    P25_DURATION = 6 # Y_values
    P75_DURATION = 6 # Y_values

    inFileNames= ("ConsistencyMngr.log.micro3", "SJoinQBox.log.micro3")
    results = {}

    benchmark_values = {}
    
    for inFileName in inFileNames:
        inFile = open(inFileName)    
        lines = inFile.readlines();
        inFile.close()
        
        for line in lines:
            items = line.split()
            benchmark = items[BENCHMARK_INDEX]
            x_value = int(items[STATE_SIZE_INDEX])
            y_value = int(items[MEDIAN_DURATION])
            y2_value = int(items[P25_DURATION])
            y3_value = int(items[P75_DURATION])
            if not results.has_key(x_value):
                results[x_value] = {}
            results[x_value][benchmark] = y_value
            if not benchmark_values.has_key(benchmark):
                benchmark_values[benchmark] = []
            benchmark_values[benchmark].append( float(y_value)/float(x_value) )
        # end_for
    # end_for
            
    # Write processed data to temporary files
    outFileName = "cpu_overhead"
    max_values = writeResults(results,outFileName)

    # Produce the gnuplot files and the graphs
    produceGnuplotFile(outFileName, "State size (tuples)", "Overhead per iteration (usec)",max_values)

    os.system("gnuplot %s.gnuplot" % outFileName);
    os.system("epstopdf %s.eps" % outFileName)

    print "\n\n"
    for pair in benchmark_values.items():
        benchmark = pair[0]
        list_value = pair[1]
        total = 0        
        for value in list_value:
            total += value
        print "Total is %.2f" % total
        print "Number items is %d" % len(list_value)
        print "Overall average overhead for benchmark %s is %.2f" % (benchmark,total/len(list_value))
    print "\n\n"

#-----------------------------------------
# Produces graphs for basic experiments 1, 2, and 3
def experiment(inFileName):

    # Process raw data
    inFile = open(inFileName)    
    lines = inFile.readlines();
    inFile.close()

    state_size_results = computeAverages2(lines, MAX_E2E, STATE_SIZE, NB_BOXES, [TUPLE_RATE,FAILURE_DURATION])
    tuple_rate_results = computeAverages3(lines, MAX_E2E, NB_INPUT_STREAMS, TUPLE_RATE, FAILURE_DURATION, [STATE_SIZE,FAILURE_DURATION])
    #failure_dur_results = computeAverages1(lines, MAX_E2E, FAILURE_DURATION, [TUPLE_RATE,STATE_SIZE])

    # Write processed data to temporary files
    outFileName1 = "%s.state_size" % inFileName
    outFileName2 = "%s.tuple_rate" % inFileName
    #outFileName3 = "%s.fail_dur" % inFileName    
    max_values1 = writeResults(state_size_results,outFileName1)
    max_values2 = writeResults(tuple_rate_results,outFileName2)
    #max_values3 = writeResults(failure_dur_results,outFileName3)

    # Produce the gnuplot files and the graphs
    produceGnuplotFile(outFileName1, "State size (tuple)", "Max. proc. delay (msec)",max_values1)
    produceGnuplotFile(outFileName2, "Failure size (tuples) ", "Max. proc. delay (msec)",max_values2)
    #produceGnuplotFile(outFileName3, "Failure duration (msec)", "Max. proc. delay (msec)",max_values3)

    # Produce the gnuplot files and the graphs
    os.system("gnuplot %s.gnuplot" % outFileName1);
    os.system("gnuplot %s.gnuplot" % outFileName2);
    #os.system("gnuplot %s.gnuplot" % outFileName3);

    os.system("epstopdf %s.eps" % (outFileName1))
    os.system("epstopdf %s.eps" % (outFileName2))
    #os.system("epstopdf %s.eps" % (outFileName3))





#-----------------------------------------
# Benefits of blocking (first one to plot nb duplicate tuples)
def benefits1(inFileName):

    #inFileName="benefits1.dat"

    # Process raw data
    inFile = open(inFileName)    
    lines = inFile.readlines();
    inFile.close()

    delay_results = computeAverages1(lines, MAX_E2E, MAX_DELAY, [])
    dup_results = computeAverages1(lines, NB_ERRONOUS , MAX_DELAY, []) 

    # Write processed data to temporary files
    outFileName1 = "%s.delay" % inFileName
    outFileName2 = "%s.dup" % inFileName
    max_values1 = writeResults(delay_results,outFileName1)
    max_values2 = writeResults(dup_results,outFileName2)

    # Produce the gnuplot files and the graphs
    produceGnuplotFile(outFileName1, "Blocking delay (msec)", "Max. proc. delay (msec)",max_values1)
    produceGnuplotFile(outFileName2, "Blocking delay (msec)", "Nb unstable tuples",max_values2)

    # Produce the gnuplot files and the graphs
    os.system("gnuplot %s.gnuplot" % outFileName1);
    os.system("gnuplot %s.gnuplot" % outFileName2);

    os.system("epstopdf %s.eps" % (outFileName1))
    os.system("epstopdf %s.eps" % (outFileName2))

#-----------------------------------------
# Benefits of moving subscriptions
def benefits(inFileName):

    #inFileName="benefits2.dat"

    # Process raw data
    inFile = open(inFileName)    
    lines = inFile.readlines();
    inFile.close()

    delay_results = computeAverages1(lines, MAX_E2E, FAILURE_DURATION, [])
    dup_results = computeAverages1(lines, NB_ERRONOUS , FAILURE_DURATION, []) 

    # Write processed data to temporary files
    outFileName1 = "%s.delay" % inFileName
    outFileName2 = "%s.dup" % inFileName
    max_values1 = writeResults(delay_results,outFileName1)
    max_values2 = writeResults(dup_results,outFileName2)

    # Produce the gnuplot files and the graphs
    produceGnuplotFile(outFileName1, "Failure duration (msec)", "Max. proc. delay (msec)",max_values1)
    produceGnuplotFile(outFileName2, "Failure duration (msec)", "Nb unstable tuples",max_values2)

    # Produce the gnuplot files and the graphs
    os.system("gnuplot %s.gnuplot" % outFileName1);
    os.system("gnuplot %s.gnuplot" % outFileName2);

    os.system("epstopdf %s.eps" % (outFileName1))
    os.system("epstopdf %s.eps" % (outFileName2))

#-----------------------------------------
# Benefits of moving subscriptions
def newbenefits(inFileName):

    #inFileName="benefits2.dat"

    # Process raw data
    inFile = open(inFileName)    
    lines = inFile.readlines();
    inFile.close()

    delay_results = staggeraverages(lines, MAX_E2E, FAILURE_DURATION, -1, -1, [])
    dup_results = staggeraverages(lines, NB_ERRONOUS , FAILURE_DURATION, -1, -1, []) 

    # Write processed data to temporary files
    outFileName1 = "%s.delay" % inFileName
    outFileName2 = "%s.dup" % inFileName
    max_values1 = writeResults(delay_results,outFileName1)
    max_values2 = writeResults(dup_results,outFileName2)

    # Produce the gnuplot files and the graphs
    produceGnuplotFile(outFileName1, "Failure duration (msec)", "Max. proc. delay (msec)",max_values1)
    produceGnuplotFile(outFileName2, "Failure duration (msec)", "Nb unstable tuples",max_values2)

    # Produce the gnuplot files and the graphs
    os.system("gnuplot %s.gnuplot" % outFileName1);
    os.system("gnuplot %s.gnuplot" % outFileName2);

    os.system("epstopdf %s.eps" % (outFileName1))
    os.system("epstopdf %s.eps" % (outFileName2))


#-----------------------------------------
def mergeFiles(inFileNamePrefix,outFileName):

    done = false
    counter = 0;
    outFile = open(outFileName,"w")

    # We assume that there were no experiments wit more than 10 ndoes
    while not done:
        
        counter += 1 
        inFileName = "%s%d" % (inFileNamePrefix,counter)
        try:
            inFile = open(inFileName)
        except IOError:
            done = true

        if not done:

            lines = inFile.readlines();
            inFile.close()

            line_nb = 1
            for line in lines:
                items = line.split()
                for item in items:
                    outFile.write("%s " % item)
                outFile.write("%d\n" % line_nb)
                line_nb += 1

    outFile.close()
    
#-----------------------------------------
def manynodes(inFileNamePrefix):

    outFileName = "%s.merged" % inFileNamePrefix
    mergeFiles(inFileNamePrefix,outFileName)

    mergedFile = open(outFileName)
    lines = mergedFile.readlines();

    delay_results = computeAverages1(lines, MAX_E2E, NB_NODES, [])
    dup_results = computeAverages1(lines, NB_ERRONOUS , NB_NODES, []) 

    # Write processed data to temporary files
    outFileName1 = "%s.delay" % inFileNamePrefix
    outFileName2 = "%s.dup" % inFileNamePrefix
    max_values1 = writeResults(delay_results,outFileName1)
    max_values2 = writeResults(dup_results,outFileName2)

    # Produce the gnuplot files and the graphs
    #produceGnuplotFile(outFileName1, "Number of nodes", "Max. proc. delay (msec)",max_values1)
    #produceGnuplotFile(outFileName2, "Number of nodes", "Nb unstable tuples",max_values2)

    # Produce the gnuplot files and the graphs
    #os.system("gnuplot %s.gnuplot" % outFileName1);
    #os.system("gnuplot %s.gnuplot" % outFileName2);

    #os.system("epstopdf %s.eps" % (outFileName1))
    #os.system("epstopdf %s.eps" % (outFileName2))


#-----------------------------------------
if __name__ == '__main__':

    if ( len(argv) < 2) :
        print "Please indicate what kinds of graphs to produce: "
        print " -> micro1, micro2, experiment1, experiment2, experiment3, benefits1, benefits1o, benefits2, adaptive1, many-nodes"
    else:

        whatToGenerate = argv[1]
        if ( whatToGenerate == "micro1"):
            microbenchmark1()
        elif (whatToGenerate == "micro2"):
            microbenchmark2()
        elif (whatToGenerate == "micro3"):
            microbenchmark3()
        elif (whatToGenerate == "experiment1"):
            experiment("experiment1.dat")
        elif (whatToGenerate == "experiment2"):
            experiment("experiment2.dat")
        elif (whatToGenerate == "experiment3"):
            experiment("experiment3.dat")
        elif (whatToGenerate == "benefits1"):
            benefits1("benefits1.dat")
        elif (whatToGenerate == "benefits1o"):
            benefits1("benefits1.dat.original")
        elif (whatToGenerate == "benefits2"):
            #benefits("benefits2.dat")
            newbenefits("benefits2.dat")
        elif (whatToGenerate == "adaptive1"):
            benefits("adaptive1.dat")
        elif (whatToGenerate == "many-nodes"):
            manynodes("many-nodes.dat.4.PP")            
            manynodes("many-nodes.dat.4.DD")
            manynodes("many-nodes.dat.4.PS")
        else:
            print "Don't know how to generate graphs for %s" % whatToGenerate
         
        



# #-----------------------------------------
# # Produces graphs for basic experiments
# def experiment2():

#     inFileName="experiment2.dat"

#     # Process raw data
#     inFile = open(inFileName)    
#     lines = inFile.readlines();
#     inFile.close()

#     state_size_results = computeAverages1(lines, MAX_E2E, STATE_SIZE, NB_BOXES, [TUPLE_RATE,FAILURE_DURATION])
#     tuple_rate_results = computeAverages2(lines, MAX_E2E, NB_INPUT_STREAMS, TUPLE_RATE, [STATE_SIZE,FAILURE_DURATION])


#     # Write processed data to temporary files
#     outFileName2 = "%s.tuple_rate" % inFileName
#     max_values2 = writeResults(tuple_rate_results,outFileName2)

#     # Produce the gnuplot files and the graphs
#     produceGnuplotFile(outFileName2, "Agg input rate (tuple/sec)", "Max. proc. delay (msec)",max_values2)

#     # Produce the gnuplot files and the graphs
#     os.system("gnuplot %s.gnuplot" % outFileName2);
#     os.system("epstopdf %s.eps" % (outFileName2))

# #-----------------------------------------
# # Exactly the same as experiment1 but we have 2 boxes this time
# def experiment3():

#     inFileName="experiment3.dat"

#     # Process raw data
#     inFile = open(inFileName)    
#     lines = inFile.readlines();
#     inFile.close()

#     state_size_results = computeAverages1(lines, MAX_E2E, STATE_SIZE, NB_BOXES, [TUPLE_RATE,FAILURE_DURATION])
#     tuple_rate_results = computeAverages2(lines, MAX_E2E, NB_INPUT_STREAMS, TUPLE_RATE, [STATE_SIZE,FAILURE_DURATION])
#     failure_dur_results = computeAverages1(lines, MAX_E2E, FAILURE_DURATION, [TUPLE_RATE,STATE_SIZE])

#     # Write processed data to temporary files
#     outFileName1 = "%s.state_size" % inFileName
#     outFileName2 = "%s.tuple_rate" % inFileName
#     outFileName3 = "%s.fail_dur" % inFileName    
#     max_values1 = writeResults(state_size_results,outFileName1)
#     max_values2 = writeResults(tuple_rate_results,outFileName2)
#     max_values3 = writeResults(failure_dur_results,outFileName3)

#     # Produce the gnuplot files and the graphs
#     produceGnuplotFile(outFileName1, "State size (tuple)", "Max. proc. delay (msec)",max_values1)
#     produceGnuplotFile(outFileName2, "Agg input rate (tuple/sec)", "Max. proc. delay (msec)",max_values2)
#     produceGnuplotFile(outFileName3, "Failure duration (msec)", "Max. proc. delay (msec)",max_values3)

#     # Produce the gnuplot files and the graphs
#     os.system("gnuplot %s.gnuplot" % outFileName1);
#     os.system("gnuplot %s.gnuplot" % outFileName2);
#     os.system("gnuplot %s.gnuplot" % outFileName3);

#     os.system("epstopdf %s.eps" % (outFileName1))
#     os.system("epstopdf %s.eps" % (outFileName2))
#     os.system("epstopdf %s.eps" % (outFileName3))
