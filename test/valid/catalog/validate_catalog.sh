#!/bin/sh

########################  validate_catalog.sh  #################################
#
#  Run a quick catalog test.
#  This is not a standalone script.  It is invoked from validate.run.sh.
#...............................................................................


removeValidationOut
runValidationTest
compareValidationOut

#
######################  end validate_catalog.sh  ###############################
