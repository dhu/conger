#####################  Makefile.am for simple array tests  #####################
#
MYFLAGS =
AM_CFLAGS   = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = validate_ais validate_ais_delta

bin_PROGRAMS = $(MAIN)
BOREALIS_SRC = ../../../@BOREALISROOT@

LDADD_EXTRAS = @XERCESCLIB@   @NMSTLLIB@ -lSDL -lSDL_image

LDADD = -L$(BOREALIS_SRC)/modules/common             -lborealiscommon     \
        -L$(BOREALIS_SRC)/modules/util               -lborealisutil       \
        $(LDADD_EXTRAS)

INCLUDES = @XERCESCINCLUDE@   @NMSTLINCLUDE@                              \
           -I$(BOREALIS_SRC)/modules/common                               \
           -I$(BOREALIS_SRC)/modules/util                                 \
           -I$(BOREALIS_SRC)/modules/rpc                                  \
           -I$(BOREALIS_SRC)/modules/catalog                              \
           -I$(BOREALIS_SRC)/modules/queryProcessor                       \
           -I$(BOREALIS_SRC)/modules/queryProcessor/statsMgr             \
           -I$(BOREALIS_SRC)/external/array

DEPENDENCIES = $(BOREALIS_SRC)/modules/common/libborealiscommon.la        \
               $(BOREALIS_SRC)/modules/catalog/libborealiscatalog.la      \
               $(BOREALIS_SRC)/modules/queryProcessor/expr/libborealisqpexpr.la

validate_ais_SOURCES      = ${validate_ais_DEPENDENCIES}              \
                               validate_ais.cc csvparser.cc csvparser.h

validate_ais_DEPENDENCIES = AisMarshal.cc  AisMarshal.h

validate_ais_LDADD        = $(LDADD_EXTRAS) $(LDADD)

${validate_ais_DEPENDENCIES}: AIS.xml AISDeploy.xml
	marshal  AIS.xml AISDeploy.xml

            ###################Validate AIS Delta ########################

validate_ais_delta_SOURCES      = ${validate_ais_delta_DEPENDENCIES}              \
                               validate_ais_delta.cc csvparser.cc csvparser.h

validate_ais_delta_DEPENDENCIES = AisdeltaMarshal.cc  AisdeltaMarshal.h

validate_ais_delta_LDADD        = $(LDADD_EXTRAS) $(LDADD)

${validate_ais_delta_DEPENDENCIES}: AIS.xml AISDeploy.xml
	marshal  AISDelta.xml AISDeltaDeploy.xml





CLEANFILES = ${validate_ais_DEPENDENCIES}                  \
	     ${validate_ais_delta_DEPENDENCIES}                  

#
###################  end Makefile.am for simple array tests  ###################
