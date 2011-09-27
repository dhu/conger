#!/bin/sh

########################  validate_two_chains.sh  ################################
#
#  Run an example Borealis system test with two chains
#  This is not a standalone script.  It is invoked via:  validate.go.sh
#...............................................................................


launchBorealis

BOREALIS_PORT=15100
launchBorealis

runValidationTest

killBorealis

#
#######################  end validate_two_chains.sh  #############################
