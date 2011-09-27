/*
 * ReviewTreeModel.java
 *
 * Created on August 4, 2005, 8:24 PM
 */

package borgui.generator;

import javax.swing.event.TreeModelEvent;
import javax.swing.event.TreeModelListener;
import javax.swing.tree.TreeModel;
import javax.swing.tree.TreePath;
import javax.swing.tree.*;
import java.util.List;
import java.util.Vector;
import org.jdom.*;


/**
 * A class that implements the tree mondel for ReviewPanel.
 *
 * @author Wenjuan Xing (vivian@cs.brown.edu)
 * version 1.0 10/12/2005
 */
public class ReviewTreeModel implements TreeModel
{
    private Vector<TreeModelListener>   m_treeModelListeners = new Vector<TreeModelListener>();
    private Element  m_root;
    
    /** 
     * Creates a new instance of ReviewTreeModel 
     */
    public ReviewTreeModel(Element elem)
    {
        super();
        m_root = elem;
    }
    
    /**
     * Change the root element to be displayed.
     */
    public void setElement(Element elem)
    {
        m_root = elem;
        showTreeUpdate();
    }
    
    /**
     * Update the tree.
     */
    public void showTreeUpdate()
    {
        fireTreeStructureChanged(m_root);
    }

    
//////////////// Fire events //////////////////////////////////////////////
    
    /**
     * The only event raised by this model is TreeStructureChanged with the
     * root as path, i.e. the whole tree has changed.
     */
    protected void fireTreeStructureChanged(Object oldRoot)
    {
        int len = m_treeModelListeners.size();

        TreeModelEvent e = new TreeModelEvent(this,
                                              new Object[] {oldRoot});

        for (int i = 0; i < len; i++)
        {
            ((TreeModelListener)m_treeModelListeners.elementAt(i)).
                                                 treeStructureChanged(e);
        }
    }
    
    
//////////////// TreeModel interface implementation ///////////////////////
    
    /**
     * Adds a listener for the TreeModelEvent posted after the tree changes.
     */    
    public void addTreeModelListener(TreeModelListener l)
    {
        m_treeModelListeners.addElement(l);
    }

    
    /**
     * Returns the child of parent at index index in the parent's child array.
     */
    public Object getChild(Object parent, int index)
    {
        if (isLeaf(parent))
        {
            return null;
        }
        
        Element  element  = (Element)parent;
        List     children = element.getChildren();
        
        //org:  children.addAll(element.getChildren());
        
        if (index < 0 || index >= children.size())
        {
            return null;
        }
        
        return children.get(index);
    }
    

    /**
     * Returns the number of children of parent.
     */
    public int getChildCount(Object parent)
    {
        
        if (isLeaf(parent))
        {
            return 0;
        }
        
        Element elem = (Element) parent;
        return elem.getChildren().size();
    }

    
    /**
     * Returns the index of child in parent.
     */
    public int getIndexOfChild(Object parent, Object child)
    {
        Element element = (Element)parent;

        return element.getChildren().indexOf(child);

        //List children = new Vector();
        //children.addAll(elem.getChildren());
        //return children.indexOf(child);
    }

    
    /**
     * Returns the root of the tree.
     */
    public Object getRoot()
    {
        return m_root;
    }

    
    /**
     * Returns true if node is a leaf.
     */
    public boolean isLeaf(Object node)
    {
        return node instanceof Attribute;
    }

    
    /**
     * Removes a listener previously added with addTreeModelListener().
     */
    public void removeTreeModelListener(TreeModelListener l)
    {
        m_treeModelListeners.removeElement(l);
    }

    
    /**
     * Messaged when the user has altered the value for the item
     * identified by path to newValue.  Not used by this model.
     */
    public void valueForPathChanged(TreePath path, Object newValue)
    {
        System.out.println("*** valueForPathChanged : "
                + path + " --> " + newValue);
    }
}


//////////////////////////  end ReviewTreeModel.java  //////////////////////////
