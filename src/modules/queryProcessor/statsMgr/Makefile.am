MYFLAGS     =
AM_CFLAGS   = $(MYFLAGS)
AM_CXXFLAGS = $(MYFLAGS)

INCLUDES = @XERCESCINCLUDE@  @NMSTLINCLUDE@                \
           -I../../common/   -I../../util/                 \
           -I../runtime      -I../expr

#noinst_LTLIBRARIES = libborealisqpstatsMgr.la
lib_LTLIBRARIES = libborealisqpstatsMgr.la

libborealisqpstatsMgr_la_SOURCES =                     \
           FixLenTimeSeries.h  FixLenTimeSeries.cc     \
           TSStats.h           TSStats.cc              \
           StatsMgr.h          StatsMgr.cc             \
           Statistics.h        Statistics.cc           \
           RWlock.h            RWlock.cc               \
           VersionRWLock.h     VersionRWLock.cc
