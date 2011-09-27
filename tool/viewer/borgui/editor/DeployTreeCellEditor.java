/*
 * DeployTreeCellEditor.java
 *
 * Created on February 13, 2006, 1:29 PM
 *
 * To change this template, choose Tools | Options and locate the template under
 * the Source Creation and Management node. Right-click the template and choose
 * Open. You can then make changes to the template in the Source Editor.
 */

package borgui.editor;

import borgui.visual.ServerButtonPanel;
import borgui.xml.*;
import javax.swing.*;
import java.awt.*;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import javax.swing.tree.*;
import org.jdom.Attribute;
import org.jdom.Element;

/**
 *
 * @author vivian
 */
public class DeployTreeCellEditor  extends DefaultTreeCellEditor
{
    protected BorealisModel      m_borealisModel;
    protected ServerButtonPanel  m_serverButtonPanel;

    
    /** Creates a new instance of DeployTreeCellEditor */
    public DeployTreeCellEditor(JTree tree, DefaultTreeCellRenderer renderer)
    {
        super(tree, renderer);
        
    }
   
 
    /**
     * Configures the editor. Passed onto the realEditor.
     */
    public Component getTreeCellEditorComponent(JTree     tree,
                                                Object    value,
                                                boolean   isSelected,
                                                boolean   expanded,
                                                boolean   leaf,
                                                int       row)
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
            {   comp.setBackground(tree.getBackground());
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
   
 
    public void setServerButtonPanel(ServerButtonPanel serverButtonPanel)
    {
         m_serverButtonPanel = serverButtonPanel;;
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
            
            if (m_borealisModel!=null && m_serverButtonPanel!=null)
            {
                String attrName = m_attr.getName();
                Element elem = ((Attribute)m_attr).getParent();
                String elemName = elem.getName();
                String oldValue = (String) e.getOldValue();
                String newValue = (String) e.getNewValue();
                
                if (elemName.equals(XmlConstants.NODE_ELEMENT))
                {
                    if (attrName.equals(XmlConstants.NODE_ENDPOINT_ATTRIBUTE))
                    {
                        m_borealisModel.changeNodeElementEndpoint(oldValue, newValue);
                        m_serverButtonPanel.changeServerButtonTip(oldValue, newValue);
                    }
                }
            }
        }
    }
}


////////////////////////  end  DeployTreeCellEditor.java  //////////////////////
