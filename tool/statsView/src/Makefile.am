java_JAVA = \
	monitor/MonitorFrameAboutBox.class \
	monitor/MonitorFrame.class \
	monitor/Monitor.class \
	monitor/MonitorPanel.class \
	monitor.jar   

%.class: %.java
	javac -classpath .:../../../utility/client/java/lib/medusaXmlRpc.jar $<

../lib:
	mkdir ../lib

monitor.jar: ../lib
	rm -rf ../lib/monitor.jar;cp ../../../utility/client/java/lib/medusaXmlRpc.jar ../lib/monitor.jar;jar uvf ../lib/monitor.jar monitor/*.class monitor/*.png

classjava.stamp: $(java_JAVA)
	echo timestamp > classjava.stamp	

CLEANFILES = monitor/*.class ../lib/*.jar
