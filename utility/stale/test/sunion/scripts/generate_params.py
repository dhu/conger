#!/usr/bin/env python
from sys import *

# 15000 tuples/sec is a bit much even for 500 tuples in state
# size so better to limit at 12K tuples/sec

#-----------------------------------------
# E2E overhead for different components of our system
def microbenchmark1():

    outFileName="microbenchmark1.params"
    outFile = open(outFileName,"w")

    repetitions = 1

    runtime=20000 
    nbStreams=3
    #benchmarks=("NONE", "TUPLE_TYPES", "SUNION", "SUNION_PUNCTUATION", "SOUTPUT")
    #benchmarks=("NONE", "TUPLE_TYPES", "SOUTPUT")
    benchmark="NONE"
    failureDur=0

    stateSizeBasis=100
    stateSizes=(200,500,1000,2000) # Must be a factor of 4 or greater than 50
    tupleRateBasis=1000
    tupleRates=(1500,2000,2500,3000,4000) # Keep in mind that we multiple these rates by 3!
    punct=100
    maxDelay=3000
    checkpointInterval=200
    maxHistory=101001000
    nbBoxes = 1

    #for benchmark in benchmarks:

    for i in range(0,repetitions):
            
        outFile.write("%s %s %s %s %s %s %s %s %s %s %s\n"
                      % (runtime,nbStreams,benchmark,failureDur,stateSizeBasis,tupleRateBasis,punct,maxDelay,checkpointInterval,maxHistory,nbBoxes))

        # Less interesting
        #for stateSize in stateSizes:
        #    outFile.write("%s %s %s %s %s %s %s %s %s %s %s\n"
        #                  % (runtime,nbStreams,benchmark,failureDur,stateSize,tupleRateBasis,punct,maxDelay,checkpointInterval, maxHistory,nbBoxes))
            
        for tupleRate in tupleRates:
            outFile.write("%s %s %s %s %s %s %s %s %s %s %s\n"
                          % (runtime,nbStreams,benchmark,failureDur,stateSizeBasis,tupleRate,punct,maxDelay,checkpointInterval, maxHistory, nbBoxes))
    outFile.close()
        
#-----------------------------------------
# That's the benchmark that analyzis the overhead of checkpoints with
# increasing checkpiont interval and the overhead of sunion/punctuation with
# increasing punctuation frequency (not interesting)
## NOT USED NOT INTERESTING
def microbenchmark2():

    outFileName="microbenchmark2.params"
    outFile = open(outFileName,"w")

    repetitions = 1

    runtime=20000
    nbStreams=3
    #benchmarks=("SUNION", "SUNION_PUNCTUATION")
    benchmark="SUNION"
    failureDur=0

    stateSize=20
    tupleRate=1000
    punctuations=(50,100,150,200,250,300)
    maxDelay=2000
    checkpointInterval=100
    maxHistory=101001000
    nbBoxes = 1

    #for benchmark in benchmarks:

    for i in range(0,repetitions):
        
        for punct in punctuations:
            outFile.write("%s %s %s %s %s %s %s %s %s %s %s\n"
                          % (runtime,nbStreams,benchmark,failureDur,stateSize,tupleRate,punct,maxDelay,checkpointInterval,maxHistory, nbBoxes))
        

    #benchmark="CHECKPOINT"
    #punc=100
    #checkpoints=(50,100,250,500)
            
    #for i in range(0,repetitions):
            
    #    for check in checkpoints:
    #        outFile.write("%s %s %s %s %s %s %s %s %s %s %s\n"
    #                      % (runtime,nbStreams,benchmark,failureDur,stateSize,tupleRate,punct,maxDelay,check,maxHistory, nbBoxes))

    outFile.close()


#-----------------------------------------
# These are the benchmarks to get the CPU overheads for undo/redo and checkpoint/redo
# Because we take the median value of a long run (30 seconds) then
# we don't need to run multiple times
def microbenchmark3():

    outFileName="microbenchmark3.params"
    outFile = open(outFileName,"w")

    repetitions = 1
    
    runtime=60000 # Long so the median and percentiles are meaningful
    nbStreams=3
    #benchmarks=("UNDO_LOG", "CHECKPOINT")
    benchmark="UNDO_LOG"
    failureDur=0

    # The only thing that we vary
    stateSizes=(2,4,10,20,50,100,150,200,300,400,500,750,1000,1250,1500,2000,2500,3000) # Must be a factor of 4 or greater than 50
    tupleRateBasis=334
    punct=100
    maxDelay=3000
    checkpointInterval=100
    maxHistory=101001000
    nbBoxes = 1

    #for benchmark in benchmarks:

    for i in range(0,repetitions):
            
        for stateSize in stateSizes:
            outFile.write("%s %s %s %s %s %s %s %s %s %s %s\n"
                          % (runtime,nbStreams,benchmark,failureDur,stateSize,tupleRateBasis,punct,maxDelay,checkpointInterval,maxHistory, nbBoxes))

    outFile.close()
        

#-----------------------------------------
# Undo/redo vs checkpoint redo performance (big undo history that we do not truncate)
def experiment1():

    outFileName="experiment1.params"
    outFile = open(outFileName,"w")

    repetitions = 3

    runtime=45000
    nbStreams=3
    benchmarks=("ALL_TOGETHER_UNDO_REDO_NO_MOVE","ALL_TOGETHER_CHECKPOINT_REDO_NO_MOVE")
    failureDurBasis=5000
    #failureDurations=(2000,10000)

    stateSizeBasis=20
    stateSizes=(500,1000,1500,2000,2500,3000,3500,4000)
    tupleRateBasis=334
    tupleRates=(1000,1500,1750,2000,2250,2500,2750,3000)
    punct=100
    maxDelay=3000
    checkpointInterval=200
    maxHistory=101001000
    nbBoxes = 1

    for benchmark in benchmarks:

        for i in range(0,repetitions):
            
            outFile.write("%s %s %s %s %s %s %s %s %s %s %s\n"
                          % (runtime,nbStreams,benchmark,failureDurBasis,stateSizeBasis,tupleRateBasis,punct,maxDelay,checkpointInterval,maxHistory, nbBoxes))

            for stateSize in stateSizes:
                outFile.write("%s %s %s %s %s %s %s %s %s %s %s\n"
                              % (runtime,nbStreams,benchmark,failureDurBasis,stateSize,tupleRateBasis,punct,maxDelay,checkpointInterval,maxHistory, nbBoxes))

            for tupleRate in tupleRates:
                outFile.write("%s %s %s %s %s %s %s %s %s %s %s\n"
                              % (runtime,nbStreams,benchmark,failureDurBasis,stateSizeBasis,tupleRate,punct,maxDelay,checkpointInterval,maxHistory, nbBoxes))

            #for failureDur in failureDurations:
            #    outFile.write("%s %s %s %s %s %s %s %s %s %s %s\n"
            #                  % (runtime,nbStreams,benchmark,failureDur,stateSizeBasis,tupleRateBasis,punct,maxDelay,checkpointInterval))
        
    outFile.close()

#-----------------------------------------
# Similar to experiment1 but with a small history (so we don't rerun the checkpoint_redo experiments
def experiment2():

    outFileName="experiment2.params"
    outFile = open(outFileName,"w")

    repetitions = 3

    runtime=45000
    nbStreams=3
    benchmark="ALL_TOGETHER_UNDO_REDO_NO_MOVE"
    failureDurBasis=5000

    stateSizeBasis=20
    stateSizes=(500,1000,1500,2000,2500,3000,3500,4000)
    tupleRateBasis=334
    tupleRates=(1000,1500,1750,2000,2250,2500,2750,3000)
    punct=100
    maxDelay=3000
    checkpointInterval=200
    maxHistory=6000
    nbBoxes = 1

    for i in range(0,repetitions):
            
        outFile.write("%s %s %s %s %s %s %s %s %s %s %s\n"
                      % (runtime,nbStreams,benchmark,failureDurBasis,stateSizeBasis,tupleRateBasis,punct,maxDelay,checkpointInterval,maxHistory,nbBoxes))

        for stateSize in stateSizes:
            outFile.write("%s %s %s %s %s %s %s %s %s %s %s\n"
                          % (runtime,nbStreams,benchmark,failureDurBasis,stateSize,tupleRateBasis,punct,maxDelay,checkpointInterval,maxHistory, nbBoxes))

        for tupleRate in tupleRates:
            outFile.write("%s %s %s %s %s %s %s %s %s %s %s\n"
                          % (runtime,nbStreams,benchmark,failureDurBasis,stateSizeBasis,tupleRate,punct,maxDelay,checkpointInterval,maxHistory, nbBoxes))
        
    outFile.close()


#-----------------------------------------
# Same as experiment1 but with 2 boxes rather than one and limited history
def experiment3():

    outFileName="experiment3.params"
    outFile = open(outFileName,"w")

    repetitions = 3

    runtime=45000
    nbStreams=3
    benchmarks=("ALL_TOGETHER_UNDO_REDO_NO_MOVE","ALL_TOGETHER_CHECKPOINT_REDO_NO_MOVE")
    failureDurBasis=5000
    failureDurations=(2000,10000)

    stateSizeBasis=20
    stateSizes=(500,1000,1500,2000,2500,3000,3500,4000)
    tupleRateBasis=334
    tupleRates=(1000,1500,1750,2000,2250,2500,2750,3000)
    punct=100
    maxDelay=3000
    checkpointInterval=200
    maxHistory=6000
    nbBoxes = 2

    for benchmark in benchmarks:

        for i in range(0,repetitions):
            
            outFile.write("%s %s %s %s %s %s %s %s %s %s %s\n"
                          % (runtime,nbStreams,benchmark,failureDurBasis,stateSizeBasis/nbBoxes,tupleRateBasis,punct,maxDelay,checkpointInterval,maxHistory, nbBoxes))

            for stateSize in stateSizes:
                outFile.write("%s %s %s %s %s %s %s %s %s %s %s\n"
                              % (runtime,nbStreams,benchmark,failureDurBasis,stateSize/nbBoxes,tupleRateBasis,punct,maxDelay,checkpointInterval,maxHistory, nbBoxes))

            for tupleRate in tupleRates:
                outFile.write("%s %s %s %s %s %s %s %s %s %s %s\n"
                              % (runtime,nbStreams,benchmark,failureDurBasis,stateSizeBasis/nbBoxes,tupleRate,punct,maxDelay,checkpointInterval,maxHistory, nbBoxes))

            #for failureDur in failureDurations:
            #    outFile.write("%s %s %s %s %s %s %s %s %s %s %s\n"
            #                  % (runtime,nbStreams,benchmark,failureDur,stateSizeBasis,tupleRateBasis,punct,maxDelay,checkpointInterval))
        
    outFile.close()


#-----------------------------------------
# Benefits of blocking vs not-blocking
# To run this benchmark change ALPHA_MAX to 1 in the code!!!
def benefits1():

    outFileName="benefits1.params"
    outFile = open(outFileName,"w")

    repetitions = 3

    runtime=35000
    nbStreams=3
    benchmark="ALL_TOGETHER_UNDO_REDO_NO_MOVE"
    failureDurBasis=5000

    stateSizeBasis=100
    tupleRateBasis=1000
    punct=100
    maxDelays=(500,1000,1500,2000,2500,3000,3500,4000,4500,5000,5500,6000)
    checkpointInterval=200
    maxHistory=6000
    nbBoxes = 1

    for i in range(0,repetitions):
            
        stagger=("1 0")
        for maxDelay in maxDelays:
            outFile.write("%s %s %s %s %s %s %s %s %s %s %s %s\n"
                          % (runtime,nbStreams,benchmark,failureDurBasis,stateSizeBasis,tupleRateBasis,punct,maxDelay,checkpointInterval,maxHistory, nbBoxes, stagger))

        
    outFile.close()


#-----------------------------------------
# Benefits of moving vs  not-moving subscriptions
def benefits2():

    outFileName="benefits2.params"
    outFile = open(outFileName,"w")

    repetitions = 3

    runtime=45000
    nbStreams=3
    benchmarks=("ALL_TOGETHER_CHECKPOINT_REDO", "ALL_TOGETHER_CHECKPOINT_REDO_NO_MOVE")
    #failureDurs=(60000,2000,30000,6000,12000)
    #failureDurs=(45000,4000,15000,8000,10000)
    #failureDurs=(3000,14000)
    failureDurs=(60000,2000,30000,6000,12000,45000,4000,15000,8000,10000,3000,14000)

    all_staggers=("0 0","1 0", "0 1","1 1")
    half_staggers=("0 0","1 0")
    stateSizeBasis=100
    tupleRateBasis=1500
    punct=100
    maxDelay=3000
    checkpointInterval=200
    maxHistory=6000
    nbBoxes = 1

    for i in range(0,repetitions):
        
        for failureDur in failureDurs:
                    
            for benchmark in benchmarks:

                staggers = all_staggers
                if benchmark == "ALL_TOGETHER_CHECKPOINT_REDO_NO_MOVE":
                    staggers = half_staggers
                for stagger in staggers:
                    actualRuntime = int(runtime + (0.75*failureDur))
                    if ( failureDur > 25000 ):
                        actualRuntime = int(2.5*failureDur)
                    outFile.write("%s %s %s %s %s %s %s %s %s %s %s %s\n"
                                  % (actualRuntime,nbStreams,benchmark,failureDur,stateSizeBasis,tupleRateBasis,punct,maxDelay,checkpointInterval,maxHistory, nbBoxes,stagger))

        
    outFile.close()

#-----------------------------------------
# Adaptive algorithm
def adaptive1():

    outFileName="adaptive1.params"
    outFile = open(outFileName,"w")

    repetitions = 3

    runtime=30000
    nbStreams=3
    benchmark="ADAPTIVE"
    failureDurs=(500,750,1000,1250,1500,2000,2500,3000,3500,4000,5000,6000,7000,8000,9000,10000,11000,12000,15000,17500,20000,25000,30000,60000,90000,120000,180000,300000)

    stateSizeBasis=100
    tupleRateBasis=1000
    punct=100
    maxDelay=3000
    checkpointInterval=200
    maxHistory=6000
    nbBoxes = 1
        
    for i in range(0,repetitions):
            
        for failureDur in failureDurs:
            actualRuntime = int(runtime + (1.75 * failureDur))
            outFile.write("%s %s %s %s %s %s %s %s %s %s %s\n"
                          % (actualRuntime,nbStreams,benchmark,failureDur,stateSizeBasis,tupleRateBasis,punct,maxDelay,checkpointInterval,maxHistory, nbBoxes))

        
    outFile.close()

#-----------------------------------------
# Adaptive algorithm with more than one node
def manynodes():

    outFileName="many-nodes.params"
    outFile = open(outFileName,"w")

    runtime=60000
    nbStreams=3

    failureDur=15000

    stateSizeBasis=10
    tupleRateBasis=334
    punct=100
    maxDelay=3000
    checkpointInterval=200
    maxHistory=6000
    nbBoxes = 1

    benchmark="ALL_TOGETHER_UNDO_REDO"
    stagger="0 0" # Process and process
    
    outFile.write("%s %s %s %s %s %s %s %s %s %s %s %s\n"
                  % (runtime,nbStreams,benchmark,failureDur,stateSizeBasis,tupleRateBasis,punct,maxDelay,checkpointInterval,maxHistory, nbBoxes, stagger))

    benchmark="ALL_TOGETHER_UNDO_REDO"
    stagger="1 1" # Delay and delay
    
    outFile.write("%s %s %s %s %s %s %s %s %s %s %s %s\n"
                  % (runtime,nbStreams,benchmark,failureDur,stateSizeBasis,tupleRateBasis,punct,maxDelay,checkpointInterval,maxHistory, nbBoxes, stagger))

    benchmark="ALL_TOGETHER_UNDO_REDO_NO_MOVE"
    stagger="0 0" # Process and don't care

    outFile.write("%s %s %s %s %s %s %s %s %s %s %s %s\n"
                  % (runtime,nbStreams,benchmark,failureDur,stateSizeBasis,tupleRateBasis,punct,maxDelay,checkpointInterval,maxHistory, nbBoxes, stagger))
    
    outFile.close()


#-----------------------------------------
if __name__ == '__main__':

    if ( len(argv) < 2) :
        print "Please indicate what kind of parameters to generate: "
        print " -> micro1, micro2, micro3, experiment1, experiment2, experiment3, benefits1, benefits2, adaptive1, many-nodes"    
    else:

        whatToGenerate = argv[1]
        if ( whatToGenerate == "micro1"):
            microbenchmark1()
        elif (whatToGenerate == "micro2"):
            microbenchmark2()
        elif (whatToGenerate == "micro3"):
            microbenchmark3()
        elif (whatToGenerate == "experiment1"):
            experiment1()
        elif (whatToGenerate == "experiment2"):
            experiment2()
        elif (whatToGenerate == "experiment3"):
            experiment3()
        elif (whatToGenerate == "benefits1"):
            benefits1()
        elif (whatToGenerate == "benefits2"):
            benefits2()
        elif (whatToGenerate == "adaptive1"):
            adaptive1()
        elif (whatToGenerate == "many-nodes"):
            manynodes()                                                
        else:
            print "Don't know how to generate parameters for %s" % whatToGenerate
