package borgui.editor;

import javax.swing.*;
import java.awt.event.*;
import java.awt.*;
import org.jgraph.graph.DefaultGraphModel;

public class ActionHandler {
    
    // action types
    public static final int NEW_GRAPH_ACTION = 0;
    public static final int SAVE_GRAPH_ACTION = 1;
    public static final int LOAD_GRAPH_ACTION  = 2;
    public static final int QUIT_ACTION  = 3;
    
    public static final int UNDO_ACTION = 4;
    public static final int REDO_ACTION = 5;
    public static final int REMOVE_ACTION = 6;
    public static final int ORGAGNIZE_ACTION = 7;
    
    public static final int ZOOM_ACTION = 8;
    public static final int ZOOMIN_ACTION = 9;
    public static final int ZOOMOUT_ACTION = 10;
    
    public static final int VIEWDIAG_ACTION = 11;
    public static final int VIEWXML_ACTION = 12;
    public static final int SELECT_ACTION = 13;
    
    public static final int NEW_DEPLOY_ACTION = 14;
    public static final int SAVE_DEPLOY_ACTION = 15;
    public static final int LOAD_DEPLOY_ACTION  = 16;
    
    
    // total number of actions
    protected static final int TOTAL_ACTIONS = 17;
    
    // array of actions indexed by action type
    protected ActionListener[] m_actions;
    
    protected EditorPanel m_editorPanel;
    
    public ActionHandler(EditorPanel editor) {
        
        m_editorPanel = editor;
        
        // init action array
        m_actions = new ActionListener[TOTAL_ACTIONS];
        
        // create and store action listeners
        m_actions[NEW_GRAPH_ACTION] = new NewGraphAction();
        m_actions[SAVE_GRAPH_ACTION] = new SaveGraphAction();
        m_actions[LOAD_GRAPH_ACTION] = new LoadGraphAction();
        m_actions[QUIT_ACTION] = new QuitAction();
        
        m_actions[UNDO_ACTION] = new UndoAction();
        m_actions[REDO_ACTION] = new RedoAction();
        m_actions[REMOVE_ACTION] = new RemoveAction();
        m_actions[ORGAGNIZE_ACTION] = new OrgagnizeAction();
        
        m_actions[ZOOM_ACTION] = new ZoomAction();
        m_actions[ZOOMIN_ACTION] = new ZoominAction();
        m_actions[ZOOMOUT_ACTION] = new ZoomoutAction();
        
        m_actions[VIEWDIAG_ACTION] = new ViewDiagAction();
        m_actions[VIEWXML_ACTION] = new ViewXmlAction();
        m_actions[SELECT_ACTION] = new SelectAction();
       
         m_actions[NEW_DEPLOY_ACTION] = new NewDeployAction();
        m_actions[SAVE_DEPLOY_ACTION] = new SaveDeployAction();
        m_actions[LOAD_DEPLOY_ACTION] = new LoadDeployAction();
        
    }
    
    // recturn actionlistener for passed in action
    public ActionListener getAction(int action) {
        
        if ((action < 0) || (action >= TOTAL_ACTIONS)) {
            return null;
        }
        
        return m_actions[action];
    }
    
    protected class NewGraphAction extends AbstractAction {
        public NewGraphAction() {
        }
        
        public void actionPerformed(ActionEvent e) {
            m_editorPanel.newGraph();
        }
    }
    
    
    protected class SaveGraphAction extends AbstractAction {
        public SaveGraphAction() {
        }
        
        public void actionPerformed(ActionEvent e) {
            m_editorPanel.saveGraph();
        }
    }
    
    protected class LoadGraphAction extends AbstractAction {
        public LoadGraphAction() {
        }
        
        public void actionPerformed(ActionEvent e) {
            m_editorPanel.loadGraph();
        }
    }
    
    
    protected class NewDeployAction extends AbstractAction {
        public NewDeployAction() {
        }
        
        public void actionPerformed(ActionEvent e) {
            m_editorPanel.newDeploy();
        }
    }
    
    
    protected class SaveDeployAction extends AbstractAction {
        public SaveDeployAction() {
        }
        
        public void actionPerformed(ActionEvent e) {
            m_editorPanel.saveDeploy();
        }
    }
    
    protected class LoadDeployAction extends AbstractAction {
        public LoadDeployAction() {
        }
        
        public void actionPerformed(ActionEvent e) {
            m_editorPanel.loadDeploy();
        }
    }
    
    protected class QuitAction extends AbstractAction {
        public QuitAction() {
        }
        
        public void actionPerformed(ActionEvent e) {
            System.exit(1);
        }
    }
    
    protected class UndoAction extends AbstractAction {
        public UndoAction() {
        }
        
        public void actionPerformed(ActionEvent e) {
            try {
                m_editorPanel.getModel().getUndoManager().undo(m_editorPanel.getBorealisGraph().getGraphLayoutCache());
            } catch (Exception ex) {
                System.err.println(ex);
            } finally {
                m_editorPanel.updateHistoryButtons();
            }
        }
    }
    
    protected class RedoAction extends AbstractAction {
        public RedoAction() {
        }
        
        public void actionPerformed(ActionEvent e) {
            try {
                m_editorPanel.getModel().getUndoManager().redo(m_editorPanel.getBorealisGraph().getGraphLayoutCache());
            } catch (Exception ex) {
                System.err.println(ex);
            } finally {
                m_editorPanel.updateHistoryButtons();
            }
        }
    }
    protected class OrgagnizeAction extends AbstractAction {
        public OrgagnizeAction() {
        }
        
        public void actionPerformed(ActionEvent e) {
            org.jgraph.layout.SugiyamaLayoutAlgorithm layout = new org.jgraph.layout.SugiyamaLayoutAlgorithm();
            layout.setVertical(false);
            
            try {
                
                layout.run(m_editorPanel.getModel().getGraph(), DefaultGraphModel.getAll(m_editorPanel.getModel().getGraphModel()), new Object[0]);
                
            } catch(Exception ex) {
            }
            
            
        }
    }
    
    protected class RemoveAction extends AbstractAction {
        public RemoveAction() {
        }
        
        public void actionPerformed(ActionEvent e) {
            if (!m_editorPanel.getBorealisGraph().isSelectionEmpty()) {
                Object[] cells = m_editorPanel.getBorealisGraph().getSelectionCells();
                cells = m_editorPanel.getBorealisGraph().getDescendants(cells);
                m_editorPanel.getModel().removeCells(cells);
            }
        }
    }
    
    protected class ZoomAction extends AbstractAction {
        public ZoomAction() {
        }
        
        public void actionPerformed(ActionEvent e) {
            m_editorPanel.getBorealisGraph().setScale(1.0);
        }
    }
    
    protected class ZoominAction extends AbstractAction {
        public ZoominAction() {
        }
        
        public void actionPerformed(ActionEvent e) {
            
            // Object src = e.getSource();
            /*
            String valStr = zoom.getSelectedItem().toString();
            valStr = valStr.substring(0, valStr.length()-2);
            double toZoom = 0.1 * Double.parseDouble(valStr);
             */
            //toolbar.setZoom(toZoom);
            m_editorPanel.getBorealisGraph().setScale(1.5 * m_editorPanel.getBorealisGraph().getScale());
        }
    }
    
    protected class ZoomoutAction extends AbstractAction {
        public ZoomoutAction() {
        }
        
        public void actionPerformed(ActionEvent e) {
            m_editorPanel.getBorealisGraph().setScale(m_editorPanel.getBorealisGraph().getScale() / 1.5);
        }
    }
    
    protected class ViewDiagAction extends AbstractAction {
        public ViewDiagAction() {
        }
        
        public void actionPerformed(ActionEvent e) {
            m_editorPanel.showDiagView();
        }
    }
    
    protected class ViewXmlAction extends AbstractAction {
        public ViewXmlAction() {
        }
        
        public void actionPerformed(ActionEvent e) {
            m_editorPanel.showXmlView();
        }
    }
    
    protected class SelectAction extends AbstractAction {
        protected JRadioButton m_prevBut = null;
        protected final Color COLOR = new Color(70, 70, 200);
        
        public SelectAction() {
        }
        
        public void actionPerformed(ActionEvent e) {
            int type = -1;
            try {
                type = Integer.parseInt(e.getActionCommand());
            } catch(Exception exp) {
            }
            
            if (m_prevBut != null) {
                m_prevBut.setBackground(new Color(0, 0, 50));
            }
            
            JRadioButton but = (JRadioButton) e.getSource();
            but.setBackground(COLOR);
            m_prevBut = but;
            
            m_editorPanel.setSelection(type);
        }
    }
    
    
}
