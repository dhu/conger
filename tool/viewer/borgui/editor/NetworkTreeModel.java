package borgui.editor;

import javax.swing.event.TreeModelEvent;
import javax.swing.event.TreeModelListener;
import javax.swing.tree.TreeModel;
import javax.swing.tree.TreePath;
import javax.swing.tree.*;
import java.util.List;
import java.util.Vector;
import java.util.Iterator;
import org.jdom.*;
import borgui.xml.*;

/**
 * A class that defines a suitable data model for a Network Tree.
 */
public class NetworkTreeModel  implements TreeModel
{
    protected final static String SCHEMA_ROOT = new String("Schemas:");
    protected final static String INPUT_ROOT  = new String("Inputs:");
    protected final static String OUTPUT_ROOT = new String("Outputs:");
    protected final static String QUERY_ROOT  = new String("Queries:");
    protected final static String BOX_ROOT    = new String("Boxes:");
    protected final static String TABLE_ROOT  = new String("Tables:");
    
    
    private Vector<TreeModelListener>  m_treeModelListeners =
                                           new Vector<TreeModelListener>();
    
    private Element m_root;
    protected BorealisDocument m_doc;
    
    /**
     * Constructor.
     */
    public NetworkTreeModel(BorealisDocument doc)
    {
        m_doc = doc;
        m_root = m_doc.getNetwork();
    }
    
    /**
     * Set the document for this model.
     */
    public void setDocument(BorealisDocument doc)
    {
        m_doc = doc;
        m_root = m_doc.getNetwork();
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

        TreeModelEvent e = new TreeModelEvent(this, new Object[] {oldRoot});

        for (int i = 0; i < len; i++)
        {
            (m_treeModelListeners.elementAt(i)).treeStructureChanged(e);
            //((TreeModelListener)m_treeModelListeners.elementAt(i)).treeStructureChanged(e);
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
     * Return the Child vector depends on parent's type.
     */
    protected Vector<Object> getChildVector(Object parent)
    {
        Vector<Object>  children = new Vector<Object>();
        
        // network node
        if (parent instanceof NetworkElement)
        {
            children.add(SCHEMA_ROOT);
            children.add(INPUT_ROOT);
            children.add(OUTPUT_ROOT);
            children.add(QUERY_ROOT);
            // query node
        }
        else if (parent instanceof QueryElement)
        {
            //children.addAll(((QueryElement)parent).getAllBoxes());
            List   boxes = ((QueryElement)parent).getAllBoxes();

            for(Iterator i = boxes.iterator(); i.hasNext();)
            {
                children.add((BoxElement)i.next());
            }

            //children.addAll(((QueryElement)parent).getAllTables());
            List   tables = ((QueryElement)parent).getAllTables();

            for(Iterator i = tables.iterator(); i.hasNext();)
            {
                children.add((TableElement)i.next());
            }

            
            /*
            children.add(BOX_ROOT);
            children.add(TABLE_ROOT);
             */
            
            // one of the string roots
        }
        else if (parent instanceof String)
        {
            String node = (String) parent;
            
            if (node.equals(SCHEMA_ROOT))
            {
                //children.addAll(m_doc.getAllSchemas());
                List   schemas = m_doc.getAllSchemas();

                for(Iterator i = schemas.iterator(); i.hasNext();)
                {
                    children.add((SchemaElement)i.next());
                }
            }
            else if (node.equals(INPUT_ROOT))
            {
                //children.addAll(m_doc.getAllInputs());
                List   inputs = m_doc.getAllInputs();

                for(Iterator i = inputs.iterator(); i.hasNext();)
                {
                    children.add((InputElement)i.next());
                }
            }
            else if (node.equals(OUTPUT_ROOT))
            {
                //children.addAll(m_doc.getAllOutputs());
                List   outputs = m_doc.getAllOutputs();

                for(Iterator i = outputs.iterator(); i.hasNext();)
                {
                    children.add((OutputElement)i.next());
                }
            }
            else if (node.equals(QUERY_ROOT))
            {
                //children.addAll(m_doc.getAllQueries());
                List   queries = m_doc.getAllQueries();

                for(Iterator i = queries.iterator(); i.hasNext();)
                {
                    children.add((QueryElement)i.next());
                }
            }
        }

        return children;
    }

    
    /**
     * Returns the child of parent at index index in the parent's child array.
     */
    public Object getChild(Object parent, int index)
    {
        Vector children = getChildVector(parent);
        
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
        return getChildVector(parent).size();
    }
    
    /**
     * Returns the index of child in parent.
     */
    public int getIndexOfChild(Object parent, Object child)
    {
        Vector children = getChildVector(parent);
        return children.indexOf(child);
    }
    
    /**
     * Returns the root of the tree.
     */
    public Object getRoot()
    {
        return m_doc.getNetwork();
    }
    
    /**
     * Returns true if node is a leaf.
     */
    public boolean isLeaf(Object node)
    {
        return (getChildCount(node) == 0);
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

/////////////////////////  end NetworkTreeModel.java  //////////////////////////
