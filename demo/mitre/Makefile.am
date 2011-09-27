MYFLAGS =
AM_CFLAGS = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = mitre_demo

bin_PROGRAMS = $(MAIN)

LDADD_EXTRAS = -ldl  @XERCESCLIB@

LDADD = -L../@BOREALISROOT@/modules/common  -lborealiscommon      \
        -L../@BOREALISROOT@/modules/util    -lborealisutil        \
        @NMSTLLIB@ $(LDADD_EXTRAS)

INCLUDES = @XERCESCINCLUDE@   @NMSTLINCLUDE@                      \
           -I ../@BOREALISROOT@/modules/common                    \
           -I ../@BOREALISROOT@/modules/queryProcessor            \
           -I ../@BOREALISROOT@/modules/queryProcessor/statsMgr \
           -I ../@BOREALISROOT@/modules/util                      \
           -I ../@BOREALISROOT@/modules/rpc                       \
           -I ../@BOREALISROOT@/modules/catalog

DEPENDENCIES = ../@BOREALISROOT@/modules/common/libborealiscommon.la

mitre_demo_DEPENDENCIES = Mitre_DemoMarshal.cc  Mitre_DemoMarshal.h

mitre_demo_SOURCES = ${mitre_demo_DEPENDENCIES} mitre_demo.cc

${mitre_demo_DEPENDENCIES}:  mitre_demo.xml mitre_demo_deployment.xml
	marshal mitre_demo.xml mitre_demo_deployment.xml

java_JAVA = \
   Visualizer.class \
   visualizer.jar   

%.class: %.java
	javac -classpath ../../utility/client/java/lib/clientApi.jar:. $<

visualizer.jar:
	jar cvf visualizer.jar *.class; rm -rf *.class

classjava.stamp: $(java_JAVA)
	echo timestamp > classjava.stamp 

%.class: %.java
	javac -classpath ../../utility/client/java/lib/clientApi.jar:. $<

visualizer.jar:
	rm -rf *.jar;cp ../../utility/client/java/lib/clientApi.jar visualizer.jar;jar uvf visualizer.jar *.class; rm -rf *.class

classjava.stamp: $(java_JAVA)
	echo timestamp > classjava.stamp 

CLEANFILES = ${mitre_demo_DEPENDENCIES} *.class *.jar
