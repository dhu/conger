#!/bin/sh

############################  borealis_root.sh  ################################
#
#  Establish the CVS root for Borealis.
#
#  These settings are site specific.
#...............................................................................



################################################################################
#
Sandbox()        # Establish the users sandbox directory.
{
#...............................................................................


if [ -z "${CVS_SANDBOX}" ]; then
    CVS_SANDBOX=${HOME}
    export  CVS_SANDBOX

elif [ ! -d "${CVS_SANDBOX}" ]; then
   echo  "ERROR:  Invalid CVS_SANDBOX setting:  ${CVS_SANDBOX}."
   exit  -1
fi

# echo  " CVS_SANDBOX = ${CVS_SANDBOX}"

return
}



################################################################################
############################### main entry point ###############################
################################################################################


# This is used by cvs commands to access the Borealis repository.
# You can use an alternate Borealis repository by changing the root path.
#
if [ -z "${CVSROOT}" ]; then
    ############################################################################
    #  Borealis site administrators should change the following line.
    #...........................................................................

    if [ -d  "/pro/borealis/cvs" ]; then
        CVSROOT=/pro/borealis/cvs
        export  CVSROOT

    else
        CVSROOT=${USER}@ssh.cs.brown.edu:/pro/borealis/cvs
        export  CVSROOT
    fi
fi


# The utility scripts use this command to run cvs.
#
if [ -z "${CVS_COMMAND}" ]; then
    ############################################################################
    #  Borealis site administrators may want to change the following line.
    #...........................................................................

    CVS_COMMAND=cvs
    export  CVS_COMMAND
fi


# Designate a temporary directory.
#
if [ -z "${TMP}" ]; then
    ############################################################################
    #  Borealis site administrators may want to change the following line.
    #...........................................................................

    TMP=/tmp
    export  TMP
fi


Sandbox                           # Establish the users sandbox directory.

#echo  "         TMP = ${TMP}"
#echo  "     CVSROOT = ${CVSROOT}"
#echo  " CVS_COMMAND = ${CVS_COMMAND}"
#echo  " CVS_SANDBOX = ${CVS_SANDBOX}"

#
##########################  end borealis_root.sh  ##############################
