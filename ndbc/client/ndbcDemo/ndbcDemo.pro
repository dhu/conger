#-------------------------------------------------
#
# Project created by QtCreator 2012-10-02T15:50:14
#
#-------------------------------------------------

QT       += core gui

TARGET = ndbcDemo
TEMPLATE = app

BOREALIS_ROOT = /home/jj/Workspace/borealis_summer_2008/conger/src
BOREALIS_TOOL = /opt/borealis-tools

SOURCES += main.cc\
        mainwindow.cc \
    trafficsimulator.cc \
    trafficstatistics.cc

HEADERS  += mainwindow.h \
    trafficsimulator.h \
    trafficstatistics.h

FORMS    += mainwindow.ui \
    trafficstatistics.ui

RESOURCES += \
    images.qrc


unix:!macx:!symbian: LIBS += -L$$BOREALIS_TOOL/nmstl/lib/ -lNMSTL            \
        -L$$BOREALIS_ROOT/modules/catalog               -lborealiscatalog    \
        -L$$BOREALIS_ROOT/modules/ha                    -lborealisha         \
        -L$$BOREALIS_ROOT/modules/queryProcessor        -lborealisqp         \
        -L$$BOREALIS_ROOT/external                      -lborealisexternal   \
        -L$$BOREALIS_ROOT/../utility/client/region      -lborealisregion     \
        -L$$BOREALIS_ROOT/modules/common                -lborealiscommon     \
        -L$$BOREALIS_ROOT/modules/util                  -lborealisutil       \
        -L$$BOREALIS_ROOT/modules/cql                   -lcongercql

INCLUDEPATH += $$BOREALIS_TOOL/nmstl/include                   \
        $$BOREALIS_ROOT/modules/common                         \
        $$BOREALIS_ROOT/modules/util                           \
        $$BOREALIS_ROOT/modules/catalog                        \
        $$BOREALIS_ROOT/modules/queryProcessor                 \
        $$BOREALIS_ROOT/modules/queryProcessor/statsMgr        \
        $$BOREALIS_ROOT/modules/rpc                            \
        $$BOREALIS_ROOT/../utility/client/region               \
        $$BOREALIS_ROOT/../utility/client/rpc

DEPENDPATH += $$BOREALIS_TOOL/nmstl/include                  \
        $$BOREALIS_ROOT/modules/common                       \
        $$BOREALIS_ROOT/modules/util                         \
        $$BOREALIS_ROOT/modules/catalog                      \
        $$BOREALIS_ROOT/modules/queryProcessor               \
        $$BOREALIS_ROOT/modules/queryProcessor/statsMgr      \
        $$BOREALIS_ROOT/modules/rpc                          \
        $$BOREALIS_ROOT/../utility/client/region             \
        $$BOREALIS_ROOT/../utility/client/rpc
