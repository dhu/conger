##################  Makefile.am for a connect box test  ########################
#
MYFLAGS =
AM_CFLAGS   = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = validate_connect1    region_connect1      \
       validate_connect2    region_connect2      \
       validate_connect3    region_connect3      \
       validate_connect4    region_connect4

bin_PROGRAMS = $(MAIN)
BOREALIS_SRC = ../../@BOREALISROOT@

LDADD_EXTRAS = @BOREALISLIB@   @EXTERNALLIB@

LDADD =  -L../../../utility/client/region              -lborealisregion     \
         -L$(BOREALIS_SRC)/modules/catalog             -lborealiscatalog    \
         -L$(BOREALIS_SRC)/modules/ha                  -lborealisha         \
         -L$(BOREALIS_SRC)/modules/queryProcessor      -lborealisqp         \
         -L$(BOREALIS_SRC)/external                    -lborealisexternal   \
         -L$(BOREALIS_SRC)/modules/common              -lborealiscommon     \
         -L$(BOREALIS_SRC)/modules/util                -lborealisutil       \
         $(LDADD_EXTRAS)


INCLUDES = @NMSTLINCLUDE@    @XERCESCINCLUDE@                             \
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

                       ###################

validate_connect1_SOURCES      = ${validate_connect1_DEPENDENCIES}          \
                                 validate_connect1.cc

validate_connect1_DEPENDENCIES = Connect1Marshal.cc  Connect1Marshal.h

validate_connect1_LDADD        = $(LDADD_EXTRAS) $(LDADD)

${validate_connect1_DEPENDENCIES}: Connect1.xml
	marshal  Connect1.xml  ConnectDeploy1.xml


region_connect1_SOURCES        = region_connect1.cc

region_connect1_LDADD          = $(LDADD_EXTRAS) $(LDADD)

                       ###################

validate_connect2_SOURCES      = ${validate_connect2_DEPENDENCIES}          \
                                 validate_connect2.cc

validate_connect2_DEPENDENCIES = Connect2Marshal.cc  Connect2Marshal.h

validate_connect2_LDADD        = $(LDADD_EXTRAS) $(LDADD)

${validate_connect2_DEPENDENCIES}: Connect2.xml
	marshal  Connect2.xml  ConnectDeploy2.xml


region_connect2_SOURCES        = region_connect2.cc

region_connect2_LDADD          = $(LDADD_EXTRAS) $(LDADD)

                       ###################

validate_connect3_SOURCES      = ${validate_connect3_DEPENDENCIES}          \
                                 validate_connect3.cc

validate_connect3_DEPENDENCIES = Connect3Marshal.cc  Connect3Marshal.h

validate_connect3_LDADD        = $(LDADD_EXTRAS) $(LDADD)

${validate_connect3_DEPENDENCIES}: Connect3.xml
	marshal  Connect3.xml  ConnectDeploy3.xml


region_connect3_SOURCES        = region_connect3.cc

region_connect3_LDADD          = $(LDADD_EXTRAS) $(LDADD)

                       ###################

validate_connect4_SOURCES      = ${validate_connect4_DEPENDENCIES}          \
                                 validate_connect4.cc

validate_connect4_DEPENDENCIES = Connect4Marshal.cc  Connect4Marshal.h

validate_connect4_LDADD        = $(LDADD_EXTRAS) $(LDADD)

${validate_connect4_DEPENDENCIES}: Connect4.xml
	marshal  Connect4.xml  ConnectDeploy4.xml


region_connect4_SOURCES        = region_connect4.cc

region_connect4_LDADD          = $(LDADD_EXTRAS) $(LDADD)

                       ###################

CLEANFILES = ${validate_connect1_DEPENDENCIES}   \
             ${validate_connect2_DEPENDENCIES}   \
             ${validate_connect3_DEPENDENCIES}   \
             ${validate_connect4_DEPENDENCIES}

#
##################  end Makefile.am for a connect box test  ####################
