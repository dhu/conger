package borgui.editor;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
//import borgui.graphic.*;
import javax.swing.tree.*;

/**
 *  A control that displays a set of hierarchical data as an outline.
 */
public class XmlTree extends JTree {
    
/*
 * Constructor.
 */
    public XmlTree() {
        super(new DefaultMutableTreeNode(new String("<borealis>")));
        
        
        DefaultTreeCellRenderer renderer = new XmlTreeCellRenderer();
        DefaultTreeCellEditor editor = new XmlTreeCellEditor(this, renderer);
        this.setCellRenderer(renderer);
        this.setCellEditor(editor);
        this.setEditable(true);
        this.setShowsRootHandles(true);
        
    }
    
    
    
}
