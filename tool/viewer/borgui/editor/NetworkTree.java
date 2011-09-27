package borgui.editor;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.tree.*;

/**
 *  A control that displays a set of hierarchical data as an outline.
 */
public class NetworkTree extends JTree {
    
/*
 * Constructor.
 */
    public NetworkTree() {
        super(new DefaultMutableTreeNode(new String("<borealis>")));
        
        
        DefaultTreeCellRenderer renderer = new XmlTreeCellRenderer();
        DefaultTreeCellEditor editor = new XmlTreeCellEditor(this, renderer);
        this.setCellRenderer(renderer);
        this.setCellEditor(editor);
        this.setShowsRootHandles(true);
        
    }
    
    
    
}
