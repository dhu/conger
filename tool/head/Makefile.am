MYFLAGS =
AM_CFLAGS = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = BigGiantHead

bin_PROGRAMS = $(MAIN)

LDADD_EXTRAS = @BOREALISLIB@   @EXTERNALLIB@

LDADD = -L.                                               -lborealisdeploy   \
        -L@BOREALISROOT@/modules/catalog               -lborealiscatalog  \
        -L@BOREALISROOT@/modules/ha                    -lborealisha       \
        -L@BOREALISROOT@/modules/queryProcessor        -lborealisqp       \
        -L@BOREALISROOT@/external                      -lborealisexternal \
        -L../../utility/client/region                     -lborealisregion   \
        -L@BOREALISROOT@/modules/common                -lborealiscommon   \
        -L@BOREALISROOT@/modules/util                  -lborealisutil     \
        -L@BOREALISROOT@/modules/cql                   -lcongercql        \
        $(LDADD_EXTRAS)

INCLUDES = @XERCESCINCLUDE@  @NMSTLINCLUDE@                            \
           -I @BOREALISROOT@/modules/common                         \
           -I @BOREALISROOT@/modules/util                           \
           -I @BOREALISROOT@/modules/catalog                        \
           -I @BOREALISROOT@/modules/queryProcessor                 \
           -I @BOREALISROOT@/modules/queryProcessor/statsMgr        \
           -I @BOREALISROOT@/modules/rpc                            \
           -I ../../utility/client/region                              \
           -I ../../utility/client/rpc


DEPENDENCIES = @BOREALISROOT@/modules/common/libborealiscommon.la   \
               @BOREALISROOT@/modules/catalog/libborealiscatalog.la

lib_LTLIBRARIES = libborealisdeploy.la

libborealisdeploy_la_SOURCES = DeployDiagram.cc  DeployDiagram.h

BigGiantHead_SOURCES = BigGiantHead.cc   HeadServer.cc  HeadServer.h
BigGiantHead_DEPENDENCIES = $(DEPENDENCIES)  libborealisdeploy.la
