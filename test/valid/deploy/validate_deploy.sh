#!/bin/sh

########################  validate_deploy.sh  ##################################
#
#  Run an example Borealis test that deploys multiple queries.
#  This is not a standalone script.  It is invoked via:  validate.go.sh
#...............................................................................


launchBorealis
launchHead

# Run validate_deploy in the background.
#
runBackgroundTest

# Wait and then validate_add taps into the output stream.
#
sleep  5
VALIDATION_TEST=add
runValidationTest

#  Wait for validate_deploy to complete.
#
sleep  10
killBorealis
killHead

#
#######################  end validate_deploy.sh  ###############################
