#!/usr/bin/env python
from sys import *
import os, time

BENCHMARK=2

# --------------------------------------------------
def run_experiments(listExperimentsFileName,resultsFileName):
    
        # Read the list of experiments to run
        
        listExperiments = open(listExperimentsFileName)
        lines = listExperiments.readlines()
        listExperiments.close()

        line_nb = 0
        for line in lines[0:]:
            print "Running %s" % line 
            parameters = line.split()
            sleepTimeMsecs = int(parameters[0]) + 10000
            sleepTime = int(sleepTimeMsecs / 1000)
            benchmark=parameters[BENCHMARK]
            print "Runtime for experiment should be %s msec" % sleepTime

            commandLineStart = "runtest sunion %s:%s %s %d" % (listExperimentsFileName,line_nb,resultsFileName,sleepTimeMsecs)
            if ( (benchmark == "ALL_TOGETHER_UNDO_REDO")
                 or (benchmark == "ALL_TOGETHER_CHECKPOINT_REDO")
                 or (benchmark == "ADAPTIVE") ):
                    commandLineStart = "runtest sunionrep %s:%s %s %d" % (listExperimentsFileName,line_nb,resultsFileName,sleepTimeMsecs)
            commandLineStop = "runtest stop"
            print "Should run %s" % commandLineStart
            print "Should run %s" % commandLineStop
                        
            sts = os.system(commandLineStart)
            time.sleep(sleepTime)
            sts = os.system(commandLineStop)

            line_nb += 1
        print "All Done"

#-----------------------------------------
if __name__ == '__main__':
    
    if ( len(argv) < 2) :
        print "Please indicate what kind of experiments to run: "
        print " -> micro1, micro2, micro3, experiment1, experiment2, benefits1, benefits2, adaptive1"    
    else:

        whatToRun = argv[1]
        
        if ( whatToRun == "micro1"):
            listExperimentsFileName = "microbenchmark1.params"
            resultsFileName = "microbenchmark1.dat"
        elif (whatToRun == "micro2"):
            listExperimentsFileName = "microbenchmark2.params"
            resultsFileName = "microbenchmark2.dat"
        elif (whatToRun == "micro3"):
            listExperimentsFileName = "microbenchmark3.params"
            resultsFileName = "microbenchmark3.dat"
        elif (whatToRun == "experiment1"):
            listExperimentsFileName = "experiment1.params"
            resultsFileName = "experiment1.dat"
        elif (whatToRun == "experiment2"):
            listExperimentsFileName = "experiment2.params"
            resultsFileName = "experiment2.dat"
        elif (whatToRun == "experiment3"):
            listExperimentsFileName = "experiment3.params"
            resultsFileName = "experiment3.dat"
        elif (whatToRun == "benefits1"):
            listExperimentsFileName = "benefits1.params"
            resultsFileName = "benefits1.dat"
        elif (whatToRun == "benefits2"):
            listExperimentsFileName = "benefits2.params"
            resultsFileName = "benefits2.dat"
        elif (whatToRun == "adaptive1"):
            listExperimentsFileName = "adaptive1.params"
            resultsFileName = "adaptive1.dat"                                    
        else:
            print "Don't know how to run %s" % whatToRun
            exit(0)

        os.system("rm %s" % resultsFileName)
        run_experiments(listExperimentsFileName,resultsFileName)

        if ( whatToRun == "micro3"):
                command = "mv ConsistencyMngr.log ConsistencyMngr.log.micro3"
                os.system(command)
                command = "mv SJoinQBox.log SJoinQBox.log.micro3"
                os.system(command)




