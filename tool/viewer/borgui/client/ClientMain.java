package borgui.client;

import javax.swing.*;
import java.awt.*;
import java.util.*;

import borgui.editor.*;
import borgui.visual.*;
import borgui.generator.*;
import borgui.xml.*;

/**
 * A class that include main method.
 */
public class ClientMain
{
    /**
     * Create the GUI and show it.  For thread safety,
     * this method should be invoked from the
     * event-dispatching thread.
     */
    private static void createAndShowGUI(String[] args)
    {
        Client client;
        client = new Client("Borealis Client");
        
        EditorPanel editorPanel = new EditorPanel(client);
        VisualPanel visualPanel = new VisualPanel(editorPanel);
        InputGenPanel inputGenPanel = new InputGenPanel();
        client.addTab("Visual", null, visualPanel, "visualize borealis query");
        client.addTab("Editor", null, editorPanel, "create/edit borealis query");
        client.addTab("Input Generator", null, inputGenPanel, "generate borealis input");
        
        
        // hack
        //if (args.length > 1)
        //{   visualPanel.load(args[0]);
        //    System.out.println("Loading Deployment File = " + args[1]);
        //}
    }

    
    /**
     * main methond
     */
    public static void main(final String[] args)
    {
        // Schedule a job for the event-dispatching thread:
        // creating and showing this application's GUI.
        //
        javax.swing.SwingUtilities.invokeLater(new Runnable()
        {
            public void run()
            {
                createAndShowGUI(args);
            }
        });
    }
}


/////////////////////////////  end  ClientMain.java  ///////////////////////////
