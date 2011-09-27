#!/bin/sh

##########################  commit_file.sh  ####################################
#
#  commit_file.sh  <File>  [-b]  { <Log> | -m "message" }
#
#     File - Relative path of a single file to check in.
#     -b   - If adding the file, use binary mode.  Otherwise ignored.
#     Log  - A log message file to apply.
#     -m   - A quoted message.
#
#  Entry:  CVSROOT and CVS_COMMAND must be set.
#
#   Note:  This script is intended to be invoked by other scripts.
#...............................................................................


if [ -z "${1}" ]; then
    echo  'USAGE:  commit_file.sh   File   { Log | -m "Message" }'
    echo  "        File - Relative path of a single file to check in."
    echo  "         Log - Relative path of a file containing a log message."
    echo  "     Message - A quoted log message."

else
    FILE=${1}
    shift
 
    FLAG=""
 
    if [ "${1}" = "-b" ]; then
        FLAG="-kb"
        shift
    fi

    # echo "${CVS_COMMAND}  log -R  ${FILE}  > /dev/null"
    ${CVS_COMMAND}  log -R  ${FILE}  > /dev/null  2>&1

    if [ $? -ne 0 ]; then
        source  yes_no.sh  "Add file (${FILE})"

        if [ "${ANSWER}" = "yes" ]; then
            if [ "${1}" = "-m" ]; then
                ${CVS_COMMAND}  add  ${FLAG} -m "${2}"  ${FILE}

            else
                ${CVS_COMMAND}  add  ${FLAG} -F ${1}  ${FILE}
            fi
        fi

    else
        ANSWER="yes"
    fi

    if [ "${ANSWER}" = "yes" ]; then
        if [ "${1}" = "-m" ]; then
            ${CVS_COMMAND}  commit  -m "${2}"  ${FILE}

        else
            ${CVS_COMMAND}  commit  -F ${1}  ${FILE}
        fi

        if [ $? -ne 0 ]; then
            echo  "ERROR:  Could not commit:  ${FILE}"
            exit  -1
        fi
    fi
fi

#
#########################  end commit_file.sh  #################################
