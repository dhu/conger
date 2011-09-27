MYFLAGS     =  
AM_CFLAGS   = $(MYFLAGS)
AM_CXXFLAGS = $(MYFLAGS)

INCLUDES= @XERCESCINCLUDE@  @NMSTLINCLUDE@                              \
                            -I../common    -I../catalog

lib_LTLIBRARIES = libborealisutil.la

libborealisutil_la_SOURCES =                                \
       BufferList.cc               BufferList.h             \
       CpuStats.cc                 CpuStats.h               \
       Exceptions.cc               Exceptions.h             \
       FieldExt.cc                 FieldExt.h               \
       HashForBufferList.cc        HashForBufferList.h      \
                                   ILockable.h              \
       LockHolder.cc               LockHolder.h             \
                                   Monitor.h                \
       nmstl_util.cc               nmstl_util.h             \
                                   OFunction.h              \
       parseutil.cc                parseutil.h              \
       PtCondition.cc              PtCondition.h            \
       PtMutex.cc                  PtMutex.h                \
       StackTrace.cc               StackTrace.h             \
       StorageMgr_Exceptions.cc    StorageMgr_Exceptions.h  \
                                   Timestamp.h              \
       tupleGenerator.cc           tupleGenerator.h         \
       util.cc                     util.h                   \
                                   xercesDomFwd.h           \
       xercesDomUtil.cc            xercesDomUtil.h          \
       XmlTempString.cc            XmlTempString.h

#                                   Registry.h
