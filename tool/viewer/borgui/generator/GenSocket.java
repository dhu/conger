/**
 * GenSocket.java
 *
 * Created on August 29, 2005, 2:22 PM
 *
 */


package borgui.generator;

import java.net.*;
import java.io.*;
import javax.swing.JOptionPane;
import java.lang.*;

/**
 * A class that used for sending data to loadGenerator by socket
 *
 * @author Wenjuan Xing (vivian@cs.brown.edu)
 * version 1.0 10/10/2005
 */
public class GenSocket {
    
    private String m_server;
    private int m_port;
    private File m_socketFile;
    
    /**
     * Creates a new instance of GenSocket
     */
    public GenSocket(File socketFile) {
        
        m_server = "localhost";
        m_port = GenConstants.DEFAULT_PORT;
        m_socketFile = socketFile;
        
    }
    
    /**
     * Send distribution XML file to loadGenerator
     */
    public boolean sendData(boolean delete) {
        boolean result = true;
        
        //temp for clear file GenSaveTuples
        if(delete) {
            File genSaveTuplesFile = new File("/tmp/GenSaveTuples");
            try{
                genSaveTuplesFile.delete();
            } catch (Exception e) {
            }
        }
        
        if(!m_socketFile.isFile()) {
            System.out.println("Cannot open file: " + m_socketFile.getPath());
            return false;
        }
        
        InetAddress ip;
        try {
            ip = InetAddress.getByName(m_server);
        } catch (UnknownHostException e) {
            showException(e);
            return false;
        }
        
        Socket clientSocket = new Socket();
        
        try {
            clientSocket.connect(new InetSocketAddress(ip, m_port));
        } catch (IOException e) {
            showException(e);
            return false;
        }
        try {
            DataOutputStream outputStream;
            DataInputStream inputStream;
            
            try {
                outputStream = new DataOutputStream(clientSocket.getOutputStream());
            } catch (Exception e) {
                showException(e);
                return false;
            }
            
            try {
                inputStream = new DataInputStream(new FileInputStream(m_socketFile));
            } catch (Exception e) {
                showException(e);
                return false;
            }
            
            int count = (new Long(m_socketFile.length())).intValue();
            
            byte[] all = new byte[count];
            try {
                inputStream.read(all, 0, count);
            } catch (Exception e) {
                showException(e);
                return false;
            }
            try {
                outputStream.write(all);
            } catch (Exception e) {
                showException(e);
                return false;
            }
            
            clientSocket.close();
            
        }catch (Exception e) {
            showException(e);
            return false;
        }
        return result;
    }
    
    /**
     * If there is any exception, pop up a dialog to show the message.
     */
    protected void showException(Exception e) {
        JOptionPane.showConfirmDialog(null, e.toString(),
                "Exception",
                JOptionPane.DEFAULT_OPTION,
                JOptionPane.ERROR_MESSAGE);
    }
}
