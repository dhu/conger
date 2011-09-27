/**
 * GenActionHandler.java
 *
 * Created on July 23, 2005, 3:13 PM
 *
 *
 */

package borgui.generator;

import javax.swing.*;
import java.awt.event.*;
import java.awt.*;
import java.util.*;
import org.jdom.filter.*;
import org.jdom.*;
import java.io.*;

import borgui.xml.*;

/**
 * A class that cope with all kinds of Action.
 *
 * @author Wenjuan Xing (vivian@cs.brown.edu)
 * version 1.0 10/10/2005
 */
public class GenActionHandler {
    
    protected ActionListener[] m_actions;
    protected InputGenPanel m_inputGenPanel;
    
    public static final int LOAD_DIST_ACTION = 0;
    public static final int SAVE_DIST_ACTION = 1;
    
    public static final int LOAD_TUPLE_ACTION = 2;
    public static final int SAVE_TUPLE_ACTION = 3;
    
    public static final int DIST1_CHOOSEDIST_ACTION = 4;
    public static final int DIST1_APPLY_ACTION = 5;
    public static final int DIST1_CANCEL_ACTION = 6;
    public static final int DIST1_APPLY_TO_ACTION = 7;
    public static final int DIST1_REVIEW_ACTION = 8;
    
    public static final int DIST2_APPLY_ACTION = 9;
    public static final int DIST2_CANCEL_ACTION = 10;
    
    
    public static final int APPLY_TO_APPLY_ACTION = 11;
    public static final int APPLY_TO_CANCEL_ACTION = 12;
    
    public static final int SOCKET_DIST_START_ACTION = 13;
    public static final int SOCKET_TUPLE_START_ACTION = 14;
    public static final int SOCKET_DIST_STOP_ACTION = 15;
    
    public static final int CHANGE_RATE_SCALE_ACTION = 16;
    public static final int SOCKET_RATE_ACTION = 17;
    
    protected static final int TOTAL_ACTIONS = 18;
    
    
    /**
     * Creates a new instance of GenActionHandler
     */
    public GenActionHandler(InputGenPanel inputGenPanel) {
        
        m_inputGenPanel = inputGenPanel;
        m_actions = new ActionListener[TOTAL_ACTIONS];
        
        m_actions[LOAD_DIST_ACTION] = new LoadDistAction();
        m_actions[LOAD_TUPLE_ACTION] = new LoadTupleAction();
        
        m_actions[DIST1_CHOOSEDIST_ACTION] = new Dist1ChooseDistAction();
        m_actions[DIST2_APPLY_ACTION] = new Dist2ApplyAction();
        m_actions[DIST2_CANCEL_ACTION] = new Dist2CancelAction();
        
        m_actions[DIST1_APPLY_ACTION] = new Dist1ApplyAction();
        m_actions[DIST1_CANCEL_ACTION] = new Dist1CancelAction();
        m_actions[DIST1_APPLY_TO_ACTION] = new Dist1ApplyToAction();
        m_actions[DIST1_REVIEW_ACTION] = new Dist1ReviewAction();
        
        m_actions[APPLY_TO_APPLY_ACTION] = new ApplyToApplyAction();
        m_actions[APPLY_TO_CANCEL_ACTION] = new ApplyToCancelAction();
        
        m_actions[SAVE_DIST_ACTION] = new SaveDistAction();
        m_actions[SAVE_TUPLE_ACTION] = new SaveTupleAction();
        
        
        m_actions[SOCKET_DIST_START_ACTION] = new SocketDistStartAction();
        m_actions[SOCKET_TUPLE_START_ACTION] = new SocketTupleStartAction();
        
        m_actions[SOCKET_DIST_STOP_ACTION] = new SocketDistStopAction();
        
        m_actions[CHANGE_RATE_SCALE_ACTION] = new ChangeRateScaleAction();
        
        m_actions[SOCKET_RATE_ACTION] = new SocketRateAction();
        
    }
    
    /**
     * Get the corresponding action
     */
    public ActionListener getAction(int action) {
        if((action<0) || (action >= TOTAL_ACTIONS)) {
            return null;
        }
        return m_actions[action];
        
    }
    
    /**
     * When user click the first load button in the toolbar, it will be called.
     * It load the query network XML document.
     */
    protected class LoadDistAction extends AbstractAction {
        public LoadDistAction() {
        }
        public void actionPerformed(ActionEvent e) {
            m_inputGenPanel.loadNetwork();
        }
    }
    
    /**
     * When user click the second button in the toolbar, it will be called.
     * It load the tuple file.
     */
    protected class LoadTupleAction extends AbstractAction {
        public LoadTupleAction() {
        }
        public void actionPerformed(ActionEvent e) {
            m_inputGenPanel.loadTupleFile();
        }
    }
    
    /**
     * When user click the Socket Sending Data button in the toolbar, it will be called.
     * It will send the distibution XML document to the load Generator.
     */
    protected class SocketDistStartAction extends AbstractAction {
        public SocketDistStartAction() {
        }
        public void actionPerformed(ActionEvent e) {
            m_inputGenPanel.m_saveTupleButton.setEnabled(true);
            m_inputGenPanel.m_socketTupleStartButton.setEnabled(true);
            
            File distributionFile = new File("/tmp/GenDistributionTemp.xml");
            
            GenSocket genSocket = new GenSocket(distributionFile);
            if(!genSocket.sendData(true)) {
                System.out.println("Wrong with sending distribution xml file throught socket.");
            }
            
        }
    }
    
    
    /**
     * When user click the Tupple Sending Data button in the toolbar, it will be called.
     * It will send the tuple file to the load Generator.
     */
    protected class SocketTupleStartAction extends AbstractAction {
        public SocketTupleStartAction() {
        }
        public void actionPerformed(ActionEvent e) {
            
            File commandFile = new File(GenConstants.COMM_FILE_NAME);
            try{
                if(commandFile.exists()) {
                    commandFile.createNewFile();
                }
                String tupleFileName = m_inputGenPanel.getTupleFileName();
                
                //write command into the file
                PrintWriter pw = new PrintWriter(new FileOutputStream(GenConstants.COMM_FILE_NAME));
                pw.println(GenConstants.TUPLE_COMM);
                pw.println(tupleFileName);
                pw.close();
            }catch(IOException exception) {
                System.out.println(exception.getMessage());
            }
            
            GenSocket genSocket = new GenSocket(commandFile);
            if(!genSocket.sendData(false)) {
                System.out.println("Wrong with sending distribution xml file throught socket.");
            }
            
        }
    }
    
    /**
     * When user click the Socket Stop button in the toolbar, it will be called.
     * It will stop Socket.
     */
    protected class SocketDistStopAction extends AbstractAction {
        public SocketDistStopAction() {
        }
        public void actionPerformed(ActionEvent e) {
            
            File commandFile = new File(GenConstants.COMM_FILE_NAME);
            try{
                if(commandFile.exists()) {
                    commandFile.createNewFile();
                }
                
                //write command into the file
                PrintWriter pw = new PrintWriter(new FileOutputStream(GenConstants.COMM_FILE_NAME));
                pw.println(GenConstants.STOP_COMM);
                pw.close();
            }catch(IOException exception) {
                System.out.println(exception.getMessage());
            }
            
            GenSocket genSocket = new GenSocket(commandFile);
            if(!genSocket.sendData(false)) {
                System.out.println("Wrong with sending stop xml file throught socket.");
            }
            
        }
    }
    
    /**
     * When user click the rate button in the bottom of the panel, it will be called.
     */
    protected class SocketRateAction extends AbstractAction {
        public SocketRateAction() {
        }
        public void actionPerformed(ActionEvent e) {
            
            File commandFile = new File(GenConstants.COMM_FILE_NAME);
            
            try{
                if(commandFile.exists()) {
                    commandFile.createNewFile();
                }
                
                //write command into the file
                int rateValue = m_inputGenPanel.getRatePanel().getRate();
                
                PrintWriter pw = new PrintWriter(new FileOutputStream(GenConstants.COMM_FILE_NAME));
                pw.println(GenConstants.RATE_COMM);
                pw.println(rateValue);
                pw.close();
            }catch(IOException exception) {
                System.out.println(exception.getMessage());
            }
            
            
            GenSocket genSocket = new GenSocket(commandFile);
            if(!genSocket.sendData(false)) {
                System.out.println("Wrong with sending rate xml file throught socket.");
            }
            
        }
    }
    
    /**
     * When user click the first Save button in the toolbar, it will be called.
     */
    protected class SaveDistAction extends AbstractAction {
        public SaveDistAction() {
        }
        public void actionPerformed(ActionEvent e) {
            m_inputGenPanel.exportGenXmlToFile();
            
        }
    }
    
    /**
     * When user click the second Save button in the toolbar, it will be called.
     */
    protected class SaveTupleAction extends AbstractAction {
        public SaveTupleAction() {
        }
        public void actionPerformed(ActionEvent e) {
            m_inputGenPanel.saveTuplesToFile();
            
        }
    }
    
    /**
     * When user click the choose ParamDistribution button in the left distribution panel, it will be called.
     */
    protected class  Dist1ChooseDistAction extends AbstractAction {
        public Dist1ChooseDistAction() {
        }
        public void actionPerformed(ActionEvent e) {
            String command= e.getActionCommand().toString();
            String selectedParam = command.substring(1);
            String selectedParamIndex = command.substring(0,1);
            
            m_inputGenPanel.getFistDistPanel().chooseParmDist(selectedParamIndex, selectedParam);
            
            
        }
    }
    
    /**
     * When user click the Apply button in the right distribution panel, it will be called.
     */
    protected class  Dist2ApplyAction extends AbstractAction {
        public Dist2ApplyAction() {
        }
        public void actionPerformed(ActionEvent e) {
            
            boolean validate = m_inputGenPanel.getSecondDistPanel().validateInput();
            m_inputGenPanel.getSecondDistPanel().setComponentEnabled(false);
            m_inputGenPanel.getFistDistPanel().applyParamDist(validate);
        }
    }
    
    /**
     * When user click the Cancel button in the right distribution panel, it will be called.
     */
    protected class  Dist2CancelAction extends AbstractAction {
        public Dist2CancelAction() {
        }
        public void actionPerformed(ActionEvent e) {
            m_inputGenPanel.getSecondDistPanel().cancel();
        }
    }
    
    /**
     * When user click the Apply button in the left distribution panel, it will be called.
     */
    protected class  Dist1ApplyAction extends AbstractAction {
        public Dist1ApplyAction() {
        }
        public void actionPerformed(ActionEvent e) {
            m_inputGenPanel.getFistDistPanel().saveDistToXmlDoc();
            m_inputGenPanel.createReview("");
        }
    }
    
    /**
     * When user click the Cancel button in the left distribution panel, it will be called.
     */
    protected class  Dist1CancelAction extends AbstractAction {
        public Dist1CancelAction() {
        }
        public void actionPerformed(ActionEvent e) {
            m_inputGenPanel.getFistDistPanel().cancel();
        }
    }
    
    /**
     * When user click the ApplyTo button in the left distribution panel, it will be called.
     */
    protected class  Dist1ApplyToAction extends AbstractAction {
        public Dist1ApplyToAction() {
        }
        public void actionPerformed(ActionEvent e) {
            m_inputGenPanel.m_applyToPanel.initDataInList();
            
        }
    }
    
    /**
     * When user click the Review button in the left distribution panel, it will be called.
     */
    protected class  Dist1ReviewAction extends AbstractAction {
        public Dist1ReviewAction() {
        }
        public void actionPerformed(ActionEvent e) {
            
            m_inputGenPanel.createReview("");
            
        }
    }
    
    /**
     * When user click the Apply button in the ApplyTo panel, it will be called.
     */
    protected class  ApplyToApplyAction extends AbstractAction {
        public ApplyToApplyAction() {
        }
        public void actionPerformed(ActionEvent e) {
            
            m_inputGenPanel.m_applyToPanel.insertDist();
        }
    }
    
    /**
     * When user click the Cancel button in the ApplyTo panel, it will be called.
     */
    protected class  ApplyToCancelAction extends AbstractAction {
        public ApplyToCancelAction() {
        }
        public void actionPerformed(ActionEvent e) {
            
            m_inputGenPanel.m_applyToPanel.cancel();
        }
    }
    
    /**
     * When user click the RateScaleChange button in the bottom panel, it will be called.
     */
    protected class ChangeRateScaleAction extends AbstractAction {
        public ChangeRateScaleAction() {
        }
        public void actionPerformed(ActionEvent e) {
            m_inputGenPanel.m_ratePanel.changeScale();
        }
    }
}
