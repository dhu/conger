package borgui.editor;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.Vector;
import borgui.xml.*;
import borgui.graph.*;
import org.jgraph.graph.DefaultPort;

/**
 * A class that represents a container for all the graph cell.
 */
public class GraphButtonPanel extends JPanel {
    protected static final int COLS = 4;
    
    protected ButtonGroup m_buttonGroup;
    protected SelectAction m_selectAction;
    protected int m_selected = 0;
    // cell count that gets put in cell label
    protected int[] cellCount = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    protected EditorPanel m_editorPanel;
    /**
     * Constructor.
     */
    public GraphButtonPanel(EditorPanel editorPanel) {
        m_editorPanel = editorPanel;
        
        m_buttonGroup = new ButtonGroup();
        m_selectAction = new SelectAction();
        
        String icons[] = new String[15];
        
        icons[0] = EditorConstants.INPUT_ICON;
        icons[1] = EditorConstants.OUTPUT_ICON;
        icons[2] = EditorConstants.TABLE_ICON;
        icons[3] = EditorConstants.BOX_FILTER_ICON;
        icons[4] = EditorConstants.BOX_MAP_ICON;
        icons[5] = EditorConstants.BOX_AGGREGATE_ICON;
        icons[6] = EditorConstants.BOX_JOIN_ICON;
        icons[7] = EditorConstants.BOX_UNION_ICON;
        icons[8] = EditorConstants.BOX_SELECT_ICON;
        icons[9] = EditorConstants.BOX_INSERT_ICON;
        icons[10] = EditorConstants.BOX_UPDATE_ICON;
        icons[11] = EditorConstants.BOX_WAITFOR_ICON;
        icons[12] = EditorConstants.BOX_RANDOMDROP_ICON;
        icons[13] = EditorConstants.BOX_WINDOWDROP_ICON;
        icons[14] = EditorConstants.BOX_USERDEFINED_ICON;
        
        
        
        for(int i=0; i < icons.length; i++) {
            JRadioButton but = new JRadioButton(new ImageIcon(icons[i]));
            //but.setSelectedIcon(new ImageIcon(icons[i]));
            but.setActionCommand(Integer.toString(i));
            but.addActionListener(m_selectAction);
            but.setBackground(EditorConstants.BUTTON_COLOR);
            m_buttonGroup.add(but);
            add(but);
            
        }
        
        setBackground(EditorConstants.BUTTON_COLOR);
        setLayout(new GridLayout((int) Math.ceil((double)EditorConstants.BUTTON_NUMBER/(double) COLS), COLS));
        
        
    }
    
    /**
     * A private class that implement select action.
     */
    private class SelectAction extends AbstractAction {
        protected JRadioButton m_prevBut = null;
        protected final Color COLOR = new Color(70, 70, 200);
        
        /**
         * Constructor.
         */
        public SelectAction() {
        }
        
        /**
         * Invoked when an action occurs.
         */
        public void actionPerformed(ActionEvent e) {
            try {
                m_selected = Integer.parseInt(e.getActionCommand());
            } catch(Exception exp) {
            }
            
            if (m_prevBut != null) {
                m_prevBut.setBackground(new Color(0, 0, 50));
            }
            
            JRadioButton but = (JRadioButton) e.getSource();
            but.setBackground(COLOR);
            m_prevBut = but;
            
            BorealisGraphCell cell;
            BorealisElement elem;
            BorealisGraphCellFactory fact = new BorealisGraphCellFactory();
            
            switch (m_selected) {
                case EditorConstants.INPUT:
                    elem = new InputElement("input "+ cellCount[m_selected], "", "0");
                    cell = new InputGraphCell("input "+ cellCount[m_selected]++);
                    break;
                case EditorConstants.OUTPUT:
                    elem = new OutputElement("Output "+ cellCount[m_selected]);
                    cell = new OutputGraphCell("Output "+ cellCount[m_selected]++);
                    break;
                case EditorConstants.TABLE:
                    elem = new TableElement("Table "+ cellCount[m_selected], "", "");
                    cell = new TableGraphCell("Table "+ cellCount[m_selected]++);
                    break;
                case EditorConstants.BOX_FILTER:
                    elem = new BoxElement("Filter "+ cellCount[m_selected]++, "filter");
                    cell = fact.createBox((BoxElement)elem);
                    break;
                case EditorConstants.BOX_MAP:
                    elem = new BoxElement("Map "+ cellCount[m_selected]++, "map");
                    cell = fact.createBox((BoxElement)elem);
                    break;
                case EditorConstants.BOX_AGGREGATE:
                    elem = new BoxElement("Aggregate "+ cellCount[m_selected]++, "aggregate");
                    cell = fact.createBox((BoxElement)elem);
                    break;
                case EditorConstants.BOX_JOIN:
                    elem = new BoxElement("Join "+ cellCount[m_selected]++, "join");
                    cell = fact.createBox((BoxElement)elem);
                    break;
                case EditorConstants.BOX_UNION:
                    elem = new BoxElement("Union "+ cellCount[m_selected]++, "union");
                    cell = fact.createBox((BoxElement)elem);
                    break;
                case EditorConstants.BOX_SELECT:
                    elem = new BoxElement("Select "+ cellCount[m_selected]++, "select");
                    cell = fact.createBox((BoxElement)elem);
                    break;
                case EditorConstants.BOX_INSERT:
                    elem = new BoxElement("Insert "+ cellCount[m_selected]++, "insert");
                    cell = fact.createBox((BoxElement)elem);
                    break;
                case EditorConstants.BOX_UPDATE:
                    elem = new BoxElement("Update "+ cellCount[m_selected]++, "update");
                    cell = fact.createBox((BoxElement)elem);
                    break;
                case EditorConstants.BOX_WAITFOR:
                    elem = new BoxElement("Waitfor "+ cellCount[m_selected]++, "waitfor");
                    cell = fact.createBox((BoxElement)elem);
                    break;
                case EditorConstants.BOX_RANDOMDROP:
                    elem = new BoxElement("RandomDrop "+ cellCount[m_selected]++, "random_drop");
                    cell = fact.createBox((BoxElement)elem);
                    break;
                case EditorConstants.BOX_WINDOWDROP:
                    elem = new BoxElement("WindowDrop "+ cellCount[m_selected]++, "window_drop");
                    cell = fact.createBox((BoxElement)elem);
                    break;
                case EditorConstants.BOX_USERDEFINED:
                    elem = new BoxElement("UserDefined "+ cellCount[m_selected]++, "user_defined");
                    cell = fact.createBox((BoxElement)elem);
                    break;
                default:
                    elem = null;
                    cell = null;
                    System.out.println("Can not figure out the cell type.");
                    
            }
            
            m_editorPanel.getModel().add(elem, cell);
            m_editorPanel.getNetworkTreeModel().showTreeUpdate();
        }
    }
    
    
}






