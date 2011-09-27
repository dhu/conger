MYFLAGS     =
AM_CFLAGS   = $(MYFLAGS)
AM_CXXFLAGS = $(MYFLAGS)

INCLUDES = @BOREALISINCLUDE@  @BDBINCLUDE@                                  \
           -I../../common    -I../../util    -I../storageMgr                \
           -I../statsMgr     -I../expr       -I../../catalog    -I../boxes  \
           -I..              -I../../rpc

noinst_LTLIBRARIES = libborealisqpruntime.la

libborealisqpruntime_la_SOURCES =                                 \
           AuroraNode.cc               AuroraNode.h               \
           AuroraNodeMonitor.cc        AuroraNodeMonitor.h        \
                                       BoxStatsSample.h           \
                                       LockSet.h                  \
           PseudoScheduler.cc          PseudoScheduler.h          \
           PseudoSchedulerMonitor.cc   PseudoSchedulerMonitor.h   \
           QBox.cc                     QBox.h                     \
           QBoxState.cc                QBoxState.h                \
           Scheduler.cc                Scheduler.h                \
           Stream.cc                   Stream.h                   \
                                       StreamStatsSample.h        \
           TuplesAvailableMailbox.cc   TuplesAvailableMailbox.h
