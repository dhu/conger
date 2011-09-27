#!/bin/sh

CVS_SANDBOX=${CVS_SANDBOX:-"${HOME}"}
echo $CVS_SANDBOX

BOREALIS_HOME=${CVS_SANDBOX}/borealis
BOREALIS_SRC_HOME=${BOREALIS_HOME}/src/src
CLIENT_HOME=${BOREALIS_HOME}/demo/mitre
LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${INSTALL_NMSTL}/lib:${INSTALL_ANTLR}/lib
PATH=${PATH}:${BOREALIS_SRC_HOME}:${BOREALIS_HOME}/tool/marshal:${BOREALIS_HOME}/tool/head

echo "CentralLookupServer"
xterm -T CLS@127.0.0.1:30000 -geometry 80x10+0+0 -e sh -l -c "LD_LIBRARY_PATH=$LD_LIBRARY_PATH ${BOREALIS_SRC_HOME}/CentralLookupServer -d 127.0.0.1:30000 " &

sleep 4

echo "borealis"
xterm -T borealis@127.0.0.1:15000 -geometry 80x10+0+200 -e sh -l -c "LD_LIBRARY_PATH=$LD_LIBRARY_PATH ${BOREALIS_SRC_HOME}/borealis -d 127.0.0.1:15000 2>&1 |tee borealis.log" &

sleep 4

echo "mitre_demo"
xterm -T client -geometry 80x10+0+400 -e sh -l -c "LD_LIBRARY_PATH=$LD_LIBRARY_PATH PATH=$PATH ./mitre_demo 2>&1 |tee mitre_demo.log" &

sleep 4

echo "client"
java -cp ${CLIENT_HOME}/visualizer.jar Visualizer

