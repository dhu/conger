package borgui.editor;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
//import borgui.graphic.*;
import javax.swing.tree.*;

/**
 * A TreeCellEditor.
 */
public class XmlTreeCellEditor extends DefaultTreeCellEditor {
    
    /**
     * Constructor.
     */
    public XmlTreeCellEditor(JTree tree, DefaultTreeCellRenderer renderer) {
        super(tree, renderer);
        
    }
    
    /**
     * Configures the editor. Passed onto the realEditor.
     */
    public Component getTreeCellEditorComponent(JTree tree, Object value,
            boolean isSelected, boolean expanded,
            boolean leaf, int row) {
        JViewport view = new JViewport();
        XmlEditor cell = new XmlEditor();
        cell.setText(value.toString());
        view.setView(cell);
        return view;
        
    }
    
}
