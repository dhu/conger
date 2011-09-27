#!/usr/bin/env python
from sys import *
import os, time

BENCHMARK=2

# --------------------------------------------------
def run_experiments(listExperimentsFileName,resultsFileName, lineNb, nbNodes):
            
        listExperiments = open(listExperimentsFileName)
        line = listExperiments.readlines()[lineNb]
        listExperiments.close()

        print "Running %s" % line 
        parameters = line.split()
        sleepTimeMsecs = int(parameters[0]) + 30000
        sleepTime = int(sleepTimeMsecs / 1000)
        benchmark=parameters[BENCHMARK]
        print "Runtime for experiment should be %s msec" % sleepTime

        for nb in range(1,nbNodes+1):

                commandLineStart = "runtest suniondoublerep %s:%s %s %d %d" % (listExperimentsFileName,lineNb,resultsFileName,sleepTimeMsecs,nb)
                commandLineStop = "runtest stop"
                print "Should run: \"%s\"" % commandLineStart
                print "Should run: \"%s\"" % commandLineStop
                        
                sts = os.system(commandLineStart)
                time.sleep(sleepTime)
                sts = os.system(commandLineStop)

        print "All Done"

#-----------------------------------------
if __name__ == '__main__':
    
    if ( len(argv) < 4) :
        print "Please indicate input file, line, nb nodes and experimentID as 4 separate arguments"
    else:

        listExperimentsFileName = argv[1]
        lineToUse =  int(argv[2])
        nbNodes = int(argv[3])
        experimentNb = argv[4]
                
        resultsFileName = "many-nodes.dat.%d.%s" % (nbNodes,experimentNb)
        
        os.system("rm %s" % resultsFileName)
        run_experiments(listExperimentsFileName,resultsFileName,lineToUse,nbNodes)





