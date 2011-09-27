package borgui.editor;

import javax.swing.*;
import javax.swing.tree.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import org.jdom.Element;
import org.jdom.Attribute;
import borgui.xml.*;
import borgui.common.*;

/**
 * A class that represents a element panel.
 */
public class ElementPanel extends JPanel
{
    protected ElementTreeEditor  m_elementTreeEditor;
    protected XmlEditor          m_xmlEditor;
    protected DeployTreeEditor   m_deployXmlEditor = null;
    
    protected Element            m_element;
    protected JTabbedPane        m_tabbedPane;
    
    protected BorealisModel      m_borealisModel;
    protected boolean            m_forEditorPanel = true;
    

    /**
     * Constructor.
     */
    public ElementPanel(BorealisModel borealisModel, boolean forEditorPanel)
    {
        super();

        m_borealisModel = borealisModel;
        m_elementTreeEditor = new ElementTreeEditor(m_borealisModel);
        m_xmlEditor = new XmlEditor();
        
        setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
        
        m_tabbedPane = new JTabbedPane();
        
        if (forEditorPanel)
        {
            m_deployXmlEditor = new DeployTreeEditor(m_borealisModel); 
            m_tabbedPane.addTab("Element Editor", null, m_elementTreeEditor,"Edit element");
            m_tabbedPane.addTab("Element XML", null, m_xmlEditor ,"Edit element XML");
            m_tabbedPane.addTab("Deployment XML", null,m_deployXmlEditor,"Desigh the deployment XML");
            m_tabbedPane.setMnemonicAt(2, KeyEvent.VK_3);
        }
        else
        {
            m_tabbedPane.addTab("ElementTree", null, m_elementTreeEditor,"Edit element");
            m_tabbedPane.addTab("XMLDoc", null, m_xmlEditor ,"Edit element XML");
        }
        
        m_tabbedPane.setMnemonicAt(0, KeyEvent.VK_1);
        m_tabbedPane.setMnemonicAt(1, KeyEvent.VK_2);
        
        // add scrollPane to this panel
        add(m_tabbedPane);
        
    }
    
    /**
     * Sets the preferred size of this component to a constant value.
     */
    public void setPreferredSize(Dimension preferredSize)
    {
        super.setPreferredSize(preferredSize);
        m_tabbedPane.setPreferredSize(preferredSize);
        
        m_elementTreeEditor.setPreferredSize(preferredSize);
        //m_xmlEditor.setPreferredSize(preferredSize);

        if (m_deployXmlEditor!=null)
        {
            m_deployXmlEditor.setPreferredSize(preferredSize);
        }
    }
    
    public void setFocusTabDeploy()
    {
        m_tabbedPane.setSelectedIndex(2);
    }
    
    /**
     * Set the borealis element for the tree.
     */
    public void setElement(BorealisElement element)
    {
        m_element = element;
        Vector<Object> childElements = new Vector<Object>();

        if (element.getName().equals(XmlConstants.BOX_ELEMENT))
        {
            childElements.add(new ParameterElement("param_name","value"));
            //m_elementTreeEditor.getAddButton().setActionCommand("AddParam");
        }
        else if (element.getName().equals(XmlConstants.SCHEMA_ELEMENT))
        {
            childElements.add(new FieldElement("field_name",XmlConstants.SCHEMA_BYTE_TYPE));
            childElements.add(new FieldElement("field_name",XmlConstants.SCHEMA_SHORT_TYPE));
            childElements.add(new FieldElement("field_name",XmlConstants.SCHEMA_INT_TYPE));
            childElements.add(new FieldElement("field_name",XmlConstants.SCHEMA_LONG_TYPE));
            childElements.add(new FieldElement("field_name",XmlConstants.SCHEMA_SINGLE_TYPE));
            childElements.add(new FieldElement("field_name",XmlConstants.SCHEMA_DOUBLE_TYPE));
            childElements.add(new FieldElement("field_name",XmlConstants.SCHEMA_STRING_TYPE, 20));
            childElements.add(new FieldElement("field_name",XmlConstants.SCHEMA_TIMESTAMP_TYPE));
            //m_elementTreeEditor.getAddButton().setActionCommand("AddField");
        }
        else if (element.getName().equals(XmlConstants.OUTPUT_ELEMENT))
        {
            SchemaElement schemaElement;
            java.util.List schemaList = m_borealisModel.getDocument().getAllSchemas();

            if (schemaList.size()!=0)
            {
                schemaElement = (SchemaElement)schemaList.get(0);
            }
            else
            {
                schemaElement = new SchemaElement();
                m_borealisModel.getDocument().addSchema(schemaElement);
            }
            
            childElements.add(schemaElement.getNameValue());
        }

        m_elementTreeEditor.setElement(element, childElements);
        m_xmlEditor.setText(XmlHandler.elementToXml(element));
        m_tabbedPane.setSelectedIndex(0);
    }
    
    public void setEditable(boolean bool)
    {
        m_elementTreeEditor.setEditable(bool);
    }
    
    public DeployTreeEditor getDeployTreeEditor()
    {
        return m_deployXmlEditor;
    }
    
    public void showTreeUpdate()
    {
        m_elementTreeEditor.showTreeUpdate();
        m_deployXmlEditor.showTreeUpdate();
    }
}


////////////////////////  end ElementPanel.java  ///////////////////////////////
