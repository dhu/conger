all = SimpleGUI.jar

CLASS_FILES1 = \
	OutputMonitorAbout.class \
	OutputMonitorPanel.class \
	DelayMonitorPanel.class \
	MultiPanel.class \
	OutputMonitorFrame.class \
	SimpleGUI.class

CLASS_FILES2 = \
	SimpleGUI2.class \
	InputMonitorFrame.class \
	InputMonitorPanel.class

java_JAVA = \
	${CLASS_FILES1} \
	${CLASS_FILES2} \
	SimpleGUI.jar


%.class: %.java
	rm -rf *.jar
	javac -classpath . $<

SimpleGUI.jar:
	jar cvf SimpleGUI.jar *.class *.png	

classjava.stamp: $(java_JAVA)
	echo timestamp > classjava.stamp	

CLEANFILES = *.class *.jar
