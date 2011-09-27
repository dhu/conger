package borgui.editor;

import borgui.xml.*;
import javax.swing.*;
import java.awt.*;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import javax.swing.tree.*;
import org.jdom.Attribute;
import org.jdom.Element;


/**
 * A TreeCellEditor.
 */
public class ElementTreeCellEditor  extends DefaultTreeCellEditor
{
    protected BorealisModel m_borealisModel;

    
    /**
     * Constructor.
     */
    public ElementTreeCellEditor(JTree                    tree,
                                 DefaultTreeCellRenderer  renderer)
    {
        super(tree, renderer);
    }

    
    /**
     * Configures the editor. Passed onto the realEditor.
     */
    public Component getTreeCellEditorComponent(JTree      tree,
                                                Object     value,
                                                boolean    isSelected,
                                                boolean    expanded,
                                                boolean    leaf,
                                                int        row)
   {
        Component comp = null;
        
        if (value instanceof Attribute)
        {
            comp = new AttributeEditor((Attribute) value);
            
            if (isSelected)
            {
                comp.setBackground(Color.lightGray);
            }
            else
            {
                comp.setBackground(tree.getBackground());
            }
        }
        // elements are not editable
        else if (value instanceof Element)
        {
            comp = tree.getCellRenderer().getTreeCellRendererComponent(
                    tree, value,
                    isSelected, expanded,
                    leaf, row, true);
        }
        
        return comp;
        
    }
    

    public void setBorealisModel(BorealisModel borealisModel)
    {
        m_borealisModel = borealisModel;
    }
   
 
    /**
     * A class that represent the attribute editor.
     */
    private class AttributeEditor  extends JPanel
                                implements PropertyChangeListener
    {
        protected Attribute            m_attr;
        protected JFormattedTextField  m_field;
        
        
        /**
         * Constructor.
         */
        public AttributeEditor(Attribute attr)
        {
            m_attr = attr;
            
            setLayout(new BoxLayout(this, BoxLayout.X_AXIS));
            
            // name is uneditable
            JLabel label = new JLabel("<html><font color=red>" +
                    attr.getName() +
                    "</font>" +
                    "=</html>");
            add(label);
            
            // value is editable
            m_field = new JFormattedTextField(attr.getValue());
            m_field.addPropertyChangeListener("value", this);
            add(m_field);
        }
       
 
        /** Called when a field's "value" property changes. */
        public void propertyChange(PropertyChangeEvent e)
        {
            m_attr.setValue(m_field.getValue().toString());
            
            if(m_borealisModel!=null)
            {
                String attrName = m_attr.getName();
                Element element = ((Attribute)m_attr).getParent();
                String elementName = element.getName();
                String oldValue = (String) e.getOldValue();
                String newValue = (String) e.getNewValue();
                
                if(elementName.equals(XmlConstants.SCHEMA_ELEMENT))
                {
                    if(attrName.equals(XmlConstants.NAME_ATTRIBUTE))
                    {
                        m_borealisModel.changeSchemaElementName(oldValue, newValue);
                    }
                }
                else if(elementName.equals(XmlConstants.INPUT_ELEMENT))
                {
                    if(attrName.equals(XmlConstants.INPUT_STREAM_ATTRIBUTE))
                    {
                        m_borealisModel.changeInputElementStream(
                                   (InputElement)element, oldValue, newValue);
                    }
                    else if(attrName.equals(XmlConstants.INPUT_SCHEMA_ATTRIBUTE))
                    {
                        m_borealisModel.changeElementSchemaAttribute(newValue);
                    }
                    else if(attrName.equals(XmlConstants.INPUT_NODE_ATTRIBUTE))
                    {
                         JOptionPane.showConfirmDialog(null,
                                "Please change the node from the context menu",
                                "Error on input node", JOptionPane.CLOSED_OPTION,
                                JOptionPane.ERROR_MESSAGE);
                    }
                }
                else if(elementName.equals(XmlConstants.OUTPUT_ELEMENT))
                {
                    if(attrName.equals(XmlConstants.OUTPUT_STREAM_ATTRIBUTE))
                    {
                        m_borealisModel.changeOutputElementStream((OutputElement)element, oldValue, newValue);
                        
                    }
                    else if(attrName.equals(XmlConstants.OUTPUT_SCHEMA_ATTRIBUTE))
                    {
                        m_borealisModel.changeElementSchemaAttribute(newValue);
                    }
                }
                else if(elementName.equals(XmlConstants.IN_ELEMENT))
                {
                    m_borealisModel.changeInElementStream((BoxElement)element.getParentElement(), oldValue, newValue);
                }
                else if(elementName.equals(XmlConstants.OUT_ELEMENT))
                {
                    m_borealisModel.changeOutElementStream((BoxElement)element.getParentElement(), oldValue, newValue);
                }
                else if(elementName.equals(XmlConstants.BOX_ELEMENT))
                {
                    if(attrName.equals(XmlConstants.NAME_ATTRIBUTE))
                    {
                        m_borealisModel.changeBoxElementName((BoxElement)element, newValue);
                    }
                    else if(attrName.equals(XmlConstants.BOX_TYPE_ATTRIBUTE))
                    {
                        JOptionPane.showConfirmDialog(null,
                                "You don't want to change the box type. Please remove it and add a new box with the proper type",
                                "Error on box type", JOptionPane.CLOSED_OPTION,
                                JOptionPane.ERROR_MESSAGE);
                    }
                    else if(attrName.equals(XmlConstants.BOX_NODE_ATTRIBUTE))
                    {
                        JOptionPane.showConfirmDialog(null,
                                "Please change the node from the context menu",
                                "Error on box node", JOptionPane.CLOSED_OPTION,
                                JOptionPane.ERROR_MESSAGE);
                    }
                }
                else if(elementName.equals(XmlConstants.TABLE_ELEMENT))
                {
                    if(attrName.equals(XmlConstants.NAME_ATTRIBUTE))
                    {
                        m_borealisModel.changeTableElementName((TableElement)element, newValue);
                    }
                    else if(attrName.equals(XmlConstants.TABLE_STREAM_ATTRIBUTE))
                    {
                        m_borealisModel.changeTableElementStream((TableElement)element, newValue);
                        
                    }
                    else if(attrName.equals(XmlConstants.TABLE_SCHEMA_ATTRIBUTE))
                    {
                        m_borealisModel.changeElementSchemaAttribute(newValue);
                    }
                    else if(attrName.equals(XmlConstants.TABLE_SCHEMA_ATTRIBUTE))
                    {
                        JOptionPane.showConfirmDialog(null,
                                "Please change the node from the context menu",
                                "Error on table node", JOptionPane.CLOSED_OPTION,
                                JOptionPane.ERROR_MESSAGE);
                    }
                }
                else if(elementName.equals(XmlConstants.ACCESS_ELEMENT))
                {
                    if(attrName.equals(XmlConstants.ACCESS_TABLE_ATTRIBUTE))
                    {
                        JOptionPane.showConfirmDialog(null,
                                "Please change the table reference in the graph.",
                                "Error on table", JOptionPane.CLOSED_OPTION,
                                JOptionPane.ERROR_MESSAGE);
                        
                    }
                }
                else if(elementName.equals(XmlConstants.FIELD_ELEMENT))
                {
                    if(attrName.equals(XmlConstants.FIELD_TYPE_ATTRIBUTE))
                    {
                         JOptionPane.showConfirmDialog(null,
                                "You don't want to change the field type. Please remove it and add a field with the proper type.",
                                "Error on field", JOptionPane.CLOSED_OPTION,
                                JOptionPane.ERROR_MESSAGE);
                    }
                }
            }
        }
    }
}


/////////////////////  end  ElementTreeCellEditor.java  ////////////////////////
