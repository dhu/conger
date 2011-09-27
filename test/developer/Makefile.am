
MYFLAGS =
AM_CFLAGS = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = lstest                             \
       lstest_simple                      \
       lstest2                            \
       lstest_split                       \
       lstest_split2                      \
       lmtest                             \
       filter_test			  \
       maptest

       # aggregaterevtest
       # lstest_simple_aggregate
       # lstest_aggregate
       # upstreamdemo1  upstreamdemo2
       # downstreamdemo1   downstreamdemo2
       # myoldtest      dynamic_mod_test
       # rpctest
       # revisiontest
       # tuple_stime_test 
       # lstest_two_server
       # two_server_test
       # movebox_union_test   movebox_stream

bin_PROGRAMS = $(MAIN)

BOREALIS_SRC = ../@BOREALISROOT@

LDADD_EXTRAS = -ldl  @BOREALISLIB@  @EXTERNALLIB@

LDADD = -L$(BOREALIS_SRC)/modules/common          -lborealiscommon     \
        -L$(BOREALIS_SRC)/modules/catalog         -lborealiscatalog    \
        -L$(BOREALIS_SRC)/modules/util            -lborealisutil       \
        -L$(BOREALIS_SRC)/modules/ha              -lborealisha         \
        -L$(BOREALIS_SRC)/modules/queryProcessor  -lborealisqp         \
        -L$(BOREALIS_SRC)/external                -lborealisexternal   \
        $(LDADD_EXTRAS)

INCLUDES = @XERCESCINCLUDE@   @NMSTLINCLUDE@                   \
           -I $(BOREALIS_SRC)/modules/common                   \
           -I $(BOREALIS_SRC)/modules/util                     \
           -I $(BOREALIS_SRC)/modules/rpc                      \
           -I $(BOREALIS_SRC)/modules/catalog                  \
           -I $(BOREALIS_SRC)/modules/queryProcessor           \
           -I $(BOREALIS_SRC)/modules/queryProcessor/statsMgr

DEPENDENCIES = $(BOREALIS_SRC)/modules/common/libborealiscommon.la

TUPLESOURCE = InputTuple.h PacketTuple.h

                 ###########################################

#myoldtest_SOURCES = ${TUPLESOURCE} myoldtest.cc

                 ###########################################

#revisiontest_DEPENDENCIES = RevisiontestMarshal.cc RevisiontestMarshal.h

#revisiontest_SOURCES = ${TUPLESOURCE} ${revisiontest_DEPENDENCIES}  \
#                       revisiontest.cc

#${revisiontest_DEPENDENCIES}: revisiontest.xml
#	marshal revisiontest.xml

                 ###########################################

#upstreamdemo1_DEPENDENCIES = Upstreamdemo1Marshal.cc Upstreamdemo1Marshal.h

#upstreamdemo1_SOURCES = ${TUPLESOURCE} ${upstreamdemo1_DEPENDENCIES}  \
#                        upstreamdemo1.cc

#${upstreamdemo1_DEPENDENCIES}: upstreamdemo1.xml
#	marshal upstreamdemo1.xml

                 ###########################################

#upstreamdemo2_DEPENDENCIES = Upstreamdemo2Marshal.cc Upstreamdemo2Marshal.h

#upstreamdemo2_SOURCES = ${TUPLESOURCE} ${upstreamdemo2_DEPENDENCIES}  \
#                        upstreamdemo2.cc

#${upstreamdemo2_DEPENDENCIES}: upstreamdemo2.xml
#	marshal upstreamdemo2.xml

                 ###########################################

#downstreamdemo1_DEPENDENCIES = Downstreamdemo1Marshal.cc \
#                               Downstreamdemo1Marshal.h

#downstreamdemo1_SOURCES = ${TUPLESOURCE} ${downstreamdemo1_DEPENDENCIES}  \
#                          downstreamdemo1.cc

#${downstreamdemo1_DEPENDENCIES}: downstreamdemo1.xml
#	marshal downstreamdemo1.xml

                 ###########################################

#downstreamdemo2_DEPENDENCIES = Downstreamdemo2Marshal.cc \
#                               Downstreamdemo2Marshal.h

#downstreamdemo2_SOURCES = ${TUPLESOURCE} ${downstreamdemo2_DEPENDENCIES}  \
#                          downstreamdemo2.cc

#${downstreamdemo2_DEPENDENCIES}: downstreamdemo2.xml
#	marshal downstreamdemo2.xml

                 ###########################################

#aggregaterevtest_DEPENDENCIES = AggregaterevtestMarshal.cc   \
#                                AggregaterevtestMarshal.h

#aggregaterevtest_SOURCES = ${TUPLESOURCE} ${aggregaterevtest_DEPENDENCIES}  \
#                           aggregaterevtest.cc

#${aggregaterevtest_DEPENDENCIES}: aggregaterevtest.xml
#	marshal aggregaterevtest.xml

                 ###########################################

filter_test_DEPENDENCIES = Filter_TestMarshal.cc Filter_TestMarshal.h

filter_test_SOURCES = ${filter_test_DEPENDENCIES}  \
                      filter_test.cc

${filter_test_DEPENDENCIES}: filter_test.xml
	marshal filter_test.xml

                 ###########################################

lstest_DEPENDENCIES = LstestMarshal.cc LstestMarshal.h

lstest_SOURCES = ${lstest_DEPENDENCIES}  \
                 lstest.cc

${lstest_DEPENDENCIES}: lstest.xml lstest_deploy.xml
	marshal lstest.xml lstest_deploy.xml

                 ###########################################

lstest_simple_DEPENDENCIES = Lstest_SimpleMarshal.cc LstestSimpleMarshal.h

lstest_simple_SOURCES = ${lstest_simple_DEPENDENCIES}  \
                        lstest_simple.cc

${lstest_simple_DEPENDENCIES}: lstest_simple.xml lstest_simple_deploy.xml
	marshal lstest_simple.xml lstest_simple_deploy.xml

                 ###########################################

lstest2_DEPENDENCIES = Lstest2Marshal.cc Lstest2Marshal.h

lstest2_SOURCES = ${lstest2_DEPENDENCIES}  \
                  lstest2.cc

${lstest2_DEPENDENCIES}: lstest2.xml lstest2_deploy.xml
	marshal lstest2.xml lstest2_deploy.xml

                 ###########################################

#lstest_aggregate_DEPENDENCIES = Lstest_AggregateMarshal.cc \
#                                Lstest_AggregateMarshal.h

#lstest_aggregate_SOURCES = ${lstest_aggregate_DEPENDENCIES}  \
#                           lstest_aggregate.cc

#${lstest_aggregate_DEPENDENCIES}: lstest_aggregate.xml \
#                                  lstest_aggregate_deploy.xml
#	marshal lstest_aggregate.xml lstest_aggregate_deploy.xml

                 ###########################################

#lstest_simple_aggregate_DEPENDENCIES = Lstest_Simple_AggregateMarshal.cc \
#                                       Lstest_Simple_AggregateMarshal.h

#lstest_simple_aggregate_SOURCES = ${lstest_simple_aggregate_DEPENDENCIES}  \
#                                  lstest_simple_aggregate.cc

#${lstest_simple_aggregate_DEPENDENCIES}: lstest_simple_aggregate.xml \
#                                         lstest_simple_aggregate_deploy.xml
#	marshal lstest_simple_aggregate.xml lstest_simple_aggregate_deploy.xml

                 ###########################################

lstest_split_DEPENDENCIES = Lstest_SplitMarshal.cc Lstest_SplitMarshal.h

lstest_split_SOURCES = ${lstest_split_DEPENDENCIES}  \
                       lstest_split.cc

${lstest_split_DEPENDENCIES}: lstest_split.xml lstest_split_deploy.xml
	marshal lstest_split.xml lstest_split_deploy.xml

                 ###########################################

lstest_split2_DEPENDENCIES = Lstest_Split2Marshal.cc Lstest_Split2Marshal.h

lstest_split2_SOURCES = ${lstest_split2_DEPENDENCIES}  \
                       lstest_split2.cc

${lstest_split2_DEPENDENCIES}: lstest_split2.xml lstest_split2_deploy.xml
	marshal lstest_split2.xml lstest_split2_deploy.xml

                 ###########################################

#lstest_two_server_DEPENDENCIES = Lstest_Two_ServerMarshal.cc  \
#                                 Lstest_Two_ServerMarshal.h

#lstest_two_server_SOURCES = ${lstest_two_server_DEPENDENCIES} \
#                            lstest_two_server.cc

#${lstest_two_server_DEPENDENCIES}: lstest_two_server.xml      \
#                                   lstest_two_server_deploy.xml
#	marshal lstest_two_server.xml lstest_two_server_deploy.xml; \
#    patch -p1 < Two_Server_TestMarshal.cc.patch Lstest_Two_ServerMarshal.cc

                 ###########################################

lmtest_DEPENDENCIES = LmtestMarshal.cc LmtestMarshal.h

lmtest_SOURCES = ${lmtest_DEPENDENCIES}  \
                 lmtest.cc

${lmtest_DEPENDENCIES}: lmtest.xml lmtest_deploy.xml
	marshal lmtest.xml lmtest_deploy.xml

                 ###########################################

#two_server_test_DEPENDENCIES = Two_Server_TestMarshal.cc \
#                               Two_Server_TestMarshal.h

#two_server_test_SOURCES = ${two_server_test_DEPENDENCIES}  \
#                          two_server_test.cc

#${two_server_test_DEPENDENCIES}: two_server_test.xml       \
#                                 two_server_deployment.xml
#	marshal two_server_test.xml two_server_deployment.xml; \
#    patch -p1 < Two_Server_TestMarshal.cc.patch Two_Server_TestMarshal.cc

                 ###########################################

#rpctest_SOURCES = rpctest.cc

                 ###########################################

#dynamic_mod_test_DEPENDENCIES = Dynamic_Mod_TestMarshal.h \
#                                Dynamic_Mod_TestMarshal.cc

#dynamic_mod_test_SOURCES = ${dynamic_mod_test_DEPENDENCIES} \
#                           dynamic_mod_test.cc

#${dynamic_mod_test_DEPENDENCIES}: dynamic_mod_test.xml \
#                                  dynamic_mod_deployment.xml
#	marshal dynamic_mod_test.xml dynamic_mod_deployment.xml

                 ###########################################

#tuple_stime_test_DEPENDENCIES = Tuple_Stime_TestMarshal.cc \
#                            Tuple_Stime_TestMarshal.h
    
#tuple_stime_test_SOURCES = ${tuple_stime_test_DEPENDENCIES} tuple_stime_test.cc

#${tuple_stime_test_DEPENDENCIES}: tuple_stime_test.xml
#	marshal tuple_stime_test.xml

                 ###########################################

maptest_DEPENDENCIES = MaptestMarshal.cc MaptestMarshal.h
maptest_SOURCES = ${TUPLESOURCE} ${maptest_DEPENDENCIES} maptest.cc

${maptest_DEPENDENCIES}: maptest.xml
	marshal maptest.xml

                 ###########################################

#movebox_union_test_SOURCES = ${TUPLESOURCE} movebox_union_test.cc

#movebox_stream_SOURCES = ${TUPLESOURCE} movebox_stream.cc

                 ###########################################


CLEANFILES = ${lmtest_DEPENDENCIES}                  \
             ${lstest_DEPENDENCIES}                  \
             ${lstest_simple_DEPENDENCIES}           \
             ${lstest2_DEPENDENCIES}                 \
             ${lstest_aggregate_DEPENDENCIES}        \
             ${lstest_simple_aggregate_DEPENDENCIES} \
             ${lstest_split_DEPENDENCIES}            \
             ${lstest_split2_DEPENDENCIES}	     \
             ${maptest_DEPENDENCIES}                 
 
#             ${revisiontest_DEPENDENCIES}            \
#             ${upstreamdemo1_DEPENDENCIES}           \
#             ${upstreamdemo2_DEPENDENCIES}           \
#             ${aggregaterevtest_DEPENDENCIES}        \
#             ${lstest_two_server_DEPENDENCIES}       \
#             ${two_server_test_DEPENDENCIES}         \
#             ${dynamic_mod_test_DEPENDENCIES}        \
#             ${tuple_stime_test_DEPENDENCIES}        \
#             ${downstreamdemo1_DEPENDENCIES}         \
#             ${downstreamdemo2_DEPENDENCIES}

##############################  end Makefile.am  ###############################
