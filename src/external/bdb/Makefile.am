MYFLAGS     =
AM_CFLAGS   = $(MYFLAGS)
AM_CXXFLAGS = $(MYFLAGS)

if COND_BDB

INCLUDES = @BOREALISINCLUDE@   @BDBINCLUDE@                                \
           -I../../modules/common                                          \
           -I../../modules/util                                            \
           -I../../modules/queryProcessor/runtime                          \
           -I../../modules/queryProcessor/storageMgr                       \
           -I../../modules/queryProcessor/statsMgr                         \
           -I../../modules/queryProcessor/expr                             \
           -I../../modules/catalog

noinst_LTLIBRARIES = libborealisbdb.la

libborealisbdb_la_SOURCES =                 \
           CatalogBdb.cc    CatalogBdb.h    \
           DeleteQBox.cc    DeleteQBox.h    \
           ExistsQBox.cc    ExistsQBox.h    \
           InsertQBox.cc    InsertQBox.h    \
           InstantiateBdb.cc                \
           NodeTable.cc     NodeTable.h     \
           SelectQBox.cc    SelectQBox.h    \
           Table.cc         Table.h         \
           TableCursors.cc  TableCursors.h  \
           UpdateQBox.cc    UpdateQBox.h

else
SUBDIRS = stub
endif
