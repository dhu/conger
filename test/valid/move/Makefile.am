###################  Makefile.am for a move box test  ##########################
#
MYFLAGS =
AM_CFLAGS   = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = validate_move   region_move

bin_PROGRAMS = $(MAIN)
BOREALIS_SRC = ../../@BOREALISROOT@

LDADD_EXTRAS =  @BOREALISLIB@   @EXTERNALLIB@

LDADD =  -L../../../utility/client/region            -lborealisregion     \
         -L$(BOREALIS_SRC)/modules/catalog           -lborealiscatalog    \
         -L$(BOREALIS_SRC)/modules/ha                -lborealisha         \
         -L$(BOREALIS_SRC)/modules/queryProcessor    -lborealisqp         \
         -L$(BOREALIS_SRC)/external                  -lborealisexternal   \
         -L$(BOREALIS_SRC)/modules/common            -lborealiscommon     \
         -L$(BOREALIS_SRC)/modules/util              -lborealisutil       \
         $(LDADD_EXTRAS)


INCLUDES = @XERCESCINCLUDE@   @NMSTLINCLUDE@                              \
           -I $(BOREALIS_SRC)/modules/common                              \
           -I $(BOREALIS_SRC)/modules/util                                \
           -I $(BOREALIS_SRC)/modules/rpc                                 \
           -I $(BOREALIS_SRC)/modules/catalog                             \
           -I $(BOREALIS_SRC)/modules/queryProcessor                      \
           -I $(BOREALIS_SRC)/modules/queryProcessor/statsMgr             \
           -I ../../../utility/client/region

DEPENDENCIES = $(BOREALIS_SRC)/modules/common/libborealiscommon.la        \
               $(BOREALIS_SRC)/modules/catalog/libborealiscatalog.la      \
               $(BOREALIS_SRC)/modules/queryProcessor/expr/libborealisqpexpr.la

validate_move_SOURCES      = ${validate_move_DEPENDENCIES}                \
                               validate_move.cc

validate_move_DEPENDENCIES = MoveMarshal.cc  MoveMarshal.h

validate_move_LDADD        = $(LDADD_EXTRAS) $(LDADD)

${validate_move_DEPENDENCIES}: Move.xml
	marshal  Move.xml  MoveDeploy.xml


region_move_SOURCES      = region_move.cc

region_move_LDADD        = $(LDADD_EXTRAS) $(LDADD)


CLEANFILES = ${validate_move_DEPENDENCIES}

#
##################  end Makefile.am for a sample move box test  ################
