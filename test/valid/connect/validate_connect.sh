#!/bin/sh

########################  validate_connect.sh  #################################
#
#  Run an Borealis connect box test.
#  Do not launch from the command line.  It is invoked via:  validate.go.sh
#
#  Two boxes are deployed.
#  The regional component inserts a box between the two boxes.
#...............................................................................



echo "###########   Single node, local connect."

launchBorealis

REGIONAL_COMPONENT=region_connect1
launchRegion

VALIDATION_TEST=connect1
runValidationTest

killBorealis
killRegion


echo  "##########   Two nodes, downstream node only in the region."
launchBorealis

BOREALIS_PORT=15100
launchBorealis

REGIONAL_COMPONENT=region_connect2
launchRegion

VALIDATION_TEST=connect2
runValidationTest

killRegion
killBorealis


echo  "##########     Two nodes, both in the region."
launchBorealis

BOREALIS_PORT=15100
launchBorealis

REGIONAL_COMPONENT=region_connect3
launchRegion

VALIDATION_TEST=connect3
runValidationTest

killRegion
killBorealis


echo  "##########     Two nodes, Upstream node only in the region."
launchBorealis

BOREALIS_PORT=15100
launchBorealis

REGIONAL_COMPONENT=region_connect4
launchRegion

VALIDATION_TEST=connect4
runValidationTest

killRegion
killBorealis


#
#######################  end validate_connect.sh  ##############################
