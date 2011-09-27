###################  Makefile.am for a global component  ######################
#
MYFLAGS =
AM_CFLAGS   = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = GlobalView

bin_PROGRAMS = $(MAIN)
BOREALIS_SRC = ../@BOREALISROOT@

LDADD_EXTRAS = @BOREALISLIB@   @EXTERNALLIB@


LDADD = -L../region                                   -lborealisregion     \
        -L$(BOREALIS_SRC)/modules/catalog             -lborealiscatalog    \
        -L$(BOREALIS_SRC)/modules/ha                  -lborealisha         \
        -L$(BOREALIS_SRC)/modules/queryProcessor      -lborealisqp         \
        -L$(BOREALIS_SRC)/external                    -lborealisexternal   \
        -L$(BOREALIS_SRC)/modules/common              -lborealiscommon     \
        -L$(BOREALIS_SRC)/modules/util                -lborealisutil       \
        $(LDADD_EXTRAS)


INCLUDES = @BOREALISINCLUDE@                                              \
           -I $(BOREALIS_SRC)/modules/common                              \
           -I $(BOREALIS_SRC)/modules/util                                \
           -I $(BOREALIS_SRC)/modules/rpc                                 \
           -I $(BOREALIS_SRC)/modules/catalog                             \
           -I $(BOREALIS_SRC)/modules/queryProcessor                      \
           -I $(BOREALIS_SRC)/modules/queryProcessor/statsMgr             \
           -I ../../../tool/head                                          \
           -I ../rpc                                                      \
           -I ../region


GlobalView_SOURCES = GlobalView.cc   GlobalView.h

GlobalView_LDADD   = $(LDADD_EXTRAS) $(LDADD)


# CLEANFILES = ${GlobalView_DEPENDENCIES}

#
###################  end Makefile.am for the GlobalView component  ###################
