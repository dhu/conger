MYFLAGS     =
AM_CFLAGS   = $(MYFLAGS)
AM_CXXFLAGS = $(MYFLAGS)

INCLUDES = @BOREALISINCLUDE@  @BDBINCLUDE@                                   \
           -I../../common    -I../../util    -I../runtime    -I../storageMgr \
           -I../expr         -I../../catalog -I..            -I../statsMgr

noinst_LTLIBRARIES = libborealisqpboxes.la

libborealisqpboxes_la_SOURCES =                                \
           FilterQBox.cc              FilterQBox.h             \
           FilterArrayQBox.cc         FilterArrayQBox.h        \
           MapQBox.cc                 MapQBox.h                \
           UnionQBox.cc               UnionQBox.h              \
           BSortQBox.cc               BSortQBox.h              \
           JoinQBox.cc                JoinQBox.h               \
           WaitForQBox.cc             WaitForQBox.h            \
           AggregateQBox.cc           AggregateQBox.h          \
           LockQBox.cc                LockQBox.h               \
           SControlQBox.cc            SControlQBox.h           \
           SJoinQBox.cc               SJoinQBox.h              \
           SOutputQBox.cc             SOutputQBox.h            \
           Bucket.cc                  Bucket.h                 \
           SUnionQBox.cc              SUnionQBox.h             \
           UnlockQBox.cc              UnlockQBox.h             \
           RevisionFilterQBox.cc      RevisionFilterQBox.h     \
           RevisionAggregateQBox.cc   RevisionAggregateQBox.h  \
           RevisionMapQBox.cc         RevisionMapQBox.h        \
           RandomDropQBox.cc          RandomDropQBox.h         \
           WindowDropQBox.cc          WindowDropQBox.h         \
           AuroraJoinQBox.cc          AuroraJoinQBox.h         
