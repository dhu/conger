##################################################################

MYFLAGS =  

AM_CFLAGS = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

if COND_TINYDB
INCLUDES= -I. -I../../rpc -I../../util -I../../common -I../../../   \
              -I../../../src   -I../../ha -I../../nhopt             \
              -I../../queryProcessor -I../../lookup                 \
              @XERCESCINCLUDE@ @NMSTLINCLUDE@ @TINYDBINCLUDE@ @ANTLRINCLUDE@  \
              -I../../util -I../../rpc -I../../queryProcessor/storageMgr  \
              -I../../queryProcessor/expr -I../../queryProcessor/runtime

lib_LTLIBRARIES = libtinydbwrapper.la
libtinydbwrapper_la_SOURCES = JSocket.cc JSocket.h \
                            DataThread.cc DataThread.h \
                            Stream.cc Stream.h \
                            Box.cc Box.h \
                            Catalog.cc Catalog.h \
                            Metric.cc Metric.h \
                            Constraint.cc Constraint.h \
                            ScoreFunction.h \
                            LifetimeFunction.cc LifetimeFunction.h \
                            LatencyFunction.cc LatencyFunction.h \
                            ThroughputFunction.cc ThroughputFunction.h \
                            QualityFunction.cc QualityFunction.h \
                            Score.cc Score.h \
                            Scores.cc Scores.h \
                            Field.cc Field.h \
                            Parameter.cc Parameter.h \
                            Property.cc Property.h \
                            Schema.cc Schema.h \
                            TQuery.cc TQuery.h \
                            TQueries.cc TQueries.h \
                            Wrapper.cc Wrapper.h \
                            WrapperConfig.h
endif
