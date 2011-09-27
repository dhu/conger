#!/bin/sh

#########################  validate_common.sh  #################################
#
#  Common functions that are invoked from validation test scripts:
#
#     ${VALIDATION_TEST}/validate_${VALIDATION_TEST}.sh
#
#  Status codes set by shared functions:
#
#    -4   Could not rebase test output (.out) file.
#    -5   A reference (.ref) file is missing.
#    -6   An output (.out) file is missing.
#    -7   An output (.out) file has incorrect results.
#
#   -10   Could not kill Borealis.
#   -11   Could not kill a regional component.
#   -12   Could not kill a persistent Head.
#...............................................................................


# Amount of time to wait after launching a program for it to start up.
#
SLEEP_TIME=2



################################################################################
#
removeValidationOut()  # Remove output prior to running a validation program.
{
#  Call this function from:  ${VALIDATION_TEST}/validate_${VALIDATION_TEST}.sh
#
#  This call must be first in your test script so that VALIDATION_OUT is set.
#
#          VALIDATION_STATUS:  Status code.
#     ${VALIDATION_TEST}.out:  Name of the output file.
#...............................................................................


if [ ${VALIDATION_STATUS} -eq 0 ]; then
    rm -f   ${VALIDATION_TEST}.out
    VALIDATION_STATUS=$?

    if [ ${VALIDATION_STATUS} -ne 0 ]; then
        echo  "validate.go.sh:  The ${VALIDATION_TEST}.out file could not be removed."
    fi
fi

# Signal that validation output is in use.
#
VALIDATION_OUT=1

return
}



################################################################################
#
runValidationTest()  # Run a validation test program.
{
#  Call this function from:  ${VALIDATION_TEST}/validate_${VALIDATION_TEST}.sh
#
#     validate_${VALIDATION_TEST}:  Name of the validation program to run.
#               VALIDATION_STATUS:  Status code.
#...............................................................................


if [ ${VALIDATION_OUT} -ne 0 ]  ||  [ ${REBASE_OUT} -eq 0 ]; then

    if [ ${VALIDATION_STATUS} -eq 0 ]; then
        if [ -d log ]; then
            ./validate_${VALIDATION_TEST} >& log/validate_${VALIDATION_TEST}.log

        else
            ./validate_${VALIDATION_TEST} >& /dev/null
        fi

        VALIDATION_STATUS=$?

        if [ ${VALIDATION_STATUS} -ne 0 ]; then
            echo  "validate.go.sh:  The validate_${VALIDATION_TEST} program returned ${VALIDATION_STATUS}."
        fi
    fi

else
    echo  "       Skipping as there is no output to rebase."
fi

return
}



################################################################################
#
runBackgroundTest()  # Run a validation test program in the background.
{
#  Call this function from:  ${VALIDATION_TEST}/validate_${VALIDATION_TEST}.sh
#
#     validate_${VALIDATION_TEST}:  Name of the validation program to run.
#               VALIDATION_STATUS:  Status code.
#...............................................................................


if [ ${VALIDATION_OUT} -ne 0 ]  ||  [ ${REBASE_OUT} -eq 0 ]; then

    if [ ${VALIDATION_STATUS} -eq 0 ]; then
        if [ -d log ]; then
            ./validate_${VALIDATION_TEST} >& log/validate_${VALIDATION_TEST}.log &

        else
            ./validate_${VALIDATION_TEST} >& /dev/null &
        fi

        VALIDATION_STATUS=$?

        if [ ${VALIDATION_STATUS} -ne 0 ]; then
            echo  "validate.go.sh:  The validate_${VALIDATION_TEST} program returned ${VALIDATION_STATUS}."
        fi
    fi

else
    echo  "       Skipping as there is no output to rebase."
fi

return
}




################################################################################
#
compareValidationOut()   # Compare output to a reference file.
{
#  Call this function from:  ${VALIDATION_TEST}/validate_${VALIDATION_TEST}.sh
#
#     ${VALIDATION_TEST}.out:  Name of the output file.
#     ${VALIDATION_TEST}.ref:  Name of the reference file.
#          VALIDATION_STATUS:  Status code.
#...............................................................................


if [ ${VALIDATION_STATUS} -eq 0 ]; then
    if [ ${REBASE_OUT} -ne 0 ]; then
        cp  ${VALIDATION_TEST}.out  ${VALID_REF}/${VALIDATION_TEST}.ref

        if [ $? -ne 0 ]; then
            echo  "validate.go.sh:  Could not rebase ${VALIDATION_TEST}.out."
            VALIDATION_STATUS=-4
        fi

    elif [ ! -e ${VALID_REF}/${VALIDATION_TEST}.ref ]; then
        echo  "validate.go.sh:  The ${VALID_REF}/${VALIDATION_TEST}.ref file is missing."
        VALIDATION_STATUS=-5

    elif [ ! -e ${VALID_REF}/${VALIDATION_TEST}.ref ]; then
        echo  "validate.go.sh:  The ${VALIDATION_TEST}.out file is missing."
        VALIDATION_STATUS=-6

    else
        diff  ${VALIDATION_TEST}.out  ${VALID_REF}/${VALIDATION_TEST}.ref
        STATUS=$?

        if [ ${STATUS} -ne 0 ]; then
            echo  "validate.go.sh:  The ${VALIDATION_TEST}.out file has incorrect results."
            VALIDATION_STATUS=-7
        fi
    fi
fi

return
}



################################################################################
#
launchBorealis()   # Launch Borealis on a local node for system tests.
{
#  Call this function from:  ${VALIDATION_TEST}/validate_${VALIDATION_TEST}.sh
#
#   BOREALIS_PORT - Zero (default port) or a port number; cleared upon exit.
#...............................................................................


if [ ${VALIDATION_STATUS} -eq 0 ]; then
    if [ ${VALIDATION_OUT} -ne 0 ]  || [ ${REBASE_OUT} -eq 0 ]; then

        if [ ! -d "log" ]; then
            mkdir  log
        fi

        if [ ${BOREALIS_PORT} -eq 0 ]; then
            exec  ../../../src/src/borealis >& log/borealis.log &

        else
            exec  ../../../src/src/borealis -d :${BOREALIS_PORT} >& log/borealis${BOREALIS_PORT}.log &
        fi

        sleep ${SLEEP_TIME}
        BOREALIS_PORT=0
    fi
fi

return
}



################################################################################
#
killBorealis()   # Terminate Borealis nodes.
{
#  Call this function from:  ${VALIDATION_TEST}/validate_${VALIDATION_TEST}.sh
#...............................................................................


if [ ${VALIDATION_OUT} -ne 0 ]  ||  [ ${REBASE_OUT} -eq 0 ]; then

    # echo  "kill Borealis ..."
    skill -9 -c lt-borealis
    skill -9 -c borealis

    if [ $? -ne 0 ]; then
        echo  "Could not kill Borealis."
        VALIDATION_STATUS=-10
    fi
fi

return
}




################################################################################
#
launchRegion()   # Launch a regional component on a local node for system tests.
{
#  Call this function from:  ${VALIDATION_TEST}/validate_${VALIDATION_TEST}.sh
#
#   REGIONAL_COMPONENT - A regional component to launch (may have arguments).
#...............................................................................


if [ ${VALIDATION_STATUS} -eq 0 ]; then

    # kill any prior copy that may have been left running.
    #
    killRegion
fi

if [ ${VALIDATION_STATUS} -eq 0 ]; then
    if [ ${VALIDATION_OUT} -ne 0 ]  || [ ${REBASE_OUT} -eq 0 ]; then

        if [ ! -d "log" ]; then
            mkdir  log
        fi

        exec ${REGIONAL_COMPONENT} >& log/region.log &

        sleep ${SLEEP_TIME}
    fi
fi

return
}



################################################################################
#
killRegion()   # Terminate a regional component.
{
#  Call this function from:  ${VALIDATION_TEST}/validate_${VALIDATION_TEST}.sh
#
#   REGIONAL_COMPONENT - A regional component to kill (may not have arguments).
#...............................................................................


if [ ${VALIDATION_OUT} -ne 0 ]  ||  [ ${REBASE_OUT} -eq 0 ]; then

    # echo  "kill a regional component..."
    skill -9 -c  lt-${REGIONAL_COMPONENT}
    skill -9 -c  ${REGIONAL_COMPONENT}

    if [ $? -ne 0 ]; then
       echo  "Could not kill a regional component."
       VALIDATION_STATUS=-11
    fi
fi

return
}



################################################################################
#
launchHead()   # Launch a persistent Head.
{
#  Call this function from:  ${VALIDATION_TEST}/validate_${VALIDATION_TEST}.sh
#
#...............................................................................


if [ ${VALIDATION_STATUS} -eq 0 ]; then
    if [ ${VALIDATION_OUT} -ne 0 ]  || [ ${REBASE_OUT} -eq 0 ]; then

        if [ ! -d "log" ]; then
            mkdir  log
        fi

        exec  ../../../tool/head/BigGiantHead >& log/head.log &

        sleep ${SLEEP_TIME}
    fi
fi

return
}



################################################################################
#
killHead()   # Terminate a persistent Head.
{
#  Call this function from:  ${VALIDATION_TEST}/validate_${VALIDATION_TEST}.sh
#
#...............................................................................


if [ ${VALIDATION_OUT} -ne 0 ]  ||  [ ${REBASE_OUT} -eq 0 ]; then

    # echo  "kill the persistent Head ..."
    skill -9 -c  lt-BigGiantHead
    skill -9 -c  BigGiantHead

    if [ $? -ne 0 ]; then
        echo  "Could not kill the persistent Head."
        VALIDATION_STATUS=-12
    fi
fi

return
}




################################################################################
#
runSelectTest()   # Run only selected tests.
{
#  SELECT_TEST - Upon entry has a list of tests to run (space seperator).
#...............................................................................


SELECT_DONE=""

while [ "${SELECT_TEST}" != "${SELECT_DONE}" ]; do
   VALIDATION_TEST=${SELECT_TEST%% *}
   validationTest

   SELECT_DONE=${SELECT_TEST}
   SELECT_TEST=${SELECT_TEST##* }
done

return
}


#
##########################  end validate_common.sh  ############################
