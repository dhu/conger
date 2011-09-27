/*
 * TreePanel.java
 *
 * Created on July 20, 2005, 5:45 PM
 */

package borgui.generator;

import javax.swing.*;
import javax.swing.tree.*;
import javax.swing.event.*;
import org.jdom.*;
import org.jdom.filter.*;
import java.util.*;

import borgui.editor.*;
import borgui.xml.*;
import java.awt.BorderLayout;


/**
 * A class that implement a panel to show the query network XML tree
 *
 * @author Wenjuan Xing (vivian@cs.brown.edu)
 * version 1.0 10/11/2005
 */
public class TreePanel extends JPanel
{
    protected InputGenPanel      m_inputGenPanel;
    protected BorealisElement    m_rootElementGenTree;
    protected ElementTreeModel   m_elementTreeModel;
    protected JTree              m_elementTree;
    protected BorealisDocument   m_originalBorealisDoc = new BorealisDocument();
    protected BorealisDocument   m_generatorDoc        = new BorealisDocument();
    
    
    /** 
     * Creates a new instance of TreePanel 
     */
    public TreePanel(InputGenPanel inputGenPanel)
    {
        super();

        m_inputGenPanel = inputGenPanel;
        m_rootElementGenTree = new NetworkElement();
        
        m_elementTreeModel = new ElementTreeModel((BorealisElement)m_rootElementGenTree);
        m_elementTree = new JTree(m_elementTreeModel);
        m_elementTree.addTreeSelectionListener(new NetworkTreeListener());
        
        DefaultTreeCellRenderer cellRenderer = new ElementTreeCellRenderer();
        m_elementTree.setCellRenderer(cellRenderer);
        ElementTreeCellEditor cellEditor = new ElementTreeCellEditor(m_elementTree, cellRenderer);
        m_elementTree.setCellEditor(cellEditor);
        this.setLayout(new BorderLayout());
        add(Box.createHorizontalStrut(10),BorderLayout.WEST);
        add(m_elementTree,BorderLayout.CENTER);
    }

    
    /**
     * Set the XML document that to be displayed
     *
     * @param the document to be displayed
     */
    public void setBorealisDoc(Document doc)
    {
        m_originalBorealisDoc = new BorealisDocument(doc);
    }
   
 
    /**
     * Set the root element to be displayed
     *
     * @param the root element to be showed
     */
    public void setElement(BorealisElement element)
    {
        m_rootElementGenTree = element;
        m_elementTreeModel.setElement(element);
    }
    

    /**
     * Display the given XML document
     *
     * @param the given XML document
     */
    public void displayDoc(Document doc)
    {
        this.setBorealisDoc(doc);
        m_generatorDoc = OriginalToGenDoc(m_originalBorealisDoc);
        setElement((BorealisElement)(m_generatorDoc.getRootElement()));
    }
   
 
    /**
     * Change the original query network XML file. Only leave the schema node and input stream node.
     *
     * @param borealisDoc the original query network XML file.
     */
    public BorealisDocument OriginalToGenDoc(BorealisDocument borealisDoc)
    {
        BorealisDocument resultDoc = (BorealisDocument) borealisDoc.clone();
        BorealisDocument copyDoc =(BorealisDocument) borealisDoc.clone();
        BorealisElement rootResultDocElement = (BorealisElement)resultDoc.getRootElement();
        rootResultDocElement.removeContent();
        
        ElementFilter schemaFilter = new ElementFilter(XmlConstants.SCHEMA_ELEMENT);
        List allSchemaElements = copyDoc.getRootElement().removeContent(schemaFilter);

        for (Iterator i = allSchemaElements.iterator(); i.hasNext(); )
        {
            BorealisElement schemaElement = (BorealisElement)i.next();
            rootResultDocElement.addContent(schemaElement);
        }

        ElementFilter inputFilter =
               new ElementFilter(XmlConstants.INPUT_ELEMENT);

        List allInputElements =
             copyDoc.getRootElement().removeContent(inputFilter);

        for (Iterator i = allInputElements.iterator(); i.hasNext(); )
        {
            BorealisElement inputElement = (BorealisElement)i.next();
            String schemaAttValue = inputElement.getAttributeValue(
                                        XmlConstants.INPUT_SCHEMA_ATTRIBUTE);

            List schemaElements = rootResultDocElement.getChildren(
                                        XmlConstants.SCHEMA_ELEMENT);

            for (Iterator j = schemaElements.iterator(); j.hasNext();)
            {
                BorealisElement schemaElement = (BorealisElement)j.next();

                String schemaName = schemaElement.getAttributeValue(
                                         XmlConstants.NAME_ATTRIBUTE);

                if (schemaName.equals(schemaAttValue))
                {
                    schemaElement.addContent(inputElement);
                }
            }
        }
        
        return resultDoc;
    }
    

    /**
     * A class that respond to the tree action.
     *
     */
    private class NetworkTreeListener implements TreeSelectionListener
    {
        public NetworkTreeListener() {}
        

        public void valueChanged(TreeSelectionEvent e)
        {
            // first path element
            TreePath path = e.getPath();
            Object last = path.getLastPathComponent();

            if (last instanceof BorealisElement)
            {
                BorealisElement element = (BorealisElement) path.getLastPathComponent();

                if (element.getName().startsWith(GenConstants.FIELD_NAME))
                {
                    String schemaName = element.getParentElement().getAttributeValue(
                                            XmlConstants.NAME_ATTRIBUTE).toString();

                    String nameAttrValue = element.getAttributeValue(
                                              XmlConstants.NAME_ATTRIBUTE);

                    String fieldName = element.getAttributeValue(
                                  XmlConstants.NAME_ATTRIBUTE).toString();

                    Element fieldDistElem = GenXmlHandler.getFieldDistElement(
                            m_inputGenPanel.m_genDistdoc,schemaName, fieldName);

                    Distribution distribution = GenXmlHandler.createDist(fieldDistElem);
                   
                    m_inputGenPanel.getFistDistPanel().initFieldConfig(
                            fieldName, 
                            distribution, 
                            schemaName, 
                            nameAttrValue);
                    m_inputGenPanel.createReview(fieldName);
                    
                    
                }
                else
                {   if (element.getName().startsWith(GenConstants.INPUT_NAME))
                    {
                        String schemaName = element.getParentElement().getAttributeValue(
                                                XmlConstants.NAME_ATTRIBUTE).toString();

                        String streamAttrValue = element.getAttributeValue(
                                                XmlConstants.INPUT_STREAM_ATTRIBUTE);

                        String streamName = element.getAttributeValue(
                                                XmlConstants.INPUT_STREAM_ATTRIBUTE).toString();

                        Element inputDistElement =
                                    GenXmlHandler.getInputDistElement(
                                                  m_inputGenPanel.m_genDistdoc,
                                                  streamName);

                        Distribution distribution = GenXmlHandler.createDist(
                                                         inputDistElement);
                        
                        m_inputGenPanel.createReview(streamName);
                        
                        String genInfoStreamNum = GenXmlHandler.getStreamNum(
                                                      m_inputGenPanel.m_genDistdoc,
                                                      streamName);

                        String genInfoTupleNum  = GenXmlHandler.getTupleNum(
                                                      m_inputGenPanel.m_genDistdoc,
                                                      streamName);
                        
                        m_inputGenPanel.getFistDistPanel().initInputStreamConfig(
                                                      streamName, 
                                                      distribution, 
                                                      schemaName, 
                                                      streamAttrValue, 
                                                      genInfoStreamNum,
                                                      genInfoTupleNum);
                    }
                }

                m_inputGenPanel.m_applyToPanel.clear();
                m_inputGenPanel.m_applyToPanel.setComponentsEnabled(false);
                
                m_inputGenPanel.repaint();
            }
        }
    }
}


/////////////////////////////  end  TreePanel.java  ////////////////////////////////
