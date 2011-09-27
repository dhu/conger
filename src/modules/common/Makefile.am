MYFLAGS     =
AM_CFLAGS   = $(MYFLAGS)
AM_CXXFLAGS = $(MYFLAGS)

INCLUDES= @BOREALISINCLUDE@                                                 \
          -I../rpc          -I../util       -I../queryProcessor/storageMgr  \
          -I../queryProcessor/expr          -I../queryProcessor/runtime     \
          -I../catalog

lib_LTLIBRARIES = libborealiscommon.la

libborealiscommon_la_SOURCES =                                  \
                                      BasicComponent.h          \
          Box.cc                      Box.h                     \
          BoxPackage.cc               BoxPackage.h              \
          common.cc                   common.h                  \
          ConsistencyState.h                                    \
          CPViewDescription.cc        CPViewDescription.h       \
          DataHandler.cc              DataHandler.h             \
          DataType.cc                 DataType.h                \
          LoadOffer.cc                LoadOffer.h               \
                                      MedusaClient.h            \
          MedusaID.cc                 MedusaID.h                \
          QueueTrimmingMessage.cc     QueueTrimmingMessage.h    \
                                      Name.h                    \
          Object.cc                   Object.h                  \
                                      Objects.h                 \
                                      Partition.h               \
          Query.cc                    Query.h                   \
          NodeType.cc                 NodeType.h                \
          HAType.cc                   HAType.h                  \
          HAMethod.cc                 HAMethod.h                \
          Schema.cc                   Schema.h                  \
                                      Stats.h                   \
                                      NodeStatus.h              \
          StreamDef.cc                StreamDef.h               \
                                      StreamEvent.h             \
                                      StreamID.h                \
          StreamProperties.cc         StreamProperties.h        \
                                      Subscription.h            \
          TableDescription.cc         TableDescription.h        \
          Tuple.cc                    Tuple.h                   \
          TupleDescription.cc         TupleDescription.h        \
          TupleHeader.cc              TupleHeader.h             \
          RecoveryTime.cc             RecoveryTime.h            \
          SlidingWindowStatistics.cc  SlidingWindowStatistics.h \
          HAUnit.cc                   HAUnit.h                  \
          CheckpointMessage.cc        CheckpointMessage.h       \
          LSPlan.cc                   LSPlan.h                  \
          FeasibleInputTable.cc       FeasibleInputTable.h
