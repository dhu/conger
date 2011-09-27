###################  Makefile.am for a sample two_chains test  #####################
#
MYFLAGS =
AM_CFLAGS   = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = validate_two_chains

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

validate_two_chains_SOURCES      = ${validate_two_chains_DEPENDENCIES}            \
                               validate_two_chains.cc

validate_two_chains_DEPENDENCIES = Two_ChainsMarshal.cc  Two_ChainsMarshal.h

validate_two_chains_LDADD        = $(LDADD_EXTRAS) $(LDADD)

${validate_two_chains_DEPENDENCIES}: Two_Chains.xml
	marshal  Two_Chains.xml  Two_Chains_deploy.xml


CLEANFILES = ${validate_two_chains_DEPENDENCIES}

#
##################  end Makefile.am for a sample two_chains test  ##################
