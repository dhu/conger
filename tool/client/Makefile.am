MYFLAGS =
AM_CFLAGS = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = enqueue enqueueUnion dequeue dequeueFilter dequeueMap \
	dequeueJoin dequeueAggregate dequeueAggregateJoin #enqueueAndDequeue

bin_PROGRAMS = $(MAIN)

LDADD_EXTRAS = @BOREALISLIB@   @EXTERNALLIB@

LDADD = -L@BOREALISROOT@/modules/catalog               -lborealiscatalog  \
        -L@BOREALISROOT@/modules/ha                    -lborealisha       \
        -L@BOREALISROOT@/modules/queryProcessor        -lborealisqp       \
        -L@BOREALISROOT@/external                      -lborealisexternal \
        -L../../utility/client/region                  -lborealisregion   \
        -L@BOREALISROOT@/modules/common                -lborealiscommon   \
        -L@BOREALISROOT@/modules/util                  -lborealisutil     \
        -L@BOREALISROOT@/modules/cql                   -lcongercql        \
        $(LDADD_EXTRAS)

INCLUDES = @XERCESCINCLUDE@  @NMSTLINCLUDE@                            \
           -I@BOREALISROOT@/modules/common                         \
           -I@BOREALISROOT@/modules/util                           \
           -I@BOREALISROOT@/modules/catalog                        \
           -I@BOREALISROOT@/modules/queryProcessor                 \
           -I@BOREALISROOT@/modules/queryProcessor/statsMgr        \
           -I@BOREALISROOT@/modules/rpc                            \
           -I ../../utility/client/region                              \
           -I ../../utility/client/rpc

enqueue_SOURCES = Enqueue.cc Enqueue.h StockTuple.h

dequeue_SOURCES = Dequeue.cc Dequeue.h StockTuple.h

enqueueAndDequeue_SOURCES = EnqueueAndDequeue.cc 

dequeueFilter_SOURCES = DequeueFilter.cc StockTuple.h

dequeueMap_SOURCES = DequeueMap.cc StockTuple.h

dequeueJoin_SOURCES = DequeueJoin.cc StockTuple.h

dequeueAggregate_SOURCES = DequeueAggregate.cc StockTuple.h

enqueueUnion_SOURCES =EnqueueUnion.cc StockTuple.h

dequeueAggregateJoin_SOURCES = DequeueAggregateJoin.cc StockTuple.h
