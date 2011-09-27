###################  Makefile.am for a sample array test  #####################
#
MYFLAGS =
AM_CFLAGS   = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = webcam_demo

bin_PROGRAMS = $(MAIN)
BOREALIS_SRC = ../@BOREALISROOT@

LDADD_EXTRAS = -ldl   -lSDL @XERCESCLIB@   @NMSTLLIB@

LDADD = -L$(BOREALIS_SRC)/modules/common             -lborealiscommon     \
        -L$(BOREALIS_SRC)/modules/util               -lborealisutil       \
        $(LDADD_EXTRAS)

INCLUDES = @XERCESCINCLUDE@ @NMSTLINCLUDE@                                                 \
           -I$(BOREALIS_SRC)/modules/common                               \
           -I$(BOREALIS_SRC)/modules/util                                 \
           -I$(BOREALIS_SRC)/modules/rpc                                  \
           -I$(BOREALIS_SRC)/modules/catalog                              \
           -I$(BOREALIS_SRC)/modules/queryProcessor                       \
           -I$(BOREALIS_SRC)/modules/queryProcessor/statsMgr

DEPENDENCIES = $(BOREALIS_SRC)/modules/common/libborealiscommon.la                \
               $(BOREALIS_SRC)/modules/catalog/libborealiscatalog.la  \
               $(BOREALIS_SRC)/modules/queryProcessor/expr/libborealisqpexpr.la

webcam_demo_SOURCES      = ${webcam_demo_DEPENDENCIES}            \
                               webcam_demo.cc

webcam_demo_DEPENDENCIES = ArrayMarshal.cc  ArrayMarshal.h

webcam_demo_LDADD        = $(LDADD_EXTRAS) $(LDADD)

${webcam_demo_DEPENDENCIES}: Array.xml
	marshal  Array.xml  ArrayDeploy.xml


CLEANFILES = ${webcam_demo_DEPENDENCIES}

#
##################  end Makefile.am for a sample array test  ##################
