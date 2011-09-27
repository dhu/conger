/*
 * CActionHandler.java
 *
 * Created on April 27, 2005, 2:25 PM
 */

package borgui.visual;

import javax.swing.*;
import java.awt.event.*;
import java.awt.*;
import java.util.*;

import borgui.common.DebugTrace;


/**
 * 
 *
 * @author Wenjuan Xing (vivian@cs.brown.edu)
 * version 1.0 10/13/2005
 *
 */
public class VisualActionHandler {
    
    protected ActionListener[] m_actions;
    protected VisualPanel m_visualPanel;
    
    
    public static final int LOAD_ACTION = 0;
    public static final int STARTSTOPPLOT_ACTION = 1;
    
    protected static final int TOTAL_ACTIONS = 2;
    
    /** Creates a new instance of CActionHandler */
    public VisualActionHandler(VisualPanel visualPanel) {
        
        m_visualPanel = visualPanel;
        m_actions = new ActionListener[TOTAL_ACTIONS];
        
        m_actions[LOAD_ACTION] = new LoadAction();
        m_actions[STARTSTOPPLOT_ACTION] = new StartStopPlotAction();
        
    }
    
    public ActionListener getAction(int action) {
        if((action<0) || (action >= TOTAL_ACTIONS)) {
            return null;
        }
        return m_actions[action];
        
    }
    
    protected class LoadAction extends AbstractAction {
        public LoadAction() {
        }
        public void actionPerformed(ActionEvent e) {
            //m_visualPanel.load();
            DebugTrace.trace("NYI:  m_visualPanel.load()");
        }
        
    }
    
    protected class StartStopPlotAction extends AbstractAction {
        public StartStopPlotAction() {
        }
        public void actionPerformed(ActionEvent e) {
            m_visualPanel.startStopPlot();
        }
    }
    
}
