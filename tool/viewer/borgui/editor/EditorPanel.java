package borgui.editor;

import javax.swing.*;
import javax.swing.tree.*;
import javax.swing.event.*;
import java.awt.*;
import java.awt.event.*;
import java.io.File;
import java.util.*;
import org.jdom.*;
import javax.swing.text.PlainDocument;
import org.xml.sax.InputSource;
import java.io.*;
import borgui.xml.*;

import borgui.common.SwingPanel;
import borgui.common.DebugTrace;
import borgui.client.*;
import borgui.graph.*;
import borgui.visual.PlotColor;
import borgui.visual.ServerButtonPanel;
import org.jgraph.event.GraphSelectionListener;
import org.jgraph.event.GraphSelectionEvent;



/**
 * A class that represent the editor panel to edit the digram.
 */
public class EditorPanel extends JPanel implements Editor
{
    protected Client              m_client;
    protected int                 m_select;
    protected ActionHandler       m_actionHandler;

    protected Toolbar             m_toolbar;
    protected XmlEditor           m_xmlEditor;
    protected JScrollPane         m_editorView;
    protected ElementPanel        m_elementPanel;
    protected ServerButtonPanel   m_serverButtonPanel = new ServerButtonPanel();
    protected GraphButtonPanel    m_graphButtonPanel;
    protected GraphOverviewPanel  m_graphOverview;

    protected NetworkTreeModel    m_networkTreeModel;
    protected BorealisElement     m_selectedElem;
    protected BorealisGraph       m_borealisGraph;
    protected BorealisModel       m_borealisModel;

    protected int                 m_newNameNumber = 1;


     ///////////////////////////////////////////////////////////////////////////
     //
     // Creates a new instance of EditorPanel.
     //
     public EditorPanel(Client client)
     {
     //.........................................................................


        super();

        m_client = client;

        m_actionHandler = new ActionHandler(this);

        ////////////////////////////////////////////////////////////////////////
        // tool bar
        //
        m_toolbar = new Toolbar();
        m_toolbar.setFloatable(false);

        ////////////////////////////////////////////////////////////////////////
        // toolbox tabbed pane
        //
        JPanel leftPanel = new JPanel(); leftPanel.setLayout(new BoxLayout(leftPanel,
        BoxLayout.Y_AXIS)); leftPanel.setPreferredSize(new Dimension(200, 600));

        m_graphButtonPanel = new GraphButtonPanel(this);
        leftPanel.setPreferredSize(new Dimension(200, 200));
        leftPanel.add(m_graphButtonPanel);


        ////////////////////////////////////////////////////////////////////////
        // main diagram
        // diagram
        // xml editor
        //
        m_xmlEditor = new XmlEditor();
        m_xmlEditor.setPreferredSize(new Dimension(600, 800));

        m_borealisGraph = new BorealisGraph();
        m_borealisGraph.addGraphSelectionListener(new EditorGraphSelectionListener());
        m_editorView = new JScrollPane(m_borealisGraph);
        m_editorView.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_AS_NEEDED);
        m_editorView.setHorizontalScrollBarPolicy(ScrollPaneConstants.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        m_editorView.setPreferredSize(new Dimension(600, 600));

        m_borealisModel = new BorealisModel(this,
                                            new BorealisDocument(),
                                            m_borealisGraph.getBorealisGraphModel(),
                                            m_borealisGraph);

        m_borealisModel.getDocument().setNetwork(new NetworkElement());


        ////////////////////////////////////////////////////////////////////////
        //
        m_elementPanel = new ElementPanel(m_borealisModel, true);
        m_elementPanel.setPreferredSize(new Dimension(200, 400));
        m_elementPanel.getDeployTreeEditor().setServerButtonPanel(m_serverButtonPanel);

        leftPanel.add(m_elementPanel);


        ////////////////////////////////////////////////////////////////////////
        //
        JPanel rightPanel = new JPanel();
        rightPanel.setLayout(new BoxLayout(rightPanel, BoxLayout.Y_AXIS));
        rightPanel.setPreferredSize(new Dimension(200, 600));


        ////////////////////////////////////////////////////////////////////////
        // diagram viewer
        //
        JPanel graphOverview = GraphOverviewPanel.createGraphOverviewPanel(m_borealisGraph, m_editorView.getViewport());
        graphOverview.setPreferredSize(new Dimension(200, 200));
        m_graphOverview = ((GraphOverviewPanelWrapper) graphOverview).getOverview();
        rightPanel.add(graphOverview);


        ////////////////////////////////////////////////////////////////////////
        // query/schema add buttons
        //
        JComponent rightAddPanel = new JPanel();

        rightAddPanel.setLayout(new BoxLayout(rightAddPanel, BoxLayout.X_AXIS));
        rightAddPanel.setPreferredSize(new Dimension(200, 25));

        JButton addQueryButton = new JButton("Add Query");
        addQueryButton.setHorizontalAlignment(SwingConstants.LEFT);
        addQueryButton.setToolTipText("Add new query to Borealis");
        addQueryButton.addActionListener(new AddQueryButtonListener());
        rightAddPanel.add(addQueryButton);

        JButton addSchemaButton = new JButton("Add Schema");
        addSchemaButton.setHorizontalAlignment(SwingConstants.LEFT);
        addSchemaButton.setToolTipText("Add new schema to Borealis");
        addSchemaButton.addActionListener(new AddSchemaButtonListener());
        rightAddPanel.add(addSchemaButton);

        m_serverButtonPanel.setPreferredSize(new Dimension(192, 105));
        m_serverButtonPanel.setMinimumSize(new Dimension(192, 105));
        m_serverButtonPanel.setBackground(Color.white);


        rightPanel.add(m_serverButtonPanel);
        rightPanel.add(rightAddPanel);


        //temp
        //
        m_networkTreeModel = new NetworkTreeModel(m_borealisModel.getDocument());
        JTree networkTree = new JTree(m_networkTreeModel);
        networkTree.addTreeSelectionListener(new NetworkTreeListener());
        networkTree.setCellRenderer(new NetworkTreeCellRenderer());

        JScrollPane treeScrollPane = new JScrollPane(networkTree);
        treeScrollPane.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_AS_NEEDED);
        treeScrollPane.setHorizontalScrollBarPolicy(ScrollPaneConstants.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        treeScrollPane.setPreferredSize(new Dimension(200,300));
        rightPanel.add(treeScrollPane);


        //end
        //set borealisGraph
        m_borealisGraph.setClient(m_client);
        m_borealisGraph.setBorealisModel(m_borealisModel);
        m_borealisGraph.setNetworkTreeModel(m_networkTreeModel);
        m_borealisGraph.setDeploydoc(getDeployDoc());
        //m_borealisGraph.setDeployXmlTreeEditor(m_elementPanel.getDeployXmlTreeEditor());


        ////////////////////////////////////////////////////////////////////////
        // messages tabbed pane
        JTabbedPane messagesTabbedPane = new JTabbedPane();

        //
        JComponent panel3 = new JPanel();
        JTextPane jpt = new JTextPane();

        jpt.setPreferredSize(new Dimension(800, 100));
        panel3.add(jpt);
        messagesTabbedPane.addTab("Typecheck Output", null, panel3, "");
        messagesTabbedPane.setMnemonicAt(0, KeyEvent.VK_1);

        //
        JComponent panel4 = new JPanel();
        messagesTabbedPane.addTab("Engine Output", null, panel4, "");
        messagesTabbedPane.setMnemonicAt(1, KeyEvent.VK_2);


        ////////////////////////////////////////////////////////////////////////
        //
        JPanel centerPanel = new JPanel();
        centerPanel.setLayout(new BoxLayout(centerPanel, BoxLayout.Y_AXIS));
        centerPanel.add(m_editorView);
        centerPanel.add(messagesTabbedPane);


        ////////////////////////////////////////////////////////////////////////
        // layout stuff
        //
        this.setLayout(new BorderLayout());
        this.add(m_toolbar, BorderLayout.NORTH);
        this.add(leftPanel, BorderLayout.WEST);
        this.add(rightPanel, BorderLayout.EAST);
        this.add(centerPanel, BorderLayout.CENTER);

        ////////////////////////////////////////////////////////////////////////
        // add menu options to main frame
        // create menus
        //
        JMenuItem menuItem = null;

        // add "File" menu options
        //
        menuItem = new JMenuItem("New NetworkGraph");
        menuItem.addActionListener(m_actionHandler.getAction(ActionHandler.NEW_GRAPH_ACTION));
        m_client.addFileMenuItem(menuItem);

        menuItem = new JMenuItem("Load NetworkGraph");
        menuItem.addActionListener(m_actionHandler.getAction(ActionHandler.LOAD_GRAPH_ACTION));
        m_client.addFileMenuItem(menuItem);

        menuItem = new JMenuItem("Save NetworkGraph");
        menuItem.addActionListener(m_actionHandler.getAction(ActionHandler.SAVE_GRAPH_ACTION));
        m_client.addFileMenuItem(menuItem);

        menuItem = new JMenuItem("Quit");
        menuItem.addActionListener(m_actionHandler.getAction(ActionHandler.QUIT_ACTION));
        m_client.addFileMenuItem(menuItem);

        // add "View" menu options
        //
        menuItem = new JMenuItem("Diagram");
        menuItem.addActionListener(m_actionHandler.getAction(ActionHandler.VIEWDIAG_ACTION));
        m_client.addViewMenuItem(menuItem);

        menuItem = new JMenuItem("Xml");
        menuItem.addActionListener(m_actionHandler.getAction(ActionHandler.VIEWXML_ACTION));
        m_client.addViewMenuItem(menuItem);


        //delete menu:  NYI
        /*
        menuItem = new JMenuItem("Re-Load");
        menuItem.addActionListener(m_actionHandler.getAction(ActionHandler.RELOAD_NETWORK_ACTION));
        m_client.addFileMenuItem(menuItem);

        menuItem = new JMenuItem("Import");
        menuItem.addActionListener(m_actionHandler.getAction(ActionHandler.IMPORT_NETWORK_ACTION));
        m_client.addFileMenuItem(menuItem);

        // add "Edit" menu options
        //
        menuItem = new JMenuItem("Copy");
        menuItem.addActionListener(m_actionHandler.getAction(ActionHandler.COPY_ACTION));
        m_client.addEditMenuItem(menuItem);

        menuItem = new JMenuItem("Cut");
        menuItem.addActionListener(m_actionHandler.getAction(ActionHandler.REMOVE_ACTION));
        m_client.addEditMenuItem(menuItem);

        menuItem = new JMenuItem("Paste");
        menuItem.addActionListener(m_actionHandler.getAction(ActionHandler.PASTE_ACTION));
        m_client.addEditMenuItem(menuItem);
         */


        ////////////////////////////////////////////////////////////////////////
        // final setup
        //
        this.setPreferredSize(new Dimension(1300, 700));
        this.setVisible(true);

    }


    public DeployDocument getDeployDoc()
    {
        return m_elementPanel.getDeployTreeEditor().getDeployDoc();
    }


    public void setDeployDoc(DeployDocument deployDoc)
    {
        m_elementPanel.getDeployTreeEditor().setDeployDoc(deployDoc);
        //test
        m_borealisGraph.setDeploydoc(deployDoc);
    }


    public BorealisModel getModel()
    {
        return m_borealisModel;
    }


    public BorealisGraph getBorealisGraph()
    {
        return m_borealisGraph;
    }


    public NetworkTreeModel getNetworkTreeModel()
    {
        return m_networkTreeModel;
    }


    public void setSelectedElement(BorealisElement element)
    {
        m_selectedElem = element;
        m_elementPanel.setElement(m_selectedElem);
    }


    public void newGraph()
    {
        m_borealisModel.getDocument().setDocument(new BorealisDocument());

        try
        {   String  warning =
                BorealisModelXmlHandler.loadBorealisModel(m_borealisModel);

            if (!warning.equals(""))
            {   System.out.println("newGraph:  " + warning);
            }

            m_networkTreeModel.showTreeUpdate();
            m_graphOverview.setGraphModel(m_borealisModel.getGraphModel());
        }
        catch (Exception  e)
        {   System.out.println("newGraph:  " + e.getMessage());
        }
    }


    public void saveGraph()
    {
        File file = SwingPanel.showFileDialog("Save Graph");

        // if a file was chosen to save
        if (file != null)
        {
            try
            {
                XmlHandler.docToXml(m_borealisModel.getDocument(), new FileOutputStream(file));
            }
            catch(java.io.FileNotFoundException e)
            {
                System.err.println(e.getMessage());
            }
        }
    }


    ////////////////////////////////////////////////////////////////////////////
    //
    public void loadGraph()
    {
    //..........................................................................


        File  file = SwingPanel.showFileDialog("Load Graph");

        // if a file was chosen to load
        //
        if (file != null)
        {
            try
            {   Document  doc = XmlHandler.xmlToDoc(file);

                if (!doc.getRootElement().getName().equals(
                                              XmlConstants.NETWORK_ELEMENT))
                {
                    JOptionPane.showConfirmDialog(null,
                        "The file you chose is not a Borealis network XML file.",
                        "Not Query XML", JOptionPane.CLOSED_OPTION,
                        JOptionPane.WARNING_MESSAGE);
                }
                else
                {   BorealisDocument borealisDoc = new BorealisDocument(doc);

                    m_borealisModel.getDocument().setDocument(
                                   (BorealisDocument)borealisDoc.clone());

                    String  warning = BorealisModelXmlHandler.loadBorealisModel(
                                                               m_borealisModel);

                    if (!warning.equals(""))
                    {   DebugTrace.trace(warning);

                        JOptionPane.showConfirmDialog(null, warning,
                                                      "XML parse warning",
                                           JOptionPane.CLOSED_OPTION,
                                           JOptionPane.WARNING_MESSAGE);
                    }
 
                    m_graphOverview.setGraphModel(
                                       m_borealisModel.getGraphModel());

                    m_elementPanel.setFocusTabDeploy();

                    Vector<String>  ipPorts = borealisDoc.getAllDistinctIpPortsString();
                    getDeployDoc().addNodes(ipPorts);

                    Vector<String>  newIpPorts = getDeployDoc().getIpPorts();
                    PlotColor plotColor = new PlotColor(newIpPorts);

                    m_serverButtonPanel.setPlotColor(plotColor);
                    m_serverButtonPanel.setServerIpPort(newIpPorts);
                    m_serverButtonPanel.getParent().getParent().getParent().repaint();

                    m_borealisGraph.setDeploydoc(getDeployDoc());

                    //  A default deployment needs to be applied here???
                    m_borealisGraph.changeNodeColor(plotColor);

                    m_elementPanel.showTreeUpdate();
                    m_networkTreeModel.showTreeUpdate();
                }
            }
            catch (Exception e)
            {   DebugTrace.trace(e.getMessage());

                JOptionPane.showConfirmDialog(null,
                                              e.getMessage(),
                                              "XML parse error",
                                              JOptionPane.CLOSED_OPTION,
                                              JOptionPane.ERROR_MESSAGE);
            }
        }
    }


    public void newDeploy()
    {
        setDeployDoc(new DeployDocument());
        m_serverButtonPanel.removeAllServers();
        //m_elementPanel.getDeployXmlTreeEditor().setServerButtonPanel(m_serverButtonPanel);
        m_elementPanel.setFocusTabDeploy();
        //m_borealisGraph.setDeploydoc(getDeployDoc());
        m_borealisModel.getDocument().clearAllNodesAttribute();
        m_borealisGraph.clearNodesColor();
    }


    public void saveDeploy()
    {
        File file = SwingPanel.showFileDialog("Save Deploy XML");

        // if a file was chosen to save
        if (file != null)
        {
            try
            {
                XmlHandler.docToXml(getDeployDoc(), new FileOutputStream(file));
            }
            catch(java.io.FileNotFoundException e)
            {
                DebugTrace.trace(e.getMessage());
            }
        }

        m_elementPanel.setFocusTabDeploy();
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    //  Load a deployment XML file.
    //
    public void loadDeploy()
    {
    //..........................................................................


        File file = SwingPanel.showFileDialog("Load Deployment XML");

        if (file != null)            // if a file was chosen to load
        {   try
            {   Document  doc = XmlHandler.xmlToDoc(file);

                if (!doc.getRootElement().getName().equals(
                                              XmlConstants.DEPLOY_ELEMENT))
                {
                    JOptionPane.showConfirmDialog(null,
                            "The file you choosed is not a deploy XML file.",
                            "Not Deploy XML", JOptionPane.CLOSED_OPTION,
                            JOptionPane.WARNING_MESSAGE);
                }
                else
                {   setDeployDoc(new DeployDocument(doc));

                    BorealisModelXmlHandler.applyDeployment(m_borealisModel,
                                                            getDeployDoc());

                    Vector<String>  ipPorts = m_borealisModel.getDocument().
                                        getAllDistinctIpPortsString();

                    getDeployDoc().addNodes(ipPorts);

                    Vector<String> newIpPorts = getDeployDoc().getIpPorts();

                    PlotColor  plotColor = new PlotColor(newIpPorts);
                    m_serverButtonPanel.setPlotColor(plotColor);

                    m_serverButtonPanel.setServerIpPort(newIpPorts);
                    m_serverButtonPanel.getParent().getParent().getParent().repaint();

                    m_borealisGraph.setDeploydoc(getDeployDoc());
                    m_borealisGraph.changeNodeColor(plotColor);

                    m_elementPanel.showTreeUpdate();
                    m_networkTreeModel.showTreeUpdate();
                }
            }
            catch (IOException e)
            {   DebugTrace.trace(e.getMessage());

                JOptionPane.showConfirmDialog(null,
                                              e.getMessage(),
                                              "Could not open an XML file.",
                                              JOptionPane.CLOSED_OPTION,
                                              JOptionPane.ERROR_MESSAGE);
            }
            catch (Exception e)
            {   DebugTrace.trace(e.getMessage());

                JOptionPane.showConfirmDialog(null,
                                              e.getMessage(),
                                              "XML parse error",
                                              JOptionPane.CLOSED_OPTION,
                                              JOptionPane.ERROR_MESSAGE);
            }
        }

        m_elementPanel.setFocusTabDeploy();
    }


    public void showDiagView()
    {
        m_editorView.setViewportView(m_borealisGraph);
    }


    public void showXmlView()
    {
        PlainDocument doc = new PlainDocument();
        ByteArrayOutputStream stream = new ByteArrayOutputStream();

        XmlHandler.docToXml(m_borealisModel.getDocument(), stream);

        m_xmlEditor.setText(stream.toString());
        m_editorView.setViewportView(m_xmlEditor);
    }


    public void setSelection(int type)
    {
        m_select = type;
    }


    private class EditorGraphSelectionListener
       implements GraphSelectionListener
    {
        public void valueChanged(GraphSelectionEvent e)
        {
            BorealisElement element = null;
            BorealisModel model = getModel();

            Object[] selectedCells = e.getCells();

            for(int i = 0; i < selectedCells.length && element == null; i++)
            {
                Object cell = selectedCells[i];

                // if a graph cell
                if (cell instanceof BorealisGraphCell)
                {
                    BorealisGraphCell graphCell = (BorealisGraphCell) cell;
                    element = model.getElement(graphCell);
                }
            }

            if (element != null)
            {
                setSelectedElement(element);
            }

            boolean enabled = !m_borealisGraph.isSelectionEmpty();
            m_toolbar.removeBut.setEnabled(enabled);
        }
    }


    protected class Toolbar extends JToolBar
    {
        // private JComboBox zoom;
        //
        public JButton undoBut;
        public JButton redoBut;
        public JButton removeBut;

        /**
         * Constructs a new Toolbar object
         */
        public Toolbar()
        {
            super();

            // create buttons
            //
            JButton newGraphBut = new JButton(new ImageIcon("images/new_Graph.gif"));
            JButton saveGraphBut = new JButton(new ImageIcon("images/save_Graph.gif"));
            JButton loadGraphBut = new JButton(new ImageIcon("images/load_Graph.gif"));

            JButton newDeployBut = new JButton(new ImageIcon("images/new_Deploy.gif"));
            JButton saveDeployBut = new JButton(new ImageIcon("images/save_Deploy.gif"));
            JButton loadDeployBut = new JButton(new ImageIcon("images/load_Deploy.gif"));



            JButton orgagniazeBut = new JButton(new ImageIcon("images/refresh.gif"));
            removeBut = new JButton(new ImageIcon("images/delete.gif"));
            JButton zoomBut = new JButton(new ImageIcon("images/zoom.gif"));
            JButton zoominBut = new JButton(new ImageIcon("images/zoomin.gif"));
            JButton zoomoutBut = new JButton(new ImageIcon("images/zoomout.gif"));


            // set tooltip text
            //
            newGraphBut.setToolTipText("Create a new query network graph");
            saveGraphBut.setToolTipText("Save the query network graph");
            loadGraphBut.setToolTipText("Load a query network graph");

            newDeployBut.setToolTipText("Create a new deployment XML");
            saveDeployBut.setToolTipText("Save the deployment XML");
            loadDeployBut.setToolTipText("Load a deployment XML");

            orgagniazeBut.setToolTipText("Organize the graph");
            removeBut.setToolTipText("Remove");
            removeBut.setEnabled(false);

            // add action listeners
            //
            newGraphBut.addActionListener(m_actionHandler.getAction(ActionHandler.NEW_GRAPH_ACTION));
            saveGraphBut.addActionListener(m_actionHandler.getAction(ActionHandler.SAVE_GRAPH_ACTION));
            loadGraphBut.addActionListener(m_actionHandler.getAction(ActionHandler.LOAD_GRAPH_ACTION));

            newDeployBut.addActionListener(m_actionHandler.getAction(ActionHandler.NEW_DEPLOY_ACTION));
            saveDeployBut.addActionListener(m_actionHandler.getAction(ActionHandler.SAVE_DEPLOY_ACTION));
            loadDeployBut.addActionListener(m_actionHandler.getAction(ActionHandler.LOAD_DEPLOY_ACTION));

            removeBut.addActionListener(m_actionHandler.getAction(ActionHandler.REMOVE_ACTION));
            orgagniazeBut.addActionListener(m_actionHandler.getAction(ActionHandler.ORGAGNIZE_ACTION));
            zoomBut.addActionListener(m_actionHandler.getAction(ActionHandler.ZOOM_ACTION));
            zoominBut.addActionListener(m_actionHandler.getAction(ActionHandler.ZOOMIN_ACTION));
            zoomoutBut.addActionListener(m_actionHandler.getAction(ActionHandler.ZOOMOUT_ACTION));

            /*  NYI
            undoBut = new JButton(new ImageIcon("images/undo.gif"));
            redoBut = new JButton(new ImageIcon("images/redo.gif"));
            undoBut.setToolTipText("Undo");
            undoBut.setEnabled(false);
            redoBut.setToolTipText("Redo");
            redoBut.setEnabled(false);
            undoBut.addActionListener(m_actionHandler.getAction(ActionHandler.UNDO_ACTION));
            redoBut.addActionListener(m_actionHandler.getAction(ActionHandler.REDO_ACTION));
             */

            // filler panel
            //
            JPanel filler = new JPanel();

            // radio buttons
            //
            JRadioButton diagBut = new JRadioButton("Diag");
            JRadioButton xmlBut = new JRadioButton("Xml");
            diagBut.setSelected(true);

            // add to but group
            //
            ButtonGroup editorButGroup = new ButtonGroup();
            editorButGroup.add(diagBut);
            editorButGroup.add(xmlBut);

            diagBut.addActionListener(m_actionHandler.getAction(ActionHandler.VIEWDIAG_ACTION));
            xmlBut.addActionListener(m_actionHandler.getAction(ActionHandler.VIEWXML_ACTION));

            /*
            String[] zoomStrs = { "200%", "150%", "100%", "90%", "80%", "70%",
                  "60%", "50%", "10%" };
            zoom = new JComboBox(zoomStrs);
            zoom.setSelectedItem("100%");
            zoom.addItemListener(new ZoomListener(zoom, this));
            add(zoom);
            */

            // add all components to toolbar
            //
            add(newGraphBut);
            add(loadGraphBut);
            add(saveGraphBut);

            addSeparator();

            add(newDeployBut);
            add(loadDeployBut);
            add(saveDeployBut);
            /*
            add(undoBut);
            add(redoBut);
             */

            addSeparator();

            add(removeBut);
            add(orgagniazeBut);

            addSeparator();

            add(zoomBut);
            add(zoominBut);
            add(zoomoutBut);
            add(filler);
            add(diagBut);
            add(xmlBut);
        }
    }


    // Update Undo/Redo Button State based on Undo Manager
    //
    protected void updateHistoryButtons()
    {
        // The View Argument Defines the Context
        //temp
        /*
        m_toolbar.undoBut.setEnabled(m_borealisModel.getUndoManager().canUndo(m_borealisGraph.getGraphLayoutCache()));
        m_toolbar.redoBut.setEnabled(m_borealisModel.getUndoManager().canRedo(m_borealisGraph.getGraphLayoutCache()));
         */
    }


    private class AddSchemaButtonListener implements ActionListener
    {
        public AddSchemaButtonListener()  {}

        public void actionPerformed(ActionEvent e)
        {
            final SchemaElement element = new SchemaElement("NewName" + m_newNameNumber++);

            JDialog diag = new JDialog(m_client, "Add New Schema To Borealis");

            Vector<FieldElement> childElements = new Vector<FieldElement>();

            childElements.add(new FieldElement("field_name",XmlConstants.SCHEMA_BYTE_TYPE));
            childElements.add(new FieldElement("field_name",XmlConstants.SCHEMA_SHORT_TYPE));
            childElements.add(new FieldElement("field_name",XmlConstants.SCHEMA_INT_TYPE));
            childElements.add(new FieldElement("field_name",XmlConstants.SCHEMA_LONG_TYPE));
            childElements.add(new FieldElement("field_name",XmlConstants.SCHEMA_SINGLE_TYPE));
            childElements.add(new FieldElement("field_name",XmlConstants.SCHEMA_DOUBLE_TYPE));
            childElements.add(new FieldElement("field_name",XmlConstants.SCHEMA_STRING_TYPE, 20));
            childElements.add(new FieldElement("field_name",XmlConstants.SCHEMA_TIMESTAMP_TYPE));

            ElementTreeEditor elementEditor = new ElementTreeEditor(
                                 m_borealisModel, element, childElements);

            elementEditor.m_addCombo.removeAllItems();
            elementEditor.m_addCombo.addItem("field ( " + XmlConstants.SCHEMA_BYTE_TYPE + " )");
            elementEditor.m_addCombo.addItem("field ( " + XmlConstants.SCHEMA_SHORT_TYPE + " )");
            elementEditor.m_addCombo.addItem("field ( " + XmlConstants.SCHEMA_INT_TYPE + " )");
            elementEditor.m_addCombo.addItem("field ( " + XmlConstants.SCHEMA_LONG_TYPE + " )");
            elementEditor.m_addCombo.addItem("field ( " + XmlConstants.SCHEMA_SINGLE_TYPE + " )");
            elementEditor.m_addCombo.addItem("field ( " + XmlConstants.SCHEMA_DOUBLE_TYPE + " )");
            elementEditor.m_addCombo.addItem("field ( " + XmlConstants.SCHEMA_STRING_TYPE + " )");
            elementEditor.m_addCombo.addItem("field ( " + XmlConstants.SCHEMA_TIMESTAMP_TYPE + " )");
            elementEditor.m_addCombo.setSelectedIndex(0);

            elementEditor.getAddButton().setText("Add Field");
            elementEditor.getAddButton().setActionCommand("AddField");
            elementEditor.getRemoveButton().setActionCommand("RemoveField");

            JButton addQueryButton = new JButton("Add Schema");

            addQueryButton.addActionListener
            (
                new ActionListener()
                {
                    public void actionPerformed(ActionEvent e)
                    {
                        if (element.getParent()==null)
                        {
                            if (m_borealisModel.getDocument().getAllSchemaNames().contains(
                                               element.getNameValue()))
                            {
                                JOptionPane.showConfirmDialog(null,
                                    "The schema name has already been used. Please change the name.",
                                    "Change Schema Name", JOptionPane.CLOSED_OPTION,
                                    JOptionPane.ERROR_MESSAGE);
                            }
                            else
                            {
                                m_borealisModel.getDocument().addSchema(
                                                        (SchemaElement)element);

                                JOptionPane.showConfirmDialog(null,
                                    "A schema has been added.",
                                    "AddSchema", JOptionPane.CLOSED_OPTION,
                                    JOptionPane.INFORMATION_MESSAGE);
                            }
                        }
                    }
                }
            );

            elementEditor.getOptionPanel().add(addQueryButton);

            diag.getContentPane().add(elementEditor);
            elementEditor.setPreferredSize(new Dimension(400,200));
            diag.setSize(new Dimension(400,200));
            diag.setVisible(true);
            m_networkTreeModel.showTreeUpdate();
        }
    }


    private class AddQueryButtonListener implements ActionListener
    {
        public AddQueryButtonListener() {}

        public void actionPerformed(ActionEvent e)
        {
            BorealisElement element = new QueryElement("NewName" + m_newNameNumber++);

            JDialog diag = new JDialog(m_client, "Add New Query To Borealis");

            Vector<BorealisElement> childElements = new Vector<BorealisElement>();

            childElements.add(element);

            ElementTreeEditor elementEditor =
                   new ElementTreeEditor(m_borealisModel, element, childElements);

            elementEditor.getComboBox().setEnabled(false);
            elementEditor.getComboBox().setVisible(false);

            elementEditor.getAddButton().setText("Add Query");
            elementEditor.getAddButton().setActionCommand("AddQuery");
            elementEditor.getRemoveButton().setVisible(false);

            diag.getContentPane().add(elementEditor);
            elementEditor.setPreferredSize(new Dimension(400,100));
            diag.setSize(new Dimension(400,100));
            diag.setVisible(true);
            m_networkTreeModel.showTreeUpdate();
        }
    }


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
                setSelectedElement(element);
            }
        }
    }
}


/////////////////////////  end EditorPanel.java  ///////////////////////////////
