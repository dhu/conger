MYFLAGS =
AM_CFLAGS = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = suniontestin suniontestout suniontestmiddle

bin_PROGRAMS = $(MAIN)

LDADD_EXTRAS = -ldl -lcrypto @XERCESCLIB@
LDADD = -L../../../@BOREALISROOT@/modules/common   -lborealiscommon      \
        -L../../../@BOREALISROOT@/modules/util     -lborealisutil        \
        @NMSTLLIB@ $(LDADD_EXTRAS)

BASIC_INCLUDES = @XERCESCINCLUDE@   @NMSTLINCLUDE@                       \
                 -I ../../../@BOREALISROOT@/modules/common               \
                 -I ../../../@BOREALISROOT@/modules/util                 \
                 -I ../../../@BOREALISROOT@/modules/rpc                  \
                 -I ../../../@BOREALISROOT@/modules/catalog              \
                 -I ../../../@BOREALISROOT@/modules/queryProcessor/statsMgr

# For temporary types of tuples and types of benchmarks
EXTRA_INCLUDES = -I ../../../@BOREALISROOT@/modules/queryProcessor/

DEPENDENCIES = ../../../@BOREALISROOT@/modules/common/libborealiscommon.la
INCLUDES = ${BASIC_INCLUDES} ${EXTRA_INCLUDES}
suniontestin_SOURCES = suniontest.h suniontest.cc suniontestin.cc 
suniontestout_SOURCES = suniontest.h suniontest.cc suniontestout.cc
suniontestmiddle_SOURCES = suniontest.h suniontest.cc suniontestmiddle.cc

