MYFLAGS =
AM_CFLAGS = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = ha_demo

bin_PROGRAMS = $(MAIN)

LDADD_EXTRAS = -ldl  @XERCESCLIB@

LDADD = -L../@BOREALISROOT@/modules/common  -lborealiscommon      \
        -L../@BOREALISROOT@/modules/util    -lborealisutil        \
        @NMSTLLIB@ $(LDADD_EXTRAS)

INCLUDES = @XERCESCINCLUDE@ @NMSTLINCLUDE@           \
           -I ../@BOREALISROOT@/modules/common       \
           -I ../@BOREALISROOT@/modules/queryProcessor/statsMgr \
           -I ../@BOREALISROOT@/modules/queryProcessor \
           -I ../@BOREALISROOT@/modules/util         \
           -I ../@BOREALISROOT@/modules/rpc          \
           -I ../@BOREALISROOT@/modules/catalog

DEPENDENCIES = ../@BOREALISROOT@/modules/common/libborealiscommon.la

ha_demo_DEPENDENCIES = Ha_DemoMarshal.cc  Ha_DemoMarshal.h

ha_demo_SOURCES = ${ha_demo_DEPENDENCIES} ha_demo.cc

${ha_demo_DEPENDENCIES}:  ha_demo.xml ha_demo_deployment.xml
	marshal ha_demo.xml ha_demo_deployment.xml

CLEANFILES = ${ha_demo_DEPENDENCIES} *.class 

java_JAVA = PacketSchema.class CountSchema.class

%.class: %.java
	javac -classpath .:../../utility/client/java/lib/clientApi.jar $<

classjava.stamp: $(java_JAVA)
	echo timestamp > classjava.stamp 
