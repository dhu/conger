#!/bin/sh

#########################  validate_array.sh  ##################################
#
#  Run an example Borealis array test.
#  This is not a standalone script.  It is invoked via:  validate.go.sh
#...............................................................................


echo "###########   Dense Array   ##########"

launchBorealis

VALIDATION_TEST=dense
runValidationTest

killBorealis


echo "###########   Sparse Array   ##########"

launchBorealis

VALIDATION_TEST=sparse
runValidationTest

killBorealis


#echo "###########   Array Chunks   ##########"

#launchBorealis

#VALIDATION_TEST=chunk
#runValidationTest

#killBorealis

#
########################  end validate_array.sh  ###############################
