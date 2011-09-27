#!/bin/ksh

I=2
N=2

if [ $# -ne 7 ]
then
  echo "Usage: <nh_mode> <global_ls_mode> <offline_duration> <percent_fit_error> <rate> <run_length> <log_level>"
  exit
else
  NH_MODE=$1
  GLOBAL=$2
  OFFLINE=$3
  FIT_ERROR=$4
  RATE=$5
  RUN_LENGTH=$6
  LOGLEVEL=$7
  let CLIENT_RUN_LENGTH=$6
  let SERVER1_RUN_LENGTH=$6+10000
  let SERVER8_RUN_LENGTH=$6
  let LS_RUN_LENGTH=$6+5000
  let CLIENT_OVERLOAD_TIME=$6-180000
  echo ${NH_MODE} ${GLOBAL} ${OFFLINE} ${FIT_ERROR} ${RATE} ${RUN_LENGTH} ${LOGLEVEL}
fi

################################################################################

CVS_SANDBOX=${CVS_SANDBOX:-"${HOME}"}
BOREALIS_HOME=${CVS_SANDBOX}/research/borealis
BOREALIS_SRC_HOME=${BOREALIS_HOME}/src/src
CLIENT_HOME=${BOREALIS_HOME}/test/composite/GlobalLoadShedder
LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${INSTALL_NMSTL}/lib:${INSTALL_ANTLR}/lib
PATH=${PATH}:${BOREALIS_SRC_HOME}:${BOREALIS_HOME}/tool/marshal:${BOREALIS_HOME}/tool/head

SERVER1="128.148.37.48"  # xblack  # input server
SERVER8="128.148.37.58"  # ysilver # output server

################################################################################

rm -f *.log
rm -f *.lp
killall xterm;killall lt-borealis
sleep 2;ssh ${SERVER1} env LD_LIBRARY_PATH=$LD_LIBRARY_PATH killall lt-borealis
sleep 2;ssh ${SERVER8} env LD_LIBRARY_PATH=$LD_LIBRARY_PATH killall lt-borealis
sleep 2

################################################################################

sleep 1;echo "BorealisServer1"
S1ARGS="-d $SERVER1:21000 -l ${NH_MODE} -t ${SERVER1_RUN_LENGTH}"
xterm -T BorealisServer1 -geometry 80x5+0+100 -e env LD_LIBRARY_PATH=$LD_LIBRARY_PATH BOREALIS_SRC_HOME=${BOREALIS_SRC_HOME} LOG_LEVEL=$LOGLEVEL S1ARGS="$S1ARGS" SERVER1="$SERVER1" sh -c 'ssh $SERVER1 env LD_LIBRARY_PATH=$LD_LIBRARY_PATH BOREALIS_SRC_HOME=${BOREALIS_SRC_HOME} LOG_LEVEL=${LOG_LEVEL} S1ARGS="'${S1ARG}S'" ${BOREALIS_SRC_HOME}/borealis $S1ARGS 2>&1 |tee server1.log' & 

################################################################################

sleep 5;echo "BorealisServer8"
S8ARGS="-d $SERVER8:28000 -l ${NH_MODE} -t ${SERVER8_RUN_LENGTH}"
xterm -T BorealisServer8 -geometry 80x5+0+200 -e env LD_LIBRARY_PATH=$LD_LIBRARY_PATH BOREALIS_SRC_HOME=${BOREALIS_SRC_HOME} LOG_LEVEL=$LOGLEVEL S8ARGS="$S8ARGS" SERVER8="$SERVER8" sh -c 'ssh $SERVER8 env LD_LIBRARY_PATH=$LD_LIBRARY_PATH BOREALIS_SRC_HOME=${BOREALIS_SRC_HOME} LOG_LEVEL=${LOG_LEVEL} S8ARGS="'${S8ARG}S'" ${BOREALIS_SRC_HOME}/borealis $S8ARGS 2>&1 |tee server8.log' & 

################################################################################

sleep 5;echo "GlobalLoadShedder"
xterm -T GlobalLoadShedder -geometry 80x5+0+300 -e sh -l -c "LD_LIBRARY_PATH=$LD_LIBRARY_PATH LOG_LEVEL=${LOGLEVEL} PATH=$PATH ${BOREALIS_HOME}/tool/optimizer/loadShedder/LoadSheddingTool -d 40000 -p 10000 -q 0 -m ${GLOBAL} -w 45000 -o ${OFFLINE} -e ${FIT_ERROR} -t ${LS_RUN_LENGTH} 2>&1 |tee global_ls.log" &

################################################################################

sleep 10;echo "chain_${I}_${N}"
xterm -T chain_${I}_${N} -geometry 80x5+0+400 -e sh -l -c "LD_LIBRARY_PATH=$LD_LIBRARY_PATH LOG_LEVEL=${LOGLEVEL} PATH=$PATH ./chain_${I}_${N} data_50000.txt ${RATE} ${CLIENT_OVERLOAD_TIME} ${CLIENT_RUN_LENGTH} 2>&1 |tee chain_${I}_${N}.log"

################################################################################

sleep 30
result1=`sh -c 'ssh ysilver cat /tmp/borealis_stats_tput.txt'`
result2=`sh -c 'ssh ysilver cat /tmp/borealis_stats_all.txt'`
off=`cat /tmp/offline`
echo ${GLOBAL} ${I} ${N} $off ${FIT_ERROR} ${RATE} $result1 $result2 |tee -a results

################################################################################

killall xterm;killall lt-borealis
sleep 2;ssh ${SERVER1} env LD_LIBRARY_PATH=$LD_LIBRARY_PATH killall lt-borealis
sleep 2;ssh ${SERVER8} env LD_LIBRARY_PATH=$LD_LIBRARY_PATH killall lt-borealis

################################################################################

sleep 2
mv -f global_ls.log global_ls.${GLOBAL}.${FIT_ERROR}.${RATE}.${I}.${N}
mv -f server1.log server1.${GLOBAL}.${FIT_ERROR}.${RATE}.${I}.${N}
mv -f server8.log server8.${GLOBAL}.${FIT_ERROR}.${RATE}.${I}.${N}

################################################################################
