#!/bin/sh

########################  validate_move.sh  ####################################
#
#  Run an Borealis move box test.
#  This is not a standalone script.  It is invoked via:  validate.go.sh
#
#  Two nodes and a regional component are launched.
#  A single box is deployed.
#  The regional component issues a periodic move operation.
#  THe head is persistent???
#...............................................................................


launchBorealis

BOREALIS_PORT=15100
launchBorealis

REGIONAL_COMPONENT=region_move
launchRegion

runValidationTest

killRegion
killBorealis

#
#######################  end validate_move.sh  #################################
