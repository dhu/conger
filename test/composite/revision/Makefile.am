MYFLAGS =
AM_CFLAGS = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = upstreamdemo1       \
       upstreamdemo2       \
       upstreamtest1       \
       upstreamtest2       \
       upstreamtest3       \
       upstreamtest4       \
       upstreamtest5       \
       upstreamtest6       \
       upstreamtest7       \
       downstreamtest1     \
       downstreamtest2     \
       downstreamtest3     \
       downstreamtest4     \
       downstreamtest5     \
       downstreamtest6     \
       downstreamtest7

#   experiment_up       
#   experiment_down
#   downstreamdemo1     
#   downstreamdemo2     

bin_PROGRAMS = $(MAIN)

BOREALIS_SRC = ../../@BOREALISROOT@

LDADD_EXTRAS = -ldl  @BOREALISLIB@  @EXTERNALLIB@

LDADD = -L$(BOREALIS_SRC)/modules/common          -lborealiscommon   \
        -L$(BOREALIS_SRC)/modules/catalog         -lborealiscatalog  \
        -L$(BOREALIS_SRC)/modules/util            -lborealisutil     \
        -L$(BOREALIS_SRC)/modules/ha              -lborealisha       \
        -L$(BOREALIS_SRC)/modules/queryProcessor  -lborealisqp       \
        -L$(BOREALIS_SRC)/external                -lborealisexternal \
        $(LDADD_EXTRAS)

INCLUDES = @XERCESCINCLUDE@   @NMSTLINCLUDE@                   \
           -I $(BOREALIS_SRC)/modules/common                   \
           -I $(BOREALIS_SRC)/modules/util                     \
           -I $(BOREALIS_SRC)/modules/rpc                      \
           -I $(BOREALIS_SRC)/modules/catalog                  \
           -I $(BOREALIS_SRC)/modules/queryProcessor           \
           -I $(BOREALIS_SRC)/modules/queryProcessor/statsMgr  \
           -I ../../simple

DEPENDENCIES = $(BOREALIS_SRC)/modules/common/libborealiscommon.la

TUPLESOURCE = ../../simple/InputTuple.h ../../simple/PacketTuple.h

                 ###########################################

upstreamdemo1_DEPENDENCIES = Upstreamdemo1Marshal.cc Upstreamdemo1Marshal.h

upstreamdemo1_SOURCES = ${TUPLESOURCE} ${upstreamdemo1_DEPENDENCIES}  \
                        upstreamdemo1.cc

${upstreamdemo1_DEPENDENCIES}: upstreamdemo1.xml
	marshal upstreamdemo1.xml

                 ###########################################

upstreamdemo2_DEPENDENCIES = Upstreamdemo2Marshal.cc Upstreamdemo2Marshal.h

upstreamdemo2_SOURCES = ${TUPLESOURCE} ${upstreamdemo2_DEPENDENCIES}  \
                    upstreamdemo2.cc

${upstreamdemo2_DEPENDENCIES}: upstreamdemo2.xml
	marshal upstreamdemo2.xml

                 ###########################################

# downstreamdemo1_DEPENDENCIES = Downstreamdemo1Marshal.cc Downstreamdemo1Marshal.h

# downstreamdemo1_SOURCES = ${TUPLESOURCE} ${downstreamdemo1_DEPENDENCIES}  \
#                     downstreamdemo1.cc

# ${downstreamdemo1_DEPENDENCIES}: downstreamdemo1.xml
#   marshal downstreamdemo1.xml

                 ###########################################

# downstreamdemo2_DEPENDENCIES = Downstreamdemo2Marshal.cc Downstreamdemo2Marshal.h

# downstreamdemo2_SOURCES = ${TUPLESOURCE} ${downstreamdemo2_DEPENDENCIES}  \
#                     downstreamdemo2.cc

# ${downstreamdemo2_DEPENDENCIES}: downstreamdemo2.xml
#   marshal downstreamdemo2.xml

                 ###########################################

downstreamtest1_DEPENDENCIES = Downstreamtest1Marshal.cc Downstreamtest1Marshal.h

downstreamtest1_SOURCES = ${TUPLESOURCE} ${downstreamtest1_DEPENDENCIES}  \
                      downstreamtest1.cc

${downstreamtest1_DEPENDENCIES}: downstreamtest1.xml
	marshal downstreamtest1.xml

                 ###########################################

downstreamtest2_DEPENDENCIES = Downstreamtest2Marshal.cc Downstreamtest2Marshal.h

downstreamtest2_SOURCES = ${TUPLESOURCE} ${downstreamtest2_DEPENDENCIES}  \
                          downstreamtest2.cc

${downstreamtest2_DEPENDENCIES}: downstreamtest2.xml
	marshal downstreamtest2.xml

                 ###########################################

downstreamtest3_DEPENDENCIES = Downstreamtest3Marshal.cc Downstreamtest3Marshal.h

downstreamtest3_SOURCES = ${TUPLESOURCE} ${downstreamtest3_DEPENDENCIES}  \
                      downstreamtest3.cc

${downstreamtest3_DEPENDENCIES}: downstreamtest3.xml
	marshal downstreamtest3.xml

                 ###########################################

downstreamtest4_DEPENDENCIES = Downstreamtest4Marshal.cc Downstreamtest4Marshal.h

downstreamtest4_SOURCES = ${TUPLESOURCE} ${downstreamtest4_DEPENDENCIES}  \
                      downstreamtest4.cc

${downstreamtest4_DEPENDENCIES}: downstreamtest4.xml
	marshal downstreamtest4.xml

                 ###########################################

downstreamtest5_DEPENDENCIES = Downstreamtest5Marshal.cc Downstreamtest5Marshal.h

downstreamtest5_SOURCES = ${TUPLESOURCE} ${downstreamtest5_DEPENDENCIES}  \
                      downstreamtest5.cc

${downstreamtest5_DEPENDENCIES}: downstreamtest5.xml
	marshal downstreamtest5.xml

                 ###########################################

downstreamtest7_DEPENDENCIES = Downstreamtest7Marshal.cc Downstreamtest7Marshal.h

downstreamtest7_SOURCES = ${TUPLESOURCE} ${downstreamtest7_DEPENDENCIES}  \
                      downstreamtest7.cc

${downstreamtest7_DEPENDENCIES}: downstreamtest7.xml
	marshal downstreamtest7.xml

                 ###########################################

downstreamtest6_DEPENDENCIES = Downstreamtest6Marshal.cc Downstreamtest6Marshal.h

downstreamtest6_SOURCES = ${TUPLESOURCE} ${downstreamtest6_DEPENDENCIES}  \
                      downstreamtest6.cc

${downstreamtest6_DEPENDENCIES}: downstreamtest6.xml
	marshal downstreamtest6.xml

                 ###########################################

upstreamtest6_DEPENDENCIES = Upstreamtest6Marshal.cc Upstreamtest6Marshal.h

upstreamtest6_SOURCES = ${TUPLESOURCE} ${upstreamtest6_DEPENDENCIES}  \
                      upstreamtest6.cc

${upstreamtest6_DEPENDENCIES}: upstreamtest6.xml
	marshal upstreamtest6.xml

                 ###########################################

upstreamtest1_DEPENDENCIES = Upstreamtest1Marshal.cc Upstreamtest1Marshal.h

upstreamtest1_SOURCES = ${TUPLESOURCE} ${upstreamtest1_DEPENDENCIES}  \
                      upstreamtest1.cc

${upstreamtest1_DEPENDENCIES}: upstreamtest1.xml
	marshal upstreamtest1.xml

                 ###########################################

upstreamtest2_DEPENDENCIES = Upstreamtest2Marshal.cc Upstreamtest2Marshal.h

upstreamtest2_SOURCES = ${TUPLESOURCE} ${upstreamtest2_DEPENDENCIES}  \
                      upstreamtest2.cc

${upstreamtest2_DEPENDENCIES}: upstreamtest2.xml
	marshal upstreamtest2.xml

                 ###########################################

upstreamtest3_DEPENDENCIES = Upstreamtest3Marshal.cc Upstreamtest3Marshal.h

upstreamtest3_SOURCES = ${TUPLESOURCE} ${upstreamtest3_DEPENDENCIES}  \
                      upstreamtest3.cc

${upstreamtest3_DEPENDENCIES}: upstreamtest3.xml
	marshal upstreamtest3.xml

                 ###########################################

upstreamtest4_DEPENDENCIES = Upstreamtest4Marshal.cc Upstreamtest4Marshal.h

upstreamtest4_SOURCES = ${TUPLESOURCE} ${upstreamtest4_DEPENDENCIES}  \
                      upstreamtest4.cc

${upstreamtest4_DEPENDENCIES}: upstreamtest4.xml
	marshal upstreamtest4.xml

                 ###########################################

upstreamtest5_DEPENDENCIES = Upstreamtest5Marshal.cc Upstreamtest5Marshal.h

upstreamtest5_SOURCES = ${TUPLESOURCE} ${upstreamtest5_DEPENDENCIES}  \
                      upstreamtest5.cc

${upstreamtest5_DEPENDENCIES}: upstreamtest5.xml
	marshal upstreamtest5.xml

                 ###########################################

upstreamtest7_DEPENDENCIES = Upstreamtest7Marshal.cc Upstreamtest7Marshal.h

upstreamtest7_SOURCES = ${TUPLESOURCE} ${upstreamtest7_DEPENDENCIES}  \
                      upstreamtest7.cc

${upstreamtest7_DEPENDENCIES}: upstreamtest7.xml
	marshal upstreamtest7.xml

                 ###########################################

# experiment_up_DEPENDENCIES = Experiment_UpMarshal.cc Experiment_UpMarshal.h

# experiment_up_SOURCES = ${TUPLESOURCE} ${experiment_up_DEPENDENCIES}  \
#                     experiment_up.cc

# ${experiment_up_DEPENDENCIES}: experiment_up.xml
#   marshal experiment_up.xml

                 ###########################################

# experiment_down_DEPENDENCIES = Experiment_DownMarshal.cc Experiment_DownMarshal.h

# experiment_down_SOURCES = ${TDOWNLESOURCE} ${experiment_down_DEPENDENCIES}  \
#                     experiment_down.cc

# ${experiment_down_DEPENDENCIES}: experiment_down.xml
#   marshal experiment_down.xml

                 ###########################################


CLEANFILES =    ${upstreamdemo1_DEPENDENCIES}       \
            ${upstreamdemo2_DEPENDENCIES}       \
            ${upstreamtest1_DEPENDENCIES}       \
            ${upstreamtest2_DEPENDENCIES}       \
        ${upstreamtest3_DEPENDENCIES}       \
        ${upstreamtest4_DEPENDENCIES}       \
        ${upstreamtest5_DEPENDENCIES}       \
        ${upstreamtest6_DEPENDENCIES}       \
        ${upstreamtest7_DEPENDENCIES}       \
        ${downstreamtest1_DEPENDENCIES}     \
        ${downstreamtest2_DEPENDENCIES}     \
        ${downstreamtest3_DEPENDENCIES}     \
        ${downstreamtest4_DEPENDENCIES}     \
        ${downstreamtest5_DEPENDENCIES}     \
        ${downstreamtest6_DEPENDENCIES}     \
        ${downstreamtest7_DEPENDENCIES}     

#           ${downstreamdemo1_DEPENDENCIES}         \
#           ${downstreamdemo2_DEPENDENCIES}         \
#       ${experiment_up_DEPENDENCIES}       \
#       ${experiment_down_DEPENDENCIES}

##############################  end Makefile.am  ###############################
