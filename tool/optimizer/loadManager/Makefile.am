MYFLAGS     =
AM_CFLAGS   = $(MYFLAGS)
AM_CXXFLAGS = $(MYFLAGS)

MAIN = LoadManager test

bin_PROGRAMS = $(MAIN)
BOREALIS_SRC = ../../@BOREALISROOT@

LDADD_EXTRAS = @BOREALISLIB@   @EXTERNALLIB@

LDADD = -L.                                           -llmLoadManager        \
        -L.                                           -lborealisloadManager  \
        -L../../../utility/client/region              -lborealisregion       \
        -L$(BOREALIS_SRC)/modules/ha                  -lborealisha           \
        -L$(BOREALIS_SRC)/modules/catalog             -lborealiscatalog      \
        -L$(BOREALIS_SRC)/modules/queryProcessor      -lborealisqp           \
        -L$(BOREALIS_SRC)/external                    -lborealisexternal     \
        -L$(BOREALIS_SRC)/modules/common              -lborealiscommon       \
        -L$(BOREALIS_SRC)/modules/util                -lborealisutil         \
        $(LDADD_EXTRAS)

INCLUDES = @XERCESCINCLUDE@  @NMSTLINCLUDE@    -I .   -I ..          \
           -I $(BOREALIS_SRC)/modules/common                         \
           -I $(BOREALIS_SRC)/modules/util                           \
           -I $(BOREALIS_SRC)/modules/catalog                        \
           -I $(BOREALIS_SRC)/modules/queryProcessor                 \
           -I $(BOREALIS_SRC)/modules/queryProcessor/statsMgr        \
           -I $(BOREALIS_SRC)/modules/rpc                            \
           -I ../../../utility/client/region

DEPENDENCIES =                                                             \
  ../../../utility/client/region/libborealisregion.la                      \
  $(BOREALIS_SRC)/modules/common/libborealiscommon.la                      \
  $(BOREALIS_SRC)/modules/util/libborealisutil.la                          \
  $(BOREALIS_SRC)/modules/catalog/libborealiscatalog.la                    \
  $(BOREALIS_SRC)/modules/queryProcessor/statsMgr/libborealisqpstatsMgr.la

lib_LTLIBRARIES = liblmLoadManager.la libborealisloadManager.la 

liblmLoadManager_la_SOURCES =                                 \
  BorealisStatisticsReader.h   BorealisStatisticsReader.cc    \
  BorealisCatalogReader.h      BorealisCatalogReader.cc       \
  BorealisLoadManager.h        BorealisLoadManager.cc         \
  BorealisController.h         BorealisController.cc 

libborealisloadManager_la_SOURCES =                                         \
  Timeval.h                                                                 \
  TimeSeries.h                         TimeSeries.cc                        \
  LMCommon.h                                                                \
  LMParams.h                           LMParams.cc                          \
  LMStatisticsPool.h                   LMStatisticsPool.cc                  \
  LMStatisticsManager.h                LMStatisticsManager.cc               \
  LMBox.h LMBox.cc                                                          \
  LMArc.h LMArc.cc                                                          \
  LMQueryNetwork.h                     LMQueryNetwork.cc                    \
  LMSuperBox.h                         LMSuperBox.cc                        \
  LMSuperEdge.h                        LMSuperEdge.cc                       \
  LMSuperGraph.h                       LMSuperGraph.cc                      \
  LMNode.h                             LMNode.cc                            \
  LMNodeManager.h                      LMNodeManager.cc                     \
  LMToolFunctions.h                    LMToolFunctions.cc                   \
  BorealisToolFunctions.h              BorealisToolFunctions.cc             \
  LMParamTypes.h                                                            \
  LMIDMatrix.h                         LMIDMatrix.cc                        \
  LMPerformanceMonitor.h               LMPerformanceMonitor.cc              \
  LMSimpleDistributor.h                LMSimpleDistributor.cc               \
  LMCorrelationBasedLoadDistributor.h  LMCorrelationBasedLoadDistributor.cc \
  LMStaticDistributor.h                LMStaticDistributor.cc               \
  LMController.h                       LMController.cc                      \
  LMLoadManager.h                      LMLoadManager.cc 


LoadManager_SOURCES = LoadManager.cc
LoadManager_DEPENDENCIES = liblmLoadManager.la libborealisloadManager.la

test_SOURCES = test.cc
test_DEPENDENCIES = liblmLoadManager.la libborealisloadManager.la
