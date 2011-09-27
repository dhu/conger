#!/bin/sh

##############################  yes_no.sh   ####################################
#
#  Prompt for a yes or no reply.
#
#  Entry:  Invoke with "source  yes_no.sh".
#          The argument is a quoted prompt.
#
#   Exit   ANSWER is yes or no.
#...............................................................................


ANSWER=""

while [ -z "${ANSWER}" ]; do
    if [ ! -z "${1}" ]; then
        echo -n "${1} [no]? "
    fi

    read  ANSWER

    if [ "${ANSWER}" = "" ]; then
        ANSWER="no"

    elif [ "${ANSWER}" = "y" ]; then
        ANSWER="yes"

    elif  [ "${ANSWER}" = "ye" ]; then
        ANSWER="yes"

    elif  [ "${ANSWER}" = "yes" ]; then
        ANSWER="yes"

    elif  [ "${ANSWER}" = "n" ]; then
        ANSWER="no"

    elif  [ "${ANSWER}" = "no" ]; then
        ANSWER="no"

    else
        echo  "Please answer yes or no."
        ANSWER=""
    fi
done

#
##############################  end yes_no.sh   ################################
