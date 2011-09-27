#!/bin/sh

########################  validate_revisions.sh  ##################################
#
#  Run an example Borealis system test.
#  This is not a standalone script.  It is invoked via:  validate.go.sh
#...............................................................................


# run all four tests one after another
for VALIDATION_TEST in   upstream_revisionfilter       \
                         upstream_revisionmap          \
                         upstream_revisionaggregate    \
                         downstream_revisionaggregate

do

  echo

  sleep  3  
  launchBorealis
  runValidationTest
  killBorealis
  killBorealis

  STATUS=$?
  
  if [ ${STATUS} -ne 0 ]; then
      echo  "The \"${VALIDATION_TEST}\" test could not be run (status=${STATUS})."
      FAIL_COUNT=`expr ${FAIL_COUNT} + 1`

  elif [ ${VALIDATION_STATUS} -ne 0 ]; then
      echo  "The \"${VALIDATION_TEST}\" test returned ${VALIDATION_STATUS}."
      FAIL_COUNT=`expr ${FAIL_COUNT} + 1`

  else
      echo  " ... The \"${VALIDATION_TEST}\" test ran successfully."
  fi
  
done


# This is just for the final printout line from validate.go.sh
echo ; echo
VALIDATION_TEST="ALL_REVISION_TESTS"


#
#######################  end validate_revisions.sh  ###############################
