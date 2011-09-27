#!/bin/sh

###########################  create.snapshot.sh  ###############################
#
#  Run this to create a snapshot of the Borealis and Nmstl directories
#  from CVS as a snapshot gzipped tar ball.
#
#  Entry:  You may want to run the ssh agent first, but it is not required.
#          Otherwise you will be prompted twice for your CVS password.
#          This script may be run from any directory.
#
#   Exit:  The Borealis tar ball is:  ${HOME}/borealis.tar.gz
#
#   Note:  The tar ball is created in a temporary directory and will not
#          alter your CVS sandbox.  Upon completion the temporary directory
#          is removed.
#...............................................................................


################################################################################
#
runCommand()    #Execute a command and check for errors.
{
# Setup: CODE=0        # Counts down for an error code for each command.
#        LAST=`pwd`    # Return to this directory on an error.
#
# Entry: $1 - A command to run in quotes.
#        $2 - An error message.
#...............................................................................


CODE=`expr ${CODE} - 1`
$1

if [ $? -ne 0 ]; then
    echo  "ERROR:  $2"
    cd  ${LAST}
    exit  ${CODE}
fi

return
}



################################################################################
####################  The main program starts here.  ###########################
################################################################################

# Used to specify a release tag for a borealis branch (not nmstl).
# Leave blank to extract from the main branch.
#
#BRANCH="-r WINTER_2007"
BRANCH=

CODE=0
LAST=`pwd`


# Prepare a temporary directory for working.
#
runCommand  "mkdir  -p  ${HOME}/create.snapshot"                    \
            "Could not create:  ${HOME}/create.snapshot"

runCommand  "cd ${HOME}/create.snapshot"                            \
            "Could not go into:  ${HOME}/create.snapshot"

runCommand  "rm -R -f  ${HOME}/create.snapshot/borealis"            \
            "Could not remove:  ${HOME}/create.snapshot/borealis"

runCommand  "rm -R -f  ${HOME}/create.snapshot/nmstl"               \
            "Could not remove:  ${HOME}/create.snapshot/nmstl"


# Export the source code from CVS.
#
runCommand  "cvs export  -DNOW  ${BRANCH}  borealis/src"            \
            "Could not export Borealis src from CVS."

runCommand  "cvs export  -DNOW  ${BRANCH}  borealis/tool"           \
            "Could not export Borealis tool from CVS."

runCommand  "cvs export  -DNOW  ${BRANCH}  borealis/test"           \
            "Could not export Borealis test from CVS."

runCommand  "cvs export  -DNOW  ${BRANCH}  borealis/utility"        \
            "Could not export Borealis utility from CVS."

runCommand  "cvs export  -DNOW  ${BRANCH}  borealis/demo"           \
            "Could not export Borealis demo from CVS."

runCommand  "cvs export  -DNOW  ${BRANCH}  borealis/data"           \
            "Could not export Borealis data from CVS."

runCommand  "cvs export  -DNOW  ${BRANCH}  borealis/report/install_guide/"  \
            "Could not export Borealis report/install_guide from CVS."

runCommand  "rm -R -f  borealis/demo/sigmod2004"                    \
            "Could not remove: borealis/demo/sigmod2004"

runCommand  "rm -R -f  borealis/demo/vldb2004"                      \
            "Could not remove: borealis/demo/vldb2004"

runCommand  "cvs export  -DNOW   nmstl"                             \
            "Could not export Nmstl from CVS."



# Create the compresed tar ball.
#
runCommand  "tar  cvf  borealis.tar  borealis  nmstl"               \
            "Could not create the Borealis tar ball."

runCommand  "gzip  borealis.tar"                                    \
            "Could not compress the Borealis tar ball."

runCommand  "mv  borealis.tar.gz  ${HOME}/borealis.tar.gz"          \
            "Could not move the Borealis tar ball to ${HOME}"

cd  ${HOME}
rm  -R -f ${HOME}/create.snapshot
cd  ${LAST}

echo  "The Borealis tar ball is:  ${HOME}/borealis.tar.gz"
exit  0
#
#
##########################  end create.snapshot.sh  ############################
