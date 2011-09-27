/*
 * reviewPanel.java
 *
 * Created on August 3, 2005, 9:50 PM
 *
 */

package borgui.generator;

import java.awt.BorderLayout;
import java.awt.Color;
import javax.swing.*;
import org.jdom.*;



/**
 * A class that implement the panel to review the distribution configuration
 *
 * @author Wenjuan Xing (vivian@cs.brown.edu)
 * version 1.0 10/11/2005
 */
public class ReviewPanel extends JPanel {
    
    protected Element m_rootElem = new Element("Distribution");
    protected ReviewTreeModel m_elemTreeModel;
    protected JTree m_elemTree = new JTree();
    
    /** 
     * Creates a new instance of reviewPanel 
     */
    public ReviewPanel() {
        super();
        showReview(m_rootElem);
        
    }
    
    /**
     * Display the Review of the distribution information
     */
    private void showReview(Element elem) {
        m_rootElem = elem;
        
        m_elemTreeModel = new ReviewTreeModel(m_rootElem);
        m_elemTree = new JTree(m_elemTreeModel);
        
        ReviewTreeCellRenderer cellRenderer = new ReviewTreeCellRenderer();
        m_elemTree.setCellRenderer(cellRenderer);
        m_elemTree.setBackground(new Color(238,238,238));
        
        this.setLayout(new BorderLayout());
        add(Box.createHorizontalStrut(78),BorderLayout.WEST);
        add(m_elemTree,BorderLayout.CENTER);
        
        expandTree();
    }
    
    /**
     * Set the root element of the review tree
     */
    public void setElement(Element elem) {
        m_rootElem = elem;
        m_elemTreeModel.setElement(m_rootElem);
        expandTree();
    }
    
    /**
     * Expand the review tree.
     */
    public void expandTree() {
        
        int rowNum = m_elemTree.getRowCount();
        for(int i = 0; i<rowNum; i++) {
            m_elemTree.expandRow(i);
        }
       
    }
}
