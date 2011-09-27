###################  Makefile.am for a sample deploy test  #####################
#
MYFLAGS =
AM_CFLAGS   = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = validate_deploy   validate_add

bin_PROGRAMS = $(MAIN)
BOREALIS_SRC = ../../@BOREALISROOT@

LDADD_EXTRAS = @XERCESCLIB@   @NMSTLLIB@

LDADD = -L$(BOREALIS_SRC)/modules/common             -lborealiscommon     \
        -L$(BOREALIS_SRC)/modules/util               -lborealisutil       \
        $(LDADD_EXTRAS)

INCLUDES = @XERCESCINCLUDE@ @NMSTLINCLUDE@                                \
           -I$(BOREALIS_SRC)/modules/common                               \
           -I$(BOREALIS_SRC)/modules/util                                 \
           -I$(BOREALIS_SRC)/modules/rpc                                  \
           -I$(BOREALIS_SRC)/modules/catalog                              \
           -I$(BOREALIS_SRC)/modules/queryProcessor                       \
           -I$(BOREALIS_SRC)/modules/queryProcessor/statsMgr              \
           -I../../../tool/head                                           \
           -I../../../utility/client/rpc

DEPENDENCIES = $(BOREALIS_SRC)/modules/common/libborealiscommon.la        \
               $(BOREALIS_SRC)/modules/catalog/libborealiscatalog.la      \
               $(BOREALIS_SRC)/modules/queryProcessor/expr/libborealisqpexpr.la

#            ---------------------------------------------

validate_deploy_SOURCES      = ${validate_deploy_DEPENDENCIES}            \
                               validate_deploy.cc

validate_deploy_DEPENDENCIES = DeployMarshal.cc  DeployMarshal.h

validate_deploy_LDADD        = $(LDADD_EXTRAS) $(LDADD)

${validate_deploy_DEPENDENCIES}: Deploy.xml
	marshal  Deploy.xml  DeployDeploy.xml

#            ---------------------------------------------

validate_add_SOURCES      = ${validate_add_DEPENDENCIES}            \
                               validate_add.cc

validate_add_DEPENDENCIES = AddMarshal.cc  AddMarshal.h

validate_add_LDADD        = $(LDADD_EXTRAS) $(LDADD)

${validate_add_DEPENDENCIES}: Add.xml
	marshal  Deploy.xml  Add.xml  AddDeploy.xml

#            ---------------------------------------------

CLEANFILES = ${validate_deploy_DEPENDENCIES}                        \
             ${validate_add_DEPENDENCIES}

#
##################  end Makefile.am for a sample deploy test  ##################
