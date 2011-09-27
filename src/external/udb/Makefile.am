MYFLAGS     =
AM_CFLAGS   = $(MYFLAGS)
AM_CXXFLAGS = $(MYFLAGS)

if COND_UDB

LDADD =

INCLUDES = @BOREALISINCLUDE@    @BDBINCLUDE@                               \
           -I../../modules/common                                          \
           -I../../modules/util                                            \
           -I../../modules/queryProcessor/runtime                          \
           -I../../modules/queryProcessor/storageMgr                       \
           -I../../modules/queryProcessor/statsMgr                         \
           -I../../modules/queryProcessor/expr                             \
           -I../../modules/queryProcessor/boxes                            \
           -I../../modules/queryProcessor                                  \
           -I../../modules/catalog
           
noinst_LTLIBRARIES = libborealisudb.la

libborealisudb_la_SOURCES =                                                  \
         CatalogUdb.cc                                                       \
		 InstantiateUdb.cc                                                   \
         CombineQBox.cc                     CombineQBox.h                    \
         CountDistinct.cc                   CountDistinct.h                  \
         DelayQBox.cc                       DelayQBox.h                      \
         DelaySelQBox.cc                    DelaySelQBox.h                   \
         FidLateTrackerFunc.cc              FidLateTrackerFunc.h             \
         FidComplianceLookthroughAndAgg.cc  FidComplianceLookthroughAndAgg.h \
         FidAlarmCounterFunc.cc             FidAlarmCounterFunc.h            \
         JoinStatic.cc                      JoinStatic.h                     \
         LRAccidentAggFunc1.cc              LRAccidentAggFunc1.h             \
         LRAccidentAggFunc2.cc              LRAccidentAggFunc2.h             \
         LRAvg.cc                           LRAvg.cc.h                       \
         LRCarCount.cc                      LRCarCount.h                     \
         LRSecToMin.cc                      LRSecToMin.h                     \
         LRSegmentStatisticsFunc2.cc        LRSegmentStatisticsFunc2.h       \
         LRWaitForQBox.cc                   LRWaitForQBox.h                  \
         RandFunction.cc                    RandFunction.h

#        StockQuoteConverterBox.cc  StockQuoteConverterBox.h 

else
SUBDIRS = stub
endif
