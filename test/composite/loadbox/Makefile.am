##################################################################
MYFLAGS = 

AM_CFLAGS = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

if COND_BDB

INCLUDES= @BOREALISINCLUDE@                                   \
    -I../../@BOREALISROOT@/modules/common                     \
    -I../../@BOREALISROOT@/modules/util                       \
    -I../../@BOREALISROOT@/modules/rpc                        \
    -I../../@BOREALISROOT@/modules/queryProcessor/runtime     \
    -I../../@BOREALISROOT@/modules/queryProcessor/expr        \
    -I../../@BOREALISROOT@/modules/queryProcessor/storageMgr  \
    -I../../@BOREALISROOT@/modules/catalog                    \
    -I ../../simple

lib_LTLIBRARIES = libdfilter.la 

MAIN = loadbox_test

bin_PROGRAMS = $(MAIN)

LDADD_MODULES = \
    -L../../@BOREALISROOT@/modules/common           -lborealiscommon   \
    -L../../@BOREALISROOT@/modules/queryProcessor   -lborealisqp       \
    -L../../@BOREALISROOT@/modules/util             -lborealisutil     \
    -L../../@BOREALISROOT@/external/bdb             -lborealisbdb

LDADD_EXTRAS = -ldl  @BOREALISLIB@  @EXTERNALLIB@

LDADD = $(LDADD_MODULES) $(LDADD_EXTRAS)

DEPENDENCIES = ../../@BOREALISROOT@/modules/common/libborealiscommon.la

TUPLESOURCE = InputTuple.h

loadbox_test_SOURCES = ${TUPLESOURCE} loadbox_test.cc

libdfilter_la_SOURCES = DynamicFilterQBox.cc DynamicFilterQBox.h

endif
