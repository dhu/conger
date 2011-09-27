package monitor;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.util.*;

/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2005</p>
 * <p>Company: </p>
 * @author not attributable
 * @version 1.0
 */

public class MonitorFrame
    extends JFrame {
  JPanel contentPane;
  JMenuBar jMenuBar1 = new JMenuBar();
  JMenu jMenuFile = new JMenu();
  JMenuItem jMenuFileExit = new JMenuItem();
  JMenu jMenuHelp = new JMenu();
  JMenuItem jMenuHelpAbout = new JMenuItem();
  JToolBar jToolBar = new JToolBar();
  JButton jButtonAbout = new JButton();
  JButton jButtonCrash = new JButton();
  ImageIcon helpImage;
  ImageIcon crashImage;
  BorderLayout borderLayout1 = new BorderLayout();
  JSplitPane jSplitPane1 = new JSplitPane();
  JPanel jLayoutPanel;
  JTabbedPane jTabbedPane1 = new JTabbedPane();
  StatisticsCollector statCollector;

  //Construct the frame
  public MonitorFrame(String configurationFile) throws Exception {
    statCollector = new StatisticsCollector(configurationFile, jTabbedPane1);
    jLayoutPanel = new LayoutPanel(statCollector);
    statCollector.registerPanel(jLayoutPanel);

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
    contentPane.setLayout(borderLayout1);
    this.setLocale(java.util.Locale.getDefault());
    this.setSize(new Dimension(800, 750));
    this.setTitle("Borealis Monitor");
    jMenuFile.setText("File");
    jMenuFileExit.setText("Exit");
    jMenuFileExit.addActionListener(new
                                    MonitorFrame_jMenuFileExit_ActionAdapter(this));
    jMenuHelp.setText("Help");
    jMenuHelpAbout.setText("About");
    jMenuHelpAbout.addActionListener(new
                                     MonitorFrame_jMenuHelpAbout_ActionAdapter(this));
    helpImage = new ImageIcon(monitor.MonitorFrame.class.getResource("help.png"));
    jButtonAbout.setIcon(helpImage);
    jButtonAbout.setToolTipText("Help");
    jButtonAbout.addActionListener(new
                                   MonitorFrame_jMenuHelpAbout_ActionAdapter(this));
    crashImage = new ImageIcon(monitor.MonitorFrame.class.getResource(
        "fire.png"));
    jButtonCrash.setIcon(crashImage);
    jButtonCrash.setToolTipText("Crash");
    jButtonCrash.addActionListener(new MonitorFrame_jMenuCrash_ActionAdapter(this));
    contentPane.setPreferredSize(new Dimension(46, 32));
    jSplitPane1.setOrientation(JSplitPane.VERTICAL_SPLIT);
    jSplitPane1.setToolTipText("");
    jLayoutPanel.setMinimumSize(new Dimension(10, 50));
    jToolBar.add(jButtonAbout);
    jToolBar.add(jButtonCrash);
    jMenuFile.add(jMenuFileExit);
    jMenuHelp.add(jMenuHelpAbout);
    jMenuBar1.add(jMenuFile);
    jMenuBar1.add(jMenuHelp);
    this.setJMenuBar(jMenuBar1);
    contentPane.add(jToolBar, BorderLayout.NORTH);
    contentPane.add(jSplitPane1, BorderLayout.CENTER);
    jSplitPane1.add(jLayoutPanel, JSplitPane.TOP);
    jSplitPane1.add(jTabbedPane1, JSplitPane.BOTTOM);
    for (int i = 0; i < statCollector.outputStreamInfo.size(); i++) {
      StatisticsCollector.OutputStreamInfo output = (StatisticsCollector.
          OutputStreamInfo) statCollector.outputStreamInfo.elementAt(i);
      try {
        output.getTupleClass().getField("seq_no");
        OutputPanel outputPanel = new OutputPanel(output);
        String name = output.name;
        name = name.substring(name.lastIndexOf('/') + 1, name.length());
        jTabbedPane1.add(outputPanel, name);
        statCollector.registerPanel(outputPanel);
      } catch (Exception e) {}
    }
    jSplitPane1.setDividerLocation(500);
  }

  //File | Exit action performed
  public void jMenuFileExit_actionPerformed(ActionEvent e) {
    System.exit(0);
  }

  //Help | About action performed
  public void jMenuHelpAbout_actionPerformed(ActionEvent e) {
    MonitorFrame_AboutBox dlg = new MonitorFrame_AboutBox(this);
    Dimension dlgSize = dlg.getPreferredSize();
    Dimension frmSize = getSize();
    Point loc = getLocation();
    dlg.setLocation( (frmSize.width - dlgSize.width) / 2 + loc.x,
                    (frmSize.height - dlgSize.height) / 2 + loc.y);
    dlg.setModal(true);
    dlg.pack();
    dlg.show();
  }

  //Help | About action performed
  public void jMenuCrash_actionPerformed(ActionEvent e) {
    int answer = JOptionPane.showConfirmDialog(this,
        "Would you like to crash chosen nodes?", "Crash Nodes?",
        JOptionPane.YES_NO_OPTION);
    if (answer == JOptionPane.YES_OPTION) {
      statCollector.crashNodes();
    }
  }

  //Overridden so we can exit when window is closed
  protected void processWindowEvent(WindowEvent e) {
    super.processWindowEvent(e);
    if (e.getID() == WindowEvent.WINDOW_CLOSING) {
      jMenuFileExit_actionPerformed(null);
    }
  }
}

class MonitorFrame_jMenuFileExit_ActionAdapter
    implements ActionListener {
  MonitorFrame adaptee;

  MonitorFrame_jMenuFileExit_ActionAdapter(MonitorFrame adaptee) {
    this.adaptee = adaptee;
  }

  public void actionPerformed(ActionEvent e) {
    adaptee.jMenuFileExit_actionPerformed(e);
  }
}

class MonitorFrame_jMenuHelpAbout_ActionAdapter
    implements ActionListener {
  MonitorFrame adaptee;

  MonitorFrame_jMenuHelpAbout_ActionAdapter(MonitorFrame adaptee) {
    this.adaptee = adaptee;
  }

  public void actionPerformed(ActionEvent e) {
    adaptee.jMenuHelpAbout_actionPerformed(e);
  }
}

class MonitorFrame_jMenuCrash_ActionAdapter
    implements ActionListener {
  MonitorFrame adaptee;

  MonitorFrame_jMenuCrash_ActionAdapter(MonitorFrame adaptee) {
    this.adaptee = adaptee;
  }

  public void actionPerformed(ActionEvent e) {
    adaptee.jMenuCrash_actionPerformed(e);
  }
}