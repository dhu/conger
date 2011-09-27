SUBDIRS = loadManager  loadShedder

##################################################################

MYFLAGS     = 
AM_CFLAGS   = $(MYFLAGS)
AM_CXXFLAGS = $(MYFLAGS)

if COND_GLPK

MAIN = MetaOptimizer

bin_PROGRAMS = $(MAIN)

BOREALIS_SRC = ../@BOREALISROOT@

LDADD_EXTRAS = -lm  -lstdc++  @BOREALISLIB@     \
               @GLPKLIB@      @EXTERNALLIB@

LDADD = -L../../utility/client/region                 -lborealisregion       \
        -LloadShedder                                 -lborealisloadShedder  \
        -LloadManager               -llmLoadManager   -lborealisloadManager  \
        -L$(BOREALIS_SRC)/modules/catalog             -lborealiscatalog      \
        -L$(BOREALIS_SRC)/modules/ha                  -lborealisha           \
        -L$(BOREALIS_SRC)/modules/queryProcessor      -lborealisqp           \
        -L$(BOREALIS_SRC)/external                    -lborealisexternal     \
		-L$(BOREALIS_SRC)/modules/common              -lborealiscommon       \
        -L$(BOREALIS_SRC)/modules/util                -lborealisutil         \
        $(LDADD_EXTRAS)

INCLUDES = @XERCESCINCLUDE@  @NMSTLINCLUDE@  @GLPKINCLUDE@         \
           -I $(BOREALIS_SRC)/modules/common                       \
           -I $(BOREALIS_SRC)/modules/util                         \
           -I $(BOREALIS_SRC)/modules/catalog                      \
           -I $(BOREALIS_SRC)/modules/queryProcessor               \
           -I $(BOREALIS_SRC)/modules/queryProcessor/statsMgr      \
           -I $(BOREALIS_SRC)/modules/rpc                          \
           -I ../../utility/client/region                          \
           -I ./loadShedder                                        \
           -I ./loadManager

DEPENDENCIES = $(BOREALIS_SRC)/modules/common/libborealiscommon.la    \
               $(BOREALIS_SRC)/modules/util/libborealisutil.la        \
               $(BOREALIS_SRC)/modules/catalog/libborealiscatalog.la  \
               $(BOREALIS_SRC)/modules/queryProcessor/statsMgr/libborealisqpstatsMgr.la \
               ./loadShedder/libborealisloadShedder.la                \
               ./loadManager/liblmLoadManager.la                      \
               ./loadManager/libborealisloadManager.la

#lib_LTLIBRARIES = libborealisMetaOptimizer.la 
#libborealisMetaOptimizer_la_SOURCES =                 

MetaOptimizer_SOURCES = MetaOptimizer.cc 
MetaOptimizer_DEPENDENCIES = $(DEPENDENCIES)
#MetaOptimizer_DEPENDENCIES = libborealisloadShedder.la libborealisloadManager.la liblmLoadManager.la libborealisregion.la

endif
