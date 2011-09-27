import java.util.*;
import java.io.*;
import java.awt.*;
import javax.swing.UIManager;

/**
 * Just a Java GUI front end for suniontestin.cc
 */
public class SimpleGUI2 {

  public static void main(String[] args) {

    try {
      UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
    }
    catch (Exception e) {
      e.printStackTrace();
    }

    InputMonitorFrame frame = new InputMonitorFrame();  

    Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
    Dimension frameSize = frame.getSize();
    System.out.println("Screen is " + screenSize.width + " by " + screenSize.height);
    System.out.println("Frame is  " + frameSize.width + " by " + frameSize.height);
    frame.setLocation((int)((0.5*screenSize.width)-frameSize.width),(int)(0.0*screenSize.height));
    frame.validate();
    frame.setVisible(true);

  }
 
}
