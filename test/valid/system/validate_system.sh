#!/bin/sh

########################  validate_system.sh  ##################################
#
#  Run an example Borealis system test.
#  This is not a standalone script.  It is invoked via:  validate.go.sh
#...............................................................................


launchBorealis

BOREALIS_PORT=15100
launchBorealis

runValidationTest

killBorealis

#
#######################  end validate_system.sh  ###############################
