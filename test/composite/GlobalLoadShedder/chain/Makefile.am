
MYFLAGS =
AM_CFLAGS = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = chain_2_2

bin_PROGRAMS = $(MAIN)

LDADD_MODULES = -L../../../@BOREALISROOT@/modules/common  -lborealiscommon    \
                -L../../../@BOREALISROOT@/modules/catalog -lborealiscatalog   \
                -L../../../@BOREALISROOT@/modules/util    -lborealisutil      \
                -L../../../@BOREALISROOT@/modules/ha      -lborealisha        \
                -L../../../@BOREALISROOT@/modules/queryProcessor -lborealisqp \
                -L../../../@BOREALISROOT@/external        -lborealisexternal

LDADD_EXTRAS = -ldl  @BOREALISLIB@   @EXTERNALLIB@

LDADD = $(LDADD_MODULES) $(LDADD_EXTRAS)

INCLUDES = @BOREALISINCLUDE@                                      \
           -I ../../../@BOREALISROOT@/modules/common              \
           -I ../../../@BOREALISROOT@/modules/util                \
           -I ../../../@BOREALISROOT@/modules/rpc                 \
           -I ../../../@BOREALISROOT@/modules/catalog             \
           -I ../../../@BOREALISROOT@/modules/queryProcessor      \
           -I ../../../@BOREALISROOT@/modules/queryProcessor/statsMgr 

DEPENDENCIES = ../../../@BOREALISROOT@/modules/common/libborealiscommon.la

 ##############################################################################

chain_2_2_DEPENDENCIES = Chain_2_2Marshal.cc Chain_2_2Marshal.h

chain_2_2_SOURCES = ${chain_2_2_DEPENDENCIES} chain_2_2.cc

${chain_2_2_DEPENDENCIES}: chain_2_2.xml chain_2_2_deploy.xml
	marshal chain_2_2.xml chain_2_2_deploy.xml

 ##############################################################################

CLEANFILES = ${chain_2_2_DEPENDENCIES}

 #############################  end Makefile.am  ##############################
