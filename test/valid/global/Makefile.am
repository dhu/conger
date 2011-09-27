###################  Makefile.am for a global component  ######################
#
MYFLAGS =
AM_CFLAGS   = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = validate_global

bin_PROGRAMS = $(MAIN)
BOREALIS_SRC = ../../@BOREALISROOT@

LDADD_EXTRAS = @BOREALISLIB@   @EXTERNALLIB@


LDADD = -L../../../utility/client/region                  -lborealisregion     \
        -L$(BOREALIS_SRC)/modules/catalog                 -lborealiscatalog    \
        -L$(BOREALIS_SRC)/modules/queryProcessor          -lborealisqp         \
        -L$(BOREALIS_SRC)/modules/ha                      -lborealisha         \
        -L$(BOREALIS_SRC)/modules/queryProcessor/statsMgr -lborealisqpstatsMgr \
        -L$(BOREALIS_SRC)/external                        -lborealisexternal   \
        -L$(BOREALIS_SRC)/modules/common                  -lborealiscommon     \
        -L$(BOREALIS_SRC)/modules/util                    -lborealisutil       \
        $(LDADD_EXTRAS)

INCLUDES = @XERCESCINCLUDE@  @NMSTLINCLUDE@                               \
           -I $(BOREALIS_SRC)/modules/common                              \
           -I $(BOREALIS_SRC)/modules/util                                \
           -I $(BOREALIS_SRC)/modules/rpc                                 \
           -I $(BOREALIS_SRC)/modules/catalog                             \
           -I $(BOREALIS_SRC)/modules/queryProcessor                      \
           -I $(BOREALIS_SRC)/modules/queryProcessor/statsMgr             \
           -I ../../../tool/head                                          \
           -I ../../../utility/client/rpc                                 \
           -I ../../../utility/client/region


validate_global_SOURCES      = validate_global.cc

validate_global_LDADD        = $(LDADD_EXTRAS) $(LDADD)

#global_component_SOURCES      = global_component.cc

#global_component_LDADD        = $(LDADD_EXTRAS) $(LDADD)


# CLEANFILES = ${validate_move_DEPENDENCIES}

#
###################  end Makefile.am for a global component  ###################
