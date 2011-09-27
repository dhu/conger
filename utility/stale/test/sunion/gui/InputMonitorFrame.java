import javax.swing.*;
import javax.swing.border.LineBorder;
import java.awt.*;
import java.awt.event.*;

public class InputMonitorFrame extends JFrame {

    private InputMonitorPanel displayed;

    public InputMonitorFrame() {
        super("Borealis Input Monitor");

        Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
        int frameWidth = (int)(0.33*screenSize.width);
        int frameHeight = (int)(0.25*screenSize.height);
        this.setSize(new Dimension(frameWidth,frameHeight));

        setDefaultCloseOperation(EXIT_ON_CLOSE);

        displayed = new InputMonitorPanel(frameWidth,frameHeight);

        Container pane = getContentPane();
        pane.setLayout(new BorderLayout());

        //Back to the pane.
        pane.add(displayed, BorderLayout.CENTER);

        //int delay = 500;         		
        //Timer timer = new Timer(delay, new UpdateListener());
        //timer.start();		

    }
    
    //private class UpdateListener implements ActionListener {
    //  public void actionPerformed(ActionEvent evt) {
    //      repaint();
    //  }
    //}
}
