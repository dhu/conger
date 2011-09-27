#!/bin/sh

##########################  checkin.change.sh  #################################
#
#  Check files into the repository.
#
#  checkin.change.sh  [-m  "Message"]  [ -b | <File> ] ...
#
#     -m  "Message"     Optional log message (must be the first argument).
#                       If no -m argument, an editor will open up for
#                          you to enter a log message.
#
#     -b                Add any new files as binaries.  You only need
#                          to specify this when adding new binary files.
#                          If you are you are checking in any combination
#                          of binary and source files and not adding them
#                          you do not need to specify -b. 
#
#     File              Relative file paths of select files to check in.
#                          Wildcards may be used.
#
#                       If no file arguments, check in new and changed
#                          source files in the working directory.
#                          Recognized extentions are:
#                             h  c  cc  am  java  sh  csh
#
#  If recent changes have been committed that are newer than the copies
#     in your sandbox then no files will be commited to the repository.
#
#  You'll be prompted before adding any new files to the repository.
#...............................................................................


# List of source file extentions to search when checking in a directory.
#
SOURCE="*.h *.c *.cc *.am *.java *.sh *.csh"


source  borealis_root.sh            # Establish the cvs environment.


# Parse and validate arguments.
#     LOG     - Null or -m if coded.
#     MESSAGE - Null or a message argument.
#     COMMIT  - List of validated files to commit.
#     FLAG    - Null or "-b" for binary mode.
#
MESSAGE=""
COMMIT=""
FLAG=""
LOG=""

if [ "${1}" = "-m" ]; then
    MESSAGE="${2}"
    LOG="-m"

    shift  2
fi

if [ "${1}" = "-b" ]; then
    shift

    FLAG="-b"
    echo "NOTE:  Adding any new files in binary mode."
fi

if [ ! -z ${1} ]; then
    until [ -z ${1} ]; do
        if [ "${1}" = "-b" ]; then
            shift

            FLAG="-b"
            echo "NOTE:  Adding any new files in binary mode."

        else
            FILE=`ls ${1}`

            if [ $? -ne 0 ]; then
                echo  "ERROR:  Invalid argument (${1}).  Nothing commited."
                exit  -1
            fi

            for CHANGE in ${FILE}; do
                #  Update files, but skip directories.
                #
                if [ -f ${CHANGE} ]; then
                    source  update_file.sh  ${CHANGE}
                fi
            done

            shift
        fi
    done

else
    # Scan this directory for new or changed source files.
    # Build up COMMIT with a list of files.
    #
    for CHANGE in `ls ${SOURCE} 2>/dev/null`; do
        if [ -f ${CHANGE} ]; then
            source  update_file.sh  ${CHANGE}
        fi
    done
fi


# Commit validated files.
#
if [ "${COMMIT}" = "" ]; then
    echo  "WARNING:  Nothing needs to be checked in."

elif [ "${COMMIT}" = "0" ]; then
    echo  WARNING:  Due to recent changes no files will be committed.

else
    if [ "${MESSAGE}" = "" ]; then
        LOG="${TMP}/cvs.message.log"
        rm  -f  ${LOG}

        # Check CVS_EDITOR???
        #
        if [ "${EDITOR}" = "" ]; then
            vi  ${LOG}

        else
            ${EDITOR}  ${LOG}
        fi

        if [ $? -ne 0 ]; then
            echo  "ERROR:  Failed to edit the message file."
            exit  -1
        fi

        if [ ! -f  ${LOG} ]; then
            echo  "ERROR:  No log message file was saved."
            exit  -1
        fi
    fi

    for CHANGE  in  ${COMMIT}; do
        commit_file.sh  ${CHANGE}  ${FLAG}  ${LOG}  "${MESSAGE}"
    done
fi

#
#############################  end checkin.change.sh  ##########################
