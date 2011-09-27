#!/bin/sh

SERVER1="128.148.37.37"   #borealis

if [ $# -lt 1 ]
then
    echo "./rundemo 1 the micro case (checkpoint interval vs checkpoint cost)"
    echo "./rundemo 2 the macro case (latency and recovery time)"
    exit 0
fi

case "$1" in
1)
echo "the micro case"

duration=500

echo "=============================="
echo "test case name: micro"
echo "duration: $duration"

CVS_SANDBOX=${CVS_SANDBOX:-"${HOME}"}
BOREALIS_HOME=${CVS_SANDBOX}/borealis
BOREALIS_SRC_HOME=${BOREALIS_HOME}/src/src
CLIENT_HOME=${BOREALIS_HOME}/demo/ha
LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${INSTALL_NMSTL}/lib:${INSTALL_ANTLR}/lib
PATH=${PATH}:${BOREALIS_SRC_HOME}:${BOREALIS_HOME}/tool/marshal:${BOREALIS_HOME}/tool/head

ROUTER="128.148.33.121"   #yacht
SERVER1="128.148.37.56"    #yblue
SERVER2="128.148.37.58"    #ysilver

rm ~/*.log -f
killall xterm;killall lt-borealis
sleep 2;ssh ${SERVER1} env LD_LIBRARY_PATH=$LD_LIBRARY_PATH killall lt-borealis
sleep 2;ssh ${SERVER2} env LD_LIBRARY_PATH=$LD_LIBRARY_PATH killall lt-borealis

sleep 3;echo "router";xterm -T router -geometry 80x5+100+100 -e sh -l -c "LD_LIBRARY_PATH=$LD_LIBRARY_PATH ${BOREALIS_SRC_HOME}/borealis -d $ROUTER:20000 2>&1 |tee router.log" &

S1ARGS="-d $SERVER1:21000 -p $SERVER2:22000 passive -i 4000 -s 15 -c round_robin decremental"
sleep 3;echo "borealis1";xterm -T borealis1 -geometry 80x5+500+100 -e env LD_LIBRARY_PATH=$LD_LIBRARY_PATH BOREALIS_SRC_HOME=${BOREALIS_SRC_HOME} S1ARGS="$S1ARGS" SERVER1="$SERVER1" sh -c 'ssh $SERVER1 env LD_LIBRARY_PATH=$LD_LIBRARY_PATH BOREALIS_LOGGING=1 BOREALIS_SRC_HOME=${BOREALIS_SRC_HOME} S1ARGS="'${S1ARG}S'" ${BOREALIS_SRC_HOME}/borealis $S1ARGS 2>&1 |tee borealis1.log' &

S2ARGS="-d $SERVER2:22000 -p $SERVER1:21000 passive -i 4000 -s 15 -c round_robin decremental"
sleep 3;echo "borealis2";xterm -T borealis2 -geometry 80x5+500+200 -e env LD_LIBRARY_PATH=$LD_LIBRARY_PATH BOREALIS_SRC_HOME=${BOREALIS_SRC_HOME} S2ARGS="$S2ARGS" SERVER2="$SERVER2" sh -c 'ssh $SERVER2 env LD_LIBRARY_PATH=$LD_LIBRARY_PATH BOREALIS_LOGGING=1 BOREALIS_SRC_HOME=${BOREALIS_SRC_HOME} S2ARGS="'${S2ARG}S'" ${BOREALIS_SRC_HOME}/borealis $S2ARGS 2>&1 |tee borealis2.log' &

sleep 13;echo "ha_demo";xterm -T ha_demo -geometry 80x5+500+700 -e sh -l -c -c "LD_LIBRARY_PATH=$LD_LIBRARY_PATH PATH=$PATH ./ha_demo 2>&1 |tee ha_demo.log" &

sleep $duration

killall xterm;killall lt-borealis
sleep 2;ssh ${SERVER1} env LD_LIBRARY_PATH=$LD_LIBRARY_PATH killall lt-borealis
sleep 2;ssh ${SERVER2} env LD_LIBRARY_PATH=$LD_LIBRARY_PATH killall lt-borealis

sleep 3;mv ~/*.log .;./rename.sh log "micro.dat"

./rename.sh $SERVER1:21000 server1
./rename.sh $SERVER2:22000 server2

;;

2)
rm -rf *.log;echo "clear all log files"
echo "the macro case"
#./inner_run.sh 200 active -i 100 -s 30 -c round_robin
./inner_run.sh 300 # no checkpointing
./inner_run.sh 301 passive -i 200 -s 20 -c whole
#./inner_run.sh 302 passive -i 50 -s 30 -c round_robin
#./inner_run.sh 303 passive -i 50 -s 30 -c min_max
#./inner_run.sh 150 passive -s 30 -c min_max
mv output*.log archive/ -f
mv 6units*.log archive/ -f
;;
esac
