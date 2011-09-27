###################### updates test

MYFLAGS =
AM_CFLAGS = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = append_queue         \
       append_queue_tumble  \
       append_queue_slide   \
       ls                   \
       ls_tumble            \
       ls_slide             \
       update_queue0        \
       update_queue_tumble0 \
       update_queue_slide0  \
       update_queue1        \
       update_queue_tumble1 \
       update_queue_slide1  \
       update_queue2        \
       update_queue_tumble2 \
       update_queue_slide2  \
       update_queue3        \
       update_queue_tumble3 \
       update_queue_slide3  \
       update_queue4        \
       update_queue_tumble4 \
       update_queue_slide4  \
       update_queue5        \
       update_queue_tumble5 \
       update_queue_slide5  \
       update_queue6        \
       update_queue7 

bin_PROGRAMS = $(MAIN)

BOREALIS_SRC = ../../@BOREALISROOT@

LDADD_MODULES = -L$(BOREALIS_SRC)/modules/common          -lborealiscommon   \
                -L$(BOREALIS_SRC)/modules/catalog         -lborealiscatalog  \
                -L$(BOREALIS_SRC)/modules/util            -lborealisutil     \
                -L$(BOREALIS_SRC)/modules/ha              -lborealisha       \
                -L$(BOREALIS_SRC)/modules/queryProcessor  -lborealisqp       \
                -L$(BOREALIS_SRC)/external                -lborealisexternal

LDADD_EXTRAS = -ldl  @BOREALISLIB@  @EXTERNALLIB@

LDADD = $(LDADD_MODULES) $(LDADD_EXTRAS)

INCLUDES = @XERCESCINCLUDE@   @NMSTLINCLUDE@              \
           -I $(BOREALIS_SRC)/modules/common              \
           -I $(BOREALIS_SRC)/modules/util                \
           -I $(BOREALIS_SRC)/modules/rpc                 \
           -I $(BOREALIS_SRC)/modules/catalog             \
           -I $(BOREALIS_SRC)/modules/queryProcessor      \
           -I $(BOREALIS_SRC)/modules/queryProcessor/statsMgr 

DEPENDENCIES = $(BOREALIS_SRC)/modules/common/libborealiscommon.la

 ##############################################################################

append_queue_DEPENDENCIES = Append_QueueMarshal.cc \
                            Append_QueueMarshal.h

append_queue_SOURCES = ${append_queue_DEPENDENCIES} \
                       append_queue.cc

${append_queue_DEPENDENCIES}: append_queue.xml deploy.xml
	marshal append_queue.xml deploy.xml

append_queue_tumble_DEPENDENCIES = Append_Queue_TumbleMarshal.cc \
                                   Append_Queue_TumbleMarshal.h

append_queue_tumble_SOURCES = ${append_queue_tumble_DEPENDENCIES} \
                              append_queue_tumble.cc

${append_queue_tumble_DEPENDENCIES}: append_queue_tumble.xml deploy.xml
	marshal append_queue_tumble.xml deploy.xml

append_queue_slide_DEPENDENCIES = Append_Queue_SlideMarshal.cc \
                                  Append_Queue_SlideMarshal.h

append_queue_slide_SOURCES = ${append_queue_slide_DEPENDENCIES} \
                             append_queue_slide.cc

${append_queue_slide_DEPENDENCIES}: append_queue_slide.xml deploy.xml
	marshal append_queue_slide.xml deploy.xml

 ##############################################################################

ls_DEPENDENCIES = LsMarshal.cc \
                  LsMarshal.h

ls_SOURCES = ${ls_DEPENDENCIES} \
             ls.cc

${ls_DEPENDENCIES}: ls.xml deploy.xml
	marshal ls.xml deploy.xml

ls_tumble_DEPENDENCIES = Ls_TumbleMarshal.cc \
                         Ls_TumbleMarshal.h

ls_tumble_SOURCES = ${ls_tumble_DEPENDENCIES} \
                    ls_tumble.cc

${ls_tumble_DEPENDENCIES}: ls_tumble.xml deploy.xml
	marshal ls_tumble.xml deploy.xml

ls_slide_DEPENDENCIES = Ls_SlideMarshal.cc \
                        Ls_SlideMarshal.h

ls_slide_SOURCES = ${ls_slide_DEPENDENCIES} \
                   ls_slide.cc

${ls_slide_DEPENDENCIES}: ls_slide.xml deploy.xml
	marshal ls_slide.xml deploy.xml

 ##############################################################################

update_queue0_DEPENDENCIES = Update_Queue0Marshal.cc \
                             Update_Queue0Marshal.h

update_queue0_SOURCES = ${update_queue0_DEPENDENCIES} \
                        update_queue0.cc

${update_queue0_DEPENDENCIES}: update_queue0.xml deploy.xml
	marshal update_queue0.xml deploy.xml

update_queue_tumble0_DEPENDENCIES = Update_Queue_Tumble0Marshal.cc \
                                    Update_Queue_Tumble0Marshal.h

update_queue_tumble0_SOURCES = ${update_queue_tumble0_DEPENDENCIES} \
                               update_queue_tumble0.cc

${update_queue_tumble0_DEPENDENCIES}: update_queue_tumble0.xml deploy.xml
	marshal update_queue_tumble0.xml deploy.xml

update_queue_slide0_DEPENDENCIES = Update_Queue_Slide0Marshal.cc \
                                   Update_Queue_Slide0Marshal.h

update_queue_slide0_SOURCES = ${update_queue_slide0_DEPENDENCIES} \
                              update_queue_slide0.cc

${update_queue_slide0_DEPENDENCIES}: update_queue_slide0.xml deploy.xml
	marshal update_queue_slide0.xml deploy.xml

 ##############################################################################

update_queue1_DEPENDENCIES = Update_Queue1Marshal.cc \
                             Update_Queue1Marshal.h

update_queue1_SOURCES = ${update_queue1_DEPENDENCIES} \
                        update_queue1.cc

${update_queue1_DEPENDENCIES}: update_queue1.xml deploy.xml
	marshal update_queue1.xml deploy.xml

update_queue_tumble1_DEPENDENCIES = Update_Queue_Tumble1Marshal.cc \
                                    Update_Queue_Tumble1Marshal.h

update_queue_tumble1_SOURCES = ${update_queue_tumble1_DEPENDENCIES} \
                               update_queue_tumble1.cc

${update_queue_tumble1_DEPENDENCIES}: update_queue_tumble1.xml deploy.xml
	marshal update_queue_tumble1.xml deploy.xml

update_queue_slide1_DEPENDENCIES = Update_Queue_Slide1Marshal.cc \
                                   Update_Queue_Slide1Marshal.h

update_queue_slide1_SOURCES = ${update_queue_slide1_DEPENDENCIES} \
                              update_queue_slide1.cc

${update_queue_slide1_DEPENDENCIES}: update_queue_slide1.xml deploy.xml
	marshal update_queue_slide1.xml deploy.xml

 ##############################################################################

update_queue2_DEPENDENCIES = Update_Queue2Marshal.cc \
                             Update_Queue2Marshal.h

update_queue2_SOURCES = ${update_queue2_DEPENDENCIES} \
                        update_queue2.cc

${update_queue2_DEPENDENCIES}: update_queue2.xml deploy.xml
	marshal update_queue2.xml deploy.xml

update_queue_tumble2_DEPENDENCIES = Update_Queue_Tumble2Marshal.cc \
                                    Update_Queue_Tumble2Marshal.h

update_queue_tumble2_SOURCES = ${update_queue_tumble2_DEPENDENCIES} \
                               update_queue_tumble2.cc

${update_queue_tumble2_DEPENDENCIES}: update_queue_tumble2.xml deploy.xml
	marshal update_queue_tumble2.xml deploy.xml

update_queue_slide2_DEPENDENCIES = Update_Queue_Slide2Marshal.cc \
                                   Update_Queue_Slide2Marshal.h

update_queue_slide2_SOURCES = ${update_queue_slide2_DEPENDENCIES} \
                              update_queue_slide2.cc

${update_queue_slide2_DEPENDENCIES}: update_queue_slide2.xml deploy.xml
	marshal update_queue_slide2.xml deploy.xml

 ##############################################################################

update_queue3_DEPENDENCIES = Update_Queue3Marshal.cc \
                             Update_Queue3Marshal.h

update_queue3_SOURCES = ${update_queue3_DEPENDENCIES} \
                        update_queue3.cc

${update_queue3_DEPENDENCIES}: update_queue3.xml deploy.xml
	marshal update_queue3.xml deploy.xml

update_queue_tumble3_DEPENDENCIES = Update_Queue_Tumble3Marshal.cc \
                                    Update_Queue_Tumble3Marshal.h

update_queue_tumble3_SOURCES = ${update_queue_tumble3_DEPENDENCIES} \
                               update_queue_tumble3.cc

${update_queue_tumble3_DEPENDENCIES}: update_queue_tumble3.xml deploy.xml
	marshal update_queue_tumble3.xml deploy.xml

update_queue_slide3_DEPENDENCIES = Update_Queue_Slide3Marshal.cc \
                                   Update_Queue_Slide3Marshal.h

update_queue_slide3_SOURCES = ${update_queue_slide3_DEPENDENCIES} \
                              update_queue_slide3.cc

${update_queue_slide3_DEPENDENCIES}: update_queue_slide3.xml deploy.xml
	marshal update_queue_slide3.xml deploy.xml

 ##############################################################################

update_queue4_DEPENDENCIES = Update_Queue4Marshal.cc \
                             Update_Queue4Marshal.h

update_queue4_SOURCES = ${update_queue4_DEPENDENCIES} \
                        update_queue4.cc

${update_queue4_DEPENDENCIES}: update_queue4.xml deploy.xml
	marshal update_queue4.xml deploy.xml

update_queue_tumble4_DEPENDENCIES = Update_Queue_Tumble4Marshal.cc \
                                    Update_Queue_Tumble4Marshal.h

update_queue_tumble4_SOURCES = ${update_queue_tumble4_DEPENDENCIES} \
                               update_queue_tumble4.cc

${update_queue_tumble4_DEPENDENCIES}: update_queue_tumble4.xml deploy.xml
	marshal update_queue_tumble4.xml deploy.xml

update_queue_slide4_DEPENDENCIES = Update_Queue_Slide4Marshal.cc \
                                   Update_Queue_Slide4Marshal.h

update_queue_slide4_SOURCES = ${update_queue_slide4_DEPENDENCIES} \
                              update_queue_slide4.cc

${update_queue_slide4_DEPENDENCIES}: update_queue_slide4.xml deploy.xml
	marshal update_queue_slide4.xml deploy.xml

 ##############################################################################

update_queue5_DEPENDENCIES = Update_Queue5Marshal.cc \
                             Update_Queue5Marshal.h

update_queue5_SOURCES = ${update_queue5_DEPENDENCIES} \
                        update_queue5.cc

${update_queue5_DEPENDENCIES}: update_queue5.xml deploy.xml
	marshal update_queue5.xml deploy.xml

update_queue_tumble5_DEPENDENCIES = Update_Queue_Tumble5Marshal.cc \
                                    Update_Queue_Tumble5Marshal.h

update_queue_tumble5_SOURCES = ${update_queue_tumble5_DEPENDENCIES} \
                               update_queue_tumble5.cc

${update_queue_tumble5_DEPENDENCIES}: update_queue_tumble5.xml deploy.xml
	marshal update_queue_tumble5.xml deploy.xml

update_queue_slide5_DEPENDENCIES = Update_Queue_Slide5Marshal.cc \
                                   Update_Queue_Slide5Marshal.h

update_queue_slide5_SOURCES = ${update_queue_slide5_DEPENDENCIES} \
                              update_queue_slide5.cc

${update_queue_slide5_DEPENDENCIES}: update_queue_slide5.xml deploy.xml
	marshal update_queue_slide5.xml deploy.xml

 ##############################################################################

update_queue6_DEPENDENCIES = Update_Queue6Marshal.cc \
                             Update_Queue6Marshal.h

update_queue6_SOURCES = ${update_queue6_DEPENDENCIES} \
                        update_queue6.cc

${update_queue6_DEPENDENCIES}: update_queue6.xml deploy.xml
	marshal update_queue6.xml deploy.xml

 ##############################################################################

update_queue7_DEPENDENCIES = Update_Queue7Marshal.cc \
                             Update_Queue7Marshal.h

update_queue7_SOURCES = ${update_queue7_DEPENDENCIES} \
                        update_queue7.cc

${update_queue7_DEPENDENCIES}: update_queue7.xml deploy.xml
	marshal update_queue7.xml deploy.xml

 ##############################################################################

CLEANFILES = ${append_queue_DEPENDENCIES} \
             ${append_queue_tumble_DEPENDENCIES} \
             ${append_queue_slide_DEPENDENCIES} \
             ${ls_DEPENDENCIES} \
             ${ls_tumble_DEPENDENCIES} \
             ${ls_slide_DEPENDENCIES} \
             ${update_queue0_DEPENDENCIES} \
             ${update_queue_tumble0_DEPENDENCIES} \
             ${update_queue_slide0_DEPENDENCIES} \
             ${update_queue1_DEPENDENCIES} \
             ${update_queue_tumble1_DEPENDENCIES} \
             ${update_queue_slide1_DEPENDENCIES} \
             ${update_queue2_DEPENDENCIES} \
             ${update_queue_tumble2_DEPENDENCIES} \
             ${update_queue_slide2_DEPENDENCIES} \
             ${update_queue3_DEPENDENCIES} \
             ${update_queue_tumble3_DEPENDENCIES} \
             ${update_queue_slide3_DEPENDENCIES} \
             ${update_queue4_DEPENDENCIES} \
             ${update_queue_tumble4_DEPENDENCIES} \
             ${update_queue_slide4_DEPENDENCIES} \
             ${update_queue5_DEPENDENCIES} \
             ${update_queue_tumble5_DEPENDENCIES} \
             ${update_queue_slide5_DEPENDENCIES} \
             ${update_queue6_DEPENDENCIES} \
             ${update_queue7_DEPENDENCIES}

 #############################  end Makefile.am  ##############################
