MYFLAGS =
AM_CFLAGS = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = suniondeploy sunionin

bin_PROGRAMS = $(MAIN)
BOREALIS_SRC = ../../../@BOREALISROOT@

LDADD_EXTRAS = -ldl  @BOREALISLIB@  @EXTERNALLIB@

LDADD = -L$(BOREALIS_SRC)/modules/common              -lborealiscommon       \
    -L$(BOREALIS_SRC)/modules/util                    -lborealisutil         \
    -L$(BOREALIS_SRC)/modules/queryProcessor          -lborealisqp           \
    -L$(BOREALIS_SRC)/modules/ha                      -lborealisha           \
    -L$(BOREALIS_SRC)/external                        -lborealisexternal     \
    -L../../../../utility/client/region               -lborealisregion       \
    -L$(BOREALIS_SRC)/modules/catalog                 -lborealiscatalog      \
    $(LDADD_EXTRAS)

BASIC_INCLUDES = @XERCESCINCLUDE@   @NMSTLINCLUDE@                 \
		-I $(BOREALIS_SRC)/modules/common                          \
		-I $(BOREALIS_SRC)/modules/util                            \
		-I $(BOREALIS_SRC)/modules/rpc                             \
		-I $(BOREALIS_SRC)/modules/catalog                         \
		-I $(BOREALIS_SRC)/modules/queryProcessor/statsMgr         \
		-I ../../../../utility/client/region                       \
		-I ../../../../utility/client/rpc                          \
		-I ../../../../tool/head

# For temporary types of tuples and types of benchmarks
EXTRA_INCLUDES = -I $(BOREALIS_SRC)/modules/queryProcessor/
DEPENDENCIES = $(BOREALIS_SRC)/modules/common/libborealiscommon.la
INCLUDES = ${BASIC_INCLUDES} ${EXTRA_INCLUDES}


##################################################
suniondeploy_SOURCES = SUnionTestParams.h SUnionTestParams.cc suniondeploy.cc

suniondeploy_DEPENDENCIES  = $(DEPENDENCIES)

suniondeploy_LDADD         = -L../../../../tool/head  -lborealisdeploy $(LDADD)


##################################################
sunionin_DEPENDENCIES = QueryMarshal.cc \
						QueryMarshal.h

sunionin_SOURCES = ${sunionin_DEPENDENCIES} SUnionTestParams.h SUnionTestParams.cc sunionin.cc

# No! Don't want to overwrite what has been generated anymore
#${sunionin_DEPENDENCIES}: Query.xml
#	marshal Query.xml


##################################################
#CLEANFILES = ${sunionin_DEPENDENCIES}
