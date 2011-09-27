/*
 * DeployXmlTreeEditor.java
 *
 * Created on February 6, 2006, 4:29 PM
 *
 *
 */

package borgui.editor;

import borgui.visual.PlotColor;
import borgui.visual.ServerButtonPanel;
import javax.swing.*;
import javax.swing.tree.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import org.jdom.Element;
import borgui.xml.*;

/**
 *
 * @author vivian
 */
public class DeployTreeEditor extends JPanel
{
    protected Vector<Element>       m_addElements;
    protected JPanel                m_optionPanel;
    protected JComboBox             m_addCombo;
    protected JButton               m_addButton;
    protected JButton               m_removeButton;
    protected JScrollPane           m_deployScrollPane;
    
    protected JTree                 m_elementTree;
    protected ElementTreeModel      m_elementTreeModel;
    protected BorealisModel         m_borealisModel;
    
    protected DeployElement         m_deployElement;
    protected DeployDocument        m_deployDoc;
    protected ServerButtonPanel     m_serverButtonPanel;
    protected DeployTreeCellEditor  m_deployTreeCellEditor;
    
    /** Creates a new instance of DeployXmlTreeEditor */
    public DeployTreeEditor(BorealisModel borealisModel)
    {
        super();

        m_deployDoc = new DeployDocument();
        m_deployElement = m_deployDoc.getDeploy();
        m_borealisModel = borealisModel;

        this.setup();
    }
    
    
    /**
     * Sets the preferred size of this component to a constant value.
     */
    public void setPreferredSize(Dimension preferredSize)
    {
        super.setPreferredSize(preferredSize);

        m_deployScrollPane.setPreferredSize(new Dimension((int) preferredSize.getWidth(),
                (int) preferredSize.getHeight() - 20));

        m_optionPanel.setPreferredSize(new Dimension((int) preferredSize.getWidth(),
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
        m_elementTreeModel = new ElementTreeModel(m_deployElement);

        // tree
        m_elementTree = new JTree(m_elementTreeModel);
        
        //custom renderers and editors
        DefaultTreeCellRenderer cellRenderer = new DeployTreeCellRenderer();  
        m_elementTree.setCellRenderer(cellRenderer); 
        m_deployTreeCellEditor = new DeployTreeCellEditor(m_elementTree, cellRenderer);
        m_deployTreeCellEditor.setBorealisModel(m_borealisModel);
        m_elementTree.setCellEditor(m_deployTreeCellEditor);
        
        // make editable
        m_elementTree.setEditable(true);
        m_elementTree.getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);
        m_elementTree.setShowsRootHandles(true);
        
        m_deployScrollPane = new JScrollPane(m_elementTree);
        m_deployScrollPane.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_AS_NEEDED);
        m_deployScrollPane.setHorizontalScrollBarPolicy(ScrollPaneConstants.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        
        add(m_deployScrollPane);
        
        // options
        m_optionPanel = new JPanel();
        m_optionPanel.setLayout(new BoxLayout(m_optionPanel, BoxLayout.X_AXIS));
        
        m_addButton = new JButton("Add");
        m_optionPanel.add(m_addButton);
        
        m_addElements = new Vector<Element>();
        m_addElements.add(new ReplicSetElement("replic_set_name", ""));
        m_addElements.add(new NodeElement(":15000", ""));
        m_addElements.add(new RegionElement(":15000", "127.0.0.1"));
        m_addElements.add(new PublishElement("stream1", "127.0.0.1"));
        m_addElements.add(new SubscribeElement("stream1", ":15000", "gap1"));
        m_addElements.add(new UnSubscribeElement("stream1"));
        m_addElements.add(new MoveElement("box1", "127.0.0.1", "table1"));
        
        Vector<String>  optionStrs = new Vector<String>();

        for(Iterator i = m_addElements.iterator(); i.hasNext(); )
        {
            optionStrs.add( ((Element) i.next()).getName() );
        }
        
        m_addCombo = new JComboBox(optionStrs);
        m_addCombo.setPreferredSize(new Dimension(60,20));
        m_optionPanel.add(m_addCombo);
        
        m_removeButton = new JButton("del");
        m_optionPanel.add(m_removeButton);
        
        m_addButton.addActionListener(new AddButtonListener());
        m_removeButton.addActionListener(new RemoveButtonListener());
        
        this.add(m_optionPanel);
        
    }
    
    public void setDeployDoc(DeployDocument deployDoc)
    {
        m_deployDoc = deployDoc;
        m_deployElement = deployDoc.getDeploy();
        m_elementTreeModel.setElement(m_deployElement);
    }
    
    
    
    public DeployDocument getDeployDoc()
    {
        return m_deployDoc;
    }
    
    public void setServerButtonPanel(ServerButtonPanel serverButtonPanel)
    {
        m_serverButtonPanel = serverButtonPanel;
        m_deployTreeCellEditor.setServerButtonPanel(m_serverButtonPanel);
    }


    /**
     * A class that response to add action.
     */
    private class AddButtonListener implements ActionListener
    {
        
        /**
         * Constructor.
         */
        public AddButtonListener() {}
        
        /**
         * Invoked when an action occurs.
         */
        public void actionPerformed(ActionEvent e)
        {
            Element element = (Element) m_addElements.get(m_addCombo.getSelectedIndex());
            
            if (element.getName().equals(XmlConstants.NODE_ELEMENT))
            {
                Double randomDouble = new Double(java.lang.Math.random()*10000);
                int nodeEndpointNum = (randomDouble).intValue();
                ((NodeElement)element).setEndpoint("127.0.0.1:" + (10000 + nodeEndpointNum));
                m_deployElement.addContent((Element)element.clone());
                
                Vector<String>  ipPorts = m_deployDoc.getIpPorts();
                PlotColor plotColor = new PlotColor(ipPorts);
                m_serverButtonPanel.setPlotColor(plotColor);
                m_serverButtonPanel.setServerIpPort(ipPorts);
                m_serverButtonPanel.getParent().getParent().getParent().repaint();
            }
            else
            {
                m_deployElement.addContent((Element)element.clone());
            }

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
                    m_deployElement.removeContent(deleteElement);
                    
                    boolean deleteResult = true;

                    if (deleteElement instanceof NodeElement)
                    {
                        Vector<String> ipPorts = m_deployDoc.getIpPorts();

                        PlotColor plotColor = new PlotColor(ipPorts);

                        m_serverButtonPanel.setPlotColor(plotColor);
                        m_serverButtonPanel.setServerIpPort(ipPorts);
                        m_serverButtonPanel.getParent().getParent().getParent().repaint();
                        
                        m_borealisModel.changeNodeElementEndpoint(
                           ((NodeElement)deleteElement).getEndpoint(), "");

                        //m_borealisModel.getGraph().setDeploydoc(m_deployDoc);
                        m_borealisModel.getGraph().changeNodeColor(plotColor);
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

    
    public void showTreeUpdate()
    {
        m_elementTreeModel.showTreeUpdate();
    }
}

////////////////////////////  end DeployTreeEditor.java  ///////////////////////
