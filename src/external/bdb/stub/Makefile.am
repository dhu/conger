MYFLAGS     =
AM_CFLAGS   = $(MYFLAGS)
AM_CXXFLAGS = $(MYFLAGS)

INCLUDES = @BOREALISINCLUDE@  @BDBINCLUDE@                                   \
           -I../../../modules/common                                         \
           -I../../../modules/util                                           \
           -I../../../modules/queryProcessor/runtime                         \
           -I../../../modules/queryProcessor/storageMgr                      \
           -I../../../modules/queryProcessor/expr                            \
           -I../../../modules/catalog

noinst_LTLIBRARIES = libborealisbdb.la

libborealisbdb_la_SOURCES =  NodeTable.cc     NodeTable.h        \
                             InstantiateBdb.cc                   \
                             CatalogBdb.cc
