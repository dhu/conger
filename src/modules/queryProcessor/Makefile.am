SUBDIRS = runtime expr storageMgr boxes statsMgr

##################################################################

MYFLAGS     = 
AM_CFLAGS   = $(MYFLAGS)
AM_CXXFLAGS = $(MYFLAGS)

INCLUDES= @BOREALISINCLUDE@   @EXTERNALINCLUDE@                            \
          -I../common       -I../util         -I../rpc                     \
          -I../catalog      -I../optimizer/nhopt                           \
          -I./runtime       -I./storageMgr    -I./statsMgr    -I./expr

lib_LTLIBRARIES = libborealisqp.la 

libborealisqp_la_SOURCES =                                 \
           ConsistencyMngr.cc       ConsistencyMngr.h      \
           DataPath.cc              DataPath.h             \
           DynamicArray.cc          DynamicArray.h         \
           IOQueue.cc               IOQueue.h              \
           QueryProcessor.cc        QueryProcessor.h       \
                                    QueryState.h           \
           StreamBuffer.cc          StreamBuffer.h         \
                                    StreamState.h          \
           AdminCatalog.cc                                 \
                                    UpdateClient.h         \
                                    UpdateServer.h         \
                                    PendingQuery.h
#           QueryProcessorWithoutHA.cc

LDADD_MODULES = @NMSTLLIB@                                             \
                -L../modules/catalog                -lborealiscatalog  \
                -L../modules/optimizer/local        -lborealislopt     \
                -L../modules/optimizer/nhopt        -lborealisnhopt    \
                -L../modules/queryProcessor         -lborealisqp       \
                -L../modules/util                   -lborealisutil

libborealisqp_la_LIBADD =                                  \
           expr/libborealisqpexpr.la                       \
           runtime/libborealisqpruntime.la                 \
           storageMgr/libborealisqpstore.la                \
           statsMgr/libborealisqpstatsMgr.la               \
           boxes/libborealisqpboxes.la

#
###########################  end  Makefile.am  ################################
