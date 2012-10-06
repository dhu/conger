MYFLAGS =
AM_CFLAGS = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = test_dequeue_statistics test_dequeue_segementtoll test_dequeue_cartoll \
	test_dequeue_curactivecars test_dequeue_segementvolume \
	test_dequeue_segementavgspeed

bin_PROGRAMS = $(MAIN)

LDADD_EXTRAS = @BOREALISLIB@   @EXTERNALLIB@

LDADD = -L@BOREALISROOT@/modules/catalog               -lborealiscatalog  \
        -L@BOREALISROOT@/modules/ha                    -lborealisha       \
        -L@BOREALISROOT@/modules/queryProcessor        -lborealisqp       \
        -L@BOREALISROOT@/external                      -lborealisexternal \
        -L@BOREALISROOT@/../utility/client/region                  -lborealisregion   \
        -L@BOREALISROOT@/modules/common                -lborealiscommon   \
        -L@BOREALISROOT@/modules/util                  -lborealisutil     \
        -L@BOREALISROOT@/modules/cql                   -lcongercql        \
        $(LDADD_EXTRAS)

INCLUDES = @XERCESCINCLUDE@  @NMSTLINCLUDE@                        \
           -I@BOREALISROOT@/modules/common                         \
           -I@BOREALISROOT@/modules/util                           \
           -I@BOREALISROOT@/modules/catalog                        \
           -I@BOREALISROOT@/modules/queryProcessor                 \
           -I@BOREALISROOT@/modules/queryProcessor/statsMgr        \
           -I@BOREALISROOT@/modules/rpc                            \
           -I@BOREALISROOT@/../utility/client/region               \
           -I@BOREALISROOT@/../utility/client/rpc

test_dequeue_statistics_SOURCES = TestDequeueStatistics.cc \
	TrafficOutputTuple.h

test_dequeue_segementtoll_SOURCES = TestDequeueSegementToll.cc \
 	TrafficOutputTuple.h

test_dequeue_cartoll_SOURCES = TestDequeueCarToll.cc \
	TrafficOutputTuple.h
	
test_dequeue_curactivecars_SOURCES = TestDequeueCurActiveCarsToll.cc \
	TrafficOutputTuple.h

test_dequeue_segementvolume_SOURCES = TestDequeueSegVolume.cc \
	TrafficOutputTuple.h

test_dequeue_segementavgspeed_SOURCES = TestDequeueSegementAvgSpeed.cc \
	TrafficOutputTuple.h

