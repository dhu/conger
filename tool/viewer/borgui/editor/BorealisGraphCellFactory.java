package borgui.editor;

import java.util.List;
import java.util.HashMap;
import javax.swing.*;
import borgui.common.*;
import borgui.xml.*;
import borgui.graph.*;
import borgui.editor.EditorConstants;

/**
 * A class that implement some useful functions of creating box.
 */
public class BorealisGraphCellFactory
{
    protected int m_numBoxes = 0;
    protected int m_numOutputs = 0;
    protected int m_numInputs = 0;
    
    protected HashMap<String, BoxGraphCell>  m_boxtypes =
                                 new HashMap<String, BoxGraphCell>();
    
    /**
     * Constructor.
     */
    public BorealisGraphCellFactory()
    {
        addBoxType("filter",      (Icon) new ImageIcon(EditorConstants.BOX_FILTER_ICON), 0, 0, 0, 0, false);
        addBoxType("map",         (Icon) new ImageIcon(EditorConstants.BOX_MAP_ICON), 0, 0, 0, 0, false);
        addBoxType("aggregate",   (Icon) new ImageIcon(EditorConstants.BOX_AGGREGATE_ICON), 0, 0, 0, 0, false);
        addBoxType("join",        (Icon) new ImageIcon(EditorConstants.BOX_JOIN_ICON), 0, 0, 0, 0, false);
        addBoxType("union",       (Icon) new ImageIcon(EditorConstants.BOX_UNION_ICON), 0, 0, 0, 0, false);
        addBoxType("select",      (Icon) new ImageIcon(EditorConstants.BOX_SELECT_ICON), 0, 0, 0, 0, true);
        addBoxType("insert",      (Icon) new ImageIcon(EditorConstants.BOX_INSERT_ICON), 0, 0, 0, 0, true);
        addBoxType("update",      (Icon) new ImageIcon(EditorConstants.BOX_UPDATE_ICON), 0, 0, 0, 0, true);
        addBoxType("waitfor",     (Icon) new ImageIcon(EditorConstants.BOX_WAITFOR_ICON), 0, 0, 0, 0, true);
        addBoxType("random_drop", (Icon) new ImageIcon(EditorConstants.BOX_RANDOMDROP_ICON), 0, 0, 0, 0, false);
        addBoxType("window_drop", (Icon) new ImageIcon(EditorConstants.BOX_WINDOWDROP_ICON), 0, 0, 0, 0, false);
    }
    
    /**
     * Create a box based on a supplied element.
     */
    public BoxGraphCell createBox(BoxElement elem)
    {
        BoxGraphCell cell = null;
        
        String type = elem.getType();
        Object obj = m_boxtypes.get(type);

        if (obj != null)
        {
            cell = new BoxGraphCell();
            BoxGraphCell typecell = (BoxGraphCell) ((BoxGraphCell) obj);
            cell.setIcon(typecell.getIcon());
            cell.setTableBox(typecell.isTableBox());
            cell.setBackground(typecell.getBackground());
        }
        else
        {
            cell = new BoxGraphCell();
            cell.setIcon((Icon) new ImageIcon(borgui.editor.EditorConstants.BOX_USERDEFINED_ICON));
            
            TablerefElement refs = elem.getTableref();

            if (refs!=null)
            {
                cell.setTableBox(true);
            }
        }
        
        return cell;
    }
    
    /**
     * Add box type.
     */
    public void addBoxType(String   type,
                           Icon     icon,
                           int      maxInputs,
                           int      maxOutputs,
                           int      minInputs,
                           int      minOutputs,
                           boolean  tableBox)
    {
        BoxGraphCell cell = new BoxGraphCell();
        cell.setIcon(icon);
        cell.setTableBox(tableBox);

        if (type.equals("random_drop"))
        {
            cell.setBackground(java.awt.Color.magenta);
        }
        else if (type.equals("window_drop"))
        {
            cell.setBackground(java.awt.Color.magenta);
        }

        m_boxtypes.put(type, cell);
    }
    
    
    private class BoxTypeInfo
    {
        String   m_type;
        Icon     m_icon;
        int      m_maxInputs;
        int      m_maxOutputs;
        int      m_minInputs;
        int      m_minOutputs;
        boolean  tableBox;
        
        public BoxTypeInfo(String   type,
                           Icon     icon,
                           int      maxInputs,
                           int      maxOutputs,
                           int      minInputs,
                           int      minOutputs,
                           boolean  tableBox)
        {
            m_type = type;
            m_icon = icon;
            m_maxInputs = maxInputs;
            m_minInputs = minInputs;
            m_minOutputs = minOutputs;

            tableBox = tableBox;
        }
    }
}


////////////////////////  end BorealisGraphCellFactory.java  ///////////////////
