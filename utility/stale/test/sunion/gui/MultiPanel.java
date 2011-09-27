import javax.swing.*;
import java.awt.*;
import java.io.*;

/**
 * <p>Title: Rectangle Intersection Query</p>
 * <p>Description: CS 252</p>
 * <p>Copyright: Copyright (c) 2003</p>
 * <p>Company: </p>
 * @author Jeong-Hyon Hwang
 * @version 1.0
 */

public class MultiPanel extends JPanel {


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


    java.util.Hashtable ports2SequencePanels = new java.util.Hashtable();
    java.util.Hashtable ports2DelayPanels = new java.util.Hashtable();
    java.util.Vector ports = new java.util.Vector();

    class ReaderThread extends Thread {

        public void run() {
            System.out.println("Java started...");
            try {
                BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
                String line;
                while ( (line = in.readLine()) != null) {
                    String seqNumber = line.substring(0, line.indexOf(' '));
                    line = line.substring(line.indexOf(' '));
                    while (line.indexOf(' ') == 0) {
                        line = line.substring(1);
                    }

                    // Added by Magda to paint based on tuple types
                    String tupleType = line.substring(0, line.indexOf(' '));
                    line = line.substring(line.indexOf(' '));
                    while (line.indexOf(' ') == 0) {
                        line = line.substring(1);
                    }

                    synchronized (this) {
                        if (parseInput) {
                            String outputPort;
                            String version;
                            int delay = -1;              

                            if (line.indexOf(' ') >= 0) {
                                outputPort = line.substring(0, line.indexOf(' '));
                                version = line.substring(line.indexOf(' '));
                                while (version.indexOf(' ') == 0) {
                                    version = version.substring(1);
                                }
                                if (version.indexOf(' ') > 0) {
                                    String d = version.substring(version.indexOf(' '));
                                    while (d.indexOf(' ') == 0) {
                                        d = d.substring(1);
                                    }
                                    version = version.substring(0, version.indexOf(' '));
                                    try {
                                        delay = Integer.parseInt(d);
                                    } catch (Exception e) {
                                    }
                                }
                            }
                            else {
                                outputPort = line;
                                version = "";
                            }
                            //System.out.println(""+outputPort+" "+version+" "+seqNumber+" "+delay+" "+tupleType);

                            if (ports2SequencePanels.get(outputPort) == null) {
                                JPanel panel = new JPanel();
                                
                                JPanel panel1 = new OutputMonitorPanel(outputPort);
                                JPanel panel2 = new DelayMonitorPanel();
                                ports2SequencePanels.put(outputPort, panel1);
                                ports2DelayPanels.put(outputPort, panel2);
                                int pos = 0;
                                for (; pos < ports.size(); pos++) {
                                    if (outputPort.compareTo(ports.elementAt(pos)) < 0)
                                        break;
                                }
                                ports.insertElementAt(outputPort, pos);
                                add(panel, pos);
                                
                                int rows = (int) (Math.sqrt(ports2SequencePanels.size()) + .5);
                                int cols = 1;
                                if (rows > 0) {
                                    cols = ports2SequencePanels.size() / rows;
                                    if (ports2SequencePanels.size() > rows * cols) {
                                        cols++;
                                    }
                                }
                                else {
                                    rows = 1;
                                }
                                gridLayout.setRows(rows);
                                gridLayout.setColumns(cols);
                                doLayout();
                
                                GridBagLayout g = new GridBagLayout();
                                panel.setLayout(g);
                                GridBagConstraints c = new GridBagConstraints();
                                c.fill = GridBagConstraints.BOTH;
                                c.weightx = 1.0;
                                c.weighty = 2.0;
                                c.gridwidth = GridBagConstraints.REMAINDER;
                                g.setConstraints(panel1, c);
                                panel.add(panel1);
                                
                                c.weighty = 1.0;
                                g.setConstraints(panel2, c);
                                panel.add(panel2);
                                panel.doLayout();
                                
                            }
                            ((OutputMonitorPanel) (ports2SequencePanels.get(outputPort))).update(version, Integer.parseInt(seqNumber),tupleType);
                            ((DelayMonitorPanel) (ports2DelayPanels.get(outputPort))).update(Integer.parseInt(seqNumber), delay);
                        }
                    }
                }
            }
            catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    GridLayout gridLayout = new GridLayout();

    public MultiPanel() {
        setLayout(gridLayout);
        new ReaderThread().start();
    }
}
