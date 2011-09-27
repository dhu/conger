package borgui.graph;

import borgui.client.Client;
import borgui.editor.*;
import borgui.visual.PlotColor;
import borgui.xml.*;
import borgui.common.*;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import org.jgraph.JGraph;
import org.jgraph.graph.*;
import org.jgraph.event.GraphSelectionListener;
import org.jgraph.event.GraphSelectionEvent;
import java.awt.geom.Point2D;
import java.awt.geom.Rectangle2D;
import java.util.*;
import javax.swing.tree.DefaultMutableTreeNode;


/**
 * A class that represent the whole graph.
 */
public class BorealisGraph extends JGraph
{
    protected Client                  m_client;
    protected BorealisModel           m_borealisModel;
    protected NetworkTreeModel        m_networkTreeModel;
    protected boolean                 m_isChangedCell = false;
    protected DefaultMutableTreeNode  m_changedCell = null;
    protected String                  m_oldValue = "";
    protected DeployDocument          m_deployDoc = new DeployDocument();
    //protected DeployXmlTreeEditor     m_deployXmlTreeEditor;


    /**
     * Constructor.
     */
    public BorealisGraph()
    {
        super(new BorealisGraphModel());
        getGraphLayoutCache().setFactory(new BorealisCellViewFactory());
        addMouseListener(new BorealisGraphMouseAdapter(this));
        addMouseWheelListener(new GraphMouseWheelListener());
        addGraphSelectionListener(new BorealisGraphSelectionListener());
        addKeyListener(new GraphKeyListener());

        setEnabled(true);
        setEditable(true);
        setPortsVisible(true);
//      setPortsScaled(true);
        setHighlightColor(Color.black);
        setMarqueeHandler(createMarqueeHandler());

    }


    // Hook for subclassers
    protected BasicMarqueeHandler createMarqueeHandler()
    {
        return new MyMarqueeHandler(this);
    }


    /**
     * Get its graph model.
     */
    public BorealisGraphModel getBorealisGraphModel()
    {
        return (BorealisGraphModel) getModel();
    }


    /**
     * Set its borealis mondel.
     */
    public void setBorealisModel(BorealisModel borealisModel)
    {
        m_borealisModel = borealisModel;
    }


    /*
    public void setDeployXmlTreeEditor(DeployXmlTreeEditor deployXmlTreeEditor)
    {
        m_deployXmlTreeEditor = deployXmlTreeEditor;
    }
    */


    public void setClient(Client client)
    {
        m_client = client;
    }


    public void setNetworkTreeModel(NetworkTreeModel treeModel)
    {
        m_networkTreeModel = treeModel;
    }


    public void setDeploydoc(DeployDocument deployDoc)
    {
        m_deployDoc = deployDoc;
    }


    // PopupMenu
    public JPopupMenu createPopupMenu(final Object cell)
    {
        JPopupMenu menu = new JPopupMenu();

        if (cell != null && m_borealisModel!=null && m_client!=null && m_networkTreeModel!=null)
        {
            /*
            JMenuItem deleteMenuItem = new JMenuItem("delete");
            deleteMenuItem.addActionListener(new ActionListener(){
                public void actionPerformed(ActionEvent e) {
                    Object[] cells = {cell};
                    m_borealisModel.removeCells(cells);
                }
            });
            menu.add(deleteMenuItem);
            menu.addSeparator();
             */

            if (cell instanceof HasNodeCell)
            {
                JMenuItem nodeMenuItem = new JMenuItem("Set/Change Node");
                nodeMenuItem.addActionListener(new ChangeNodeMenuListener(this));
                menu.add(nodeMenuItem);
                menu.addSeparator();
            }

            if (cell instanceof HasQueryCell)
            {
                JMenuItem queryMenuItem = new JMenuItem("Change Query");
                queryMenuItem.addActionListener(new ChangeQueryMenuListener(this));
                menu.add(queryMenuItem);
            }

            if (cell instanceof HasSchemaCell)
            {
                JMenuItem schemaMenuItem = new JMenuItem("Change Schema");
                schemaMenuItem.addActionListener(new ChangeSchemaMenuListener(this));
                menu.add(schemaMenuItem);
            }
        }    // Remove

        return menu;
    }


    private class ChangeQueryMenuListener implements ActionListener
    {
        BorealisGraph m_borealisGraph;

        public ChangeQueryMenuListener(BorealisGraph borealisGraph)
        {
            m_borealisGraph = borealisGraph;
        }

        public void actionPerformed(ActionEvent e)
        {
            JDialog diag = new JDialog(m_client, "Change the Selected Box/Table's Query");
            BorealisGraphCell selectedCell= (BorealisGraphCell)(m_borealisGraph.getSelectionCell());

            if (selectedCell instanceof HasQueryCell)
            {
                JLabel label0 = new JLabel();
                JLabel label1 = new JLabel(" The selected cell is:  '" + (String)selectedCell.getUserObject() + "'.");
                JLabel label2 = new JLabel(" Available Queries: ");
                JButton changeButton = new JButton("Change");
                final JComboBox comboBox = new JComboBox();

                String queryName = "";

                try
                {
                    final HasQueryElement selectedElement = (HasQueryElement) m_borealisModel.getElement(selectedCell);

                    queryName =selectedElement.getQueryName();

                    changeButton.addActionListener(new ActionListener()
                    {
                        public void actionPerformed(ActionEvent evt)
                        {
                            selectedElement.getQueryElement().removeBox(
                                           selectedElement.getNameValue());

                            String newQueryName = ((String)comboBox.getSelectedItem());
                            java.util.List querys = m_borealisModel.getDocument().getAllQueries();

                            for (Iterator i = querys.iterator(); i.hasNext();)
                            {
                                QueryElement queryElement = (QueryElement)i.next();

                                if (queryElement.getNameValue().equals(
                                                                newQueryName))
                                {
                                    queryElement.addContent(
                                         (BorealisElement)selectedElement);

                                    JOptionPane.showConfirmDialog(null,
                                            "Its query has been changed.",
                                            "ChangeQuery", JOptionPane.CLOSED_OPTION,
                                            JOptionPane.INFORMATION_MESSAGE);
                                }
                            }

                        }
                    }
                    );

                    m_networkTreeModel.showTreeUpdate();
                    java.util.List querys = m_borealisModel.getDocument().getAllQueries();
                    int index = 0;

                    for (Iterator i = querys.iterator(); i.hasNext();)
                    {
                        QueryElement queryElement =(QueryElement) i.next();
                        comboBox.addItem(queryElement.getNameValue());

                        if (queryElement.getNameValue().equals(queryName))
                        {
                            comboBox.setSelectedIndex(index);
                        }

                        index++;
                    }

                    JPanel panel = new JPanel();
                    panel.setLayout(new BoxLayout(panel, BoxLayout.Y_AXIS));
                    panel.add(Box.createVerticalStrut(5));
                    JPanel topPanel = new JPanel();
                    JPanel bottomPanel = new JPanel();
                    panel.add(topPanel);
                    panel.add(Box.createVerticalStrut(5));
                    panel.add(bottomPanel);
                    panel.add(Box.createVerticalStrut(5));


                    topPanel.setLayout(new BorderLayout());
                    topPanel.add(label0, BorderLayout.NORTH);
                    topPanel.add(label1, BorderLayout.SOUTH);

                    bottomPanel.setLayout(new BoxLayout(bottomPanel,  BoxLayout.X_AXIS));
                    bottomPanel.add(label2);
                    bottomPanel.add(comboBox);
                    bottomPanel.add(Box.createHorizontalStrut(5));
                    bottomPanel.add(changeButton);
                    bottomPanel.add(Box.createHorizontalStrut(5));

                    diag.getContentPane().add(panel);
                    diag.setSize(new Dimension(400,80));
                    diag.setVisible(true);
                }
                catch (Exception ex)
                {
                    System.out.println("Error: The node you selected is not a Box/Table node.");
                }
            }
            else
            {
                JOptionPane.showConfirmDialog(null,
                        "Please choose a Box/Table cell.",
                        "Warning", JOptionPane.DEFAULT_OPTION,
                        JOptionPane.INFORMATION_MESSAGE);
            }
        }
    }


    private class ChangeSchemaMenuListener implements ActionListener
    {
        BorealisGraph m_borealisGraph;

        public ChangeSchemaMenuListener(BorealisGraph borealisGraph)
        {
            m_borealisGraph = borealisGraph;
        }

        public void actionPerformed(ActionEvent e)
        {
            JDialog diag = new JDialog(m_client, "Change the Selected In/Output's Schema");

            GraphCell selectedCell= (GraphCell)m_borealisGraph.getSelectionCell();

            if (selectedCell instanceof HasSchemaCell)
            {
                JLabel label0 = new JLabel();
                JLabel label1 = new JLabel(" The selected cell is:  '" + (String)((BorealisGraphCell)selectedCell).getUserObject() + "'.");
                JLabel label2 = new JLabel(" Available Schemas: ");
                JButton changeButton = new JButton("Change");
                final JComboBox comboBox = new JComboBox();

                String schemaName = "";

                try
                {
                    final HasSchemaElement selectedElement = (HasSchemaElement) m_borealisModel.getElement(selectedCell);
                    schemaName = selectedElement.getSchema();

                    changeButton.addActionListener(new ActionListener()
                    {
                        public void actionPerformed(ActionEvent evt)
                        {
                            selectedElement.setSchema((String)comboBox.getSelectedItem());

                            JOptionPane.showConfirmDialog(null,
                                    "Its schema has been changed.",
                                    "ChangeSchema", JOptionPane.CLOSED_OPTION,
                                    JOptionPane.INFORMATION_MESSAGE);
                        }
                    }
                    );

                    m_networkTreeModel.showTreeUpdate();

                    java.util.List schemas = m_borealisModel.getDocument().getAllSchemas();
                    int index = 0;

                    for (Iterator i = schemas.iterator(); i.hasNext();)
                    {
                        SchemaElement schema =(SchemaElement) i.next();
                        comboBox.addItem(schema.getNameValue());

                        if (schema.getNameValue().equals(schemaName))
                        {
                            comboBox.setSelectedIndex(index);
                        }

                        index++;
                    }

                    JPanel panel = new JPanel();
                    panel.setLayout(new BoxLayout(panel, BoxLayout.Y_AXIS));
                    panel.add(Box.createVerticalStrut(5));
                    JPanel topPanel = new JPanel();
                    JPanel bottomPanel = new JPanel();
                    panel.add(topPanel);
                    panel.add(Box.createVerticalStrut(5));
                    panel.add(bottomPanel);
                    panel.add(Box.createVerticalStrut(5));


                    topPanel.setLayout(new BorderLayout());
                    topPanel.add(label0, BorderLayout.NORTH);
                    topPanel.add(label1, BorderLayout.SOUTH);

                    bottomPanel.setLayout(new BoxLayout(bottomPanel,  BoxLayout.X_AXIS));
                    bottomPanel.add(label2);
                    bottomPanel.add(comboBox);
                    bottomPanel.add(Box.createHorizontalStrut(5));
                    bottomPanel.add(changeButton);
                    bottomPanel.add(Box.createHorizontalStrut(5));

                    diag.getContentPane().add(panel);
                    diag.setSize(new Dimension(400,80));
                    diag.setVisible(true);

                }
                catch (Exception ex)
                {
                    System.out.println("Error: The node you selected is not a Table/Input/Output node.");
                }
            }
            else
            {
                JOptionPane.showConfirmDialog(null,
                        "Please choose a Input/Output/Table cell.",
                        "Warning", JOptionPane.DEFAULT_OPTION,
                        JOptionPane.INFORMATION_MESSAGE);
            }
        }
    }


    private class ChangeNodeMenuListener  implements ActionListener
    {
        BorealisGraph m_borealisGraph;


        public ChangeNodeMenuListener(BorealisGraph borealisGraph)
        {
            m_borealisGraph = borealisGraph;
        }


        public void actionPerformed(ActionEvent e)
        {
            JDialog diag = new JDialog(m_client, "Set/Change the Selected Cell's Node");

            final BorealisGraphCell selectedCell= (BorealisGraphCell)m_borealisGraph.getSelectionCell();

            if (selectedCell instanceof HasNodeCell)
            {
                JLabel label0 = new JLabel();
                JLabel label1 = new JLabel(" The selected cell is:  '" + (String)((BorealisGraphCell)selectedCell).getUserObject() + "'.");
                JLabel label2 = new JLabel(" Available Nodes: ");
                JButton changeButton = new JButton("Change");
                final JComboBox comboBox = new JComboBox();

                try
                {
                    final HasNodeElement selectedElement = (HasNodeElement)(m_borealisModel.getElement(selectedCell));

                    String oldNodeName = selectedElement.getNodeAttribute();
                    final String oldNodeNameCopy = oldNodeName;

                    changeButton.addActionListener(new ActionListener()
                    {
                        public void actionPerformed(ActionEvent evt)
                        {
                            String newIpPort = (String)comboBox.getSelectedItem();
                            selectedElement.setNodeAttribute(newIpPort);

                            JOptionPane.showConfirmDialog(null,
                                    "Its node has been set/changed.",
                                    "ChangeSchema", JOptionPane.CLOSED_OPTION,
                                    JOptionPane.INFORMATION_MESSAGE);

                            if (m_deployDoc.getIpPorts().size()!=0)
                            {
                                PlotColor plotColor = new PlotColor(m_deployDoc.getIpPorts());
                                Color nodeColor = plotColor.getNodeColor(newIpPort);
                                ((HasNodeCell)selectedCell).changeNodeColor(nodeColor);
                                m_borealisGraph.getBorealisGraphModel().flushBuffer();
                            }
                            else
                            {
                                JOptionPane.showConfirmDialog(null,
                                        "Please add nodes definition to the deployment XML.",
                                        "Warning", JOptionPane.DEFAULT_OPTION,
                                        JOptionPane.INFORMATION_MESSAGE);
                            }
                        }
                    }
                    );

                    Vector nodes = m_deployDoc.getIpPorts();
                    int index = 0;
                    //comboBox.setSelectedIndex(0);

                    for (Iterator i = nodes.iterator(); i.hasNext();)
                    {
                        String node = (String)i.next();
                        comboBox.addItem(node);

                        if(node.equals(oldNodeName))
                        {
                            comboBox.setSelectedIndex(index);
                        }

                        index++;
                    }

                    JPanel panel = new JPanel();
                    panel.setLayout(new BoxLayout(panel, BoxLayout.Y_AXIS));
                    panel.add(Box.createVerticalStrut(5));
                    JPanel topPanel = new JPanel();
                    JPanel bottomPanel = new JPanel();
                    panel.add(topPanel);
                    panel.add(Box.createVerticalStrut(5));
                    panel.add(bottomPanel);
                    panel.add(Box.createVerticalStrut(5));


                    topPanel.setLayout(new BorderLayout());
                    topPanel.add(label0, BorderLayout.NORTH);
                    topPanel.add(label1, BorderLayout.SOUTH);

                    bottomPanel.setLayout(new BoxLayout(bottomPanel,  BoxLayout.X_AXIS));
                    bottomPanel.add(label2);
                    bottomPanel.add(comboBox);
                    bottomPanel.add(Box.createHorizontalStrut(5));
                    bottomPanel.add(changeButton);
                    bottomPanel.add(Box.createHorizontalStrut(5));

                    diag.getContentPane().add(panel);
                    diag.setSize(new Dimension(400,80));
                    diag.setVisible(true);

                }
                catch (Exception ex)
                {
                    ex.printStackTrace();
                    System.out.println("Error: The node you selected is not a Box/Table/Input node.");
                }
            }
            else
            {
                JOptionPane.showConfirmDialog(null,
                        "Please choose a Box/Table/Input cell.",
                        "Warning", JOptionPane.DEFAULT_OPTION,
                        JOptionPane.INFORMATION_MESSAGE);
            }
        }
    }


    /**
     * A class that response to mouse click.
     */
    private class BorealisGraphMouseAdapter extends MouseAdapter
    {
        BorealisGraph m_borealisGraph;

        public BorealisGraphMouseAdapter(BorealisGraph borealisGraph)
        {
            super();
            m_borealisGraph = borealisGraph;
        }

        public void mousePressed(MouseEvent e)
        {
            if (e.getClickCount() == 2)
            {
                DefaultMutableTreeNode graphCell = (DefaultMutableTreeNode)m_borealisGraph.getSelectionCell();

                if(graphCell!=null)
                {
                    m_isChangedCell = true;
                    m_changedCell = graphCell;
                    m_oldValue = (String)graphCell.getUserObject();
                }
            }
        }
    }


    /**
     * A class that response to the selection of graph.
     */
    private class BorealisGraphSelectionListener implements GraphSelectionListener
    {
        public void valueChanged(GraphSelectionEvent e)
        {
            if (m_isChangedCell && m_borealisModel!=null)
            {
                if (m_changedCell instanceof InputGraphCell )
                {
                    if(!m_changedCell.getUserObject().equals(m_oldValue))
                    {
                        m_borealisModel.changeInputCellName((InputGraphCell)m_changedCell, m_oldValue, (String)m_changedCell.getUserObject());
                    }
                }
                else if(m_changedCell instanceof OutputGraphCell)
                {
                    if(!m_changedCell.getUserObject().equals(m_oldValue))
                    {
                        m_borealisModel.changeOutputCellName((OutputGraphCell)m_changedCell, m_oldValue, (String)m_changedCell.getUserObject());
                    }
                }
                else if(m_changedCell instanceof BoxGraphCell)
                {
                    if(!m_changedCell.getUserObject().equals(m_oldValue))
                    {
                        m_borealisModel.changeBoxCellName((BoxGraphCell)m_changedCell,(String)m_changedCell.getUserObject() );
                    }
                }
                else if(m_changedCell instanceof BorealisEdge)
                {
                    if(!m_changedCell.getUserObject().equals(m_oldValue))
                    {
                        m_borealisModel.changeEdgeCellName((BorealisEdge)m_changedCell, m_oldValue, (String)m_changedCell.getUserObject());
                    }
                }
                else if(m_changedCell instanceof TableGraphCell)
                {
                    if(!m_changedCell.getUserObject().equals(m_oldValue))
                    {
                        m_borealisModel.changeTableCellName((TableGraphCell)m_changedCell, (String)m_changedCell.getUserObject());
                    }
                }
            }

            m_isChangedCell = false;
        }
    }


    /**
     * A class that response to mouse wheel.
     */
    private class GraphMouseWheelListener implements MouseWheelListener
    {
        public void mouseWheelMoved(MouseWheelEvent e)
        {
            int notches = e.getWheelRotation();
            double adjust = .05 * notches;
            setScale(getScale() + adjust);
        }
    }


    /**
     * A class that response to key press.
     */
    private class GraphKeyListener extends KeyAdapter
    {

        public void keyPressed(KeyEvent e)
        {
            if(e.getKeyCode()==KeyEvent.VK_DELETE)
            {
                if (!isSelectionEmpty() && m_borealisModel!=null)
                {
                    Object[] cells = getSelectionCells();
                    cells = getDescendants(cells);
                    m_borealisModel.removeCells(cells);
                }

            }
            else if (e.getKeyCode() == KeyEvent.VK_1)
            {
                double adjust = .025 * -1;
                setScale(getScale() + adjust);
            }
            else if (e.getKeyCode() == KeyEvent.VK_2)
            {
                double adjust = .025 * 1;
                setScale(getScale() + adjust);
            }
        }
    }


// MarqueeHandler that Connects Vertices and Displays PopupMenus
    public class MyMarqueeHandler extends BasicMarqueeHandler
    {

        // Holds the Start and the Current Point
        protected Point2D start, current;

        // Holds the First and the Current Port
        protected PortView port, firstPort;

        protected BorealisGraph m_borealisGraph;
        protected HasNodeCell m_hasNodeCell;

        public MyMarqueeHandler(BorealisGraph borealisGraph)
        {
            super();
            m_borealisGraph = borealisGraph;
        }


        public boolean isForceMarqueeEvent(MouseEvent e)
        {
            // If Right Mouse Button we want to Display the PopupMenu
            if (SwingUtilities.isRightMouseButton(e))
                // Return Immediately
                return true;
            // Find and Remember Port
            port = getSourcePortAt(e.getPoint());
            // If Port Found and in ConnectMode (=Ports Visible)
            if (port != null && isPortsVisible())
                return true;
            // Else Call Superclass
            return super.isForceMarqueeEvent(e);
        }

        // Display PopupMenu or Remember Start Location and First Port
        public void mousePressed(final MouseEvent e)
        {

            // If Right Mouse Button
            if (SwingUtilities.isRightMouseButton(e))
            {
                // Find Cell in Model Coordinates
                Object cell = m_borealisGraph.getFirstCellForLocation(e.getX(), e.getY());
                // Create PopupMenu for the Cell
                JPopupMenu menu = createPopupMenu( cell);
                // Display PopupMenu
                menu.show(m_borealisGraph, e.getX(), e.getY());
                // Else if in ConnectMode and Remembered Port is Valid
            }
            else if (port != null && isPortsVisible())
            {
                // Remember Start Location
                start = toScreen(port.getLocation());
                // Remember First Port
                firstPort = port;
            }
            else
            {
                // Call Superclass
                super.mousePressed(e);
            }
        }


        // Find Port under Mouse and Repaint Connector
        public void mouseDragged(MouseEvent e)
        {
            // If remembered Start Point is Valid
            if (start != null)
            {
                // Fetch Graphics from Graph
                Graphics g = getGraphics();
                // Reset Remembered Port
                PortView newPort = getTargetPortAt(e.getPoint());

                // Do not flicker (repaint only on real changes)
                if (newPort == null || newPort != port)
                {
                    // Xor-Paint the old Connector (Hide old Connector)
                    paintConnector(Color.black, getBackground(), g);
                    // If Port was found then Point to Port Location
                    port = newPort;
                    if (port != null)
                        current = toScreen(port.getLocation());
                    // Else If no Port was found then Point to Mouse Location
                    else
                        current = snap(e.getPoint());
                    // Xor-Paint the new Connector
                    paintConnector(getBackground(), Color.black, g);
                }
            }
            // Call Superclass
            super.mouseDragged(e);
        }


        public PortView getSourcePortAt(Point2D point)
        {
            // Disable jumping
            setJumpToDefaultPort(false);
            PortView result;
            try
            {
                // Find a Port View in Model Coordinates and Remember
                result = getPortViewAt(point.getX(), point.getY());
            }
            finally
            {
                setJumpToDefaultPort(true);
            }

            return result;
        }


        // Find a Cell at point and Return its first Port as a PortView
        protected PortView getTargetPortAt(Point2D point)
        {
            // Find a Port View in Model Coordinates and Remember
            return getPortViewAt(point.getX(), point.getY());
        }


        // Connect the First Port and the Current Port in the Graph or Repaint
        public void mouseReleased(MouseEvent e)
        {
            // If Valid Event, Current and First Port
            if (e != null && port != null && firstPort != null && firstPort != port && m_borealisModel!=null)
            {
                m_borealisModel.connect(firstPort, port);
                e.consume();
                // Else Repaint the Graph
            }
            else
                repaint();

            // Reset Global Vars
            firstPort = port = null;
            start = current = null;
            // Call Superclass
            super.mouseReleased(e);
        }


        // Show Special Cursor if Over Port
        public void mouseMoved(MouseEvent e)
        {
            // Check Mode and Find Port
            if (e != null && getSourcePortAt(e.getPoint()) != null && isPortsVisible())
            {
                // Set Cusor on Graph (Automatically Reset)
                setCursor(new Cursor(Cursor.HAND_CURSOR));
                // Consume Event
                // Note: This is to signal the BasicGraphUI's
                // MouseHandle to stop further event processing.
                e.consume();
            }
            else
                // Call Superclass
                super.mouseMoved(e);
        }


        // Use Xor-Mode on Graphics to Paint Connector
        protected void paintConnector(Color fg, Color bg, Graphics g)
        {
            // Set Foreground
            g.setColor(fg);
            // Set Xor-Mode Color
            g.setXORMode(bg);
            // Highlight the Current Port
            paintPort(getGraphics());
            // If Valid First Port, Start and Current Point
            if (firstPort != null && start != null && current != null)
                // Then Draw A Line From Start to Current Point
                g.drawLine((int) start.getX(), (int) start.getY(),
                        (int) current.getX(), (int) current.getY());
        }

        // Use the Preview Flag to Draw a Highlighted Port
        protected void paintPort(Graphics g) {
            // If Current Port is Valid
            if (port != null) {
                // If Not Floating Port...
                boolean o = (GraphConstants.getOffset(port.getAllAttributes()) != null);
                // ...Then use Parent's Bounds
                Rectangle2D r = (o) ? port.getBounds() : port.getParentView()
                .getBounds();
                // Scale from Model to Screen
                r = toScreen((Rectangle2D) r.clone());
                // Add Space For the Highlight Border
                r.setFrame(r.getX() - 3, r.getY() - 3, r.getWidth() + 6, r
                        .getHeight() + 6);
                // Paint Port in Preview (=Highlight) Mode
                getUI().paintCell(g, port, r, true);
            }
        }
    } // End of Editor.MyMarqueeHandler



    ////////////////////////////////////////////////////////////////////////////
    //
    public void changeNodeColor(PlotColor plotColor)
    {
        String     node;
        Color      nodeColor;
    //..........................................................................


        Object[] allCells = this.getRoots();
        DebugTrace.trace(allCells.length + " cells.");

        for(int i = 0; i < allCells.length; i++)
        {
            if (allCells[i] instanceof HasNodeCell)
            {
                HasNodeElement  element = (HasNodeElement)
                                          m_borealisModel.getElementMap().get(
                                             (HasNodeCell)allCells[i]);

                node = element.getNodeAttribute();

                node = HostEndpoint.formEndpoint(node,
                                    XmlConstants.DEFAULT_BOREALIS_PORT);

                DebugTrace.trace(node);

                nodeColor = plotColor.getNodeColor(node);
                ((HasNodeCell)allCells[i]).changeNodeColor(nodeColor);
            }
            else if (allCells[i] instanceof OutputGraphCell)
            {   OutputElement  element = (OutputElement)
                                           m_borealisModel.getElementMap().get(
                                               (OutputGraphCell)allCells[i]);

                node = element.getNodeAttribute();
                DebugTrace.trace("color output @ " + node);

                node = HostEndpoint.formEndpoint(node,
                                      XmlConstants.DEFAULT_BOREALIS_PORT);
                
                nodeColor = plotColor.getNodeColor(node);
                ((OutputGraphCell)allCells[i]).changeNodeColor(nodeColor);
            }
        }

        getBorealisGraphModel().flushBuffer();
    }


    ////////////////////////////////////////////////////////////////////////////
    //
    public void clearNodesColor()
    {
    //..........................................................................


        Object[] allCells = this.getRoots();

        for (int i = 0; i<allCells.length; i++)
        {
            if (allCells[i] instanceof HasNodeCell)
            {
                ((HasNodeCell)allCells[i]).changeNodeColor(Color.GRAY);

            }
        }

        this.getBorealisGraphModel().flushBuffer();
    }
}


//////////////////////////  end
