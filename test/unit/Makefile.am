MYFLAGS =
AM_CFLAGS = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = statsMgrtest   Diagramtest    Regiontest   Headtest

bin_PROGRAMS = $(MAIN)
BOREALIS_SRC = ../@BOREALISROOT@

LDADD_EXTRAS = -ldl  @BOREALISLIB@  @EXTERNALLIB@

LDADD = -L../../utility/client/region                 -lborealisregion     \
        -L$(BOREALIS_SRC)/modules/catalog             -lborealiscatalog    \
        -L$(BOREALIS_SRC)/modules/queryProcessor      -lborealisqp         \
        -L$(BOREALIS_SRC)/modules/ha                  -lborealisha         \
        -L$(BOREALIS_SRC)/external                    -lborealisexternal   \
        -L$(BOREALIS_SRC)/modules/common              -lborealiscommon     \
        -L$(BOREALIS_SRC)/modules/util                -lborealisutil       \
         $(LDADD_EXTRAS)


INCLUDES = @BOREALISINCLUDE@  @EXTERNALINCLUDE@                            \
           -I $(BOREALIS_SRC)/modules/common                               \
           -I $(BOREALIS_SRC)/modules/util                                 \
           -I $(BOREALIS_SRC)/modules/rpc                                  \
           -I $(BOREALIS_SRC)/modules/ha                                   \
           -I $(BOREALIS_SRC)/modules/catalog                              \
           -I $(BOREALIS_SRC)/modules/queryProcessor/statsMgr              \
           -I $(BOREALIS_SRC)/modules/queryProcessor/storageMgr            \
           -I $(BOREALIS_SRC)/modules/queryProcessor/expr                  \
           -I $(BOREALIS_SRC)/modules/queryProcessor/runtime               \
           -I $(BOREALIS_SRC)/modules/queryProcessor                       \
           -I ../../utility/client/region                                  \
           -I ../../utility/client/rpc                                     \
           -I ../../tool/head


DEPENDENCIES = $(BOREALIS_SRC)/modules/common/libborealiscommon.la         \
               $(BOREALIS_SRC)/modules/queryProcessor/expr/libborealisqpexpr.la   \
               $(BOREALIS_SRC)/modules/catalog/libborealiscatalog.la


statsMgrtest_SOURCES      = statsMgrtest.cc

statsMgrtest_DEPENDENCIES = $(DEPENDENCIES)                                \
       $(BOREALIS_SRC)/modules/queryProcessor/statsMgr/libborealisqpstatsMgr.la

statsMgrtest_LDADD        = $(LDADD_EXTRAS) $(LDADD)                       \
     -L$(BOREALIS_SRC)/modules/queryProcessor/statsMgr -lborealisqpstatsMgr


Diagramtest_SOURCES       = Diagramtest.cc

Diagramtest_DEPENDENCIES  = $(DEPENDENCIES)                                \
                            ../../tool/head/libborealisdeploy.la

Diagramtest_LDADD         = -L../../tool/head         -lborealisdeploy     \
                            $(LDADD)


Regiontest_SOURCES       = Regiontest.cc

Regiontest_DEPENDENCIES  = $(DEPENDENCIES)                                 \
                           ../../utility/client/region/libborealisregion.la

Regiontest_LDADD         = -L../../tool/head       -lborealisdeploy        \
                           $(LDADD)

Headtest_SOURCES       = Headtest.cc

Headtest_DEPENDENCIES  = $(DEPENDENCIES)                                   \
                         ../../utility/client/region/libborealisregion.la

Headtest_LDADD         = -L../../tool/head         -lborealisdeploy        \
                        $(LDADD) 

#
#####################  end  Makefile.am for unit tests  ########################
