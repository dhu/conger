##################################################################

MYFLAGS =  

AM_CFLAGS = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

if COND_TINYDB
MAIN= Test TestReader

bin_PROGRAMS= $(MAIN)

INCLUDES= -I../tinydbwraper -I../../rpc -I../../util -I../../common  \
          -I../../../ -I../../../src   -I../../ha                    \
          -I../../nhopt -I../../queryProcessor -I../../lookup @XERCESCINCLUDE@ @NMSTLINCLUDE@ @TINYDBINCLUDE@ @ANTLRINCLUDE@ -I../../util -I../../rpc -I../../queryProcessor/storageMgr -I../../queryProcessor/expr -I../../queryProcessor/runtime

Test_SOURCES = Test.cc
Test_LDADD = -L../tinydbwraper -ltinydbwrapper @XERCESCLIB@ @NMSTLLIB@ -L../../util -lborealisutil -L../../common -lborealiscommon -ldl -lcrypto 
Test_DEPENDENCIES = ../tinydbwraper/libtinydbwrapper.la ../../util/libborealisutil.la ../../common/libborealiscommon.la

TestReader_SOURCES = TestReader.cc
TestReader_LDADD = -L../tinydbwraper -ltinydbwrapper @XERCESCLIB@ @NMSTLLIB@ -L../../util -lborealisutil -L../../common -lborealiscommon -ldl -lcrypto 
TestReader_DEPENDENCIES = ../../util/libborealisutil.la ../../common/libborealiscommon.la

endif
