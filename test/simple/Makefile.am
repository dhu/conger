YFLAGS =
AM_CFLAGS = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = mytest                                            \
       mytestdist                                        \
       aurorajoin_test    join_test     joinstatic_test  \
       aggregate_test                                    \
       update_queue    update_queue1    update_queue2    \
       movebox_test                                      \
       table_test                                        \
       cptest          cp2test

bin_PROGRAMS = $(MAIN)
BOREALIS_SRC = @BOREALISROOT@

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
           -I $(BOREALIS_SRC)/modules/queryProcessor/statsMgr  \
		   -I ../../tool/head                                  \
		   -I ../../utility/client/rpc

DEPENDENCIES = $(BOREALIS_SRC)/modules/common/libborealiscommon.la

TUPLESOURCE = InputTuple.h PacketTuple.h

                 ###########################################

mytest_DEPENDENCIES = MytestMarshal.cc MytestMarshal.h

mytest_SOURCES = ${TUPLESOURCE} ${mytest_DEPENDENCIES}  \
                 mytest.cc

${mytest_DEPENDENCIES}: mytest.xml
	marshal mytest.xml

                 ###########################################

mytestdist_LDADD = ${LDADD}                                     \
				-L../../tool/head/             -lborealisdeploy \
				-L../../utility/client/region  -lborealisregion

mytestdist_DEPENDENCIES = MytestdistMarshal.cc MytestdistMarshal.h

mytestdist_SOURCES = ${TUPLESOURCE} ${mytestdist_DEPENDENCIES}  \
                 mytestdist.cc

# We modified these files so we do not want them to be generated automatically
#${mytestdist_DEPENDENCIES}: mytestdist.xml
#	marshal mytestdist.xml

                 ###########################################

cptest_DEPENDENCIES = CptestMarshal.cc CptestMarshal.h
cptest_SOURCES = ${TUPLESOURCE} ${cptest_DEPENDENCIES} cptest.cc

${cptest_DEPENDENCIES}: cptest.xml
	marshal cptest.xml

                 ###########################################

cp2test_DEPENDENCIES = Cp2TestMarshal.cc Cp2TestMarshal.h
cp2test_SOURCES = ${TUPLESOURCE} ${cp2test_DEPENDENCIES} cp2test.cc

${cp2test_DEPENDENCIES}: cp2test.xml
	marshal cp2test.xml

                 ###########################################

aurorajoin_test_DEPENDENCIES = Aurorajoin_TestMarshal.cc Aurorajoin_TestMarshal.h
aurorajoin_test_SOURCES = ${TUPLESOURCE} ${aurorajoin_test_DEPENDENCIES} aurorajoin_test.cc

${aurorajoin_test_DEPENDENCIES}: aurorajoin_test.xml
	marshal aurorajoin_test.xml

                 ###########################################

join_test_DEPENDENCIES = Join_TestMarshal.cc Join_TestMarshal.h
join_test_SOURCES = ${TUPLESOURCE} ${join_test_DEPENDENCIES} join_test.cc

${join_test_DEPENDENCIES}: join_test.xml
	marshal join_test.xml

                 ###########################################

joinstatic_test_DEPENDENCIES = Joinstatic_TestMarshal.cc \
                               Joinstatic_TestMarshal.h
joinstatic_test_SOURCES = ${TUPLESOURCE} ${joinstatic_test_DEPENDENCIES} \
                          joinstatic_test.cc

${joinstatic_test_DEPENDENCIES}: joinstatic_test.xml
	marshal joinstatic_test.xml

                 ###########################################

aggregate_test_DEPENDENCIES = Aggregate_TestMarshal.cc Aggregate_TestMarshal.h

aggregate_test_SOURCES = ${TUPLESOURCE} ${aggregate_test_DEPENDENCIES}  \
                         aggregate_test.cc

${aggregate_test_DEPENDENCIES}: aggregate_test.xml
	marshal aggregate_test.xml

                 ###########################################

update_queue_DEPENDENCIES = Update_QueueMarshal.cc \
                            Update_QueueMarshal.h

update_queue_SOURCES = ${update_queue_DEPENDENCIES} \
                       update_queue.cc

${update_queue_DEPENDENCIES}: update_queue.xml update_queue_deploy.xml
	marshal update_queue.xml update_queue_deploy.xml

update_queue1_DEPENDENCIES = Update_Queue1Marshal.cc \
                                 Update_Queue1Marshal.h

update_queue1_SOURCES = ${update_queue1_DEPENDENCIES} \
                        update_queue1.cc

${update_queue1_DEPENDENCIES}: update_queue1.xml update_queue_deploy.xml
	marshal update_queue1.xml update_queue_deploy.xml

update_queue2_DEPENDENCIES = Update_Queue2Marshal.cc \
                                 Update_Queue2Marshal.h

update_queue2_SOURCES = ${update_queue2_DEPENDENCIES} \
                        update_queue2.cc

${update_queue2_DEPENDENCIES}: update_queue2.xml update_queue_deploy.xml
	marshal update_queue2.xml update_queue_deploy.xml

                 ###########################################

table_test_DEPENDENCIES = Table_TestMarshal.cc \
                          Table_TestMarshal.h
    
table_test_SOURCES = ${table_test_DEPENDENCIES} table_test.cc

${table_test_DEPENDENCIES}: table_test.xml table_deployment.xml
	marshal table_test.xml table_deployment.xml

                 ###########################################

movebox_test_DEPENDENCIES = Movebox_TestMarshal.cc \
                            Movebox_TestMarshal.h
    
movebox_test_SOURCES = ${movebox_test_DEPENDENCIES} movebox_test.cc

${movebox_test_DEPENDENCIES}: movebox_test.xml movebox_deployment.xml
	marshal movebox_test.xml movebox_deployment.xml


                 ###########################################


CLEANFILES = ${mytest_DEPENDENCIES}                  \
             ${cptest_DEPENDENCIES}                  \
             ${cp2test_DEPENDENCIES}                 \
             ${aurorajoin_test_DEPENDENCIES}         \
             ${join_test_DEPENDENCIES}               \
             ${joinstatic_test_DEPENDENCIES}         \
             ${aggregate_test_DEPENDENCIES}          \
             ${filter_test_DEPENDENCIES}             \
             ${update_queue_DEPENDENCIES}            \
             ${update_queue1_DEPENDENCIES}           \
             ${update_queue2_DEPENDENCIES}           \
             ${movebox_test_DEPENDENCIES}            \
             ${table_test_DEPENDENCIES}

##############################  end Makefile.am  ###############################
