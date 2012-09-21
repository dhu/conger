MYFLAGS =
AM_CFLAGS = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = main

bin_PROGRAMS = $(MAIN)

LDADD_EXTRAS = @BOREALISLIB@   @EXTERNALLIB@

LDADD = -L@BOREALISROOT@/modules/catalog               -lborealiscatalog  \
        -L@BOREALISROOT@/modules/ha                    -lborealisha       \
        -L@BOREALISROOT@/modules/queryProcessor        -lborealisqp       \
        -L@BOREALISROOT@/external                      -lborealisexternal \
        -L@BOREALISROOT@/../utility/client/region                  -lborealisregion   \
        -L@BOREALISROOT@/modules/common                -lborealiscommon   \
        -L@BOREALISROOT@/modules/util                  -lborealisutil     \
        -L@BOREALISROOT@/modules/cql                   -lcongercql        \
        $(LDADD_EXTRAS)

INCLUDES = @XERCESCINCLUDE@  @NMSTLINCLUDE@                        \
           -I@BOREALISROOT@/modules/common                         \
           -I@BOREALISROOT@/modules/util                           \
           -I@BOREALISROOT@/modules/catalog                        \
           -I@BOREALISROOT@/modules/queryProcessor                 \
           -I@BOREALISROOT@/modules/queryProcessor/statsMgr        \
           -I@BOREALISROOT@/modules/rpc                            \
           -I@BOREALISROOT@/../utility/client/region               \
           -I@BOREALISROOT@/../utility/client/rpc

main_SOURCES = main.cc DataProvider.cc DataProvider.h TrafficInputTuple.h
