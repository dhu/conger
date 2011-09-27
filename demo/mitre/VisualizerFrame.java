import clientApi.*;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.util.*;
import java.io.*;

/**
 * <p>Title: MITRE Demo Visualizer</p>
 * <p>Description: MITRE Demo Visualizer</p>
 * <p>Copyright: Copyright (c) 2003</p>
 * <p>Company: </p>
 * @author Jeong-Hyon Hwang
 * @version 1.0
 */

public class VisualizerFrame
    extends JFrame {
  JPanel contentPane;
  JMenuBar jMenuBar = new JMenuBar();
  JMenu jMenuFile = new JMenu();
  JMenuItem jMenuFileExit = new JMenuItem();
  JLabel statusBar = new JLabel();

  protected static BattleField battleField = new BattleField();

  Vector panels = new Vector();
  JPanel displayPanel = new JPanel();
  GridLayout gridLayout = new GridLayout();
  JMenuItem jMenuFileOpen = new JMenuItem();
  String remoteIp;
  int myPort;
  MitreTupleReceptionEventHandler eventHandler = new
      MitreTupleReceptionEventHandler();

  //Construct the frame
  public VisualizerFrame(String[] args) {
  
      remoteIp = MitreDemoMarshalImpl.DEFAULT_IP;
      myPort = MitreDemoMarshalImpl.DEFAULT_RECEIVE_PORT;
      
      try {
        remoteIp = args[0];
        myPort = Integer.parseInt(args[1]);
      } catch (Exception e) {
      }
    
    enableEvents(AWTEvent.WINDOW_EVENT_MASK);
    try {
      jbInit();
    }
    catch (Exception e) {
      e.printStackTrace();
    }
  }

  //Component initialization
  private void jbInit() throws Exception {
    contentPane = (JPanel)this.getContentPane();
    this.setSize(new Dimension(891, 670));
    this.setTitle("Battlefield Monitoring");
    statusBar.setText(" ");
    jMenuFile.setText("File");
    contentPane.setLayout(borderLayout1);
    jMenuFileExit.setText("Exit");
    jMenuFileExit.addActionListener(new
                                    VisualizerFrame_jMenuFileExit_ActionAdapter(this));
    jMenuFile.add(jMenuFileExit);
    jMenuBar.add(jMenuFile);
    contentPane.add(displayPanel, BorderLayout.CENTER);
    contentPane.add(statusBar, BorderLayout.EAST);
    displayPanel.setLayout(gridLayout);
    gridLayout.setHgap(3);
    gridLayout.setVgap(3);
    this.setJMenuBar(jMenuBar);
    displayPanel.add(new LegendPanel());
    adjust();

    MitreDemoMarshalImpl marshal = new MitreDemoMarshalImpl(myPort, eventHandler);
  }

  java.util.HashMap output2panel = new java.util.HashMap();

  class MitreTupleReceptionEventHandler
      extends TupleReceptionEventHandler {
      
    public synchronized void handle(String output, Tuple tuple) {
      Object panel;
      int index = output.lastIndexOf('/');
      if (index >= 0 && index < output.length())
        output = output.substring(index+1, output.length());
      panel = output2panel.get(output);
      if (panel == null) {
        if (tuple instanceof AppElement) {
          panel = addBattleFieldMonitoringPanel(output);
        }
        else if (tuple instanceof ImageData) {
          panel = addVideoPanel(output);
        }
        output2panel.put(output, panel);
      }
      if (panel instanceof BattleFieldMonitoringPanel)
        ((BattleFieldMonitoringPanel)panel).update((AppElement)tuple);
      else if (panel instanceof VideoMonitoringPanel)
        ((VideoMonitoringPanel)panel).update((ImageData)tuple);
    }
  }

  BattleFieldMonitoringPanel addBattleFieldMonitoringPanel(String title) {
    BattleFieldMonitoringPanel panel = new BattleFieldMonitoringPanel(this,
        battleField, title);
    addPanel(panel);
    panel.start();
    return panel;
  }

  VideoMonitoringPanel addVideoPanel(String title) {
    VideoMonitoringPanel panel = new VideoMonitoringPanel(this, title);
    addPanel(panel);
    panel.start();
    return panel;
  }

  protected void addPanel(JPanel panel) {
    System.out.println("panel added");
    displayPanel.add(panel, panels.size());
    panels.add(panel);
    adjust();
  }

  protected void removePanel(JPanel panel) {
    System.out.println("panel removed");
    panels.remove(panel);
    displayPanel.remove(panel);
    adjust();
  }

  protected void removeAllPanels() {
    while (panels.size() > 0) {
      removePanel( (JPanel) panels.elementAt(0));
    }
  }

  protected void adjust() {
    int nSubViews = panels.size() + 1;
    int rows = (int) (Math.sqrt(nSubViews) + .5);
    int cols = 1;
    if (rows > 0) {
      cols = nSubViews / rows;
      if (nSubViews > rows * cols) {
        cols++;
      }
    }
    else {
      rows = 1;
    }
    gridLayout.setRows(rows);
    gridLayout.setColumns(cols);
    displayPanel.doLayout();
    for (int i = 0; i < panels.size(); i++) {
      Object o = panels.elementAt(i);
      if (o instanceof BattleFieldMonitoringPanel) {
        ( (BattleFieldMonitoringPanel) o).initialize();
      }
    }
    System.out.println("adjustment completed for " + nSubViews +
                       " sub views on (" + rows + ", " + cols + ").");
  }

  BorderLayout borderLayout1 = new BorderLayout();

//File | Exit action performed
  public void jMenuFileExit_actionPerformed(ActionEvent e) {
    int reply = JOptionPane.showConfirmDialog(this,
                                              "Do you really want to exit?",
                                              "JFC Application - Exit",
                                              JOptionPane.YES_NO_OPTION,
                                              JOptionPane.QUESTION_MESSAGE);
    if (reply == JOptionPane.YES_OPTION) {
      System.exit(0);
    }
  }

//Overridden so we can exit when window is closed
  protected void processWindowEvent(WindowEvent e) {
    if (e.getID() == WindowEvent.WINDOW_CLOSING) {
      jMenuFileExit_actionPerformed(null);
    }
  }
}

class VisualizerFrame_jMenuFileExit_ActionAdapter
    implements ActionListener {
  VisualizerFrame adaptee;

  VisualizerFrame_jMenuFileExit_ActionAdapter(VisualizerFrame adaptee) {
    this.adaptee = adaptee;
  }

  public void actionPerformed(ActionEvent e) {
    adaptee.jMenuFileExit_actionPerformed(e);
  }
}
