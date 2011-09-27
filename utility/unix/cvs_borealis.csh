#!/bin/csh

##########################  cvs_borealis.csh  ##################################
#
#  To use the Borealis utilities run this script from your login script.
#
#     source  <utility location>/cvs_borealis.csh  <utility location>
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


if ( ! $?1 ) then
    echo  "USAGE:  cvs.borealis.csh  <utility>"
    echo  "            utility - Location of the Borealis utility scripts."

    exit
endif

set  TOOL_BIN=${1}

if ( ! -e  ${TOOL_BIN}/borealis_root.csh )  then
    echo  "ERROR:  Can not find the Borealis utilities."

    exit
endif


#  Setup CVSROOT and CVS_RSH for your site.
#
source  ${TOOL_BIN}/borealis_root.csh


#  Add the Borealis utility directory to the PATH environment variable.
#
setenv  PATH  ${PATH}:${TOOL_BIN}


#  Declare aliases for user level cvs utilities.
#
alias  checkout  ${TOOL_BIN}/checkout.borealis.sh
alias  checkin   ${TOOL_BIN}/checkin.change.sh
#
########################  end cvs_borealis.csh  ################################
