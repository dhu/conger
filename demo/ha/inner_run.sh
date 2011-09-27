#!/bin/sh

if [ $# -lt 1 ]
then
duration=100
else
duration=$1
fi

ARGS="$2 $3 $4 $5 $6 $7 $8" 
echo "=============================="
echo "test case name: $1"
echo "duration: $duration"
echo "args: $ARGS"


CVS_SANDBOX=${CVS_SANDBOX:-"${HOME}"}
BOREALIS_HOME=${CVS_SANDBOX}/borealis
BOREALIS_SRC_HOME=${BOREALIS_HOME}/src/src
CLIENT_HOME=${BOREALIS_HOME}/demo/ha
LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${INSTALL_NMSTL}/lib:${INSTALL_ANTLR}/lib
PATH=${PATH}:${BOREALIS_SRC_HOME}:${BOREALIS_HOME}/tool/marshal:${BOREALIS_HOME}/tool/head

ROUTER="128.148.33.121"   #yacht
#SERVER1="128.148.37.37"   #borealis
#SERVER2=$ROUTER
#SERVER1="128.148.37.48"    #xblack
#SERVER2="128.148.37.49"    #xsilver
#SERVER3="128.148.37.50"    #xyellow
#SERVER4="128.148.37.52"    #xblue
#SERVER5="128.148.37.54"    #xgreen

SERVER1="128.148.37.56"    #yblue
SERVER2="128.148.37.58"    #ysilver
SERVER3="128.148.37.59"    #yblack
SERVER4="128.148.37.60"    #ygreen
SERVER5="128.148.31.135"    #borealis

rm ~/*.log -f
killall xterm;killall lt-borealis
sleep 2;ssh ${SERVER1} env LD_LIBRARY_PATH=$LD_LIBRARY_PATH killall lt-borealis
sleep 2;ssh ${SERVER2} env LD_LIBRARY_PATH=$LD_LIBRARY_PATH killall lt-borealis
sleep 2;ssh ${SERVER3} env LD_LIBRARY_PATH=$LD_LIBRARY_PATH killall lt-borealis
sleep 2;ssh ${SERVER4} env LD_LIBRARY_PATH=$LD_LIBRARY_PATH killall lt-borealis
sleep 2;ssh ${SERVER5} env LD_LIBRARY_PATH=$LD_LIBRARY_PATH killall lt-borealis

sleep 3;echo "router";xterm -T router -geometry 80x5+100+100 -e sh -l -c "LD_LIBRARY_PATH=$LD_LIBRARY_PATH ${BOREALIS_SRC_HOME}/borealis -d $ROUTER:20000 2>&1 |tee router.log" &

S1ARGS="-d $SERVER1:21000 -p $SERVER2:22000 -p $SERVER3:23000 -p $SERVER4:24000 -p $SERVER5:25000 $ARGS"
sleep 1;echo "borealis1";xterm -T borealis1 -geometry 80x5+500+100 -e env LD_LIBRARY_PATH=$LD_LIBRARY_PATH BOREALIS_SRC_HOME=${BOREALIS_SRC_HOME} S1ARGS="$S1ARGS" SERVER1="$SERVER1" sh -c 'ssh $SERVER1 env LD_LIBRARY_PATH=$LD_LIBRARY_PATH BOREALIS_LOGGING=1 BOREALIS_SRC_HOME=${BOREALIS_SRC_HOME} S1ARGS="'${S1ARG}S'" ${BOREALIS_SRC_HOME}/borealis $S1ARGS 2>&1 |tee borealis1.log' &

S2ARGS="-d $SERVER2:22000 -p $SERVER3:23000 -p $SERVER4:24000 -p $SERVER5:25000 -p $SERVER1:21000 $ARGS"
sleep 1;echo "borealis2";xterm -T borealis2 -geometry 80x5+500+200 -e env LD_LIBRARY_PATH=$LD_LIBRARY_PATH BOREALIS_SRC_HOME=${BOREALIS_SRC_HOME} S2ARGS="$S2ARGS" SERVER2="$SERVER2" sh -c 'ssh $SERVER2 env LD_LIBRARY_PATH=$LD_LIBRARY_PATH BOREALIS_LOGGING=1 BOREALIS_SRC_HOME=${BOREALIS_SRC_HOME} S2ARGS="'${S2ARG}S'" ${BOREALIS_SRC_HOME}/borealis $S2ARGS 2>&1 |tee borealis2.log' &

S3ARGS="-d $SERVER3:23000 -p $SERVER4:24000 -p $SERVER5:25000 -p $SERVER1:21000 -p $SERVER2:22000 $ARGS"
sleep 1;echo "borealis3";xterm -T borealis3 -geometry 80x5+500+300 -e env LD_LIBRARY_PATH=$LD_LIBRARY_PATH BOREALIS_SRC_HOME=${BOREALIS_SRC_HOME} S3ARGS="$S3ARGS" SERVER3="$SERVER3" sh -c 'ssh $SERVER3 env LD_LIBRARY_PATH=$LD_LIBRARY_PATH BOREALIS_LOGGING=1 BOREALIS_SRC_HOME=${BOREALIS_SRC_HOME} S3ARGS="'${S3ARG}S'" ${BOREALIS_SRC_HOME}/borealis $S3ARGS 2>&1 |tee borealis3.log' &

S4ARGS="-d $SERVER4:24000 -p $SERVER5:25000 -p $SERVER1:21000 -p $SERVER2:22000 -p $SERVER3:23000 $ARGS"
sleep 1;echo "borealis4";xterm -T borealis4 -geometry 80x5+500+400 -e env LD_LIBRARY_PATH=$LD_LIBRARY_PATH BOREALIS_SRC_HOME=${BOREALIS_SRC_HOME} S4ARGS="$S4ARGS" SERVER4="$SERVER4" sh -c 'ssh $SERVER4 env LD_LIBRARY_PATH=$LD_LIBRARY_PATH BOREALIS_LOGGING=1 BOREALIS_SRC_HOME=${BOREALIS_SRC_HOME} S4ARGS="'${S4ARG}S'" ${BOREALIS_SRC_HOME}/borealis $S4ARGS 2>&1 |tee borealis4.log' &

S5ARGS="-d $SERVER5:25000 -p $SERVER1:21000 -p $SERVER2:22000 -p $SERVER3:23000 -p $SERVER4:24000 $ARGS"
sleep 1;echo "borealis5";xterm -T borealis5 -geometry 80x5+500+500 -e env LD_LIBRARY_PATH=$LD_LIBRARY_PATH BOREALIS_SRC_HOME=${BOREALIS_SRC_HOME} S5ARGS="$S5ARGS" SERVER5="$SERVER5" sh -c 'ssh $SERVER5 env LD_LIBRARY_PATH=$LD_LIBRARY_PATH BOREALIS_LOGGING=1 BOREALIS_SRC_HOME=${BOREALIS_SRC_HOME} S5ARGS="'${S5ARG}S'" ${BOREALIS_SRC_HOME}/borealis $S5ARGS 2>&1 |tee borealis5.log' &

sleep 13;echo "ha_demo";xterm -T ha_demo -geometry 80x5+500+700 -e sh -l -c -c "LD_LIBRARY_PATH=$LD_LIBRARY_PATH PATH=$PATH ./ha_demo 2>&1 |tee ha_demo.log" &

sleep $duration

killall xterm;killall lt-borealis
sleep 2;ssh ${SERVER1} env LD_LIBRARY_PATH=$LD_LIBRARY_PATH killall lt-borealis
sleep 2;ssh ${SERVER2} env LD_LIBRARY_PATH=$LD_LIBRARY_PATH killall lt-borealis
sleep 2;ssh ${SERVER3} env LD_LIBRARY_PATH=$LD_LIBRARY_PATH killall lt-borealis
sleep 2;ssh ${SERVER4} env LD_LIBRARY_PATH=$LD_LIBRARY_PATH killall lt-borealis
sleep 2;ssh ${SERVER5} env LD_LIBRARY_PATH=$LD_LIBRARY_PATH killall lt-borealis

sleep 3;mv ~/*.log .;./rename.sh log "$1.dat"

./rename.sh $SERVER1:21000 server1
./rename.sh $SERVER2:22000 server2
./rename.sh $SERVER3:23000 server3
./rename.sh $SERVER4:24000 server4
./rename.sh $SERVER5:25000 server5
