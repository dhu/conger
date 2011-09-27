package monitor;

import medusaXmlRpc.*;
import org.apache.xmlrpc.*;
import java.util.*;
import javax.swing.UIManager;
import java.awt.*;

/**
 * Borealis client that periodically connects to Borealis nodes
 * and retrieves their current load statistics. The statistics
 * are then displayed in a fancy GUI ;-)
 */
public class Monitor
    extends Thread {

  protected boolean parseInput = true;

  public void enableInputParsing() {
    synchronized (this) {
      parseInput = true;
    }
  };

  public void disableInputParsing() {
    synchronized (this) {
      parseInput = false;
    }
  };

  public void toggleInputParsing() {
    synchronized (this) {
      parseInput = !parseInput;
    }
  };

  // Constants used for the RPC call and result interpretation
  private static final String METHOD_STATS = "QueryProcessor.get_stats";
  private static final String METHOD_MOVES = "NHOptimizer.get_load_moves";
  private static final int INPUT_RATE_INDEX = 2;

  // Constants used for conversions
  private static final double Bps_to_Kbps = 8.0 / 1000;

  // Constants used for the monitor behavior
  static final long SLEEP_MSECS = 500;
  static final int numberOfSlots = 120;
  static final long monitoringSpan = SLEEP_MSECS * numberOfSlots;

  String[] nodeDescriptions = null;
  MonitorPanel monitorPanel;
  double[][] inputRates = null;
  long[][] measurementTimes = null;
  Vector[] moveTimes = null;
  Color[] colors;
  int cursor = 0;

  public Monitor(String[] nodeDescriptions, MonitorPanel monitorPanel) {
    this.nodeDescriptions = nodeDescriptions;
    this.monitorPanel = monitorPanel;
  }

  /**
   * Monitors a set or Borealis nodes
   */
  public static void main(String[] args) {
    try {
      UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
    }
    catch (Exception e) {
      e.printStackTrace();
    }

    String[] nodeDescriptions = new String[args.length-1];
    for (int i = 0; i < nodeDescriptions.length; i++)
      nodeDescriptions[i] = args[i+1];

    MonitorPanel.maxCapacity = Double.parseDouble(args[0]);
    MonitorFrame frame = new MonitorFrame(nodeDescriptions);
    frame.validate();
    //Center the window
    Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
    Dimension frameSize = frame.getSize();
    if (frameSize.height > screenSize.height) {
      frameSize.height = screenSize.height;
    }
    if (frameSize.width > screenSize.width) {
      frameSize.width = screenSize.width;
    }
    frame.setLocation( (screenSize.width - frameSize.width) / 2,
                      (screenSize.height - frameSize.height) / 2);
    frame.setVisible(true);
  }

  /**
   * Starts monitoring the list of Borealis nodes periodically
   * In each period, retrieves the load statistics from each Borealis node
   * @param nodes the set of Borealis nodes to monitor
   */
  public void run() {

    // Magda modified otherwise order of lines != order of IP addresses
    //Set nodes = new HashSet();
    java.util.List nodes = new ArrayList();

    // Parse the list of server nodes to monitor
    for (int i = 0; i < nodeDescriptions.length; i++) {
      ServerNode n = new ServerNode(nodeDescriptions[i]);
      nodes.add(n);
    }

    long currentTime = System.currentTimeMillis();
    inputRates = new double[nodes.size()][numberOfSlots];
    measurementTimes = new long[nodes.size()][numberOfSlots];
    for (int i = 0; i < nodes.size(); i++) {
      for (int j = 0; j < numberOfSlots; j++) {
        inputRates[i][j] = 0;
        measurementTimes[i][j] = currentTime;
      }
    }
    moveTimes = new Vector[nodes.size()];
    for (int i = 0; i < nodes.size(); i++) {
      moveTimes[i] = new Vector();
    }
    colors = new Color[nodes.size()];
    for (int i = 0; i < colors.length; i++) {
      int c1 = (i % 3 == 0) ? (255 - 128 * i / colors.length) : 128;
      int c2 = (i % 3 == 1) ? (255 - 128 * i / colors.length) : 128;
      int c3 = (i % 3 == 2) ? (255 - 128 * i / colors.length) : 128;
      System.out.println(c1);
      colors[i] = new Color(c1, c2, c3);
    }

    // Monitor continuously
    while (true) {
      synchronized (this) {
        if (parseInput) {

          // In sequence, contact each node in the list
          Iterator nodeIter = nodes.iterator();
          for (int i = 0; i < nodes.size(); i++) {
            currentTime = System.currentTimeMillis();
            while (moveTimes[i].size() > 0) { // remove move times falling out of the monitoring window
              if ( ( (Long) (moveTimes[i].elementAt(0))).longValue() +
                  monitoringSpan < currentTime) {
                moveTimes[i].remove(0);
              }
              else {
                break;
              }
            }
            ServerNode node = (ServerNode) nodeIter.next();
            try {
              // Get the stats for this node
              currentTime = System.currentTimeMillis();
              Vector stats = (Vector) MedusaXmlRpc.invoke(node.serverRPC(),
                  METHOD_STATS, new Vector());
              long measurementTime = (System.currentTimeMillis()+currentTime)/2;
              updateStats(i, stats, measurementTime);
            }
            catch (Exception e) {
              System.err.println("Failed updating stats for " + node + " " + e);
              long measurementTime = (System.currentTimeMillis()+currentTime)/2;
              updateStats(i, null, measurementTime);
            }
            try {
              Vector moves = (Vector) MedusaXmlRpc.invoke(node.serverRPC(),
                  METHOD_MOVES, new Vector());
              // Update and display the stats for that node
              updateMoveTimes(i, moves);
            }
            catch (Exception e) {
              System.err.println("Failed updating move times for " + node + " " + e);
              updateMoveTimes(i, null);
            }
          }

          monitorPanel.paint();
          monitorPanel.repaint();
          cursor++;
          cursor %= numberOfSlots;
        }
      }

      // Sleep until the end of SLEEP_MSECS so that iterations have a fixed duration
      try {
        Thread.sleep(SLEEP_MSECS);
      }
      catch (InterruptedException e) {
        // sleep interrupted
      }
    }
  }

  /**
   * Updates the stats displayed for a borealis node
   * @param node the borealis node for which the stats are updated
   * @param results the list of stats for <code>node</code>
   * @param measurementTime the time of measurement.
   */
  private void updateStats(int nodeIndex, Vector stats, long measurementTime) {

    if (stats == null) {
      inputRates[nodeIndex][cursor] = 0;
      measurementTimes[nodeIndex][cursor] = measurementTime;
      return;
    }

    double inputRate = 0.0;

    // Iterating through the set of query hunk stats
    for (Iterator i = stats.iterator(); i.hasNext(); ) {

      Vector statItem = (Vector) i.next();
      inputRate += ( (Double) statItem.get(INPUT_RATE_INDEX)).doubleValue();

    }

    inputRate = inputRate * Bps_to_Kbps;
    System.out.println("For node " + nodeDescriptions[nodeIndex]
                       + " current total input rate is aroud: " + inputRate +
                       " Kbps");
    inputRates[nodeIndex][cursor] = inputRate;
    measurementTimes[nodeIndex][cursor] = measurementTime;
  }

  /**
   * Updates the move times for a borealis node
   * @param node the borealis node for which the move times are updated.
   * @param results the list of stats for <code>node</code>
   */
  private void updateMoveTimes(int nodeIndex, Vector moves) {

    if (moves == null) {
      return;
    }

    // Iterating through the load movement information
    for (Iterator j = moves.iterator(); j.hasNext(); ) {
      Vector sequenceMoves = (Vector) j.next();
      long time = Long.parseLong( (String) sequenceMoves.lastElement());
      //moveTimes[nodeIndex].add(new Long(time)); //???
      moveTimes[nodeIndex].add(new Long(time / 1000)); //???
      long deltaTime = System.currentTimeMillis() - time / 1000;
      System.out.println("Last movement at: " + deltaTime + " msecs ago");
    }
  }
}
