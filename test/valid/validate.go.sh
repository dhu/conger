#!/bin/sh

############################  validate.go.sh  ##################################
#
#  This test driver launches a collection of quick self-checking tests
#  for Borealis.
#
#  Each test resides in a subdirectory and contains a main validation
#  script that runs the test and determines if it passed or failed.
#  Typically the subdirectory also includes test data files and a test
#  program that exercises part of the Borealis code base.
#
#  Support is also provided to compare test output against a reference
#  file containg the correct output.
#
#  Setting up reference output required to run the script:
#
#     Some tests use reference output for regression testing.
#     The installDefault function below established a directory to hold
#     reference output.  It defaults to a temporary directory; which
#     may not be where you want to put files for the long term.
#
#     If you modify this script for your site, you may want to put the copy in
#     a common test area.
#
#     Before the script can be run the reference output needs to be created:
#
#       > validate.go.sh -rebase
#
#
#  Running the tests:
#
#       > validate.go.sh                  # Run all the tests.
#       > validate.go.sh  <test> ...      # Run individual tests.
#
#
#  Debugging a test:
#
#     When successful these lightweight pass/fail tests only show that the
#     tested feature basically works.  If a test fails it means nothing and
#     the tests do not provide any useful diagnostic information for users.
#     Any error codes or messages are only helpful for debugging the tests.
#
#     Test failures could be due to any number of causes:
#
#       * Your environment is not set up properly.
#
#       * There is a stale copy of a component still running.
#         You can list and kill processes with:  utility/unix/killer.sh
#
#       * The was a timing glitch.  Borealis and the components are not
#         synchronized at startup.  Sometimes programs take too long to
#         initialize or shut down and communication fails.  Usually this
#         only happens to one test in the suite.  It will then work if
#         you run rerun the failed test by itself.  You can also increase
#         the sleep times in the scripts.
#
#       * There is a some other problem.  Run the test manually with debugging
#         on.  For example, to run the system test:
#
#            Open up a terminal window and run borealis.
#
#               borealis/src> setenv LOG_LEVEL 2
#               borealis/src> src/borealis
#         
#            Open up a second terminal window and run a second borealis.
#
#               borealis/src> setenv LOG_LEVEL 2
#               borealis/src> src/borealis -d :15100
#
#            Open up a third window and run the system test.
#
#               borealis/src> setenv LOG_LEVEL 2
#               borealis/test/valid/system> validate_system
#          
#
#  Adding a new test:
#
#    * Create a subdirectory (named <test>) for the test with a validation
#      script named:  validate_<test>.sh
#
#      The script may use the shared functions declared in validate_common.sh:
#         removeValidationOut  runValidationTest   compareValidationOut
#         launchBorealis       killBorealis
#
#      Upon return it should set the variable VALIDATION_STATUS to 0 if okay;
#          > 0 for an error detected by your test;
#          < 0 for an error detected by shared functions.
#
#    * You may want to include a main program named:  validate_<test>
#      The return code is 0 if okay; else an error code (> 0).
#
#    * The subdirectory will need a Makefile.am and any test input files.
#
#    * Add a log/ directory to the test subdirectory to record log files.
#      Any files with a .log suffix will be automatically erased from the
#      log/ directory before running the test.
#
#    * Modify the script validate_all.sh to launch your test.
#
#    * Modify borealis/test/valid/Makefile.am and
#             borealis/test/configure.ac (see AC_OUTPUT at the bottom).
#
#  If you use the removeValidationOut and compareValidationOut reference
#  output files will be created automatically.  Otherwise you'll need to
#  include rebasing code in your validation script (validate.<test>.sh).
#  The script can check the variable REBASE_OUT for 1 when rebasing;
#  otherwise it will be 0.
#
#  To install this software check the InstallDefault function below
#  to see if it is suitable for your site or computer.
#  This script may be run from any directory, but can not be relocated.
#...............................................................................



################################################################################
#
installDefault()              #  Site specific default settings.
{
#  VALID_REF - This is the directory to hold reference output files.
#
#...............................................................................


# See if a reference directory is established by the VALID_REF variable.
#
if [ -z ${VALID_REF} ]; then
    #  This directory is used by developers.
    #  You may want to change this for your site.
    #
    VALID_REF=/pro/borealis/test/valid

    if [ ! -d ${VALID_REF} ]; then
       echo  "No reference file directory:  ${VALID_REF}"
    fi

elif [ ! -d ${VALID_REF} ]; then

    # Establish a default directory.
    #
    VALID_REF=${TMP}/valid
    mkdir  ${VALID_REF}

    if [ $? -ne 0 ]; then
        echo  "Can not create the reference file directory:  ${VALID_REF}"
        exit -1
    fi
fi

return
}



################################################################################
#
validationTest()        # Run a single validation test
{
#...............................................................................


echo  "validate.go.sh:  Running the ${VALIDATION_TEST} test ..."

VALIDATION_MAIN=${VALIDATION_TEST}    # Save the name of the main test.
VALIDATION_STATUS=0   # Status code for each test.
VALIDATION_OUT=0      # Nonzero when comparing output (see removeValidationOut).
BOREALIS_PORT=0       # Zero to use the default port (see launchBorealis).

if [ -d "${TEST_VALID}/${VALIDATION_TEST}" ]; then
    cd  ${TEST_VALID}/${VALIDATION_TEST}

    if [ $? -ne 0 ]; then
        echo  "Could not go into the test directory:  ${TEST_VALID}/${VALIDATION_TEST}"
        FAIL_COUNT=`expr ${FAIL_COUNT} + 1`

    else
        if [ -d log ]; then
            rm -f log/*.log
        fi

        source  ./validate_${VALIDATION_TEST}.sh
        STATUS=$?

        if [ ${STATUS} -ne 0 ]; then
            echo  "The ${VALIDATION_TEST} test could not be run (status=${STATUS})."
            FAIL_COUNT=`expr ${FAIL_COUNT} + 1`

        elif [ ${VALIDATION_STATUS} -ne 0 ]; then
            echo  "The ${VALIDATION_TEST} test returned ${VALIDATION_STATUS}."
            FAIL_COUNT=`expr ${FAIL_COUNT} + 1`

        else
            echo  "   ... The ${VALIDATION_MAIN} test ran successfully."
        fi
    fi

else
    echo  "   ... The ${VALIDATION_TEST} test does not exist."
    FAIL_COUNT=`expr ${FAIL_COUNT} + 1`
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



################################################################################
#
printUsage()      # Message issued when the command line syntax is bad.
{
#...............................................................................


echo  "USAGE:  validate.go.sh  [-rebase]  [test ...]"
echo  "           -rebase   Run tests and reset the baseline output."
echo  "          test ...   Run only the listed tests."

return
}



################################################################################
#
parseCommand()      # Parse command line arguments.
{
#...............................................................................


REBASE_OUT=0
SELECT_TEST=""

while [ $# -ge 1 ]; do
    if [ "$1" = "-rebase" ]; then
        REBASE_OUT=1

    elif [ `expr substr $1 1 1` = "-" ]; then
        printUsage
        exit  -1

    elif [ "${SELECT_TEST}" = "" ]; then
        SELECT_TEST="$1"

    else
        SELECT_TEST="${SELECT_TEST} $1"
    fi

    shift 1
done

return
}



################################################################################
############################### main entry point ###############################
################################################################################


# Establish the rooted path name of the directory containing this script.
#
TEST_VALID=`dirname $0`
cd  ${TEST_VALID}
TEST_VALID=`pwd`


# Define common test script functions.
#
source  ${TEST_VALID}/validate_common.sh


installDefault
parseCommand  $@

if [ ${REBASE_OUT} -ne 0 ]; then
    echo  "validate.go.sh:  Rebasing output to:  ${VALID_REF}"
fi

FAIL_COUNT=0                  # Total number of tests that have failed.


# Kill off any components that may have been left running.
#
VALIDATION_OUT=0
VALIDATION_STATUS=0

killBorealis

if [ ${VALIDATION_STATUS} -eq 0 ]; then
    killHead
fi

if [ ${VALIDATION_STATUS} -eq 0 ]; then
    if [ "${SELECT_TEST}" = "" ]; then
        # Run all the tests and check the final results.
        #
        source  ${TEST_VALID}/validate_all.sh

    else
        runSelectTest
    fi
fi

echo

if [ ${FAIL_COUNT} -ne 0 ]; then
    if [ ${REBASE_OUT} -eq 0 ]; then
        echo  "validate.go.sh:  ****  ${FAIL_COUNT} validation test(s) failed.  ****"

    else
        echo  "validate.go.sh:  ****  ${FAIL_COUNT} test(s) failed to rebase.  ****"
    fi

    exit  -1

elif [ ${REBASE_OUT} -eq 0 ]; then
    echo  "validate.go.sh:  ****  All validation tests ran successfully.  ****"

else
    echo  "validate.go.sh:  ****  All validation tests were rebased successfully.  ****"
fi

exit 0

#
###########################  end validate.go.sh  ###############################
