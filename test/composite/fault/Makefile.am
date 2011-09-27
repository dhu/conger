MYFLAGS =
AM_CFLAGS = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = faulttest sunions

bin_PROGRAMS = $(MAIN)

LDADD_EXTRAS = -ldl  @XERCESCLIB@

LDADD = -L../../@BOREALISROOT@/modules/common   -lborealiscommon      \
        -L../../@BOREALISROOT@/modules/util     -lborealisutil        \
        @NMSTLLIB@ $(LDADD_EXTRAS)

BASIC_INCLUDES = @XERCESCINCLUDE@   @NMSTLINCLUDE@                       \
                 -I ../../@BOREALISROOT@/modules/common               \
                 -I ../../@BOREALISROOT@/modules/util                 \
                 -I ../../@BOREALISROOT@/modules/rpc                  \
                 -I ../../@BOREALISROOT@/modules/catalog              \
                 -I ../../@BOREALISROOT@/modules/queryProcessor/statsMgr

# For temporary types of tuples and types of benchmarks
EXTRA_INCLUDES = -I ../../@BOREALISROOT@/modules/queryProcessor/

DEPENDENCIES = ../../@BOREALISROOT@/modules/common/libborealiscommon.la
INCLUDES = ${BASIC_INCLUDES} ${EXTRA_INCLUDES}



faulttest_DEPENDENCIES = FaulttestMarshal.cc \
                         FaulttestMarshal.h

faulttest_SOURCES = ${faulttest_DEPENDENCIES} faulttest.cc

#${faulttest_DEPENDENCIES}: faulttest.xml
#	marshal faulttest.xml

sunions_DEPENDENCIES = SunionsMarshal.h \
                       SunionsMarshal.cc

sunions_SOURCES = ${sunions_DEPENDENCIES} sunions.cc

#${sunions_DEPENDENCIES}: sunions.xml
#	marshal sunions.xml


#CLEANFILES = ${faulttest_DEPENDENCIES} \
#			 ${sunions_DEPENDENCIES}
