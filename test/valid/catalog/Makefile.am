###################  Makefile.am for catalog validation  #######################
#
MYFLAGS =
AM_CFLAGS   = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = validate_catalog

bin_PROGRAMS = $(MAIN)
BOREALIS_SRC = ../../@BOREALISROOT@

LDADD_EXTRAS = @BOREALISLIB@  @EXTERNALLIB@

LDADD = -L../../../utility/client/region              -lborealisregion     \
        -L../../../tool/head                          -lborealisdeploy     \
        -L$(BOREALIS_SRC)/modules/catalog             -lborealiscatalog    \
        -L$(BOREALIS_SRC)/modules/ha                  -lborealisha         \
        -L$(BOREALIS_SRC)/modules/queryProcessor      -lborealisqp         \
        -L$(BOREALIS_SRC)/external                    -lborealisexternal   \
        -L$(BOREALIS_SRC)/modules/common              -lborealiscommon     \
        -L$(BOREALIS_SRC)/modules/util                -lborealisutil       \
        $(LDADD_EXTRAS)

INCLUDES = @BOREALISINCLUDE@                                              \
           -I$(BOREALIS_SRC)/modules/common                               \
           -I$(BOREALIS_SRC)/modules/util                                 \
           -I$(BOREALIS_SRC)/modules/rpc                                  \
           -I$(BOREALIS_SRC)/modules/catalog                              \
           -I$(BOREALIS_SRC)/modules/queryProcessor/statsMgr              \
           -I$(BOREALIS_SRC)/modules/queryProcessor                       \
           -I../../../utility/client/region                               \
           -I../../../tool/head

DEPENDENCIES = $(BOREALIS_SRC)/modules/common/libborealiscommon.la        \
               $(BOREALIS_SRC)/modules/catalog/libborealiscatalog.la      \
               $(BOREALIS_SRC)/modules/queryProcessor/expr/libborealisqpexpr.la

validate_catalog_SOURCES      = validate_catalog.cc

validate_catalog_DEPENDENCIES = $(DEPENDENCIES)

validate_catalog_LDADD        = $(LDADD_EXTRAS) $(LDADD)

#
###################  end Makefile.am for catalog validation  ###################
