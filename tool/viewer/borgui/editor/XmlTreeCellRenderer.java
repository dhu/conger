package borgui.editor;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
//import borgui.graphic.*;
import javax.swing.tree.*;

/**
 * Displays an entry in a tree.
 */
public class XmlTreeCellRenderer extends DefaultTreeCellRenderer {
    
    /**
     * Constructor.
     */
    public XmlTreeCellRenderer() {
        super();
        
    }
    
    /**
     * Configures the renderer based on the passed in components.
     */
    public Component getTreeCellRendererComponent(JTree tree, Object value,
            boolean sel, boolean expanded,
            boolean leaf, int row, boolean hasFocus) {
        
        JViewport view = new JViewport();
        XmlEditor cell = new XmlEditor();
        cell.setText(value.toString());
        view.setView(cell);
        view.setMinimumSize(new Dimension(100, 100));
        
        if(sel)
            setForeground(getTextSelectionColor());
        else
            setForeground(getTextNonSelectionColor());
        
        
        return view;
    }
    
}
