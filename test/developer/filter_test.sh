#!/bin/sh

CVS_SANDBOX=${CVS_SANDBOX:-"${HOME}"}
echo $CVS_SANDBOX

BOREALIS_HOME=${CVS_SANDBOX}/borealis
BOREALIS_SRC_HOME=${BOREALIS_HOME}/src/src
CLIENT_HOME=${BOREALIS_HOME}/demo/mitre
LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${INSTALL_NMSTL}/lib:${INSTALL_ANTLR}/lib
PATH=${PATH}:${BOREALIS_SRC_HOME}:${BOREALIS_HOME}/tool/marshal:${BOREALIS_HOME}/tool/head

echo "CentralLookupServer"
xterm -T CLS@127.0.0.1:30000 -geometry 80x5+0+0 -e sh -l -c "LD_LIBRARY_PATH=$LD_LIBRARY_PATH ${BOREALIS_SRC_HOME}/CentralLookupServer -d 127.0.0.1:30000 " &

sleep 1

echo "borealis"
xterm -T borealis@127.0.0.1:30000 -geometry 80x5+0+100 -e sh -l -c "LD_LIBRARY_PATH=$LD_LIBRARY_PATH ${BOREALIS_SRC_HOME}/borealis -d 127.0.0.1:15000 2>&1 |tee borealis.log" &

sleep 1

echo "filter_test"
xterm -T client@127.0.0.1:30000 -geometry 80x5+0+200 -e sh -l -c "LD_LIBRARY_PATH=$LD_LIBRARY_PATH PATH=$PATH ./filter_test 2>&1 |tee filter_test.log" 
