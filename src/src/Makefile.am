MYFLAGS     =
AM_CFLAGS   = $(MYFLAGS)
AM_CXXFLAGS = $(MYFLAGS)

MAIN = borealis     monitor

bin_PROGRAMS = $(MAIN)

LDADD_EXTRAS = -ldl  @BOREALISLIB@  @EXTERNALLIB@

LDADD = -L../modules/queryProcessor        -lborealisqp          \
        -L../modules/catalog               -lborealiscatalog     \
        -L../external                      -lborealisexternal    \
        -L../modules/queryProcessor/expr   -lborealisqpexpr      \
        -L../modules/availMon              -lborealisavailmon    \
        -L../modules/ha                    -lborealisha          \
        -L../modules/optimizer/nhopt       -lborealisnhopt       \
        -L../modules/optimizer/local       -lborealislopt        \
        -L../modules/common                -lborealiscommon      \
        -L../modules/util                  -lborealisutil

INCLUDES = @BOREALISINCLUDE@   @EXTERNALINCLUDE@               \
           -I../modules/common                                 \
           -I../modules/util                                   \
           -I../modules/rpc                                    \
           -I../modules/availMon                               \
           -I../modules/optimizer/local                        \
           -I../modules/optimizer/nhopt                        \
           -I../modules/catalog                                \
           -I../modules/ha                                     \
           -I../modules/queryProcessor                         \
           -I../modules/queryProcessor/runtime                 \
           -I../modules/queryProcessor/storageMgr              \
           -I../modules/queryProcessor/statsMgr                \
           -I../modules/queryProcessor/expr                    \
           -I../modules/queryProcessor/boxes

DEPENDENCIES = ../modules/common/libborealiscommon.la

borealis_SOURCES = borealis.cc
borealis_LDADD = $(LDADD) $(LDADD_EXTRAS) 

borealis_DEPENDENCIES =                                                     \
                ../modules/catalog/libborealiscatalog.la                    \
                ../modules/availMon/libborealisavailmon.la                  \
                ../modules/optimizer/local/libborealislopt.la               \
                ../modules/optimizer/nhopt/libborealisnhopt.la              \
                ../modules/ha/libborealisha.la                              \
                ../modules/queryProcessor/libborealisqp.la                  \
                ../external/libborealisexternal.la                          \
                ../modules/queryProcessor/statsMgr/libborealisqpstatsMgr.la \
                $(DEPENDENCIES) 


# Not all are needed for the monitor???
#
monitor_LDADD = $(LDADD) $(LDADD_EXTRAS) 
monitor_DEPENDENCIES = $(DEPENDENCIES)
monitor_SOURCES = monitor.cc
