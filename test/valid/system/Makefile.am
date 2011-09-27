###################  Makefile.am for a sample system test  #####################
#
MYFLAGS =
AM_CFLAGS   = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = validate_system

bin_PROGRAMS = $(MAIN)
BOREALIS_SRC = ../../@BOREALISROOT@

LDADD_EXTRAS = @XERCESCLIB@   @NMSTLLIB@

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

validate_system_SOURCES      = ${validate_system_DEPENDENCIES}            \
                               validate_system.cc

validate_system_DEPENDENCIES = SystemMarshal.cc  SystemMarshal.h

validate_system_LDADD        = $(LDADD_EXTRAS) $(LDADD)

${validate_system_DEPENDENCIES}: System.xml
	marshal  System.xml  SystemDeploy.xml


CLEANFILES = ${validate_system_DEPENDENCIES}

#
##################  end Makefile.am for a sample system test  ##################
