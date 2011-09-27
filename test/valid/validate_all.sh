#!/bin/sh

#########################  validate_all.sh  ####################################
#
#  Add new subdirectory/test names here and invoke the main test script.
#  This is not a standalone script.  It is invoked from:  validate.go.sh
#...............................................................................


VALIDATION_TEST="catalog"
validationTest

VALIDATION_TEST="system"
validationTest

VALIDATION_TEST="move"
validationTest

VALIDATION_TEST="two_chains"
validationTest

VALIDATION_TEST="global"
validationTest

VALIDATION_TEST="connect"
validationTest

#VALIDATION_TEST="array"
#validationTest

#VALIDATION_TEST="revisions"
#validationTest

#
#########################  end validate_all.sh  ################################
