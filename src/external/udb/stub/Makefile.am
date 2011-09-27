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

noinst_LTLIBRARIES = libborealisudb.la

libborealisudb_la_SOURCES =  InstantiateUdb.cc                \
                             CatalogUdb.cc
