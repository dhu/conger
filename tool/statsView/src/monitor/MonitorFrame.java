package monitor;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.border.*;

/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2004</p>
 * <p>Company: </p>
 * @author not attributable
 * @version 1.0
 */

public class MonitorFrame extends JFrame {
  JPanel contentPane;
  JMenuBar jMenuBar = new JMenuBar();
  JMenu jMenuFile = new JMenu();
  JMenuItem jMenuFileExit = new JMenuItem();
  JToolBar jToolBar = new JToolBar();
  JButton jButtonOpen = new JButton();
  JButton jButtonClose = new JButton();
  JButton jButtonHelp = new JButton();
  JButton jButtonStop = new JButton();

  ImageIcon image1;
  ImageIcon image2;
  ImageIcon image3;
  ImageIcon image4;
  JLabel statusBar = new JLabel();
  BorderLayout borderLayout = new BorderLayout();
  MonitorPanel monitorPanel = new MonitorPanel();
  GridLayout gridLayout = new GridLayout();
  TitledBorder titledBorder1;
  TitledBorder titledBorder2;
  JMenu jMenuViewOption = new JMenu();
  JMenuItem jMenuItemTile = new JMenuItem();
  JMenuItem jMenuItemOverlap = new JMenuItem();
  JMenu jMenuHelp = new JMenu();
  JMenuItem jMenuHelpAbout = new JMenuItem();

  Monitor m;

  //Construct the frame
  public MonitorFrame(String[] nodeDescriptions) {
    enableEvents(AWTEvent.WINDOW_EVENT_MASK);
    try {
      jbInit();
      m = new Monitor(nodeDescriptions, monitorPanel);
      monitorPanel.setMonitor(m, -1);
      m.start();
    }
    catch(Exception e) {
      e.printStackTrace();
    }
  }
  //Component initialization
  private void jbInit() throws Exception  {
    image1 = new ImageIcon(MonitorFrame.class.getResource("openFile.png"));
    image2 = new ImageIcon(MonitorFrame.class.getResource("closeFile.png"));
    image3 = new ImageIcon(MonitorFrame.class.getResource("help.png"));
    image4 = new ImageIcon(MonitorFrame.class.getResource("stop.png"));
    contentPane = (JPanel) this.getContentPane();
    titledBorder1 = new TitledBorder("");
    titledBorder2 = new TitledBorder("");
    contentPane.setLayout(borderLayout);
    // Modified by Magda to get bigger display
    //this.setSize(new Dimension(787, 264));
    this.setSize(new Dimension(787, 464));
    this.setTitle("Borealis Monitor");
    statusBar.setText(" ");
    jMenuFile.setText("File");
    jMenuFileExit.setText("Exit");
    jMenuFileExit.addActionListener(new MonitorFrame_jMenuFileExit_ActionAdapter());
    jMenuFile.add(jMenuFileExit);
    jMenuBar.add(jMenuFile);
    jMenuViewOption.setText("View Option");
    jMenuItemTile.setText("Tile");
    jMenuViewOption.add(jMenuItemTile);
    jMenuItemOverlap.setText("Overlap");
    jMenuViewOption.add(jMenuItemOverlap);
    jMenuBar.add(jMenuViewOption);
    jMenuHelp.setText("Help");
    jMenuHelpAbout.setText("About");
    jMenuHelpAbout.addActionListener(new MonitorFrame_jMenuHelpAbout_ActionAdapter());
    jMenuBar.add(jMenuHelp);
    jMenuHelp.add(jMenuHelpAbout);
    this.setJMenuBar(jMenuBar);
    jButtonOpen.setIcon(image1);
    jButtonOpen.setToolTipText("Open File");
    jButtonClose.setIcon(image2);
    jButtonClose.setToolTipText("Close File");
    jButtonHelp.setIcon(image3);
    jButtonHelp.setToolTipText("Help");
    jButtonStop.setIcon(image4);
    jButtonStop.setToolTipText("Stop Input Parsing");
    jButtonStop.addActionListener(new MonitorFrame_Stop_ActionAdapter());

    gridLayout.setColumns(2);
    jToolBar.add(jButtonStop);
    jToolBar.add(jButtonOpen);
    jToolBar.add(jButtonClose);
    jToolBar.add(jButtonHelp);
    contentPane.add(jToolBar, BorderLayout.NORTH);
    contentPane.add(statusBar, BorderLayout.SOUTH);
    contentPane.add(monitorPanel, BorderLayout.CENTER);
  }
  //File | Exit action performed
  public void jMenuFileExit_actionPerformed(ActionEvent e) {
    System.exit(0);
  }
  //Help | About action performed
  public void jMenuHelpAbout_actionPerformed(ActionEvent e) {
    MonitorFrameAboutBox dlg = new MonitorFrameAboutBox(this);
    Dimension dlgSize = dlg.getPreferredSize();
    Dimension frmSize = getSize();
    Point loc = getLocation();
    dlg.setLocation((frmSize.width - dlgSize.width) / 2 + loc.x, (frmSize.height - dlgSize.height) / 2 + loc.y);
    dlg.setModal(true);
    dlg.pack();
    dlg.show();
  }

  public void jStop_actionPerformed(ActionEvent e) {
    m.toggleInputParsing();
  }

  //Query | Clear action performed
//  public void jMenuQueryClear_actionPerformed(ActionEvent e) {
//    tester.clearQueryResults();
//    jPanelBruteForce.repaint();
//    jPanelRS.repaint();
//  }

  //Overridden so we can exit when window is closed
  protected void processWindowEvent(WindowEvent e) {
    super.processWindowEvent(e);
    if (e.getID() == WindowEvent.WINDOW_CLOSING) {
      jMenuFileExit_actionPerformed(null);
    }
  }

  class MonitorFrame_jMenuFileExit_ActionAdapter implements ActionListener {
    public void actionPerformed(ActionEvent e) {
      jMenuFileExit_actionPerformed(e);
    }
  }

  class MonitorFrame_jMenuHelpAbout_ActionAdapter implements ActionListener {
    public void actionPerformed(ActionEvent e) {
      jMenuHelpAbout_actionPerformed(e);
    }
  }

  class MonitorFrame_Stop_ActionAdapter implements ActionListener {
    public void actionPerformed(ActionEvent e) {
      jStop_actionPerformed(e);
    }
  }

}
