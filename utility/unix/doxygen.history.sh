#!/bin/sh

############################  doxygen.history.sh  ##############################
#
# Generate history logs and link to them from the generated doxygen history.
#
# Entry:  borealis must be checked out.
#
#         The Borealis utilities (which include this file and borealis_root.sh)
#             must be along the PATH variable.
#
#         The ssh-agent must allow you to access CVSROOT.
#
#         INPUT - Directories to look up (relative to ${CVS_SANDBOX}/src/doc/).
#                 If not set this is extracted from the doxygen.conf file.
#...............................................................................


#  Ensure the ssh-agent is running.
#
ssh-add -l

if [ $? != 0 ]; then
    echo  "ERROR:  The ssh-agent needs to be activated."

    exit  -1
fi
    

# CVS log data file.
#
LOG=/tmp/history_$$.log

source  borealis_root.sh               # Establish the cvs environment.
cd  ${CVS_SANDBOX}/borealis/src/doc    # Go to the code document directory.

if [ ! -x "doxygenHistory" ]; then
    echo  "ERROR:  Could not find the doxygenHistory program."

    exit  -1
fi

# Extract the directories containing source code from doxygen.conf.
#
if [ -z "${INPUT}" ]; then
    INPUT=`cat ./doxygen.conf | grep 'INPUT ' | grep '=' | grep '../' | grep -v '#'`

    if [ $? -ne 0 ]; then
        echo  "ERROR:  Could not extract INPUT from the doxygen.conf file."

        exit  -1
    fi
fi

echo  ${INPUT}
doxygen  doxygen.conf                 # Generate html for Borealis.


if [ $? -ne 0 ]; then
    echo  "ERROR:  Could not generate html with doxygen."

    exit  -1
fi

for MODULE  in  ${INPUT}; do
    if [ "${MODULE}" != "INPUT" ]  &&
       [ "${MODULE}" != "=" ]; then

        echo  "directory ${MODULE}"
        SOURCE=`ls  ${MODULE}/*.cc  ${MODULE}/*.h`

        for FILE in  ${SOURCE}; do
            cvs log  ${FILE} > ${LOG}
            STATUS=$?

            if [ ${STATUS} -eq 1 ]; then
                echo  "WARNING:  No log for ${FILE}"

            elif [ ${STATUS} -ne 0 ]; then
                echo  "ERROR:  cvs log failed (${STATUS})."

                exit  -1

            else
                echo  "  doxygenHistory  ${FILE}"
                doxygenHistory  ${FILE}  ${LOG}

                if [ $? -ne 0 ]; then
                    echo  "ERROR:  doxygenHistory failed."

                    exit  -1
                fi
            fi

            rm -f  ${LOG}

            if [ $? -ne 0 ]; then
                echo  "ERROR:  Could not remove the cvs log file (${LOG})."

                exit  -1
            fi
        done
    fi
done

#
############################  end doxygen.history.sh  ##########################
