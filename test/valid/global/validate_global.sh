#!/bin/sh

##############################  validate_global.sh  ############################
#
#  Run a Global Component test.
#  This is not a standalone script.  It is invoked via:  validate.go.sh
#
#  One node, a global component and a persistent Head are launched.
#  Some XML is deployed, but no queries are run.
#  The global component requests the global catalog and writes it.
#...............................................................................


removeValidationOut      # This must go first to setup the environment.

launchBorealis
launchHead

runValidationTest
compareValidationOut

killBorealis
killHead

#
##########################  end validate_global.sh  ############################
