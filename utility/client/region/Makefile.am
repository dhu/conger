MYFLAGS =
AM_CFLAGS = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

#MAIN = Regiontest

#bin_PROGRAMS = $(MAIN)

LDADD_EXTRAS = @XERCESCLIB@   @NMSTLLIB@

LDADD = -L @BOREALISROOT@/modules/common          -lborealiscommon  \
        -L @BOREALISROOT@/modules/util            -lborealisutil    \
        -L .                                         -lborealisregion  \
        $(LDADD_EXTRAS)

INCLUDES = @XERCESCINCLUDE@  @NMSTLINCLUDE@          -I ../rpc         \
           -I @BOREALISROOT@/modules/common                         \
           -I @BOREALISROOT@/modules/util                           \
           -I @BOREALISROOT@/modules/catalog                        \
           -I @BOREALISROOT@/modules/rpc                            \
           -I @BOREALISROOT@/modules/queryProcessor/statsMgr        

DEPENDENCIES = @BOREALISROOT@/modules/common/libborealiscommon.la   \
  @BOREALISROOT@/modules/catalog/libborealiscatalog.la

lib_LTLIBRARIES = libborealisregion.la

libborealisregion_la_SOURCES = RegionalCatalog.cc  RegionalCatalog.h   \
                               RegionServer.cc     RegionServer.h      \
                               WriteRegion.cc      WriteRegion.h       \
                               StatisticsServer.cc StatisticsServer.h 

#Regiontest_SOURCES       = Regiontest.cc

#Regiontest_DEPENDENCIES  = $(DEPENDENCIES)   libborealisregion.la

#Regiontest_LDADD         = $(LDADD_EXTRAS) $(LDADD)                          \
#     -L@BOREALISROOT@/modules/catalog     -lborealiscatalog

#
##########################  end  Makefile.am for regions  ######################
