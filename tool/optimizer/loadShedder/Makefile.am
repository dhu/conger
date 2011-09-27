MYFLAGS     =
AM_CFLAGS   = $(MYFLAGS)
AM_CXXFLAGS = $(MYFLAGS)

if COND_GLPK

MAIN = LoadSheddingTool

bin_PROGRAMS = $(MAIN)
BOREALIS_SRC = ../../@BOREALISROOT@

LDADD_EXTRAS = -lm  -lstdc++                                           \
               @BOREALISLIB@  @GLPKLIB@  @EXTERNALLIB@

LDADD = -L.                                      -lborealisloadShedder \
        -L../../../utility/client/region         -lborealisregion      \
        -L$(BOREALIS_SRC)/modules/ha             -lborealisha          \
        -L$(BOREALIS_SRC)/modules/catalog        -lborealiscatalog     \
        -L$(BOREALIS_SRC)/modules/queryProcessor -lborealisqp          \
        -L$(BOREALIS_SRC)/external               -lborealisexternal    \
        -L$(BOREALIS_SRC)/modules/common         -lborealiscommon      \
        -L$(BOREALIS_SRC)/modules/util           -lborealisutil        \
        $(LDADD_EXTRAS)

INCLUDES = @BOREALISINCLUDE@   @GLPKINCLUDE@                       \
           -I ../../@BOREALISROOT@/modules/common                  \
           -I ../../@BOREALISROOT@/modules/util                    \
           -I ../../@BOREALISROOT@/modules/catalog                 \
           -I ../../@BOREALISROOT@/modules/queryProcessor          \
           -I ../../@BOREALISROOT@/modules/queryProcessor/statsMgr \
           -I ../../@BOREALISROOT@/modules/optimizer/local         \
           -I ../../@BOREALISROOT@/modules/rpc                     \
           -I ../../../utility/client/region                       \
           -I ..                                                   \
           -I .

DEPENDENCIES =                                                               \
    $(BOREALIS_SRC)/modules/common/libborealiscommon.la                      \
    $(BOREALIS_SRC)/modules/catalog/libborealiscatalog.la                    \
    $(BOREALIS_SRC)/modules/util/libborealisutil.la                          \
    $(BOREALIS_SRC)/modules/queryProcessor/statsMgr/libborealisqpstatsMgr.la \
    $(BOREALIS_SRC)/modules/optimizer/local/libborealislopt.la               \
    ../../../utility/client/region/libborealisregion.la

lib_LTLIBRARIES = libborealisloadShedder.la

libborealisloadShedder_la_SOURCES =             \
    LoadShedder.cc LoadShedder.h                \
    LoadSheddingControl.cc LoadSheddingControl.h

LoadSheddingTool_SOURCES      = LoadSheddingTool.cc
LoadSheddingTool_DEPENDENCIES = libborealisloadShedder.la

endif
