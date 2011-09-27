MYFLAGS =
AM_CFLAGS = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = loadnetwork_test

bin_PROGRAMS = $(MAIN)

LDADD_EXTRAS = -ldl  @XERCESCLIB@  @NMSTLLIB@

LDADD = -L../../@BOREALISROOT@/modules/common -lborealiscommon          \
        -L../../@BOREALISROOT@/modules/util -lborealisutil              \
        $(LDADD_EXTRAS)

INCLUDES = @XERCESCINCLUDE@   @NMSTLINCLUDE@                            \
           -I ../../@BOREALISROOT@/modules/common                       \
           -I ../../@BOREALISROOT@/modules/util                         \
           -I ../../@BOREALISROOT@/modules/rpc                          \
           -I ../../@BOREALISROOT@/modules/catalog                      \
           -I ../../simple

DEPENDENCIES = ../../@BOREALISROOT@/modules/common/libborealiscommon.la

loadnetwork_test_SOURCES = loadnetwork_test.cc
