#!/bin/sh


##########################  checkout.borealis.sh  ##############################
#
#  Run this to check out a fresh copy of the Borealis or Nmstl source code
#  to the sandbox under your home directory.
#
#     checkout.borealis.sh  [borealis]   Check out Borealis.
#     checkout.borealis.sh   nmstl       Check out Nmstl.
#
#  Exit:  This will delete a prior copy of your borealis or nmstl
#         sandbox directory.
#
#         The working directory is changed to the sandbox directory.
#
#...............................................................................


if [ -z "${1}" ]; then
    MODULE="borealis"

elif [ "${1}" = "borealis" ]; then
    MODULE="borealis"

elif [ "${1}" = "nmstl" ]; then
    MODULE="nmstl"

else
    echo  "USAGE:  checkout.borealis.sh  [borealis | nmstl]"
    echo  "           borealis - Check out Borealis."
    echo  "              nmstl - Check out Nmstl instead of Borealis."

    exit  -1
fi

source  borealis_root.sh            # Establish the cvs environment.

cd   ${CVS_SANDBOX}                 # Go to your sandbox directory.

if [ -d  "${MODULE}" ]; then
    if [ -d  "prior_${MODULE}" ]; then
        echo  "Removing the prior copy of ${MODULE}:  prior_${MODULE}"
        rm -f -r prior_${MODULE}    # Delete your old sandbox.
    fi

    echo  "Renaming the exisitng ${MODULE} to:  prior_${MODULE}"
    mv  ${MODULE}  prior_${MODULE}
fi

if [ $? -ne 0 ]; then
   echo  "ERROR:  Your prior ${MODULE} sandbox is in use and partially deleted."
   echo  "        Check for other shells using the sandbox."
   echo  "        Then rerun checkout.borealis.sh to get a clean copy."

   exit  -1
fi


${CVS_COMMAND}  checkout ${MODULE}     # Populate a new sandbox.

if [ $? -ne 0 ]; then
   echo  "ERROR:  Could not check out ${MODULE}."

   exit  -1
fi

#
##########################  end checkout.borealis.sh  ##########################
