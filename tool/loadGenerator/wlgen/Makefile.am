MYFLAGS     = -Wno-deprecated
AM_CFLAGS   = $(MYFLAGS)
AM_CXXFLAGS = $(MYFLAGS)


MAIN = workloadGenerator

bin_PROGRAMS = $(MAIN)
BOREALIS_SRC = ../../@BOREALISROOT@

LDADD_EXTRAS = @BOREALISLIB@   @EXTERNALLIB@

LDADD = -L$(BOREALIS_SRC)/modules/common          -lborealiscommon   \
        -L$(BOREALIS_SRC)/modules/util            -lborealisutil     \
        -L$(BOREALIS_SRC)/modules/catalog         -lborealiscatalog  \
        -L$(BOREALIS_SRC)/modules/ha              -lborealisha       \
        -L$(BOREALIS_SRC)/modules/queryProcessor  -lborealisqp       \
        -L$(BOREALIS_SRC)/external                -lborealisexternal \
        $(LDADD_EXTRAS)

INCLUDES = @XERCESCINCLUDE@  @NMSTLINCLUDE@       -I ..     -I .     \
           -I $(BOREALIS_SRC)/modules/common                         \
           -I $(BOREALIS_SRC)/modules/util                           \
           -I $(BOREALIS_SRC)/modules/catalog                        \
           -I $(BOREALIS_SRC)/modules/queryProcessor                 \
           -I $(BOREALIS_SRC)/modules/queryProcessor/statsMgr        \
           -I $(BOREALIS_SRC)/modules/rpc

DEPENDENCIES =                                                       \
  $(BOREALIS_SRC)/modules/common/libborealiscommon.la                \
  $(BOREALIS_SRC)/modules/util/libborealisutil.la                    \
  $(BOREALIS_SRC)/modules/catalog/libborealiscatalog.la              \
  $(BOREALIS_SRC)/modules/queryProcessor/statsMgr/libborealisqpstatsMgr.la 

  
workloadGenerator_SOURCES = workloadGenerator.cc \
    AggSourceFileReader.h AggSourceFileReader.cc \
    AggStreamSource.h AggStreamSource.cc \
    AttributeSource.h AttributeSource.cc \
    GenDataType.h GenDataType.cc \
    RandomNumGenerator.h RandomNumGenerator.cc \
    SingleSource.h SingleSource.cc \
    SocketServer.h SocketClient.h \
    SourceGenerator.h SourceGenerator.cc \
    SourcePool.h SourcePool.cc \
    StreamBuffer.h StreamBuffer.cc \
    StreamIDGenerator.h \
    StreamSender.h StreamSender.cc \
    StreamTuple.h StreamTuple.cc \
    TimeStampGenerator.h TimeStampGenerator.cc \
    Timeval.h Types.h \
    WGController.h WGController.cc \
    WGServer.h WGServer.cc \
    WorkloadGenerator.h WorkloadGenerator.cc \
    XmlHandler.h XmlHandler.cc \
	TuplesSave.h TuplesSave.cc

