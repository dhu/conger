MYFLAGS     =
AM_CFLAGS   = $(MYFLAGS)
AM_CXXFLAGS = $(MYFLAGS)

INCLUDES = @BOREALISINCLUDE@                                       \
           -I../../util      -I../runtime                          \
           -I../expr         -I../../common   -I../../catalog

noinst_LTLIBRARIES = libborealisqpstore.la

libborealisqpstore_la_SOURCES =                  \
           PagePool.cc          PagePool.h       \
           TupleQueue.cc        TupleQueue.h     \
           UpdateQueue.cc       UpdateQueue.h    \
           CPStorage.cc         CPStorage.h      \
           CPView.cc            CPView.h         \
		   CPQBox.cc            CPQBox.h         \
		   SweeperQBox.cc       SweeperQBox.h
