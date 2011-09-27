#!/bin/sh

##########################  cvs_borealis.sh  ###################################
#
#  To use the Borealis utilities run this script from your login script.
#
#     source  <utility>/cvs_borealis.csh  <utility>
#
#  It sets the CVSROOT, adds the tools to the PATH variable, and
#  declares some handy aliases.
#
#  You can override default settings for the utilities by setting:
#
#         CVSROOT - The directory containing the Borealis CVS repository.
#                   The default is to use a site-specific setting.
#                   Change this to work with a different repository.
#
#     CVS_SANDBOX - The directory where Borealis sources will be checked out.
#                   The default uses the HOME environment variable.
#                   Change this to keep local sources in a different directory.
#
#     CVS_COMMAND - The command used to invoke CVS.  The default is "cvs".
#                   Change this to use a different executable.
#...............................................................................


if [ -z "${1}" ]; then
    echo  "USAGE:  cvs_borealis.csh  <utility>"
    echo  "            utility - Location of the Borealis utility scripts."

else
    TOOL_BIN=${1}

    if [ ! -f  ${TOOL_BIN}/borealis_root.csh ]; then
        echo  "ERROR:  Can not find the Borealis utilities."

    else
        #  Setup CVSROOT and CVS_RSH for your site.
        #
        source  ${TOOL_BIN}/borealis_root.sh


        #  Add the Borealis utility directory to the PATH environment variable.
        # 
        PATH=${PATH}:${TOOL_BIN}
        export  PATH

        #  Declare aliases for user level cvs utilities.
        #
        alias  checkout=${TOOL_BIN}/checkout.borealis.sh
        alias  checkin=${TOOL_BIN}/checkin.change.sh
    fi
fi

# TOOL_BIN=`echo $0 | sed 's:\(.*\)/[^/]*$:\1:'`
#
########################  end cvs_borealis.sh  #################################
