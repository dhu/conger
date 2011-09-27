MYFLAGS     =
AM_CFLAGS   = $(MYFLAGS)
AM_CXXFLAGS = $(MYFLAGS)

INCLUDES = @XERCESCINCLUDE@  @NMSTLINCLUDE@             \
           -I../../common    -I../../util               \
           -I../../rpc       -I../../queryProcessor     \
           -I../../catalog                              \
           -I../../queryProcessor/statsMgr

lib_LTLIBRARIES = libborealisnhopt.la

libborealisnhopt_la_SOURCES =                                    \
           NHOptimizer.h             NHOptimizer.cc              \
           ContractBasedOptimizer.h  ContractBasedOptimizer.cc   \
           QOSOptimizer.h            QOSOptimizer.cc             \
           NHLoadShedder.h           NHLoadShedder.cc
