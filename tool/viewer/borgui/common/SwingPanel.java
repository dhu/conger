package borgui.common;

import javax.swing.SwingUtilities;
import javax.swing.*;
import java.io.File;



////////////////////////////////////////////////////////////////////////////////
//
public class SwingPanel
{

    static JFileChooser  m_fileChooser = null;


    ////////////////////////////////////////////////////////////////////////////
    //
    //  Pop up a browser to select a file.  Returns null if none selected.
    //
    public static File showFileDialog(String approveButtonText)
    {
        File   file;
    //
    //..........................................................................


        // Pop up file chooser starting from the launch directory.
        //
        if (m_fileChooser == null)
        {   m_fileChooser = new JFileChooser();

            file = new File(System.getProperty("user.dir"));
            m_fileChooser.setCurrentDirectory(file);
        }

        int returnValue = m_fileChooser.showDialog(null, approveButtonText);

        // See if a file was chosen
        //
        if (returnValue != JFileChooser.APPROVE_OPTION)
        {
            return null;
        }

        // Next time select from the same directory.
        //
        file = m_fileChooser.getSelectedFile();
        m_fileChooser.setCurrentDirectory(file);

        return file;
    }
}


///////////////////////  end  SwingPanel.java  ////////////////////////////////
