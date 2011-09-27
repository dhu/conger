package borgui.editor;

import borgui.graph.BoxGraphCell;
import javax.swing.*;
import javax.swing.tree.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import org.jdom.Element;
import borgui.xml.*;


/**
 * A class that represents the element tree editor.
 */
public class ElementTreeEditor extends JPanel
{
    protected BorealisElement   m_element;
    protected Vector            m_addElements;
    protected JPanel            m_optionPanel;
    protected JTree             m_elementTree;
    protected ElementTreeModel  m_elementTreeModel;
    protected JComboBox         m_addCombo;
    protected JButton           m_addButton;
    protected JButton           m_removeButton;
    protected JScrollPane       m_editorScrollPane;
    protected BorealisModel     m_borealisModel;


    /**
     * Default constructor.
     */
    public ElementTreeEditor(BorealisModel borealisModel)
    {
        super();

        m_borealisModel = borealisModel;
        m_element       = new NetworkElement();
        m_addElements   = new Vector();

        this.setup();
    }


    public ElementTreeEditor(BorealisModel    borealisModel,
                             BorealisElement  element,
                             Vector addElements)
    {
        super();

        m_borealisModel = borealisModel;
        m_element       = element;
        m_addElements   = addElements;

        this.setup();
    }


    /**
     * Sets the preferred size of this component to a constant value.
     */
    public void setPreferredSize(Dimension preferredSize)
    {
        super.setPreferredSize(preferredSize);

        m_editorScrollPane.setPreferredSize(new Dimension(
                (int)preferredSize.getWidth(),
                (int)preferredSize.getHeight() - 20));

        m_optionPanel.setPreferredSize(new Dimension(
                (int)preferredSize.getWidth(),
                     20));
    }


    /**
     * Used by constructor to set up the layout.
     */
    private void setup()
    {
        this.removeAll();
        this.setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
        
        // tree model
        m_elementTreeModel = new ElementTreeModel((BorealisElement) m_element);
        // tree
        m_elementTree = new JTree(m_elementTreeModel);
        
        
        //custom renderers and editors
        DefaultTreeCellRenderer cellRenderer = new ElementTreeCellRenderer();
        m_elementTree.setCellRenderer(cellRenderer);
        ElementTreeCellEditor elementTreeCellEditor = new ElementTreeCellEditor(m_elementTree, cellRenderer);
        elementTreeCellEditor.setBorealisModel(m_borealisModel);
        m_elementTree.setCellEditor(elementTreeCellEditor);
        
        
        // make editable
        m_elementTree.setEditable(true);
        m_elementTree.getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);
        m_elementTree.setShowsRootHandles(true);
        
        
        m_editorScrollPane = new JScrollPane(m_elementTree);
        m_editorScrollPane.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_AS_NEEDED);
        m_editorScrollPane.setHorizontalScrollBarPolicy(ScrollPaneConstants.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        
        add(m_editorScrollPane);
        
        // options
        m_optionPanel = new JPanel();
        m_optionPanel.setLayout(new BoxLayout(m_optionPanel, BoxLayout.X_AXIS));
        
        m_addButton = new JButton("Add");
        m_optionPanel.add(m_addButton);
        
        Vector<String>  optionStrs = new Vector<String>();

        for(Iterator i = m_addElements.iterator(); i.hasNext(); ) {
            optionStrs.add( ((Element) i.next()).getName() );
        }
        
        m_addCombo = new JComboBox(optionStrs);
        m_addCombo.setPreferredSize(new Dimension(60,20));
        m_optionPanel.add(m_addCombo);
        
        m_removeButton = new JButton("delete");
        m_optionPanel.add(m_removeButton);
        
        m_addButton.addActionListener(new AddButtonListener());
        m_removeButton.addActionListener(new RemoveButtonListener());
        
        this.add(m_optionPanel);
    }


    /**
     * Set the Borealis Element of the tree.
     */
    public void setElement(BorealisElement element, Vector addElements)
    {
        m_element     = element;
        m_addElements = addElements;
        
        m_addCombo.removeAllItems();

        if (element.getName().equals(XmlConstants.BOX_ELEMENT))
        {
            m_addCombo.addItem(XmlConstants.PARAMETER_ELEMENT);
            m_addCombo.setSelectedIndex(0);
            m_addButton.setActionCommand("AddParam");
            
        }
        else if(element.getName().equals(XmlConstants.SCHEMA_ELEMENT))
        {
            m_addCombo.addItem("field(" + XmlConstants.SCHEMA_BYTE_TYPE + ")");
            m_addCombo.addItem(XmlConstants.SCHEMA_SHORT_TYPE);
            m_addCombo.addItem(XmlConstants.SCHEMA_INT_TYPE);
            m_addCombo.addItem(XmlConstants.SCHEMA_LONG_TYPE);
            m_addCombo.addItem(XmlConstants.SCHEMA_SINGLE_TYPE);
            m_addCombo.addItem(XmlConstants.SCHEMA_DOUBLE_TYPE );
            m_addCombo.addItem(XmlConstants.SCHEMA_STRING_TYPE );
            m_addCombo.addItem(XmlConstants.SCHEMA_TIMESTAMP_TYPE );
            m_addCombo.setSelectedIndex(0);

            m_addButton.setActionCommand("AddField");
        }
        else if(element.getName().equals(XmlConstants.OUTPUT_ELEMENT))
        {
            m_addCombo.addItem(XmlConstants.SCHEMA_ELEMENT);
            m_addCombo.setSelectedIndex(0);
            m_addButton.setActionCommand("AddSchemaToOutput");
        }

        m_elementTreeModel.setElement(m_element);
    }


    /**
     * Return the Borealis Element in the current tree.
     */
    public BorealisElement getElement()
    {
        return m_element;
    }

    
    public JComboBox getComboBox()
    {
        return m_addCombo;
    }


    public JButton getRemoveButton()
    {
        return m_removeButton;
    }


    public JButton getAddButton()
    {
        return m_addButton;
    }


    public JPanel getOptionPanel()
    {
        return m_optionPanel;
    }


    /**
     * A class that response to add action.
     */
    private class AddButtonListener implements ActionListener
    {
        //JComboBox m_combo;
        
        /**
         * Constructor.
         */
        public AddButtonListener() {}


        /**
         * Invoked when an action occurs.
         */
        public void actionPerformed(ActionEvent e)
        {
            if (e.getActionCommand().equals("AddQuery"))
            {
                QueryElement element = (QueryElement)m_addElements.get(0);

                if (m_borealisModel.getDocument().getAllQueryNames().contains(
                                                      element.getNameValue()))
                {
                    JOptionPane.showConfirmDialog(null,
                            "The query name has already been used. Please change the name.",
                            "Change Query Name", JOptionPane.CLOSED_OPTION,
                            JOptionPane.ERROR_MESSAGE);
                }
                else
                {   m_borealisModel.getDocument().addQuery(
                          (QueryElement)(Element)element.clone());

                    JOptionPane.showConfirmDialog(null,
                            "A query has been added.",
                            "AddQuery", JOptionPane.CLOSED_OPTION,
                            JOptionPane.INFORMATION_MESSAGE);
                }
            }
            else if (e.getActionCommand().equals("AddSchemaToOutput"))
            {
                if (((OutputElement)m_element).hasSchema())
                {
                    JOptionPane.showConfirmDialog(null,
                            "The output cell can only has one schema.",
                            "Error on output", JOptionPane.CLOSED_OPTION,
                            JOptionPane.ERROR_MESSAGE);
                }
                else
                {
                    try
                    {
                        if (m_addCombo.getItemCount()!=0)
                        {
                            String schemaName = (String) m_addElements.get(m_addCombo.getSelectedIndex());
                            ((OutputElement)m_element).setSchema(schemaName);
                        }
                    }
                    catch (Exception ex)
                    {
                        System.out.println("Can't add a element to the XML Tree");
                    }
                }
            }
            else
            {
                try
                {
                    if (m_addCombo.getItemCount()!=0)
                    {
                        Element element = (Element) m_addElements.get(m_addCombo.getSelectedIndex());
                        m_element.addContent((Element)element.clone());
                    }
                }
                catch (Exception ex)
                {
                    System.out.println("Can't add a element to the XML Tree");
                }
            }

            /*
            if (e.getActionCommand().equals("AddField"))
            {
                Element element = (Element) m_addElements.get(m_addCombo.getSelectedIndex());
                m_element.addContent((Element)element.clone());
            }
            else if(e.getActionCommand().equals("AddParam"))
            {
                Element element = (Element) m_addElements.get(m_addCombo.getSelectedIndex());
                m_element.addContent((Element)element.clone());
            }
            */

            m_elementTreeModel.showTreeUpdate();
        }
    }

    
    /**
     * A class that response to remove action.
     */
    private class RemoveButtonListener implements ActionListener
    {
        /**
         * Constructor.
         */
        public RemoveButtonListener() {}


        /**
         * Invoked when an action occurs.
         */
        public void actionPerformed(ActionEvent e)
        {
            // path to first selected node
            TreePath path = m_elementTree.getSelectionPath();

            if (path != null)
            {
                Object last = path.getLastPathComponent();

                if (last instanceof BorealisElement)
                {
                    // remove last node
                    BorealisElement deleteElement =(BorealisElement)  path.getLastPathComponent();
                    boolean deleteResult = true;

                    if (deleteElement instanceof BoxElement)
                    {
                        deleteResult = m_borealisModel.removeBoxElement(
                                           (BoxElement)deleteElement);
                    }
                    else if(deleteElement instanceof TableElement)
                    {
                        deleteResult = m_borealisModel.removeTableElement(
                                         (TableElement)deleteElement);
                    }
                    else if (deleteElement instanceof QueryElement)
                    {
                        deleteResult = m_borealisModel.removeQueryElement(
                                         (QueryElement)deleteElement);
                    }
                    else if (deleteElement instanceof SchemaElement)
                    {
                        deleteResult = m_borealisModel.removeSchemaElement(
                                        (SchemaElement)deleteElement);
                    }
                    else if (deleteElement instanceof InputElement)
                    {
                        deleteResult = m_borealisModel.removeInputElement(
                                         (InputElement)deleteElement);
                    }
                    else if (deleteElement instanceof OutputElement)
                    {
                        deleteResult = m_borealisModel.removeOutputElement(
                                        (OutputElement)deleteElement);
                    }
                    else if(deleteElement instanceof TablerefElement)
                    {
                        deleteResult = m_borealisModel.removeTablerefElement(
                                      (TablerefElement)deleteElement);
                    }
                    else if(deleteElement instanceof InElement)
                    {
                        deleteResult = m_borealisModel.removeInElement(
                                            (InElement)deleteElement);
                    }
                    else if(deleteElement instanceof OutElement)
                    {
                        deleteResult = m_borealisModel.removeOutElement(
                                           (OutElement)deleteElement);
                    }
                    else
                    {
                        m_element.removeContent(
                               (BorealisElement)path.getLastPathComponent());
                    }
                    
                    if (!deleteResult)
                    {
                        System.out.println("Error in deleting element.");
                    }

                    m_elementTreeModel.showTreeUpdate();
                }
            }
        }
    }


    public void setEditable(boolean bool)
    {
        m_elementTree.setEditable(bool);
        m_addCombo.setEnabled(bool);
        m_addButton.setEnabled(bool);
        m_removeButton.setEnabled(bool);
    }


    public void showTreeUpdate()
    {
        m_elementTreeModel.showTreeUpdate();
    }
}


////////////////////////  end ElementTreeEditor.java  //////////////////////////
