MYFLAGS     =
AM_CFLAGS   = $(MYFLAGS)
AM_CXXFLAGS = $(MYFLAGS)

MAIN = InputGenerator OutputReceiver xmlgen NetworkTrafficQueryGen test

bin_PROGRAMS = $(MAIN)
BOREALIS_SRC = ../../@BOREALISROOT@

LDADD_EXTRAS = @BOREALISLIB@   @EXTERNALLIB@

LDADD =  -L.                                          -linputGenerator     \
         -L$(BOREALIS_SRC)/modules/catalog            -lborealiscatalog    \
         -L$(BOREALIS_SRC)/modules/ha                 -lborealisha         \
         -L$(BOREALIS_SRC)/modules/queryProcessor     -lborealisqp         \
         -L$(BOREALIS_SRC)/external                   -lborealisexternal   \
         -L$(BOREALIS_SRC)/modules/common             -lborealiscommon     \
         -L$(BOREALIS_SRC)/modules/util               -lborealisutil       \
         $(LDADD_EXTRAS)

INCLUDES = @XERCESCINCLUDE@  @NMSTLINCLUDE@       -I ..     -I .     \
           -I $(BOREALIS_SRC)/modules/common                         \
           -I $(BOREALIS_SRC)/modules/util                           \
           -I $(BOREALIS_SRC)/modules/catalog                        \
           -I $(BOREALIS_SRC)/modules/queryProcessor                 \
           -I $(BOREALIS_SRC)/modules/queryProcessor/statsMgr        \
           -I $(BOREALIS_SRC)/modules/rpc

DEPENDENCIES =                                                      \
  $(BOREALIS_SRC)/modules/common/libborealiscommon.la               \
  $(BOREALIS_SRC)/modules/util/libborealisutil.la                   \
  $(BOREALIS_SRC)/modules/catalog/libborealiscatalog.la             \
  $(BOREALIS_SRC)/modules/queryProcessor/statsMgr/libborealisqpstatsMgr.la 

lib_LTLIBRARIES = libinputGenerator.la

xmlgen_SOURCES = xmlgen.cc                   \
  BRQueryParams.h BRQueryParams.cc           \
  BRQueryGenerator.h BRQueryGenerator.cc 

NetworkTrafficQueryGen_SOURCES = NetworkTrafficQueryGen.cc          \
  NetworkTrafficQueryParams.h NetworkTrafficQueryParams.cc          \
  NetworkTrafficQueryGenerator.h NetworkTrafficQueryGenerator.cc

libinputGenerator_la_SOURCES =                         \
  BRIOParams.h BRIOParams.cc                           \
  BRInputGenerator.h BRInputGenerator.cc               \
  BROutputReceiver.h BROutputReceiver.cc               \
  RandomPulseFunction.h RandomPulseFunction.cc         \
  Function.h                                           \
  ConstantFunction.h                                   \
  UniformFunction.h                                    \
  RandomWalkFunction.h                                 \
  TraceFunction.h TraceFunction.cc                     \
  TCPTraceFunction.h TCPTraceFunction.cc 

test_SOURCES = test.cc 
test_DEPENDENCIES = libinputGenerator.la 

InputGenerator_SOURCES = InputGenerator.cc 
InputGenerator_DEPENDENCIES = libinputGenerator.la 

OutputReceiver_SOURCES = OutputReceiver.cc 
OutputReceiver_DEPENDENCIES = libinputGenerator.la 

