MYFLAGS = -DENABLE_CONTAINER_SERIALIZATION=1
AM_CFLAGS   = $(MYFLAGS)
AM_CXXFLAGS = $(MYFLAGS)

INCLUDES = @BOREALISINCLUDE@                         \
           -I../common    -I../util    -I../rpc      \
           -I../queryProcessor/expr                  \
           -I../queryProcessor/congerDeploy

lib_LTLIBRARIES = libborealiscatalog.la

libborealiscatalog_la_SOURCES = Catalog.cc          Catalog.h             \
                                CatalogValidate.cc                        \
                                Diagram.cc          Diagram.h             \
                                BoxPort.cc          BoxPort.h             \
                                BoxParameter.cc     BoxParameter.h        \
                                CatalogBox.cc       CatalogBox.h          \
                                CatalogQuery.cc     CatalogQuery.h        \
                                CatalogSchema.cc    CatalogSchema.h       \
                                CatalogStream.cc    CatalogStream.h       \
                                CatalogTable.cc     CatalogTable.h        \
                                CatalogView.cc      CatalogView.h         \
                                SchemaArray.cc      SchemaArray.h         \
                                                    SchemaField.h         \
                                                    CatalogSubscription.h \
                                CongerDiagram.cc    CongerDiagram.h
