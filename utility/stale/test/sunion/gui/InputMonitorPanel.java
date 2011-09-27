import javax.swing.*;
import java.awt.*;
import java.util.*;
import java.io.*;

public class InputMonitorPanel extends JPanel{

    static int updateInterval = 100;

    private static int INPUT_WIDTH = 50;
    private static int INPUT_HEIGHT = 50;
    private static int TEXT_Y_OFFSET = 10;
     
    private int input_spacing;
    private int input_y_offset;

    class UpdateThread
        extends Thread {

        InputMonitorPanel p;

        public UpdateThread(InputMonitorPanel p) {
            this.p = p;
        }

        public void run() {
            try {
                for (; ; ) {
                    Thread.sleep(updateInterval);
                    //p.paint();
                    p.repaint();
                }
            }
            catch (Exception e) {
                e.printStackTrace();
            }
        }
    }


    /**
     * Represents one input to monitor
     */
    public class Input {

        private boolean status;
        private String name;

        public Input(String new_name, boolean new_status) {
            name = new_name;
            status = new_status;
        }

        public void setState(boolean new_status) {
            status = new_status;
        }

        public boolean getState() {
            return status;
        }

        public String getName() {
            return name;
        }
     
    };

    class ReaderThread extends Thread {

        public void run() {
            System.out.println("Java started...");
            try {
                BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
                String line;
                System.out.println("Starting iterations");
                while ( (line = in.readLine()) != null) {

                    // Input name
                    String input = line.substring(0, line.indexOf(' '));
                    line = line.substring(line.indexOf(' '));
                    while (line.indexOf(' ') == 0) {
                        line = line.substring(1);
                    }

                    // Input status
                    String status = line;
                    update(input,Boolean.valueOf(status).booleanValue());
                }
                System.out.println("Done");
            } catch (IOException e) {
                e.printStackTrace();
            }

        }
    };


    private Map inputs;
    private int width;
    private int height;

    public InputMonitorPanel(int w, int h) {

        this.setBackground(Color.black);

        width = w;
        height = h;

        inputs = new TreeMap();

        new ReaderThread().start();
        new UpdateThread(this).start();

    }

    public synchronized void update(String input, boolean status)
    {
        Input i = (Input)inputs.get(input);
        if ( i != null )
        {
            i.setState(status);
        } else {
            inputs.put(input,new Input(input,status));
        }

        // Update location of inputs on screen
        int nb_inputs = inputs.size();
        int total_input_width = nb_inputs*INPUT_WIDTH;
        input_spacing = (width - total_input_width)/(nb_inputs+1);
        input_y_offset = (height - INPUT_WIDTH)/2;
    }

    public void updateInputMonitor() {
        repaint();
    }

    public synchronized void paint(Graphics g) {
        super.paint(g);

        Font current_font = g.getFont();
        Font big_font = new Font(current_font.getName(), current_font.getStyle(),current_font.getSize()*2);
        g.setFont(big_font);

        int i = input_spacing;
        for ( Iterator e = inputs.values().iterator(); e.hasNext(); ) {
            Input input = (Input)e.next();
            if ( input.getState() ) {
                int red = 150;
                int green = Math.max(0, 220 - 1/ 3 * 55);
                int blue = 150;
                g.setColor(new Color(red, green, blue));
            } else {
                g.setColor(Color.RED);
            }
            g.fillOval(i, input_y_offset, INPUT_WIDTH, INPUT_HEIGHT);
            g.drawString("Input " + input.getName(),i,input_y_offset - TEXT_Y_OFFSET);

            i += input_spacing + INPUT_WIDTH;
        }
    }


}



