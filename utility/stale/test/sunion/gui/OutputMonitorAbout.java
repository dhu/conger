import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2004</p>
 * <p>Company: </p>
 * @author not attributable
 * @version 1.0
 */

public class OutputMonitorAbout extends JDialog  implements ActionListener{

  JPanel panel1 = new JPanel();
  JPanel panel2 = new JPanel();
  JPanel insetsPanel1 = new JPanel();
  JPanel insetsPanel2 = new JPanel();
  JPanel insetsPanel3 = new JPanel();
  JButton button1 = new JButton();
  JLabel imageLabel = new JLabel();
  JLabel label1 = new JLabel();
  JLabel label2 = new JLabel();
  JLabel label3 = new JLabel();
  JLabel label4 = new JLabel();
  ImageIcon image1 = new ImageIcon();
  BorderLayout borderLayout1 = new BorderLayout();
  BorderLayout borderLayout2 = new BorderLayout();
  FlowLayout flowLayout1 = new FlowLayout();
  GridLayout gridLayout1 = new GridLayout();
  String product = "Rectangle Intersection Query";
  String version = "1.0";
  String copyright = "Copyright (c) 2003";
  String comments = "CS 252";

  public OutputMonitorAbout(Frame parent) {
    super(parent);
    enableEvents(AWTEvent.WINDOW_EVENT_MASK);
    try {
      jbInit();
    }
    catch(Exception e) {
      e.printStackTrace();
    }
  }
  //Component initialization
  private void jbInit() throws Exception  {
    image1 = new ImageIcon(OutputMonitorAbout.class.getResource("about.png"));
    imageLabel.setIcon(image1);
    this.setTitle("About");
    panel1.setLayout(borderLayout1);
    panel2.setLayout(borderLayout2);
    insetsPanel1.setLayout(flowLayout1);
    insetsPanel2.setLayout(flowLayout1);
    insetsPanel2.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
    gridLayout1.setRows(4);
    gridLayout1.setVgap(2);
    gridLayout1.setColumns(1);
    gridLayout1.setHgap(0);
    label1.setFont(new java.awt.Font("Dialog", 0, 12));
    label1.setHorizontalAlignment(SwingConstants.LEFT);
    label1.setHorizontalTextPosition(SwingConstants.CENTER);
    label1.setIconTextGap(4);
    label1.setText("Borealis Output Monitor");
    label2.setFont(new java.awt.Font("Dialog", 0, 12));
    label2.setHorizontalAlignment(SwingConstants.LEFT);
    label2.setHorizontalTextPosition(SwingConstants.CENTER);
    label2.setText(version);
    label3.setFont(new java.awt.Font("Dialog", 0, 12));
    label3.setHorizontalAlignment(SwingConstants.LEFT);
    label3.setHorizontalTextPosition(SwingConstants.CENTER);
    label3.setText("by Jeong-Hyon Hwang (jhhwang@cs.brown.edu)");
    label4.setFont(new java.awt.Font("Dialog", 0, 12));
    label4.setHorizontalAlignment(SwingConstants.LEFT);
    label4.setHorizontalTextPosition(SwingConstants.CENTER);
    label4.setText("Magdalena Balazinska (mbalazin@mit.edu)");
    insetsPanel3.setLayout(gridLayout1);
    insetsPanel3.setBorder(BorderFactory.createEmptyBorder(10, 60, 10, 10));
    button1.setFont(new java.awt.Font("Dialog", 0, 12));
    button1.setText("Ok");
    button1.addActionListener(this);
    panel1.setFont(new java.awt.Font("Dialog", 0, 15));
    insetsPanel3.add(label1, null);
    insetsPanel3.add(label2, null);
    insetsPanel3.add(label3, null);
    insetsPanel3.add(label4, null);
    this.getContentPane().add(insetsPanel1, BorderLayout.SOUTH);
    insetsPanel1.add(button1, null);
    panel2.add(insetsPanel2, BorderLayout.WEST);
    insetsPanel2.add(imageLabel, null);
    panel2.add(insetsPanel3, BorderLayout.CENTER);
    panel1.add(panel2, BorderLayout.NORTH);
    this.getContentPane().add(panel1, BorderLayout.CENTER);
    setResizable(true);
  }
  //Overridden so we can exit when window is closed
  protected void processWindowEvent(WindowEvent e) {
    if (e.getID() == WindowEvent.WINDOW_CLOSING) {
      cancel();
    }
    super.processWindowEvent(e);
  }
  //Close the dialog
  void cancel() {
    dispose();
  }
  //Close the dialog on a button event
  public void actionPerformed(ActionEvent e) {
    if (e.getSource() == button1) {
      cancel();
    }
  }


}