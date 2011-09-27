MYFLAGS     =
AM_CFLAGS   = $(MYFLAGS)
AM_CXXFLAGS = $(MYFLAGS)

if COND_ARRAY

INCLUDES = @BOREALISINCLUDE@   @BDBINCLUDE@      @OCVINCLUDE@        \
           -I../../modules/common                                    \
           -I../../modules/util                                      \
           -I../../modules/queryProcessor/runtime                    \
           -I../../modules/queryProcessor/storageMgr                 \
           -I../../modules/queryProcessor/statsMgr                   \
           -I../../modules/queryProcessor/expr                       \
           -I../../modules/queryProcessor/boxes                      \
           -I../../modules/queryProcessor                            \
           -I../../modules/catalog                   

           
noinst_LTLIBRARIES = libborealisarray.la

libborealisarray_la_SOURCES =                                        \
     ScatterQBox.cc                     ScatterQBox.h                    \
     GatherQBox.cc                      GatherQBox.h	\
    CatalogArray.cc                 InstantiateArray.cc

else
SUBDIRS = stub
endif
