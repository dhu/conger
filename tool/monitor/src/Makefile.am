java_JAVA = monitor/InputRatePanel.class          \
            monitor/MonitorFrame_AboutBox.class   \
            monitor/DelayPanel.class              \
            monitor/Monitor.class                 \
            monitor/LayoutPanel.class             \
            monitor/MonitorFrame.class            \
            monitor/StatisticsCollector.class     \
            monitor.jar

%.class: %.java
	javac -classpath  .:../../../utility/client/java/lib/medusaXmlRpc.jar:../../../utility/client/java/lib/clientApi.jar:../lib/xerces.jar $<

../lib:
	mkdir ../lib

monitor.jar: ../lib
	rm -rf ../lib/monitor.jar
	cp ../../../utility/client/java/lib/medusaXmlRpc.jar ../lib/monitor.jar
	jar uvf ../lib/monitor.jar  monitor/*.class  monitor/*.png

classjava.stamp: $(java_JAVA)
	echo timestamp > classjava.stamp

CLEANFILES = monitor/*.class  ../lib/monitor.jar
