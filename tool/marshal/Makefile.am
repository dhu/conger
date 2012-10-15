MYFLAGS =
AM_CFLAGS = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = marshal

bin_PROGRAMS = $(MAIN)

LDADD_EXTRAS = @BOREALISLIB@   @EXTERNALLIB@

LDADD = -L@BOREALISROOT@/modules/catalog               -lborealiscatalog \
        -L@BOREALISROOT@/modules/ha                    -lborealisha      \
        -L@BOREALISROOT@/modules/queryProcessor        -lborealisqp      \
        -L@BOREALISROOT@/external                      -lborealisexternal \
        -L@BOREALISROOT@/modules/common                -lborealiscommon  \
        -L@BOREALISROOT@/modules/util                  -lborealisutil    \
        -L../../utility/client/region                     -lborealisregion  \
        -L@BOREALISROOT@/modules/cql                   -lcongercql        \
        $(LDADD_EXTRAS)

INCLUDES = @XERCESCINCLUDE@  @NMSTLINCLUDE@                            \
           -I@BOREALISROOT@/modules/common                         \
           -I@BOREALISROOT@/modules/util                           \
           -I@BOREALISROOT@/modules/catalog                        \
           -I@BOREALISROOT@/modules/rpc

DEPENDENCIES = @BOREALISROOT@/modules/common/libborealiscommon.la   \
               @BOREALISROOT@/modules/catalog/libborealiscatalog.la

marshal_SOURCES = marshal.cc                                           \
                  GenerateMarshal.cc      GenerateMarshal.h            \
                  GenerateHeader.cc                                    \
                  GenerateBody.cc                                      \
                  MarshalDiagram.cc       MarshalDiagram.h
