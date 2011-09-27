import java.util.*;
import java.io.*;
import java.awt.*;
import javax.swing.UIManager;

/**
 * Just a Java GUI front end for simple.cc
 */
public class SimpleGUI {

  public static void main(String[] args) {
     
    try {
      UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
    }
    catch (Exception e) {
      e.printStackTrace();
    }

    OutputMonitorFrame frame = new OutputMonitorFrame();
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
 
}
