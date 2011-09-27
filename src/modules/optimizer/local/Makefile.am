MYFLAGS     =
AM_CFLAGS   = $(MYFLAGS)
AM_CXXFLAGS = $(MYFLAGS)

#LDADD_EXTRAS = @XERCESCLIB@ @NMSTLLIB@

#LDADD = -L../../common \
#        -L../../util \
#        -L../../catalog \
#        -L../../queryProcessor \
#        -lborealiscommon -lborealisutil -lborealiscatalog \
#        -lborealisqp \
#        $(LDADD_EXTRAS)
#
INCLUDES = @XERCESCINCLUDE@ @NMSTLINCLUDE@ \
           -I../../common                  \
           -I../../util                    \
           -I../../rpc                     \
           -I../../catalog                 \
           -I../../queryProcessor          \
           -I../../queryProcessor/statsMgr 

#DEPENDENCIES =  ../../common/libborealiscommon.la \
#           ../../util/libborealisutil.la \
#           ../../catalog/libborealiscatalog.la \
#           ../../queryProcessor/libborealisqp.la \
#           ../../queryProcessor/statsMgr/libborealisqpstatsMgr.la 

lib_LTLIBRARIES = libborealislopt.la

libborealislopt_la_SOURCES =                           \
           LocalOptimizer.h        LocalOptimizer.cc   \
           LocalLoadShedder.h      LocalLoadShedder.cc
    
