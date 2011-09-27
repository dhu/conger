#!/bin/sh

###########################  update_file.sh  ###################################
#
#  $1 - Relative path of a single file to update.
#
#  Entry:  TMP, CVSROOT and CVS_COMMAND must be set.
#
#   Note:  This script is intended to be invoked by other scripts.
#...............................................................................


if [ -z "${1}" ]; then
    echo  "USAGE:  update_file.sh   File"
    echo  "        File - Relative path of a single file to update."

    exit  -1
fi


if [ ! -f  ${1} ]; then
    echo  "ERROR:  File (${1}) not found."

    exit  -1
fi


# Check if the file is new.
#
${CVS_COMMAND}  log  -R  ${1} > /dev/null  2>&1

if [ $? -ne 0 ]; then
    #  Put new files at the front of the list.
    #
    COMMIT="${1} ${COMMIT}"

else
    # Check if there have been any recent revisions.
    #
    cp  ${1}  ${TMP}/${1}

    # echo  "${CVS_COMMAND}  update  ${1}  > /dev/null"
    ${CVS_COMMAND}  update  ${1}  > /dev/null

    diff  ${1}  ${TMP}/${1}

    if [ $? -ne 0 ]; then
        echo  "WARNING:  File (${1}) has been changed since you checked it out."
        ${CVS_COMMAND}  diff  ${1}  > /dev/null

        if [ $? -ne 0 ]; then
            echo  "          The local copy has your changes merged with the recent changes."
            echo  "          The copy in ${TMP}/${1} has only your changes."
            echo  "          Checkout a fresh copy and merge in your changes."

            COMMIT=0

        else
            echo  "          Your local copy has been updated."
        fi
     
    elif [ ! "${COMMIT}" = "0" ]; then
        ${CVS_COMMAND}  diff  ${1}  > /dev/null

        if [ $? -ne 0 ]; then
            COMMIT="${COMMIT} ${1}"

        else
            # This listing will cease should COMMIT be set to zero.
            #
            echo  "NOTE:  Up to date:  ${1}"
        fi
    fi
fi

#
#########################  end update_file.sh  #################################
