#!/bin/sh

########################  build.borealis.sh  ###################################
#
#  Build Borealis from source in your sandbox.  Before running this script:
#
#    * You need to check out borealis.
#
#    * Set any environment variables you'd like to have override defaults.
#      There are several packages required to build and run Borealis.
#      The installation directories for these packages are determined by:
#
#            First using the following command arguments:
#
#                -nmstl     <install>
#                -antlr     <install>
#                -bdb       <install>
#                -xercesc   <install>
#                -glpk      <install>
#                -gsl       <install>
#                -ocv       <install>
#
#            Secondly by checking these environment variables:
#
#                INSTALL_NMSTL
#                INSTALL_ANTLR
#                INSTALL_BDB
#                INSTALL_XERCESC
#                INSTALL_GLPK
#                INSTALL_GSL
#                INSTALL_OCV
#
#            Thirdly by using default values encoded in this script.
#            Site administrators my want to make a local copy of this script
#            and modify the InstallDefault function to use different values.
#
#            Note that components to access Berkeley-Db are optional.
#            By default those components are not built unless specified.
#
#        The CVS_SANDBOX variable can be used to specify where you have
#        checked out the source code.  By default CVS_SANDBOX is set to
#        the value of the HOME variable.  If you are not using CVS, but are
#        working with a snapshot, set CVS_SANDBOX to the directory where
#        you have unrolled the snapshot tar file.
#
#    * If nmstl is not already installed this script can build it.
#      In this case you need to checkout nmstl as well.
#      If the -nmstl argument is not used and INSTALL_NMSTL is not set
#      then this script will build nmstl under your home directory.
#      It will be installed in the ${HOME}/install_nmstl/ directory.
#
#    * -configure  Run configure for the selected components.
#                  Creates Makefiles, but does not run them.
#                  After performing a configure you can run
#                  make in individual directories.
#
#    * -static     Build using static linking.  The default is dynamic.
#
#    * -udb        Build the library containing user defined boxes.
#
#    * -array      Build the array processing component.
#                  This component requires ocv.
#
#    * -test    Build the test area (borealis/test) as well.
#               Borealis will be built before building the the test area.
#               If Borealis has already been built then it will not be rebuilt.
#
#               You can also compile individual test directories with:
#
#                   -test.simple  -test.composite  -test.unit
#                   -test.valid   -test.developer
#
#    * -demo    Build the demos (borealis/demo) as well.
#               Borealis will be built before building the the demos.
#               If Borealis has already been built then it will not be rebuilt.
#
#    * -tool    Build the tools (borealis/tool) as well.
#               Borealis will be built before building the the tools.
#               If Borealis has already been built then it will not be rebuilt.
#
#               You can also compile individual tool directories with:
#
#                   -tool.head  -tool.marshal  -tool.optimzer
#                   -tool.load  -tool.monitor  -tool.stats
#
#    * -client  Build the client interfaces (borealis/utility/client) as well.
#               Borealis will be built before building the the clients.
#               If Borealis has already been built then it will not be rebuilt.
#
#  This script will also set and export the LD_LIBRARY_PATH unless it is
#  already defined.  It will be set to to the libraries needed to run Borealis.
#  You may want to do this in your login script.
#
#  To run Borealis you will also need to add the nmstl bin/ directory to the
#  PATH variable.  If you let this script build it for you it will be in
#  "${HOME}/install_nmstl/bin".  You may want to do this in your login script.
#
#  The return status is 0 when the script runs to completion.
#  If an error is detected (not all may be) the return status is -1.
#...............................................................................



################################################################################
#
InstallDefault()              #  Site specific default settings.
{
#  These settings work at Brown.
#
#  You may want to make a copy of this script and modify it for use at
#  your site.  Note the there is no default location for nmstl.  This
#  is because it's location must be specified as input to this script
#  or nmstl is built locally.
#...............................................................................


INSTALL_ROOT=/pro/borealis/software

if [ -z ${INSTALL_ANTLR} ]; then
    INSTALL_ANTLR=${INSTALL_ROOT}/antlr
fi

if [ -z ${INSTALL_XERCESC} ]; then
    INSTALL_XERCESC=${INSTALL_ROOT}/xerces
fi

#  The remaining settings are commented out because they are optional.

#if [ -z ${INSTALL_BDB} ]; then
#    INSTALL_BDB=${INSTALL_ROOT}/bdb
#fi

#if [ -z ${INSTALL_GLPK} ]; then
#    INSTALL_GLPK=${INSTALL_ROOT}/glpk
#fi

#if [ -z ${INSTALL_GSL} ]; then
#    INSTALL_GSL=${INSTALL_ROOT}/gsl
#fi

#if [ -z ${INSTALL_OCV} ]; then
#    INSTALL_OCV=${INSTALL_ROOT}/ocv
#fi


return
}



################################################################################
#
Usage()      # Message issued when the command line syntax is bad.
{
#...............................................................................


echo  "USAGE:  build.borealis.sh"
echo  "           -configure    -demo    -client   -static    -udb    -array"
echo  "           -tool[.marshal | .head | .stats | .monitor | .load | optimizer]"
echo  "           -test[.simple | .composite | .unit | .valid | .developer]"
echo  "           -nmstl     <install path>"
echo  "           -antlr     <install path>"
echo  "           -bdb       <install path>"
echo  "           -xercesc   <install path>"
echo  "           -glpk      <install path>"
echo  "           -gsl       <install path>"
echo  "           -ocv       <install path>"

return
}



################################################################################
#
ParseCommandLine()      # Parse command line arguments.
{
#...............................................................................


BUILD_UDB=0
BUILD_ARRAY=0

BUILD_TEST=0
BUILD_TEST_SIMPLE=0
BUILD_TEST_COMPOSITE=0
BUILD_TEST_UNIT=0
BUILD_TEST_VALID=0
BUILD_TEST_DEVELOPER=0

BUILD_TOOL=0
BUILD_TOOL_MARSHAL=0
BUILD_TOOL_HEAD=0
BUILD_TOOL_STATS=0
BUILD_TOOL_MONITOR=0
BUILD_TOOL_LOAD=0
BUILD_TOOL_OPTIMIZER=0

BUILD_DEMO=0
BUILD_CLIENT=0

CONFIGURE_ONLY=0
BUILD_STATIC=0


while [ $# -ge 1 ]; do
    if [ "$1" = "-test" ]; then
        shift  1
        BUILD_TEST=1
        BUILD_TEST_SIMPLE=1
        BUILD_TEST_COMPOSITE=1
        BUILD_TEST_UNIT=1
        BUILD_TEST_VALID=1
        BUILD_TEST_DEVELOPER=1

    elif [ "$1" = "-test.simple" ]; then
        shift  1
        BUILD_TEST=1
        BUILD_TEST_SIMPLE=1

    elif [ "$1" = "-test.composite" ]; then
        shift  1
        BUILD_TEST=1
        BUILD_TEST_COMPOSITE=1

    elif [ "$1" = "-test.unit" ]; then
        shift  1
        BUILD_TEST=1
        BUILD_TEST_UNIT=1

    elif [ "$1" = "-test.valid" ]; then
        shift  1
        BUILD_TEST=1
        BUILD_TEST_VALID=1

    elif [ "$1" = "-test.developer" ]; then
        shift  1
        BUILD_TEST=1
        BUILD_TEST_DEVELOPER=1

    elif [ "$1" = "-demo" ]; then
        shift  1
        BUILD_DEMO=1

    elif [ "$1" = "-tool" ]; then
        shift  1
        BUILD_TOOL=1
        BUILD_TOOL_MARSHAL=1
        BUILD_TOOL_HEAD=1
        BUILD_TOOL_STATS=1
        BUILD_TOOL_MONITOR=1
        BUILD_TOOL_LOAD=1
        BUILD_TOOL_OPTIMIZER=1

    elif [ "$1" = "-tool.marshal" ]; then
        BUILD_TOOL=1
        BUILD_TOOL_MARSHAL=1
        shift  1
 
    elif [ "$1" = "-tool.head" ]; then
        BUILD_TOOL=1
        BUILD_TOOL_HEAD=1
        shift  1

    elif [ "$1" = "-tool.stats" ]; then
        BUILD_TOOL=1
        BUILD_TOOL_STATS=1
        shift  1

    elif [ "$1" = "-tool.monitor" ]; then
        BUILD_TOOL=1
        BUILD_TOOL_MONITOR=1
        shift  1

    elif [ "$1" = "-tool.load" ]; then
        BUILD_TOOL=1
        BUILD_TOOL_LOAD=1
        shift  1
 
    elif [ "$1" = "-tool.optimizer" ]; then
        BUILD_TOOL=1
        BUILD_TOOL_OPTIMIZER=1
        shift  1
  
    elif [ "$1" = "-client" ]; then
        shift  1
        BUILD_CLIENT=1

    elif [ "$1" = "-udb" ]; then
        shift  1
        BUILD_UDB=1

    elif [ "$1" = "-array" ]; then
        shift  1
        BUILD_ARRAY=1

    elif [ "$1" = "-configure" ]; then
        shift  1
        CONFIGURE_ONLY=1

    elif [ "$1" = "-static" ]; then
        if [ ${BUILD_STATIC} -eq 1 ]; then
           echo  "ERROR:  The -static option was set more than once."
           exit  -1
        fi

        shift  1
        BUILD_STATIC=1

    elif [ "$1" = "-help" ]; then
        Usage
        exit

    else
        if [ $# -eq 1 ]   ||
           [ ! -d "$2" ]; then

           echo  "ERROR:  Unknown keyword $1 or a following install path argument is missing."
           echo  "  got:  $2"
           exit  -1
        fi

        if [ ! -d "$2/lib" ]; then
           echo  "ERROR:  Missing library directory for $1."
           echo  "  got:  $2/lib"
           exit  -1
        fi

        case $1 in
          -xercesc)  INSTALL_XERCESC=$2;;
            -nmstl)    INSTALL_NMSTL=$2;;
            -antlr)    INSTALL_ANTLR=$2;;
              -bdb)      INSTALL_BDB=$2;;
             -glpk)     INSTALL_GLPK=$2;;
              -gsl)      INSTALL_GSL=$2;;
              -ocv)      INSTALL_OCV=$2;;
                 *)  echo  "Unknown argument: $1 $2"
                     Usage
                     exit  -1    ;;
        esac

        shift 2
    fi
done

return
}



################################################################################
#
ValidateInstall()        # Validate installation libraries.
{
#...............................................................................


echo " INSTALL_ANTLR   = ${INSTALL_ANTLR}"
echo " INSTALL_XERCESC = ${INSTALL_XERCESC}"

if [ -z ${INSTALL_NMSTL} ]; then
    echo   " INSTALL_NMSTL   = ${HOME}/install_nmstl"
fi

if [ ! -z ${INSTALL_NMSTL} ]; then
    echo   " INSTALL_NMSTL   = ${INSTALL_NMSTL}"

    if [ ! -d "${INSTALL_NMSTL}/lib" ]; then
        echo  "ERROR:  Missing nmstl library (${INSTALL_NMSTL}/lib"
        exit  -1
fi; fi

if [ ! -d "${INSTALL_XERCESC}/lib" ]; then
    echo  "ERROR:  Missing xercesc library (${INSTALL_XERCESC}/lib"
    exit  -1
fi

if [ ! -d "${INSTALL_ANTLR}/lib" ]; then
    echo  "ERROR:  Missing antlr library (${INSTALL_ANTLR}/lib"
    exit  -1
fi

if [ ! -z ${INSTALL_BDB} ]; then
    echo " INSTALL_BDB     = ${INSTALL_BDB}"

    if [ ! -d "${INSTALL_BDB}/lib" ]; then
        echo  "ERROR:  Missing Berkeley-Db library (${INSTALL_BDB}/lib"
        exit  -1
fi; fi

if [ ! -z ${INSTALL_GLPK} ]; then
    echo " INSTALL_GLPK    = ${INSTALL_GLPK}"

    if [ ! -d "${INSTALL_GLPK}/lib" ]; then
        echo  "ERROR:  Missing Glpk library (${INSTALL_GLPK}/lib"
        exit  -1
fi; fi

if [ ! -z ${INSTALL_GSL} ]; then
    echo " INSTALL_GSL     = ${INSTALL_GSL}"

    if [ ! -d "${INSTALL_GSL}/lib" ]; then
        echo  "ERROR:  Missing Gsl library (${INSTALL_GSL}/lib"
        exit  -1
fi; fi

if [ ! -z ${INSTALL_OCV} ]; then
    echo " INSTALL_OCV     = ${INSTALL_OCV}"

    if [ ! -d "${INSTALL_OCV}/lib" ]; then
        echo  "ERROR:  Missing Open Computer Vision library (${INSTALL_OCV}/lib"
        exit  -1
fi; fi


echo

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
#
BuildNmstl()    # Build nmstl locally.
{
#...............................................................................


#  Configure nmstl for your environment.
#
if [ ! -d "${CVS_SANDBOX}/nmstl" ]; then
    echo  "ERROR:  You need to checkout nmstl."
    exit  -1
fi

cd  ${CVS_SANDBOX}/nmstl

if [ $? -ne 0 ]; then
    echo  "ERROR:  Could not:  cd  ${CVS_SANDBOX}/nmstl"
    exit  -1
fi

echo  "*************** Building Nmstl ******************"
autoconf

if [ $? -ne 0 ]; then
    echo  "ERROR:  Nmstl autoconf failed."
    exit  -1
fi

./configure  --prefix=${INSTALL_NMSTL}

if [ $? -ne 0 ]; then
    echo  "ERROR:  Nmstl configure failed."
    exit  -1
fi


# Now you can build and install nmstl.
#
if [ ${CONFIGURE_ONLY} -eq 0 ]; then
    # Install nmstl in your home directory.  Then use the same directory path as
    # argument when configuring borealis (below)
    #
    rm  -r -f  ${INSTALL_NMSTL}                           # Remove any prior installation.
    mkdir      ${INSTALL_NMSTL}                           # Any subdirectory of home.

    if [ $? -ne 0 ]; then
        echo  "ERROR:  Could not:  mkdir  ${INSTALL_NMSTL}"
        exit  -1
    fi

    make clean

    if [ $? -ne 0 ]; then
        echo  "ERROR:  Nmstl 'make clean' failed."
        exit  -1
    fi


    make

    if [ $? -ne 0 ]; then
        echo  "ERROR:  Nmstl 'make' failed."
        exit  -1
    fi


    make install

    if [ $? -ne 0 ]; then
        echo  "ERROR:  Nmstl 'make install' failed."
        exit  -1
    fi
fi

return
}                            # end BuildNmstl



################################################################################
#
BuildBorealis()    # Build Borealis.
{
#  Run setup to generate a fresh configure script.  The setup script runs:
#
#      aclocal      - Automatically generate aclocal.m4 from configure.ac
#      libtoolize   - Configures either shared or static libraries.
#      autoheader   - Create a template header for configure.
#      automake --add-missing   automatically create Makefile.in's from Makefile.am's
#      autoconf     - Generate  a  configuration script from `configure.ac'
#                     Output is sent to `configure'.
#
#      configure.ac ->  aclocal      -> aclocal.m4
#
#                                       AC_CONFIG_AUX_DIR=config
#                                    -> config/config.sub   @ /usr/share/libtool
#      configure.ac ->  libtoolize   -> config/config.guess @ /usr/share/libtool
#                                    -> config/ltmain.sh    @ /usr/share/libtool
#
#                                    -> config.h.in
#      configure.ac ->  autoheader   -> autom4te.cache/output.0
#                                    -> autom4te.cache/requests
#                                    -> autom4te.cache/traces.0
#
#                                    -> stamp-h.in
#     */Makefile.am ->               -> */Makefile.in
#      configure.ac ->  automake     -> config/install-sh    @ /usr/share/automake-1.4
#        aclocal.m4 ->               -> config/mkinstalldirs @ /usr/share/automake-1.4
#                                    -> config/ missing      @ /usr/share/automake-1.4
#
#      configure.ac ->  autoconf     -> configure
#        aclocal.m4 ->
#
#       source code ->  configure    -> config.log
#                           |        -> config.status
#                           V
#       config.h.in -> config.status -> config.h
#     */Makefile.in ->               -> */Makefile
#
# config.status
#    Run this file to recreate the current configuration.
#
# config.log
#    Compiler output produced by configure, useful for debugging
#    configure, is in config.log if it exists.
#
# stamp-h.in
#    It always contains the string `timestamp'.  It is used as a timestamp
#    file indicating whether `config.in' is up to date. 
#...............................................................................


if [ ! -d "${CVS_SANDBOX}/borealis" ]; then
    echo  "ERROR:  You need to checkout borealis."
    exit  -1
fi

cd  ${CVS_SANDBOX}/borealis/src

if [ $? -ne 0 ]; then
    echo  "ERROR:  Could not:  cd  ${CVS_SANDBOX}/borealis/src"
    exit  -1
fi

echo  "*************** Building Borealis ***************"
./setup

if [ $? -ne 0 ]; then
    echo  "ERROR:  Borealis setup failed."
    exit  -1
fi


# Configure Borealis.
#
# "wtf" comes with nmstl and pretty-prints all gcc error messages (great tool)!
# It translates acronyms and filename suffixes for you.
# The "--enable-shared --disable-static" flags should prevent the
# compiler from generating static libraries.
#
WITH_OPTION=

if [ ! -z ${INSTALL_BDB} ]; then
    WITH_OPTION="--with-bdb=${INSTALL_BDB}"
fi

if [ ! -z ${INSTALL_GSL} ]; then
    WITH_OPTION="${WITH_OPTION} --with-gsl=${INSTALL_GSL}"
fi

if [ ! -z ${INSTALL_OCV} ]; then
    WITH_OPTION="${WITH_OPTION} --with-ocv=${INSTALL_OCV}"
fi

if [ ${BUILD_UDB} -eq 1 ]; then
    WITH_OPTION="${WITH_OPTION} --enable-udb"
fi

if [ ${BUILD_ARRAY} -eq 1 ]; then
    if [ -z ${INSTALL_OCV} ]; then
        echo  "ERROR:  The array processor requires OCV."
        exit  -1
    fi

    WITH_OPTION="${WITH_OPTION} --enable-array"
fi


wtf ./configure  ${ENABLE_BINARY}           \
     --with-antlr=${INSTALL_ANTLR}          \
     --with-nmstl=${INSTALL_NMSTL}          \
     --with-xercesc=${INSTALL_XERCESC}      \
     ${WITH_OPTION}

if [ $? -ne 0 ]; then
    echo  "ERROR:  Could not configure Borealis."
    exit  -1
fi


#
# Compile Borealis
#
if [ ${CONFIGURE_ONLY} -eq 0 ]; then
    make  clean

    if [ $? -ne 0 ]; then
        echo  "ERROR:  Borealis 'make clean' failed."
        exit  -1
    fi


    make

    if [ $? -ne 0 ]; then
        echo  "ERROR:  Borealis 'make' failed."
        exit  -1
    fi
fi

return
}                            # end BuildBorealis



################################################################################
#
BuildTest()    # Build the test area.
{
#...............................................................................


#
#  Run setup to generate a fresh configure script.
#
if [ ! -d "${CVS_SANDBOX}/borealis/test" ]; then
    echo  "ERROR:  You need to checkout the Borealis test area."
    exit  -1
fi

cd  ${CVS_SANDBOX}/borealis/test

if [ $? -ne 0 ]; then
    echo  "ERROR:  Could not:  cd  ${CVS_SANDBOX}/borealis/test"
    exit  -1
fi

echo  "*************** Building Tests ******************"
./setup

if [ $? -ne 0 ]; then
    echo  "ERROR:  Borealis test area setup failed."
    exit  -1
fi


#
# Configure the Borealis test area.
#
WITH_OPTION=

if [ ! -z ${INSTALL_BDB} ]; then
    WITH_OPTION="--with-bdb=${INSTALL_BDB}"
fi

if [ ! -z ${INSTALL_OCV} ]; then
    WITH_OPTION="${WITH_OPTION} --with-ocv=${INSTALL_OCV}"
fi

wtf ./configure   ${ENABLE_BINARY}        \
     --with-nmstl=${INSTALL_NMSTL}        \
     --with-xercesc=${INSTALL_XERCESC}    \
     --with-antlr=${INSTALL_ANTLR}        \
     ${WITH_OPTION}


if [ $? -ne 0 ]; then
    echo  "ERROR:  Could not configure the test area."
    exit  -1
fi



#
# Compile the Borealis test directories.
#
if [ ${CONFIGURE_ONLY} -eq 0 ]; then
    echo "BUILD_TEST_SIMPLE=${BUILD_TEST_SIMPLE}"
    echo "BUILD_TEST_COMPOSITE=${BUILD_TEST_COMPOSITE}"
    echo "BUILD_TEST_UNIT=${BUILD_TEST_UNIT}"
    echo "BUILD_TEST_VALID=${BUILD_TEST_VALID}"
    echo "BUILD_TEST_DEVELOPER=${BUILD_TEST_DEVELOPER}"

    if [ ${BUILD_TEST_SIMPLE} -ne 0 ]; then
        cd  ${CVS_SANDBOX}/borealis/test/simple
        make  clean

        if [ $? -ne 0 ]; then
            echo  "ERROR:  Borealis test/simple/ 'make clean' failed."
            exit  -1
        fi

        make

        if [ $? -ne 0 ]; then
            echo  "ERROR:  Borealis test/simple/ 'make' failed."
            exit  -1
        fi
    fi

    if [ ${BUILD_TEST_COMPOSITE} -ne 0 ]; then
        cd  ${CVS_SANDBOX}/borealis/test/composite
        make  clean

        if [ $? -ne 0 ]; then
            echo  "ERROR:  Borealis test/composite/ 'make clean' failed."
            exit  -1
        fi

        make

        if [ $? -ne 0 ]; then
            echo  "ERROR:  Borealis test/composite/ 'make' failed."
            exit  -1
        fi
    fi

    if [ ${BUILD_TEST_UNIT} -ne 0 ]; then
        cd  ${CVS_SANDBOX}/borealis/test/unit
        make  clean

        if [ $? -ne 0 ]; then
            echo  "ERROR:  Borealis test/unit/ 'make clean' failed."
            exit  -1
        fi

        make

        if [ $? -ne 0 ]; then
            echo  "ERROR:  Borealis test/unit/ 'make' failed."
            exit  -1
        fi
    fi

    if [ ${BUILD_TEST_VALID} -ne 0 ]; then
        cd  ${CVS_SANDBOX}/borealis/test/valid
        make  clean

        if [ $? -ne 0 ]; then
            echo  "ERROR:  Borealis test/valid/ 'make clean' failed."
            exit  -1
        fi

        make

        if [ $? -ne 0 ]; then
            echo  "ERROR:  Borealis test/valid/ 'make' failed."
            exit  -1
        fi
    fi

    if [ ${BUILD_TEST_DEVELOPER} -ne 0 ]; then
        cd  ${CVS_SANDBOX}/borealis/test/developer
        make  clean

        if [ $? -ne 0 ]; then
            echo  "ERROR:  Borealis test/developer/ 'make clean' failed."
            exit  -1
        fi

        make

        if [ $? -ne 0 ]; then
            echo  "ERROR:  Borealis test/developer/ 'make' failed."
            exit  -1
        fi
    fi
fi

return
}                            # end BuildTest



################################################################################
#
BuildTool()    # Build the Borealis tools.
{
#...............................................................................


#  Run setup to generate a fresh configure script.
#
if [ ! -d "${CVS_SANDBOX}/borealis/tool" ]; then
    echo  "ERROR:  You need to checkout the Borealis tool directory."
    exit  -1
fi

cd  ${CVS_SANDBOX}/borealis/tool

if [ $? -ne 0 ]; then
    echo  "ERROR:  Could not:  cd  ${CVS_SANDBOX}/borealis/tool"
    exit  -1
fi

echo  "*************** Building Tools ******************"
./setup

if [ $? -ne 0 ]; then
    echo  "ERROR:  Borealis tool setup failed."
    exit  -1
fi


#
# Configure the Borealis tool build.
#
# We build the tools statically so they can be installed without depending
# on dynamic libraries in developer's sandbox.  This doesn't quite work.
#
# For dynamic build use:  wtf ./configure --enable-shared --disable-static
#
WITH_OPTION=

if [ ! -z ${INSTALL_BDB} ]; then
    WITH_OPTION="--with-bdb=${INSTALL_BDB}"
fi

if [ ! -z ${INSTALL_GLPK} ]; then
    WITH_OPTION="${WITH_OPTION} --with-glpk=${INSTALL_GLPK}"
fi

if [ ! -z ${INSTALL_OCV} ]; then
    WITH_OPTION="${WITH_OPTION} --with-ocv=${INSTALL_OCV}"
fi

wtf ./configure   ${ENABLE_BINARY}           \
     --with-nmstl=${INSTALL_NMSTL}           \
     --with-xercesc=${INSTALL_XERCESC}       \
     --with-antlr=${INSTALL_ANTLR}           \
     ${WITH_OPTION}


if [ $? -ne 0 ]; then
    echo  "ERROR:  Could not configure the tool build."
    exit  -1
fi


#
# Compile the Borealis tools.
#
if [ ${CONFIGURE_ONLY} -eq 0 ]; then
    echo "BUILD_TOOL_MARSHAL=${BUILD_TOOL_MARSHAL}"
    echo "BUILD_TOOL_HEAD=${BUILD_TOOL_HEAD}"
    echo "BUILD_TOOL_STATS=${BUILD_TOOL_STATS}"
    echo "BUILD_TOOL_MONITOR=${BUILD_TOOL_MONITOR}"
    echo "BUILD_TOOL_LOAD=${BUILD_TOOL_LOAD}"
    echo "BUILD_TOOL_OPTIMIZER=${BUILD_TOOL_OPTIMIZER}"

    if [ ${BUILD_TOOL_MARSHAL} -ne 0 ]; then
        cd  ${CVS_SANDBOX}/borealis/tool/marshal
        make  clean

        if [ $? -ne 0 ]; then
            echo  "ERROR:  Borealis tool/marshal/ 'make clean' failed."
            exit  -1
        fi

        make

        if [ $? -ne 0 ]; then
            echo  "ERROR:  Borealis tool/marshal/ 'make' failed."
            exit  -1
        fi
    fi

    if [ ${BUILD_TOOL_HEAD} -ne 0 ]; then
        cd  ${CVS_SANDBOX}/borealis/tool/head
        make  clean

        if [ $? -ne 0 ]; then
            echo  "ERROR:  Borealis tool/head/ 'make clean' failed."
            exit  -1
        fi

        make

        if [ $? -ne 0 ]; then
            echo  "ERROR:  Borealis tool/head/ 'make' failed."
            exit  -1
        fi
    fi

    if [ ${BUILD_TOOL_STATS} -ne 0 ]; then
        cd  ${CVS_SANDBOX}/borealis/tool/statsView
        make  clean

        if [ $? -ne 0 ]; then
            echo  "ERROR:  Borealis tool/statsView/ 'make clean' failed."
            exit  -1
        fi

        make

        if [ $? -ne 0 ]; then
            echo  "ERROR:  Borealis tool/statsView/ 'make' failed."
            exit  -1
        fi
    fi

    if [ ${BUILD_TOOL_MONITOR} -ne 0 ]; then
        cd  ${CVS_SANDBOX}/borealis/tool/monitor
        make  clean

        if [ $? -ne 0 ]; then
            echo  "ERROR:  Borealis tool/monitor/ 'make clean' failed."
            exit  -1
        fi

        make

        if [ $? -ne 0 ]; then
            echo  "ERROR:  Borealis tool/monitor/ 'make' failed."
            exit  -1
        fi
    fi

    if [ ${BUILD_TOOL_LOAD} -ne 0 ]; then
        cd  ${CVS_SANDBOX}/borealis/tool/loadGenerator
        make  clean

        if [ $? -ne 0 ]; then
            echo  "ERROR:  Borealis tool/loadGenerator/ 'make clean' failed."
            exit  -1
        fi

        make

        if [ $? -ne 0 ]; then
            echo  "ERROR:  Borealis tool/loadGenerator/ 'make' failed."
            exit  -1
        fi
    fi

    if [ ${BUILD_TOOL_OPTIMIZER} -ne 0 ]; then
        cd  ${CVS_SANDBOX}/borealis/tool/optimizer
        make  clean

        if [ $? -ne 0 ]; then
            echo  "ERROR:  Borealis tool/optimizer/ 'make clean' failed."
            exit  -1
        fi

        make

        if [ $? -ne 0 ]; then
            echo  "ERROR:  Borealis tool/optimizer/ 'make' failed."
            exit  -1
        fi
    fi
fi

return
}                            # end BuildTool



################################################################################
#
BuildDemo()    # Build the Borealis demos.
{
#...............................................................................


#  Run setup to generate a fresh configure script.
#
if [ ! -d "${CVS_SANDBOX}/borealis/demo" ]; then
    echo  "ERROR:  You need to checkout the Borealis demo directory."
    exit  -1
fi

cd  ${CVS_SANDBOX}/borealis/demo

if [ $? -ne 0 ]; then
    echo  "ERROR:  Could not:  cd  ${CVS_SANDBOX}/borealis/demo"
    exit  -1
fi

echo  "*************** Building Demos ******************"
./setup

if [ $? -ne 0 ]; then
    echo  "ERROR:  Borealis demo setup failed."
    exit  -1
fi


WITH_OPTION=

if [ ! -z ${INSTALL_BDB} ]; then
    WITH_OPTION="--with-bdb=${INSTALL_BDB}"
fi

if [ ! -z ${INSTALL_OCV} ]; then
    WITH_OPTION="${WITH_OPTION} --with-ocv=${INSTALL_OCV}"
fi

if [ ! -z ${INSTALL_GSL} ]; then
    WITH_OPTION="${WITH_OPTION} --with-gsl=${INSTALL_GSL}"
fi


#
# Configure the Borealis demo build.
#
wtf ./configure   ${ENABLE_BINARY}            \
     --with-nmstl=${INSTALL_NMSTL}            \
     --with-xercesc=${INSTALL_XERCESC}        \
     --with-antlr=${INSTALL_ANTLR}            \
     ${WITH_OPTION}


if [ $? -ne 0 ]; then
    echo  "ERROR:  Could not configure the demo build."
    exit  -1
fi


#
# Compile the Borealis demos.
#
if [ ${CONFIGURE_ONLY} -eq 0 ]; then
    make  clean

    if [ $? -ne 0 ]; then
        echo  "ERROR:  Borealis demo 'make clean' failed."
        exit  -1
    fi

    make

    if [ $? -ne 0 ]; then
        echo  "ERROR:  Borealis demo 'make' failed."
        exit  -1
    fi
fi

return
}                            # end BuildDemo



################################################################################
#
BuildClient()    # Build the Borealis clients.
{
#...............................................................................


#  Run setup to generate a fresh configure script.
#
if [ ! -d "${CVS_SANDBOX}/borealis/utility/client" ]; then
    echo  "ERROR:  You need to checkout the Borealis client directory."
    exit  -1
fi

cd  ${CVS_SANDBOX}/borealis/utility/client

if [ $? -ne 0 ]; then
    echo  "ERROR:  Could not:  cd  ${CVS_SANDBOX}/borealis/utility/client"
    exit  -1
fi

echo  "*************** Building Clients ****************"
./setup

if [ $? -ne 0 ]; then
    echo  "ERROR:  Borealis client setup failed."
    exit  -1
fi


#
# Configure the Borealis client build.
#
if [ ! -z ${INSTALL_BDB} ]; then
    WITH_OPTION="--with-bdb=${INSTALL_BDB}"
fi

if [ ! -z ${INSTALL_OCV} ]; then
    WITH_OPTION="${WITH_OPTION} --with-ocv=${INSTALL_OCV}"
fi

if [ ! -z ${INSTALL_GSL} ]; then
    WITH_OPTION="${WITH_OPTION} --with-gsl=${INSTALL_GSL}"
fi

wtf ./configure   ${ENABLE_BINARY}        \
     --with-nmstl=${INSTALL_NMSTL}        \
     --with-xercesc=${INSTALL_XERCESC}    \
     --with-antlr=${INSTALL_ANTLR}        \
     ${WITH_OPTION}


if [ $? -ne 0 ]; then
    echo  "ERROR:  Could not configure the client build."
    exit  -1
fi


#
# Compile the Borealis clients.
#
if [ ${CONFIGURE_ONLY} -eq 0 ]; then
    make  clean

    if [ $? -ne 0 ]; then
        echo  "ERROR:  Borealis client 'make clean' failed."
        exit  -1
    fi

    make

    if [ $? -ne 0 ]; then
        echo  "ERROR:  Borealis client 'make' failed."
        exit  -1
    fi
fi

return
}                            # end BuildClient


################################################################################
#
LibraryPath()   # Set up the LD_LIBRARY_PATH to contain the libraries needed to run Borealis.
{
#...............................................................................


LD_LIBRARY_PATH=${INSTALL_ANTLR}/lib
LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${INSTALL_XERCESC}/lib

if [ ! -z ${INSTALL_BDB} ]; then
    LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${INSTALL_BDB}/lib
fi

if [ ! -z ${INSTALL_GLPK} ]; then
    LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${INSTALL_GLPK}/lib
fi

if [ ! -z ${INSTALL_GSL} ]; then
    LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${INSTALL_GSL}/lib
fi

if [ ! -z ${INSTALL_OCV} ]; then
    LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${INSTALL_OCV}/lib
fi

if [ ! -z ${INSTALL_NMSTL} ]; then
    LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${INSTALL_NMSTL}/lib

else
    LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${HOME}/install_nmstl/lib
fi

return
}                            # end LibraryPath



################################################################################
############################### main entry point ###############################
################################################################################


InstallDefault                    # Default installation directories.
ParseCommandLine  $@              # Parse the build options.
ValidateInstall                   # Validate installed libraries.


# Set up the LD_LIBRARY_PATH to contain the libraries needed to run Borealis.
# You may want to set LD_LIBRARY_PATH in your login script.
#
if [ -z ${LD_LIBRARY_PATH} ]; then
    LibraryPath                   # Setup the LD_LIBRARY_PATH for Borealis.
    export  LD_LIBRARY_PATH       # Export it when not set.

else
    LibraryPath                   # Setup the LD_LIBRARY_PATH for Borealis.
fi

echo "LD_LIBRARY_PATH = ${LD_LIBRARY_PATH}"

Sandbox                          # Locate the CVS_SANDBOX.

if [ ${BUILD_STATIC} -eq 0 ]; then
    ENABLE_BINARY="--enable-shared --disable-static"
    echo  "   NOTE:  Building with dynamic linking."
else

    ENABLE_BINARY="--enable-static --disable-shared"
    echo  "   NOTE:  Building with static linking."
fi

BUILD_SET=""

# Optionally build and install nmstl.
#
if [ -z ${INSTALL_NMSTL} ]; then
    INSTALL_NMSTL=${HOME}/install_nmstl
    BuildNmstl
    BUILD_SET="Nmstl, "
fi


# Add the nmstl bin/ directory to your path.
# If you installed locally it will be in "${HOME}/install_nmstl/bin".
# You may want to set this up in your login script.
#
PATH=${PATH}:${INSTALL_NMSTL}/bin


if [ ${BUILD_TEST}   -eq 0 ]  &&                   \
   [ ${BUILD_DEMO}   -eq 0 ]  &&                   \
   [ ${BUILD_TOOL}   -eq 0 ]  &&                   \
   [ ${BUILD_CLIENT} -eq 0 ]; then

    BuildBorealis
    BUILD_SET="${BUILD_SET}Borealis "

else
    if [ ! -f "${CVS_SANDBOX}/borealis/src/src/borealis" ]; then
        BuildBorealis
        BUILD_SET="${BUILD_SET}Borealis, "
    fi

    if [ ${BUILD_CLIENT} -ne 0 ]; then
        BuildClient
        BUILD_SET="${BUILD_SET}Clients"

        if [ ${BUILD_DEMO} -ne 0 ]  ||              \
           [ ${BUILD_TOOL} -ne 0 ]  ||              \
           [ ${BUILD_TEST} -ne 0 ]; then

            BUILD_SET="${BUILD_SET}, "
        fi
    fi

    # The statsView tool dependes on the Java client.
    #
    if [ ${BUILD_TOOL} -ne 0 ]; then
        BuildTool
        BUILD_SET="${BUILD_SET}Tools"

        if [ ${BUILD_DEMO} -ne 0 ]  ||               \
           [ ${BUILD_TEST} -ne 0 ]; then

            BUILD_SET="${BUILD_SET}, "
        fi 
    fi

    # The unit Diagramtest depends on the head tool.
    #
    if [ ${BUILD_TEST} -ne 0 ]; then
        BuildTest
        BUILD_SET="${BUILD_SET}Tests"

        if [ ${BUILD_DEMO} -ne 0 ]; then
            BUILD_SET="${BUILD_SET}, "
        fi
    fi

    if [ ${BUILD_DEMO} -ne 0 ]; then
        BuildDemo
        BUILD_SET="${BUILD_SET}Demos"
    fi
fi

if [ ${CONFIGURE_ONLY} -eq 0 ]; then
    echo  "<<<  build.borealis.sh:  Successfully built:  ${BUILD_SET} >>>"

else
    echo  "<<<  build.borealis.sh:  Successfully configured:  ${BUILD_SET} >>>"
fi

exit  0

#
############################  end build.borealis.sh  ###########################
