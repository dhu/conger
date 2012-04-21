MYFLAGS =
AM_CFLAGS = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = padmin

bin_PROGRAMS = $(MAIN)

LDADD_EXTRAS = @BOREALISLIB@   @EXTERNALLIB@

LDADD = -L../@BOREALISROOT@/modules/catalog               -lborealiscatalog  \
        -L../@BOREALISROOT@/modules/ha                    -lborealisha       \
        -L../@BOREALISROOT@/modules/queryProcessor        -lborealisqp       \
        -L../@BOREALISROOT@/external                      -lborealisexternal \
        -L../../utility/client/region                     -lborealisregion   \
        -L../@BOREALISROOT@/modules/common                -lborealiscommon   \
        -L../@BOREALISROOT@/modules/util                  -lborealisutil     \
        -L.                                               -lantlr3c          \
        $(LDADD_EXTRAS)

INCLUDES = @XERCESCINCLUDE@  @NMSTLINCLUDE@                            \
           -I../@BOREALISROOT@/modules/common                         \
           -I../@BOREALISROOT@/modules/util                           \
           -I../@BOREALISROOT@/modules/catalog                        \
           -I../@BOREALISROOT@/modules/queryProcessor                 \
           -I../@BOREALISROOT@/modules/queryProcessor/statsMgr        \
           -I../@BOREALISROOT@/modules/rpc                            \
           -I../../utility/client/region                              \
           -I../../utility/client/rpc                                 \
           -I/opt/antlr3c/include

padmin_SOURCES = PolarisAdmin.h PolarisAdmin.cc DeployDiagram.h HeadServer.h \
		SQLLexer.h SQLLexer.c SQLParser.h SQLParser.c DeployDiagram.cc HeadServer.cc

