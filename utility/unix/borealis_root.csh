#!/bin/csh

############################  borealis_root.csh  ###############################
#
#  Establish the CVS root for Borealis.
#
#  This file can be invoked from a csh login script.
#      > source    <Boralis project directory>/bin/borealis_root.csh
#
#  This setting is site specific.
#  You can use an alternate Borealis repository by changing the root path.
#...............................................................................



################################################################################
#  Borealis site administrators may want to change the CVSROOT setting.
#  This is the Borealis repository at Brown.
#...............................................................................



if ( -e "/pro/borealis/cvs" ) then
    setenv  CVSROOT  /pro/borealis/cvs

else
    setenv  CVSROOT  ${USER}@ssh.cs.brown.edu:/pro/borealis/cvs
endif


#  CVS_RSH tells cvs to use ssh to transfer files.
#
if ( ! ${?CVS_RSH} ) then
    setenv  CVS_RSH  ssh
endif

#
#########################  end borealis_root.csh  ##############################
