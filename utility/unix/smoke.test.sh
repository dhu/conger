#!/bin/sh

########################  smoke.test.sh  #######################################
#
#  Run lightweight regression tests for Borealis.
#  Warning:  This will destroy your sandbox.  You might want to save it first.
#
#  Exit:  status = 0 for pass.
#                 -1 checkout failed.
#                 -2 build failed.
#                 -3 testing failed.
#...............................................................................


# Checkout a fresh copy of Borealis.
#
checkout.borealis.sh  borealis

if [ $? -ne 0 ]; then
    echo
    echo  "ERROR:  Borealis checkout failed."
    exit  -1
fi

echo
echo  "smoke.test.sh:  Borealis checkout worked."
echo


# Checkout a fresh copy of Nmstl.
# This is commented out while Nmstl is in disarry.
#
#checkout.borealis.sh  nmstl

#if [ $? -ne 0 ]; then
#    echo
#    echo  "ERROR:  Nmstl checkout failed."
#    exit  -1
#fi

#echo
#echo  "smoke.test.sh:  Nmstl checkout worked."
#echo


#  Build Nmstl, the primary version of Borealis, and the smoke test area.
#
build.borealis.sh  -udb  -client  -tool  -test.valid

if [ $? -ne 0 ]; then
    echo
    echo  "ERROR:  Nmstl or Borealis build failed."
    exit  -2
fi

echo
echo  "smoke.test.sh:  Borealis and Test Area build worked."
echo


# Run some simple tests.
#
source  borealis_root.sh

${CVS_SANDBOX}/borealis/utility/test/valid/validate.run.sh

exit  $?

#
###########################  end smoke.test.sh  ################################
