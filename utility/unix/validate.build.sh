#!/bin/sh


###########################  validate.build.sh  ################################
#
#  Check to see that Borealis builds properly in various configurations.
#  All optional external packages must be installed to use this script.
#  Borealis and nmstl must be checked out in your sandbox.
#
#  Before running you may want to set the environment variables as
#  described in the build.borealis.sh script.  If not set they will be
#  assigned default values.
#...............................................................................



################################################################################
#
InstallDefault()              #  Site specific default settings.
{
#  These settings work at Brown.
#
#  You may want to make a copy of this script and modify these defaults for
#  use at your site.  Note the there is no default location for nmstl.
#  This is because it's location must be specified as input to this script
#  or nmstl is built locally.
#...............................................................................


INSTALL_ROOT=/pro/borealis/software

if [ -z ${INSTALL_NMSTL} ]; then
    INSTALL_NMSTL=${INSTALL_ROOT}/nmstl
    export  INSTALL_NMSTL
fi

if [ -z ${INSTALL_ANTLR} ]; then
    INSTALL_ANTLR=${INSTALL_ROOT}/antlr
    export  INSTALL_ANTLR
fi

if [ -z ${INSTALL_XERCESC} ]; then
    INSTALL_XERCESC=${INSTALL_ROOT}/xerces
    export  INSTALL_XERCESC
fi

if [ -z ${INSTALL_BDB} ]; then
    INSTALL_BDB=${INSTALL_ROOT}/bdb
    export  INSTALL_BDB
fi

#if [ -z ${INSTALL_TINYDB} ]; then
#    INSTALL_TINYDB=${INSTALL_ROOT}/tinydb
#    export  INSTALL_TINYDB
#fi

return
}



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

echo  " CVS_SANDBOX = ${CVS_SANDBOX}"

return
}



################################################################################
############################### main entry point ###############################
################################################################################


InstallDefault                    # Default installation directories.
Sandbox                           # Locate the sandbox to build.


# Use the marshal program being built to build the tests.
#
PATH=${CVS_SANDBOX}/borealis/tool/marshal:${PATH}
export  PATH


echo  "####################################################################"
echo  "#########  Reconfigure all components, tools and tests.  ###########"
echo  "####################################################################"

# Borealis will not be rebuilt if the old version is there; so delete it.
#
rm -f  ${CVS_SANDBOX}/borealis/src/src/borealis

build.borealis.sh  -configure  -udb  -test  -demo  -tool  -client  -array

if [ $? -ne 0 ]; then
    echo  "ERROR:  Full Borealis configure failed."
    exit  -1

else
    echo  "SUCCESS:  Full Borealis configure worked."
fi


echo  "####################################################################"
echo  "#############  Build excluding udb, tools and tests.  ##############"
echo  "####################################################################"

rm -f  ${CVS_SANDBOX}/borealis/src/src/borealis
build.borealis.sh     # Do first so borealis builds even if a prior version.

if [ $? -ne 0 ]; then
    echo  "ERROR:  Borealis build (sans udb and tests) failed."
    exit  -1

else
    echo  "SUCCESS:  Borealis build (sans udb and tests) worked."
fi


echo  "####################################################################"
echo  "###########  Rebuild all components, tools and tests.  #############"
echo  "####################################################################"

rm -f  ${CVS_SANDBOX}/borealis/src/src/borealis
build.borealis.sh  -udb  -test  -demo  -tool  -client  -array

if [ $? -ne 0 ]; then
    echo  "ERROR:  Full Borealis build failed."
    exit  -1

else
    echo  "SUCCESS:  Full Borealis build worked."
fi


echo  "####################################################################"
echo  "#########  Rebuild excluding Bdb, udb, tools and tests.  ###########"
echo  "####################################################################"

INSTALL_BDB=
rm -f  ${CVS_SANDBOX}/borealis/src/src/borealis
build.borealis.sh

if [ $? -ne 0 ]; then
    echo  "ERROR:  Borealis build (sans Bdb, udb and tests) failed."
    exit  -1

else
    echo  "SUCCESS:  Borealis build (sans Bdb, udb and tests) worked."
fi


echo  "####################################################################"
echo  "#########  Rebuild Nmstl, tools and tests (skip Borealis).  ########"
echo  "####################################################################"

INSTALL_NMSTL=
rm -f  ${CVS_SANDBOX}/borealis/src/src/borealis
build.borealis.sh  -test  -demo  -tool  -client

if [ $? -ne 0 ]; then
    echo  "ERROR:  Nmstl and Borealis test build failed."
    exit  -1

else
    echo  "SUCCESS:  Nmstl and Borealis test build worked."
fi


echo  "####################################################################"
echo  "############  Rebuild Nmstl and Borealis (with udb).  ##############"
echo  "####################################################################"

rm -f  ${CVS_SANDBOX}/borealis/src/src/borealis
build.borealis.sh  -udb

if [ $? -ne 0 ]; then
    echo  "ERROR:  Nmstl and Borealis build (with udb) failed."
    exit  -1

else
    echo  "SUCCESS:  Nmstl and Borealis build (with udb) worked."
fi

echo
echo  "###########  All configurations were successfully built.   #########"

exit  0

#
#########################  end validate.build.sh  ##############################
