SUBDIRS = udb  bdb  array

###########################################################

MYFLAGS     =
AM_CFLAGS   = $(MYFLAGS)
AM_CXXFLAGS = $(MYFLAGS)

lib_LTLIBRARIES = libborealisexternal.la

libborealisexternal_la_DEPENDENCIES =
libborealisexternal_la_LIBADD = 

if COND_UDB
   libborealisexternal_la_DEPENDENCIES += udb/libborealisudb.la
   libborealisexternal_la_LIBADD += udb/libborealisudb.la 
else
   libborealisexternal_la_DEPENDENCIES += udb/stub/libborealisudb.la
   libborealisexternal_la_LIBADD += udb/stub/libborealisudb.la
endif

if COND_BDB
    libborealisexternal_la_DEPENDENCIES += bdb/libborealisbdb.la
    libborealisexternal_la_LIBADD += bdb/libborealisbdb.la
else
    libborealisexternal_la_DEPENDENCIES += bdb/stub/libborealisbdb.la
    libborealisexternal_la_LIBADD += bdb/stub/libborealisbdb.la
endif

if COND_ARRAY
   libborealisexternal_la_DEPENDENCIES += array/libborealisarray.la
   libborealisexternal_la_LIBADD += array/libborealisarray.la
else
   libborealisexternal_la_DEPENDENCIES += array/stub/libborealisarray.la
   libborealisexternal_la_LIBADD += array/stub/libborealisarray.la
endif

libborealisexternal_la_SOURCES = empty.cc

###############################################################################
