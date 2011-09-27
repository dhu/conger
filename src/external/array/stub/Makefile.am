MYFLAGS     =
AM_CFLAGS   = $(MYFLAGS)
AM_CXXFLAGS = $(MYFLAGS)

INCLUDES = @BOREALISINCLUDE@   @BDBINCLUDE@                                  \
           -I../../../modules/common                                         \
           -I../../../modules/util                                           \
           -I../../../modules/queryProcessor/runtime                         \
           -I../../../modules/queryProcessor/storageMgr                      \
           -I../../../modules/queryProcessor/expr                            \
           -I../../../modules/catalog

noinst_LTLIBRARIES = libborealisarray.la

libborealisarray_la_SOURCES = InstantiateArray.cc                \
                              CatalogArray.cc
