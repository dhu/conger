package monitor;

import java.util.*;
import javax.swing.*;
import java.awt.*;
import medusaXmlRpc.*;
import clientApi.*;
import org.apache.xerces.parsers.DOMParser;


public class StatisticsCollector
    extends Thread {

  static boolean no_dups = false;
  static long warmUpTime = 0;

  static long inputGenerationInterval = 100;

  static int numberOfSections = 5;
  static int numberOfColumnsPerSection = 20;
  static int nTupleColumns = numberOfColumnsPerSection * numberOfSections;
  static int nTupleRows = 10;
  static int bucketSize = nTupleColumns * nTupleRows;

  static int numberOfSamples = 60; // 60 samples to constitute a monitoring window.

  /**
   * The sampling interval.
   */
  static public int updateInterval = 1000; // 1 second.

  static public int monitoringTimeSpan = updateInterval * numberOfSamples;

  /**
   * Constant used for conversions from Bps to Kbps.
   */
  private static final double Bps_to_Kbps = 8.0 / 1000;

  /**
   * The name of the rpc used to get the statistics of a borealis server.
   */
  static final String METHOD_STATS = "QueryProcessor.get_stats";

  static int INDEX_INPUT_RATE = 2;

  static final String METHOD_ADMIN_STATUS = "QueryProcessor.get_status";

  static int INDEX_IS_PRIMARY = 1;

  static int INDEX_HA_METHOD = 2;

  static int INDEX_PARTNER = 3;

  static int INDEX_QUERY_XMLS = 4;

  static int INDEX_SUBSCRIPTION_XML = 5;

  static final String METHOD_CRASH = "QueryProcessor.crash";

  static int NO_HA = 0;
  static int AMNESIA = 1;
  static int UPSTREAM_BACKUP = 2;
  static int PASSIVE_STANDBY = 3;
  static int ACTIVE_STANDBY = 4;

  String HAMethods[] = {
      "No HA",
      "Amnesia",
      "Upstream Backup",
      "Passive Standby",
      "Active Standby"
  };

  /**
   * The two-dimensional layout of the nodes.
   */
  Vector nodeColumns = new Vector();

  Hashtable nodes = new Hashtable();

  /**
   * The panels associated with this StatisticsCollector.
   */
  Vector panels = new Vector();

  int nRows = -1;

  int nCols = -1;

  /**
   * The cursor.
   */
  int cursor = 0;

  Vector inputStreamInfo = new Vector();
  Vector outputStreamInfo = new Vector();

  OutputStreamInfo findOutputStreamInfo(String name) {
    OutputStreamInfo output;
    for (int i = 0; i < outputStreamInfo.size(); i++) {
      output = (OutputStreamInfo) (outputStreamInfo.elementAt(i));
      if (output.name.equals(name)) {
        return output;
      }
    }
    return null;
  }

  public static String getCanonicalDescription(String node) {
    try {
      String ip = node.substring(0, node.indexOf(':'));
      ip = Marshal.getHostIpAddress(ip);
      int port = Integer.parseInt(node.substring(node.indexOf(':')+1));
      return ip + ":" + port;
    } catch (Exception e) {}
    return node;
  }

/**
   * Constructs a StatisticsCollector instance.
   * @param xmlFile the xml configuration file.
   * @throws java.lang.Exception if an error occurs.
   */
  public StatisticsCollector(String xmlFile, JTabbedPane jTabbedPane1) throws Exception {
    DOMParser parser = new DOMParser();
    parser.parse(xmlFile);
    org.w3c.dom.Document doc = parser.getDocument();

    // output streams
    org.w3c.dom.NodeList outputStreams = doc.getElementsByTagName("output");
    for (int i = 0; i < outputStreams.getLength(); i++) {
      org.w3c.dom.Node outputStream = ( (org.w3c.dom.Node) (outputStreams.item(
          i)));
      org.w3c.dom.NamedNodeMap attrs = outputStream.getAttributes();
      String name = attrs.getNamedItem("stream").getNodeValue();
      String className = attrs.getNamedItem("schema").getNodeValue();
      String node = attrs.getNamedItem("node").getNodeValue();
      outputStreamInfo.add(new OutputStreamInfo(name, className, node));
    }

    // layout
    org.w3c.dom.NodeList columns = doc.getElementsByTagName("layout").item(0).
        getChildNodes();
    for (int i = 0; i < columns.getLength(); i++) {
      org.w3c.dom.Node column = columns.item(i);
      if ("column".equals(column.getLocalName())) {
        Vector nodeColumn = new Vector();
        nodeColumns.add(nodeColumn);
        org.w3c.dom.NodeList nodeList = column.getChildNodes();
        for (int j = 0; j < nodeList.getLength(); j++) {
          org.w3c.dom.Node domNode = nodeList.item(j);
          if ("node".equals(domNode.getLocalName())) {
            String name = domNode.getChildNodes().item(0).getNodeValue();
            name = name.substring(1, name.length() - 1);
            Node node = new Node(name);
            nodeColumn.add(node);
            nodes.put(node.toString(), node);
          }
        }
      }
    }
    nCols = nodeColumns.size();
    for (int i = 0; i < nCols; i++) {
      int n = ( (Vector) (nodeColumns.elementAt(i))).size();
      if (nRows < n) {
        nRows = n;
      }
    }
    // input streams
    org.w3c.dom.NodeList inputStreams = doc.getElementsByTagName("input");
    for (int i = 0; i < inputStreams.getLength(); i++) {
      org.w3c.dom.Node inputStream = ( (org.w3c.dom.Node) (inputStreams.item(i)));
      org.w3c.dom.NamedNodeMap attrs = inputStream.getAttributes();
      String name = attrs.getNamedItem("stream").getNodeValue();
      String className = attrs.getNamedItem("schema").getNodeValue();
      String node = attrs.getNamedItem("node").getNodeValue();
      if ("yes".equals(attrs.getNamedItem("generate").getNodeValue()))
        inputStreamInfo.add(new InputStreamInfo(name, className, node, true));
      else
        inputStreamInfo.add(new InputStreamInfo(name, className, node, false));
    }

    // gcc version
    org.w3c.dom.Node gcc = doc.getElementsByTagName("gcc").item(0);
    String gccVersion = gcc.getAttributes().getNamedItem("version").
        getNodeValue();
    if (gccVersion != null && gccVersion.equals("3.3.5")) {
      MonitorMarshal.shortRepresentation = false;
    }
    else {
      MonitorMarshal.shortRepresentation = true;
    }

    try {
        // warm up time - delay before connecting to the input/output streams
        org.w3c.dom.Node warmup = doc.getElementsByTagName("warmup").item(0);
        String time = warmup.getAttributes().getNamedItem("time").getNodeValue();
        warmUpTime = Integer.parseInt(time);
    } catch (Exception e) {}
    try {
        // warm up time - delay before connecting to the input/output streams
        org.w3c.dom.Node warmup = doc.getElementsByTagName("no-dups").item(0);
        String value = warmup.getAttributes().getNamedItem("value").getNodeValue();
        if (value.equals("yes"))
            no_dups = true;
    } catch (Exception e) {}

    int myPort = 60000;
    try {
    // my port
    org.w3c.dom.Node receiver = doc.getElementsByTagName("receiver").item(0);
    String port = receiver.getAttributes().getNamedItem("port").getNodeValue();
    myPort = Integer.parseInt(port);
    } catch (Exception e) {}

    // monitors
    org.w3c.dom.NodeList monitors = doc.getElementsByTagName("monitor");
    for (int i = 0; i < monitors.getLength(); i++) {
      org.w3c.dom.Node monitor = ( (org.w3c.dom.Node) (monitors.item(i)));
      String name = monitor.getChildNodes().item(0).getNodeValue();
      name = name.substring(1, name.length() - 1);
      if (name.equals("delay")) {
        JPanel jDelayPanel = new DelayPanel(this);
        registerPanel(jDelayPanel);
        jTabbedPane1.add(jDelayPanel, "delays (output streams)");
      } else if (name.equals("input rate")) {
        JPanel jInputRatePanel = new InputRatePanel(this);
        registerPanel(jInputRatePanel);
        jTabbedPane1.add(jInputRatePanel, "input rates (nodes)");
      }
    }


    start();
    MonitorMarshal marshal = new MonitorMarshal(myPort,
                                                new
                                                MonitorTupleReceptionHandler());
  }

  public synchronized void registerPanel(JPanel panel) {
    panels.add(panel);
  }

  public void crashNodes() {
    for (int i = 0; i < nCols; i++) {
      Vector column = ( (Vector) (nodeColumns.elementAt(i)));
      int n = column.size();
      for (int j = 0; j < n; j++) {
        Node node = (Node) (column.elementAt(j));
        try {
          if (node.setToCrash) {
            node.crash();
          }
        }
        catch (Exception e) {
        }
      }
    }
  }

  public double getMaxInputRate() {
    double maxRate = -1;
    for (int i = 0; i < nCols; i++) {
      Vector column = ( (Vector) (nodeColumns.elementAt(i)));
      int n = column.size();
      for (int j = 0; j < n; j++) {
        double rate = ( (Node) (column.elementAt(j))).getMaxInputRate();
        if (maxRate < rate) {
          maxRate = rate;
        }
      }
    }
    return maxRate;
  }

  /**
   * Starts monitoring the list of Borealis nodes periodically
   * In each period, retrieves the load statistics from each Borealis node
   * @param nodes the set of Borealis nodes to monitor
   */
  public void run() {
    while (true) {
      try {
        synchronized (this) {
          Enumeration e = nodes.elements();
          while (e.hasMoreElements()) {
            ( (Node) e.nextElement()).initialize();
          }
          e = outputStreamInfo.elements();
          while (e.hasMoreElements()) {
            ( (OutputStreamInfo) e.nextElement()).initialize();
          }
          for (int i = 0; i < nCols; i++) {
            Vector column = ( (Vector) (nodeColumns.elementAt(i)));
            int n = column.size();
            for (int j = 0; j < n; j++) {
              ( (Node) (column.elementAt(j))).updateStatistics();
            }
          }
          cursor++;
          cursor %= numberOfSamples;
          updatePanels();
        }
        sleep(updateInterval);
      }
      catch (Exception e) {
        e.printStackTrace();
      }
    }
  }

  public synchronized void updatePanels() {
    for (int i = 0; i < panels.size(); i++) {
      ( (JPanel) (panels.elementAt(i))).repaint();
    }
  }

  class StreamInfo {
    String name;
    Class tupleClass;
    String node;

    StreamInfo(String name, String className, String node) throws Exception {
      this.name = name;
      tupleClass = Class.forName(className);
      this.node = getCanonicalDescription(node);
    }
    
    public Class getTupleClass() {
        return tupleClass;
    }

    public String toString() {
      return name;
    }
  }

  public long getMaxDelay() {
    long maxDelay = 0;
    Enumeration e = outputStreamInfo.elements();
    while (e.hasMoreElements()) {
      long delay = ( (OutputStreamInfo) e.nextElement()).getMaxDelay();
      if (maxDelay < delay) {
        maxDelay = delay;
      }
    }
    return maxDelay;
  }

  public long getOldestTime() {
    long oldtime = 0;
    for (int i = 0; i < outputStreamInfo.size(); i++) {
      long time = ( (OutputStreamInfo) outputStreamInfo.elementAt(i)).
          getOldestTime();
      if (i == 0) {
        oldtime = time;
      }
      else if (time > oldtime) {
        oldtime = time;
      }
    }
    return oldtime;
  }

  public class InputStreamInfo
      extends StreamInfo {
    boolean generate;

    InputStreamInfo(String name, String className, String node, boolean generate) throws
        Exception {
      super(name, className, node);
      this.generate = generate;
    }
  }

  public class OutputStreamInfo
      extends StreamInfo {

    int cursor = 0;

    int cursor2;
    int[] counts;
    Color[] colors;
    int currentSequenceNumber;

    boolean isDisplayOn = true;

    Rectangle rectangle;

    Color color;

    long[] delays = new long[bucketSize];
    long[] timestamps = new long[bucketSize];
    Vector subscriptions = new Vector();

    void initialize() {
      subscriptions.clear(); ;
    }

    public OutputStreamInfo(String name, String className, String node) throws
        Exception {
      super(name, className, node);
      for (int j = 0; j < bucketSize; j++) {
        delays[j] = -1;
        timestamps[j] = -1;
      }

      counts = new int[bucketSize];
      colors = new Color[bucketSize];
      for (int i = 0; i < colors.length; i++) {
        colors[i] = null;
      }
      cursor2 = counts.length - 1;
      currentSequenceNumber = cursor2;

      color = LayoutPanel.getColor();

      Marshal.register(name, className);
    }

    public String toString() {
      return super.toString();
    }

    synchronized void update(Tuple tuple) {
      System.out.print(tuple);
      try {
        long time = System.currentTimeMillis();
        Class tupleClass = tuple.getClass();
        long timestamp = ( (Long) (tupleClass.getField("timestamp").get(tuple))).
            longValue();
        long delay = time - timestamp;
        System.out.print(" : " + delay);
        delays[cursor] = delay;
        timestamps[cursor] = timestamp;
        cursor++;
        cursor %= bucketSize;
      }
      catch (Exception e) {
//        e.printStackTrace();
      }
      try {
        update( ( (Integer) (tupleClass.getField("seq_no").get(tuple))).
               intValue());
      }
      catch (Exception e) {
//        e.printStackTrace();
      }
      System.out.println();
    }

    public void update(int sequenceNumber) {
      if (sequenceNumber > currentSequenceNumber) {
        int n = sequenceNumber - currentSequenceNumber;
        for (int i = 0; i < n; i++) {
          cursor2++;
          cursor2 %= counts.length;
          counts[cursor2] = 0;
          colors[cursor2] = null;
        }
        currentSequenceNumber = sequenceNumber;
      }
      if (sequenceNumber + counts.length > currentSequenceNumber) {
        int pos = (cursor2 - currentSequenceNumber + sequenceNumber) %
            counts.length;
        if (pos < 0) {
          pos += counts.length;
        }
        if (color != null) {
          int r = color.getRed();
          int g = color.getGreen();
          int b = color.getBlue();
          int count = counts[pos] + 1;
          counts[pos] = count;
          if (no_dups)
            count = 1;
          colors[pos] = new Color(r / count, g / count, b / count);
        }
      }
    }

    synchronized long getOldestTime() {
      long oldTime = -1;
      for (int i = 0; i < bucketSize; i++) {
        long time = timestamps[i];
        if (time >= 0) {
          if (oldTime < 0 || time < oldTime) {
            oldTime = time;
          }
        }
      }
      if (oldTime < 0) {
        oldTime = 0;
      }
      return oldTime;
    }

    synchronized long getMaxDelay() {
      long maxDelay = -1;
      for (int i = 0; i < bucketSize; i++) {
        long delay = delays[i];
        if (maxDelay < delay) {
          maxDelay = delay;
        }
      }
      if (maxDelay < 0) {
        return 0;
      }
      else {
        return maxDelay;
      }
    }

    synchronized int getbucketSize() {
      int count = 0;
      for (int i = 0; i < bucketSize; i++) {
        if (delays[i] >= 0) {
          count++;
        }
      }
      return count;
    }

  }

  synchronized int getMaxbucketSize() {
    int max = 0;
    for (int i = 0; i < outputStreamInfo.size(); i++) {
      int count = ( (OutputStreamInfo) outputStreamInfo.elementAt(i)).
          getbucketSize();
      if (i == 0) {
        max = count;
      }
      else if (max < count) {
        max = count;
      }
    }
    return max;
  }

  public class Node {

    Color color;

    boolean isPrimary = true;

    boolean isActive = false;

    boolean isDisplayOn = true;

    Rectangle rectangle;

    String nodeDescription;

    ServerNode serverNode;

    String haMethod;

    String partner;

    boolean drawnPair = false;

    Vector boxColumns = new Vector();

    Hashtable streams = new Hashtable();

    Hashtable subscriptions = new Hashtable();

    double[] inputRates = new double[numberOfSamples];
    long[] measurementTimes = new long[numberOfSamples];

    int nColumns;
    int nRows;

    boolean setToCrash = false;
    
    public Node(String nodeDescription) {
      this.nodeDescription = getCanonicalDescription(nodeDescription);
      long time = System.currentTimeMillis();
      color = LayoutPanel.getColor();
      for (int j = 0; j < numberOfSamples; j++) {
        inputRates[j] = 0;
        measurementTimes[j] = time;
      }
      serverNode = new ServerNode(this.nodeDescription);
    }

    void initialize() {
      boxColumns.clear();
      subscriptions.clear();
      streams.clear();
      nRows = 0;
      nColumns = 0;
    }

    void parseQueries(Vector queries) {
      if (queries.size() > 0) {
        for (int i = 0; i < queries.size(); i++) {
          parseQuery( (String) (queries.elementAt(i)));
        }
      }
      nColumns = boxColumns.size();
      nRows = 0;
      for (int i = 0; i < nColumns; i++) {
        Vector boxColumn = (Vector) boxColumns.elementAt(i);
        if (nRows < boxColumn.size()) {
          nRows = boxColumn.size();
        }
      }
    }

    void parseQuery(String query) {
      try {
        DOMParser parser = new DOMParser();
        parser.parse(new org.xml.sax.InputSource(new java.io.StringReader(query)));
        org.w3c.dom.Document doc = parser.getDocument();
        org.w3c.dom.NodeList boxes = doc.getElementsByTagName("box");
        for (int i = 0; i < boxes.getLength(); i++) {
          Box box = new Box(boxes.item(i));
          int column = box.getColumn();
          if (column == boxColumns.size()) {
            boxColumns.add(new Vector());
          }
          Vector boxColumn = (Vector) boxColumns.elementAt(column);
          boxColumn.add(box);
        }
      }
      catch (Exception e) {
        e.printStackTrace();
      }
    }

    void parseSubscriptions(String subscriptionXml) {
      try {
        DOMParser parser = new DOMParser();
        parser.parse(new org.xml.sax.InputSource(new java.io.StringReader(
            subscriptionXml)));
        org.w3c.dom.Document doc = parser.getDocument();
        org.w3c.dom.NodeList subs = doc.getElementsByTagName("subscription");
        for (int i = 0; i < subs.getLength(); i++) {
          org.w3c.dom.Node subscription = subs.item(i);
          org.w3c.dom.NamedNodeMap attrs = subscription.getAttributes();
          String name = attrs.getNamedItem("stream").getNodeValue();
          String destination = attrs.getNamedItem("destination").getNodeValue();
          String ip = destination.substring(0, destination.indexOf(':'));
          int port = Integer.parseInt(destination.substring(destination.indexOf(
              ':') + 1, destination.length()));
          destination = ip + ":" + (port - Marshal.DATA_PORT_OFFSET);
          Port p = (Port) (streams.get(name));
          Object destinationNode = (Node) (nodes.get(destination));
          if (destinationNode == null) {
            Object o = findOutputStreamInfo(name);
            if (o != null) {
              OutputStreamInfo output = (OutputStreamInfo) o;
              output.subscriptions.add(p);
            }
          }
          else {
            ( (Node) destinationNode).subscriptions.put(name, p);
          }
        }
      }
      catch (Exception e) {
        e.printStackTrace();
      }
    }

    public String toString() {
      return nodeDescription;
    }

    void crash() throws Exception {
      setToCrash = false;
      MedusaXmlRpc.invoke(serverNode.serverRPC(),
                          METHOD_CRASH, new Vector());
    }

    void setToCrash() {
      setToCrash = !setToCrash;
    }

    void updateStatistics() {
      long currentTime = System.currentTimeMillis();
      try {
        // Get the stats for this node
        currentTime = System.currentTimeMillis();
        Vector stats = (Vector) MedusaXmlRpc.invoke(serverNode.serverRPC(),
            METHOD_STATS, new Vector());
        updateStats(stats, (System.currentTimeMillis() + currentTime) / 2);
        isActive = true;
      }
      catch (Exception e) {
//        e.printStackTrace();
        updateStats(null, (System.currentTimeMillis() + currentTime) / 2);
        isActive = false;
      }
      try {
        Vector stats = (Vector) MedusaXmlRpc.invoke(serverNode.serverRPC(),
            METHOD_ADMIN_STATUS, new Vector());
        isPrimary = ( (Boolean) (stats.elementAt(INDEX_IS_PRIMARY))).
            booleanValue();
        haMethod = HAMethods[ ( (Integer) (stats.elementAt(INDEX_HA_METHOD))).
            intValue()];
        Object p = stats.elementAt(INDEX_PARTNER);
        partner = null;
        if (p != null && !p.equals("")) {
          partner = (String) p;
        }
        isActive = true;
        parseQueries( ( (Vector) (stats.elementAt(INDEX_QUERY_XMLS))));
        parseSubscriptions( ( (String) (stats.elementAt(INDEX_SUBSCRIPTION_XML))));
      }
      catch (Exception e) {
        isActive = false;
      }
    }

    /**
     * Updates the stats displayed for a borealis node
     * @param node the borealis node for which the stats are updated
     * @param results the list of stats for <code>node</code>
     * @param measurementTime the time of measurement.
     */
    private void updateStats(Vector stats, long measurementTime) {
      if (stats == null) {
        inputRates[cursor] = 0;
        measurementTimes[cursor] = measurementTime;
        return;
      }
      double inputRate = 0.0;
      // Iterating through the set of query hunk stats
      for (Iterator i = stats.iterator(); i.hasNext(); ) {
        Vector statItem = (Vector) i.next();
        inputRate += ( (Double) statItem.get(INDEX_INPUT_RATE)).doubleValue();
      }
      inputRate = inputRate * Bps_to_Kbps;
      inputRates[cursor] = inputRate;
      measurementTimes[cursor] = measurementTime;
    }

    public double getMaxInputRate() {
      double maxRate = -1;
      for (int i = 0; i < numberOfSamples; i++) {
        double rate = inputRates[i];
        if (maxRate < rate) {
          maxRate = rate;
        }
      }
      return maxRate;
    }

    class Port {
      String name;
      int column;
      Point point;

      Port(String name, int column) {
        this.name = name;
        this.column = column;
      }
    }

    class Box {
      Vector inputs = new Vector();
      Vector outputs = new Vector();
      String name;
      int column = 0;

      Box(org.w3c.dom.Node box) {
        org.w3c.dom.NamedNodeMap attrs = box.getAttributes();
        name = attrs.getNamedItem("name").getNodeValue();
        org.w3c.dom.NodeList ports = box.getChildNodes();
        Vector outputPorts = new Vector();
        for (int i = 0; i < ports.getLength(); i++) {
          org.w3c.dom.Node port = ports.item(i);
          if (port.getNodeName().equals("in")) {
            String stream = port.getAttributes().getNamedItem("stream").
                getNodeValue();
            Object p = streams.get(stream);
            if (p == null) {
              Port po = new Port(stream, 0);
              inputs.add(po);
              streams.put(stream, po);
            }
            else {
              Port po = (Port) p;
              if (column < po.column) {
                column = po.column;
              }
              inputs.add(po);
            }
          }
          else if (port.getNodeName().equals("out")) {
            outputPorts.add(port);
          }
        }
        for (int i = 0; i < outputPorts.size(); i++) {
          org.w3c.dom.Node port = (org.w3c.dom.Node) outputPorts.elementAt(i);
          String stream = port.getAttributes().getNamedItem("stream").
              getNodeValue();
          Port po = new Port(stream, column + 1);
          outputs.add(po);
          streams.put(stream, po);
        }
      }

      int getColumn() {
        return column;
      }
    }

  }

  class MonitorTupleReceptionHandler
      extends TupleReceptionEventHandler {

    public synchronized void handle(String output, Tuple tuple) {
      Object oInfo = findOutputStreamInfo(output);
      if (oInfo != null) {
        ( (OutputStreamInfo) oInfo).update(tuple);
      }
    }
  }

  /**
       * The marshaling code for the mitre demo (should be generated automatically).
   *
   * @author Jeong-Hyon Hwang (jhhwang@cs.brown.edu)
   * @version 1.0 02/04/2005
   */
  class MonitorMarshal
      extends Marshal {

    class ConnectThread
        extends Thread {

      public ConnectThread() {
      }

      public void run() {

        try {
          Thread.sleep(warmUpTime);
        }
        catch (Exception e) {
        }
        try {
          Enumeration e = outputStreamInfo.elements();
          while (e.hasMoreElements()) {
            StreamInfo streamInfo = (StreamInfo) e.nextElement();
            connectOutput(streamInfo.name, streamInfo.node);
          }
        }
        catch (Exception e) {
          e.printStackTrace();
        }
        try {
          Enumeration e = inputStreamInfo.elements();
          while (e.hasMoreElements()) {
            InputStreamInfo streamInfo = (InputStreamInfo) e.nextElement();
            if (streamInfo.generate) {
              connectInput(streamInfo.name, streamInfo.node);
              int counter = 0;
              Class c = streamInfo.tupleClass;
              Object tuple = c.newInstance();
              while (true) {
                c.getField("seq_no").set(tuple, new Integer(counter++));
                c.getField("timestamp").set(tuple,
                                            new Long(System.currentTimeMillis()));
                batchInput(streamInfo.name, (Tuple) tuple);
                send(streamInfo.name);
                Thread.sleep(inputGenerationInterval +
                             (counter % inputGenerationInterval));
              }
            }
          }

        }
        catch (Exception e) {
          e.printStackTrace();
        }
      }
    }

    /**
     * Constructs a MonitorMarshal object.
     * @param ip the ip address of the Borealis node to talk with.
     * @param rpcPort the rpc port of the Borealis node to talk with.
     * @param receivePort my tcp port to receive StreamEvents.
     * @param receivePort my tcp port to receive StreamEvents.
     * @param handler the tuple reception event handler.
     */
    MonitorMarshal(int receivePort, TupleReceptionEventHandler handler) throws
        Exception {
      super(receivePort, handler);
      new ConnectThread().start();
    }
  }

}