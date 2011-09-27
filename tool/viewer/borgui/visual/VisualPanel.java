package borgui.visual;

import javax.swing.*;
import java.awt.*;
import java.util.*;
import java.awt.event.*;

import borgui.editor.*;
import borgui.graph.*;
import borgui.xml.*;
import borgui.common.SwingPanel;
import borgui.common.DebugTrace;
import borgui.common.HostEndpoint;

import java.io.*;
import org.jdom.*;
import org.xml.sax.InputSource;

import org.jgraph.graph.*;
import org.jgraph.event.GraphSelectionListener;
import org.jgraph.event.GraphSelectionEvent;



/**
 * A class that represents the visual panel.
 *
 * @author Wenjuan Xing (vivian@cs.brown.edu)
 * version 1.0 10/13/2005
 *
 */
public class VisualPanel  extends JPanel
                       implements Visual
{
    protected EditorPanel          m_editorPanel;
    protected VisualActionHandler  m_cActionHandler;

    protected JToolBar             m_jToolBar   = new JToolBar();

    protected JScrollPane          m_graphScrollPane;
    protected VisualGraph          m_visualGraph;
    protected GraphOverviewPanel   m_graphOverview;
    protected BorealisModel        m_borealisModel;

    protected BorealisElement      m_selectedElem;

    protected ElementPanel         m_elementPanel;

    protected JTabbedPane          m_jTabbedPane = new JTabbedPane();
    protected AllPlotPanel         m_allPlotPanelTab;
    protected PlotHistory          m_historyPanelTab;
    protected PlotData             m_plotData;
    protected PlotColor            m_plotColor;

    protected ControllerPanel      m_controllerPanel   = new ControllerPanel();
    public    ServerButtonPanel    m_serverButtonPanel = new ServerButtonPanel();
    protected JTabbedPane          m_rightTabbedPane   = new JTabbedPane();
    protected XmlEditor            m_xmlEditor         = new XmlEditor();
    protected StatPlotUpdater      m_statplotUpdater   = new StatPlotUpdater();

    private   javax.swing.Timer    m_timerAllPlot;

    private   javax.swing.Timer    m_timerView;
    protected GlobalView           m_GlobalView = new GlobalView();

    protected Vector<String>       m_ipPort = new Vector<String>();
    protected boolean              m_DeployXmlHasBeenLoad = false;

    //protected Vector<String>     m_totalStreamVector = new Vector<String>();


    ////////////////////////////////////////////////////////////////////////////
    //
    public VisualPanel(EditorPanel editorPanel)
    {
        super();

        m_editorPanel  = editorPanel;
        m_elementPanel = new ElementPanel(m_editorPanel.getModel(), false);
        m_elementPanel.setEditable(false);

        m_plotData        = new PlotData(this);
        m_allPlotPanelTab = new AllPlotPanel(m_plotData);
        m_historyPanelTab = new PlotHistory(m_plotData);
        m_cActionHandler  = new VisualActionHandler(this);

        try
        {   jbInit();
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }

        m_controllerPanel.addControllable(m_visualGraph);
        m_controllerPanel.addControllable(m_historyPanelTab);

        this.setVisible(true);
        this.setSize(900, 700);       //seem to be no use???
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    private void jbInit() throws Exception
    {
        //North: m_jToolBar

        ModulePanel modulePanel = new ModulePanel();
        m_jToolBar.add(modulePanel);
        m_jToolBar.setFloatable(false);

        //West: leftPanel
        JPanel leftPanel = new JPanel();
        leftPanel.setLayout(new BoxLayout(leftPanel, BoxLayout.Y_AXIS));
        leftPanel.setPreferredSize(new Dimension(800, 600));

        m_visualGraph = new VisualGraph(this, m_controllerPanel);
        m_visualGraph.setPreferredSize(new Dimension(1500,1500));
        m_visualGraph.setMinimumSize(new Dimension(1500,1500));
        m_visualGraph.setBackground(Color.white);
        m_visualGraph.addGraphSelectionListener(new VisualGraphSelectionListener());

        m_graphScrollPane = new JScrollPane(m_visualGraph);
        m_graphScrollPane.setPreferredSize(new Dimension(800, 320));
        m_graphScrollPane.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_AS_NEEDED);
        m_graphScrollPane.setHorizontalScrollBarPolicy(ScrollPaneConstants.HORIZONTAL_SCROLLBAR_AS_NEEDED);

        leftPanel.add(m_graphScrollPane);
        leftPanel.add(m_controllerPanel);

        m_allPlotPanelTab.setPreferredSize(new Dimension(800,280));
        m_historyPanelTab.setPreferredSize(new Dimension(800, 280));
        m_jTabbedPane.addTab("Statistics",null,m_allPlotPanelTab);
        m_jTabbedPane.addTab("History",null,m_historyPanelTab);

        leftPanel.add(m_jTabbedPane);

        //East: rightPanel
        JPanel rightPanel = new JPanel();
        rightPanel.setLayout(new BoxLayout(rightPanel, BoxLayout.Y_AXIS));
        rightPanel.setPreferredSize(new Dimension(192, 600));

        JPanel graphOverview = GraphOverviewPanel.createGraphOverviewPanel(m_visualGraph,
                m_graphScrollPane.getViewport());

        graphOverview.setPreferredSize(new Dimension(200, 200));
        graphOverview.setMinimumSize(new Dimension(200, 200));
        graphOverview.setMaximumSize(new Dimension(200, 200));
        m_graphOverview = ((GraphOverviewPanelWrapper) graphOverview).getOverview();
        rightPanel.add(graphOverview);

        m_serverButtonPanel.setPreferredSize(new Dimension(192, 90));
        m_serverButtonPanel.setMinimumSize(new Dimension(192, 90));
        m_serverButtonPanel.setBackground(Color.white);

        rightPanel.add(m_serverButtonPanel);

        m_elementPanel.setPreferredSize(new Dimension(192, 310));
        rightPanel.add(m_elementPanel);

        this.setLayout(new BorderLayout());
        this.add(m_jToolBar, BorderLayout.NORTH);
        this.add(leftPanel,  BorderLayout.WEST);
        this.add(rightPanel, BorderLayout.EAST);
        this.setPreferredSize(new Dimension(1300, 700));
        this.setVisible(true);

        DebugTrace.trace("Make timers ...");

        // Timer used to poll for catalog changes.
        //
        m_timerView = new javax.swing.Timer(XmlConstants.VIEWER_VIEW,
                                            new ActionListener()
            {
                // The event is currently always null.
                //
                public void actionPerformed(ActionEvent event)
                {
                    //DebugTrace.trace("poll:  " + event.getActionCommand());

                    if (m_GlobalView.m_xml.equals(""))
                    {   m_GlobalView.updateView();
                    }
                    else
                    {   loadGraph(m_GlobalView.m_xml);
                        m_GlobalView.m_xml = "";
                    }
               }
            }
        );


        // Timer for gathering statistics.
        //
        m_timerAllPlot = new javax.swing.Timer(XmlConstants.VIEWER_PLOT,
                                               new ActionListener()
            {
                public void actionPerformed(ActionEvent evt)
                {
                    //m_statplotUpdater.updateStats();
                    m_statplotUpdater.updateStatistics();
                }
            }
        );

        m_borealisModel = new BorealisModel(m_editorPanel,
                                    new BorealisDocument(),
                                    m_visualGraph.getBorealisGraphModel(),
                                    m_visualGraph);
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    public BorealisModel getModel()
    {
    //..........................................................................


        return m_borealisModel;
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    public void setSelectedElement(BorealisElement elem)
    {
    //..........................................................................


        m_selectedElem = elem;
        m_elementPanel.setElement(m_selectedElem);
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    //  Load a catalog via XML annotated with node locations by GlobalView.
    //  Invoked by the polling timer m_timerView when new XML arraives.
    //
    public void loadGraph(// A full annotated XML diagram.
                          //
                          String  xml)
    {
        Document        doc;
        Vector<String>  oldPorts;
    //..........................................................................


        try
        {   doc = XmlHandler.xmlToDoc(xml);

            if (!doc.getRootElement().getName().equals(
                                          XmlConstants.NETWORK_ELEMENT))
            {
                JOptionPane.showConfirmDialog(null,
                    "The graph passed in is not Borealis network XML.",
                    "Not Query XML", JOptionPane.CLOSED_OPTION,
                    JOptionPane.WARNING_MESSAGE);
            }
            else
            {   // Save node endpoints in case there are nodes without boxes.
                //
                oldPorts = m_borealisModel.getDocument().getAllDistinctIpPortsString();

                BorealisDocument borealisDoc = new BorealisDocument(doc);
                m_borealisModel.getDocument().setDocument((BorealisDocument)borealisDoc.clone());

                String  warning = BorealisModelXmlHandler.loadBorealisModel(
                                                              m_borealisModel);
                if (!warning.equals(""))
                {   DebugTrace.trace(warning);

                    JOptionPane.showConfirmDialog(null, warning,
                                                  "XML parse warning",
                                       JOptionPane.CLOSED_OPTION,
                                       JOptionPane.WARNING_MESSAGE);
                }

                m_graphOverview.setGraphModel(m_borealisModel.getGraphModel());

                m_ipPort = borealisDoc.getAllDistinctIpPortsString(
                                          new  HashSet<String>(oldPorts));

                // WIP:  Use the first node found to collect some plot points.
                //       This depends on what statistics are selected.
                //       For cpu utilization there is one sample per node.
                //       For stream latency and tuple rates there is
                //       one sample per stream per node.
                //
                //m_statplotUpdater.setNodeIpPort(m_ipPort.get(0));

                // Poll all nodes.  Access has to by synched for
                // polling for the catalog and polling for statistics.
                // Only called when the catalog has changed.
                //
                m_statplotUpdater.setStatisticsNode(m_ipPort);

                DebugTrace.trace("host name = "
                               + HostEndpoint.displayEndpoint(m_ipPort.get(0)));

                m_plotColor = new PlotColor(m_ipPort);
                m_allPlotPanelTab.setPlotColor(m_plotColor);
                m_historyPanelTab.setPlotColor(m_plotColor);   //???
                m_serverButtonPanel.setPlotColor(m_plotColor); //???
                m_serverButtonPanel.setServerIpPort(m_ipPort); //???
 
                // Set one plot for this panel.
                //
                m_statplotUpdater.setStatPlot(m_allPlotPanelTab);
                //m_statplotUpdater.addStatPlot(m_allPlotPanelTab);

                // Color the boxes in the graph by node.
                //
                m_visualGraph.changeNodeColor(m_plotColor);
 
                //m_elementPanel.showTreeUpdate();
                //m_networkTreeModel.showTreeUpdate();
                DebugTrace.trace("drew a graph.");
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



    /********************
    ////////////////////////////////////////////////////////////////////////////
    //
    public void load()
    {
        File  file = SwingPanel.showFileDialog("Open Deployment");

        if (file != null)
        {
            load(file.getAbsolutePath());
        }
    }
    **********************/



    ////////////////////////////////////////////////////////////////////////////
    //
    public void load(String filename)
    {
        DebugTrace.trace("NYI");

        /**************************
        //wenjuan test4 reload
        //reload(filename);
        //m_visualGraph.changeNodeColor();

        //load
        DebugTrace.trace("hi");

        try
        {   File     file = new File(filename);
            Document  doc = XmlHandler.xmlToDoc(file);

            DeployDocument  docCopy = new DeployDocument((Document)doc.clone());

            m_ipPort = docCopy.getIpPorts();

            if (m_ipPort.size() == 0)
            {
                DebugTrace.trace("Warning: can not load any ip and port");
            }
            else
            {   if (m_DeployXmlHasBeenLoad)
                {
                    int  tabcount = m_jTabbedPane.getTabCount();

                    for (int i = 0; i < tabcount-2; i++)
                    {
                        m_jTabbedPane.removeTabAt(2);
                    }

                    m_serverButtonPanel.removeAllServers();
                }

                m_plotColor = new PlotColor(m_ipPort);
                m_allPlotPanelTab.setPlotColor(m_plotColor);
                m_historyPanelTab.setPlotColor(m_plotColor);
                m_serverButtonPanel.setPlotColor(m_plotColor);
                m_serverButtonPanel.setServerIpPort(m_ipPort);

                //m_statplotUpdater.setNodeIpPort((String) m_ipPort.get(0));
                //m_plotData.setNodeIpPorts(m_ipPort);
                //m_statplotUpdater.addStatPlot(m_allPlotPanelTab);
                m_statplotUpdater.setStatisticsNode(m_ipPort);

                int nodeIndex = 0;

                for (Iterator i = m_ipPort.iterator(); i.hasNext();)
                {
                    String ipPort = (String) i.next();
                    NodeXPlotPanel nodeXPlotPanel = new NodeXPlotPanel(ipPort, nodeIndex);
                    nodeXPlotPanel.setPlotColor(m_plotColor);
                    nodeXPlotPanel.setNodeIpPorts(m_ipPort);
                    m_statplotUpdater.addStatPlot(nodeXPlotPanel);

                    m_jTabbedPane.addTab(ipPort,nodeXPlotPanel);
                    m_visualGraph.addNode(ipPort, m_plotColor.getNodeColor(ipPort));
                    nodeIndex = nodeIndex + 1;
                }

                m_DeployXmlHasBeenLoad = true;
            }
        }
        catch (IOException e)
        {   JOptionPane.showConfirmDialog(null,
                                          e.getMessage(),
                                          "Could not open an XML file.",
                                          JOptionPane.CLOSED_OPTION,
                                          JOptionPane.ERROR_MESSAGE);
        }
        catch (Exception e)
        {   JOptionPane.showConfirmDialog(null,
                                          e.getMessage(),
                                          "XML parse error",
                                          JOptionPane.CLOSED_OPTION,
                                          JOptionPane.ERROR_MESSAGE);
        }
        *****************/
    }


    /********************
    ////////////////////////////////////////////////////////////////////////////
    //
    public void  reload(String filename)
    {
        File file = new File(filename);
        FileReader fr = null;

        try
        {
            Document  doc = XmlHandler.xmlToDoc(file);

            if (doc == null)
            {   DebugTrace.trace("File " + filename + "not found");
            }
            else
            {   m_borealisModel.getDocument().setDocument(new BorealisDocument(doc));

                String  warning =
                     BorealisModelXmlHandler.loadBorealisModel(m_borealisModel);

                if (!warning.equals(""))
                {   DebugTrace.trace("loadGraph:  " + warning);

                    JOptionPane.showConfirmDialog(null, warning,
                                                  "XML parse warning",
                                       JOptionPane.CLOSED_OPTION,
                                       JOptionPane.WARNING_MESSAGE);
                }
 
                m_graphOverview.setGraphModel(m_borealisModel.getGraphModel());
            }
        }
        catch (IOException e)
        {   JOptionPane.showConfirmDialog(null,
                                          e.getMessage(),
                                          "Could not open an XML file.",
                                          JOptionPane.CLOSED_OPTION,
                                          JOptionPane.ERROR_MESSAGE);
        }
        catch (Exception e)
        {   JOptionPane.showConfirmDialog(null,
                                          e.getMessage(),
                                          "XML parse error",
                                          JOptionPane.CLOSED_OPTION,
                                          JOptionPane.ERROR_MESSAGE);
        }
    }
    *******************/


    ////////////////////////////////////////////////////////////////////////////
    //
    // Toggle the network monitor to poll a Network.
    //
    // To connect to a Borealis network the GlobalView program is launched.
    // It in turn connects to a persistent Borealis Head to get the Global
    // Catalog.
    // The Head can be launched by the application either before or after.
    // The monitor polls the GlobalView program for catalog updates.
    // NYI:  It also polls Nodes for statistics.
    // A disconect will stop polling and kill the GlobalView program.
    //
    public void startStopView()
    {
        if (m_timerView.isRunning())
        {   int  ok = JOptionPane.showConfirmDialog(null,
                            "Do you want to stop the Viewer?",
                            "Viewer is Running",
                            JOptionPane.OK_CANCEL_OPTION,
                            JOptionPane.QUESTION_MESSAGE);

            if (ok == JOptionPane.OK_OPTION)
            {   m_GlobalView.killView();
                m_timerView.stop();
                // CLEAR SCREEN?
            }
        }
        else
        {   // Set the endpoint for the head and global viewer.
            // NYI:  Add an edit window for the GV endpoint???
            //
            m_GlobalView.setHeadPoint(XmlConstants.BOREALIS_HEAD);
            m_GlobalView.launchView();

            // Start polling for updates.
            //
            m_timerView.start();
        }

        return;
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    // Toggle the statistics monitor to poll all nodes.
    //
    public void startStopPlot()
    {
        if (m_ipPort.size()==0)
        {
            DebugTrace.trace("from 'startStopPlot():-- Didn't get IP and Port information.");
            //m_plotButton.setSelected(false);
        }
        else
        {
            if (m_timerAllPlot.isRunning())
            {
                m_timerAllPlot.stop();
            }
            else
            {   m_timerAllPlot.start();
            }
        }
    }


    ////////////////////////////////////////////////////////////////////////////
    //
    private class  ModulePanel extends JPanel implements ActionListener
    {
        public ModulePanel()
        {
            ButtonGroup butGroup = new ButtonGroup();
            setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

            JButton viewButton = new JButton("View");
            viewButton.setToolTipText("View Network Diagram");
            viewButton.setActionCommand("View");
            viewButton.addActionListener(this);
            butGroup.add(viewButton);
            add(viewButton);

            JButton plotButton = new JButton("Plot");
            plotButton.setToolTipText("Plot Activity on All Nodes");
            plotButton.setActionCommand("Plot");
            plotButton.addActionListener(this);
            butGroup.add(plotButton);
            add(plotButton);
        }


        ////////////////////////////////////////////////////////////////////////////
        //
        public void actionPerformed(ActionEvent event)
        {
            String command = event.getActionCommand();

            if (command.equals("View"))
            {
                startStopView();
            }
            else if (command.equals("Plot"))
            {
                startStopPlot();
            }
            else
            {   DebugTrace.trace("Unknown action");
            }

            return;
        }
    }


    ////////////////////////////////////////////////////////////////////////////
    //
    private class LoadListener implements ActionListener
    {
        private final String LOAD_FILE = new String(
                  System.getProperty("java.io.tmpdir") + "/LoadLevel");

        public void actionPerformed(ActionEvent e)
        {
            try
            {
                FileWriter fw = new FileWriter(LOAD_FILE, false);
                fw.write((String) ((JComboBox) e.getSource()).getSelectedItem());
                fw.flush();
                fw.close();
            }
            catch(java.io.IOException excep)
            {
                DebugTrace.trace(excep.getMessage());
            }
        }
    }


    ////////////////////////////////////////////////////////////////////////////
    //
    private class VisualGraphSelectionListener implements GraphSelectionListener
    {
        protected HashMap<String,BorealisEdge>  m_streams =
                                            new HashMap<String,BorealisEdge>();

        public void valueChanged(GraphSelectionEvent e)
        {
            BorealisElement elem = null;
            BorealisModel model = getModel();

            Object[] selectedCells = e.getCells();


            // handle single clicking on stream case
            if ((selectedCells.length == 1) && (selectedCells[0] instanceof BorealisEdge))
            {
                BorealisEdge edge = (BorealisEdge) selectedCells[0];
                Object obj = edge.getSource();

                if (obj instanceof OutputPort)
                {
                    GraphCell cell = ((OutputPort)obj).getParentCell();
                    elem = model.getElement(cell);
                    String stream = ((StreamAttributeElement) elem).getStream();

                    m_streams.put(stream, edge);
                    Vector<String> showStreamVector = new Vector<String>(m_streams.keySet());
                    // temp hack

                    if (m_plotColor != null)
                    {
                        m_plotColor.refreshShowStreamVector(showStreamVector);
                        m_allPlotPanelTab.refreshStreamVector(showStreamVector);
                        m_historyPanelTab.refreshStreamVector(showStreamVector);
                        m_plotData.refreshStreamVector(showStreamVector);

                        for (int i = 0; i<m_jTabbedPane.getTabCount()-2; i++)
                        {
                            NodeXPlotPanel tabComponent = (NodeXPlotPanel)  m_jTabbedPane.getComponentAt(2+i);
                            tabComponent.refreshStreamVector(showStreamVector);
                        }
                    }
                }
            }

            for(int i = 0; i < selectedCells.length && elem == null; i++)
            {
                Object cell = selectedCells[i];
                // if a graph cell

                if (cell instanceof BorealisGraphCell)
                {
                    BorealisGraphCell graphCell = (BorealisGraphCell) cell;
                    elem = model.getElement(graphCell);
                }
            }

            if (elem != null)
            {   setSelectedElement(elem);
            }
        }
    }
}


///////////////////////////  end VisualPanel.java  /////////////////////////////
