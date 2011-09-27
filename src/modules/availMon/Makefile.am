MYFLAGS =  
AM_CFLAGS   = $(MYFLAGS)
AM_CXXFLAGS = $(MYFLAGS)

INCLUDES = @XERCESCINCLUDE@ \
           @NMSTLINCLUDE@ \
           -I../common \
           -I../util \
           -I../rpc \
           -I../queryProcessor \
           -I../catalog \
           -I../queryProcessor/statsMgr

lib_LTLIBRARIES = libborealisavailmon.la

libborealisavailmon_la_SOURCES = AvailabilityMonitor.cc AvailabilityMonitor.h
