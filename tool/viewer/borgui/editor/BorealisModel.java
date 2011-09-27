package borgui.editor;
import java.util.*;
import org.jgraph.graph.*;

import borgui.xml.*;
import borgui.graph.*;
import borgui.common.DebugTrace;

import javax.swing.JOptionPane;
import javax.swing.event.UndoableEditEvent;
import org.jgraph.graph.GraphUndoManager;

/**
 * A class that defines a suitable data model for a Borealis.BoxElement
 */
public class BorealisModel
{
    BorealisDocument    m_doc;
    BorealisGraphModel  m_borealisGraphModel;
    BorealisGraph       m_borealisGraph;

    public Map<GraphCell, BorealisElement>  m_elements;
    public Map<BorealisElement, GraphCell>  m_cells;

    EditorPanel         m_editorPanel;

    protected int       edgeNameCount = 0;
    
    // Undo Manager
    protected GraphUndoManager m_undoManager;
    
        /*
        public BorealisModel(BorealisDocument doc, BorealisGraphModel model)
        {
                setDocument(doc);
                setGraphModel(model);
                m_elements = new HashMap<GraphCell, BorealisElement>();
                m_cells = new HashMap<BorealisElement, GraphCell>();
        }*/

    
    /**
     * Constructor.
     */
    public BorealisModel(EditorPanel editorPanel, BorealisDocument doc, BorealisGraphModel model, BorealisGraph graph)
    {
        m_borealisGraph = graph;
        setDocument(doc);
        setGraphModel(model);
        m_elements = new HashMap<GraphCell, BorealisElement>();
        m_cells = new HashMap<BorealisElement, GraphCell>();
        m_editorPanel = editorPanel;
        
        // Construct Command History
        //
        // Create a GraphUndoManager which also Updates the ToolBar
        m_undoManager = new GraphUndoManager()
        {
            // Override Superclass
            public void undoableEditHappened(UndoableEditEvent e)
            {
                // First Invoke Superclass
                super.undoableEditHappened(e);
                // Then Update Undo/Redo Buttons
                m_editorPanel.updateHistoryButtons();
            }
        };
        
        // Register UndoManager with the Model
        m_borealisGraphModel.addUndoableEditListener(m_undoManager);
    }

    
    /**
     * Set the model
     */
    public void setModel(BorealisModel model)
    {
        m_elements.clear();
        m_cells.clear();
        m_elements = model.getElementMap();
        m_cells = model.getCellMap();
        m_doc = model.getDocument();
        m_borealisGraphModel = model.getGraphModel();
    }

    
    /**
     * Set the borealis document for this model.
     */
    public void setDocument(BorealisDocument doc)
    {
        m_doc = doc;
    }

    
    public GraphUndoManager getUndoManager()
    {
        return m_undoManager;
    }

    
    /**
     * Return the borealis document of this model.
     */
    public BorealisDocument getDocument()
    {
        return m_doc;
    }

    
    /**
     * Set the borealis graph model for this model.
     */
    public void setGraphModel(BorealisGraphModel model)
    {
        m_borealisGraphModel = model;
        m_borealisGraph.setModel(m_borealisGraphModel);
        
    }

    
    /**
     * Return the borealis graph model of this model.
     */
    public BorealisGraphModel getGraphModel()
    {
        return m_borealisGraphModel;
    }

    
    public BorealisGraph getGraph()
    {
        return m_borealisGraph;
    }
    
    
    /**
     * Return the borealis element map of this model.
     */
    public Map<GraphCell, BorealisElement>  getElementMap()
    {
        return m_elements;
    }

    
    /**
     * Return the borealis cell map of this model.
     */
    public Map<BorealisElement, GraphCell>  getCellMap()
    {
        return m_cells;
    }

    
    /**
     * Clear cell map.
     */
    public void clearCellMap()
    {
        m_cells.clear();
    }
    
    /**
     * Clear element map.
     */
    public void clearElementMap()
    {
        m_elements.clear();
    }
    
    /**
     * Add a mapping between graph cell and element
     */
    public void addMapping(GraphCell cell, BorealisElement element)
    {
        m_elements.put(cell, element);
    }
    
    /**
     * Add a mapping between element and graph cell
     */
    public void addMapping(BorealisElement element, GraphCell cell)
    {
        m_elements.put(cell, element);
        m_cells.put(element, cell);
    }
    
    /**
     * Remove a mapping between element and graph cell
     */
    public void removeMapping(BorealisElement element, GraphCell cell)
    {
        m_elements.remove(cell);
        m_cells.remove(element);
    }
    
    /**
     * Return the element which map the supplied cell.
     */
    public BorealisElement getElement(GraphCell cell)
    {
        return (BorealisElement) m_elements.get(cell);
    }
    
    /**
     * Return the cell which map the supplied element.
     */
    public GraphCell getCell(BorealisElement element)
    {
        return (GraphCell) m_cells.get(element);
    }
    
    //////////////////////////////////////////////////////////////////
    //remove a cell or element from the query network
    public void removeCells(Object cells[])
    {
        int cellNum = cells.length;

        for (int i = 0; i < cellNum; i++)
        {
            removeCell((GraphCell)cells[i]);
        }

        m_borealisGraphModel.remove(cells);
        m_editorPanel.getNetworkTreeModel().showTreeUpdate();
    }
    
    /**
     * Remove a given cell
     */
    public void removeCell(GraphCell cell)
    {
        boolean result = false;

        if(cell instanceof InputGraphCell)
        {
            result = removeInputCell((InputGraphCell)cell);
        }
        else if(cell instanceof OutputGraphCell)
        {
            result =  removeOutputCell((OutputGraphCell)cell);
        }
        else if(cell instanceof BoxGraphCell)
        {
            result = removeBoxCell((BoxGraphCell)cell);
        }
        else if(cell instanceof TableGraphCell)
        {
            result =  removeTableCell((TableGraphCell)cell);
        }
        else if(cell instanceof BorealisEdge)
        {
            result =  removeEdge((BorealisEdge)cell);
        }
        else if(cell instanceof BorealisPort)
        {
            result = true;
        }
        
        if (!result)
        {
            System.out.println("Error in rmoving cell: " + cell);
        }
    }
    
    private boolean removeInput(InputElement element, InputGraphCell cell)
    {
        boolean ret = m_doc.removeInput(element.getStream());

        if (ret)
        {
            OutputPort outputPort = cell.getOutputPort();
            Set edgesSet = outputPort.getEdges();

            if (edgesSet.size()!=0)
            {
                Object[] edgesArray = new Object[1];
                Iterator i = edgesSet.iterator();
                Edge edge = (Edge)i.next();
                edgesArray[0] = edge;
                
                BorealisPort targetPort = (BorealisPort)edge.getTarget();
                GraphCell connectedCell = targetPort.getParentCell();

                if (connectedCell instanceof BoxGraphCell)
                {
                    BoxElement connectedElement =
                             (BoxElement)m_elements.get(connectedCell);

                    if (connectedElement!=null)
                    {
                        connectedElement.removeIn(
                                 (String)cell.getUserObject());
                    }
                }
                
                m_borealisGraphModel.remove(edgesArray);
            }

            removeMapping(element, cell);
        }
        
        return ret;
    }
    
    /**
     * Remove a input element from the model.
     */
    public boolean removeInputElement(InputElement element)
    {
        InputGraphCell cell = (InputGraphCell) m_cells.get(element);

        if (cell == null)
        {
            return false;
        }
        
        boolean result = removeInput(element, cell);
        Object[] cells = {cell, cell.getOutputPort()};
        
        m_borealisGraphModel.remove(cells);
        m_editorPanel.getNetworkTreeModel().showTreeUpdate();
        return result;
    }
    
    /**
     * Remove a input cell from the model.
     */
    public boolean removeInputCell(InputGraphCell cell)
    {
        InputElement element = (InputElement) m_elements.get(cell);

        if (element == null)
        {
            return false;
        }

        return removeInput(element, cell);
    }
    
    
    private boolean removeOutput(OutputElement element, OutputGraphCell cell)
    {
        boolean ret = m_doc.removeOutput(element.getStream());

        if (ret)
        {
            InputPort inputPort = cell.getInputPort();
            Set edgesSet = inputPort.getEdges();

            if (edgesSet.size()!=0)
            {
                Object[] edgesArray = new Object[1];
                Iterator i = edgesSet.iterator();
                Edge edge = (Edge)i.next();
                edgesArray[0] = edge;
                
                BorealisPort sourcePort = (BorealisPort)edge.getSource();
                GraphCell connectedCell = sourcePort.getParentCell();

                if(connectedCell instanceof BoxGraphCell)
                {
                    BoxElement connectedElement =(BoxElement) m_elements.get(connectedCell);

                    if (connectedElement != null)
                    {
                        connectedElement.removeOut((String) cell.getUserObject());
                    }
                }
                
                m_borealisGraphModel.remove(edgesArray);
            }

            removeMapping(element, cell);
        }

        return ret;
    }
    
    /**
     * Remove an output element from the model.
     */
    public boolean removeOutputElement(OutputElement element)
    {
        OutputGraphCell cell = (OutputGraphCell) m_cells.get(element);

        if (cell == null)
        {
            return false;
        }
        
        boolean result = removeOutput(element, cell);
        Object[] cells = {cell, cell.getInputPort()};
        
        m_borealisGraphModel.remove(cells);
        m_editorPanel.getNetworkTreeModel().showTreeUpdate();

        return result;
    }
    
    /**
     * Remove an output cell from the model.
     */
    public boolean removeOutputCell(OutputGraphCell cell)
    {
        OutputElement element = (OutputElement) m_elements.get(cell);
        
        if (element == null)
        {
            return false;
        }

        return removeOutput(element, cell);
    }
    
    
    private boolean removeBox(BoxElement element, BoxGraphCell cell)
    {
        boolean ret = m_doc.removeBox(element.getNameValue());

        if (ret)
        {
            int index = 0;
            
            InputPort inputPort = cell.getInputPort();
            Set inEdgesSet = inputPort.getEdges();
            int inNum = inEdgesSet.size();
            
            Object[] inEdgesArray = new Object[inNum];

            for(Iterator i = inEdgesSet.iterator(); i.hasNext();)
            {
                BorealisEdge inEdge = (BorealisEdge)i.next();
                inEdgesArray[index] = inEdge;
                index++;
                
                BorealisPort sourcePort = (BorealisPort)inEdge.getSource();
                
                GraphCell inConnectedCell = sourcePort.getParentCell();

                if (inConnectedCell instanceof BoxGraphCell)
                {
                    BoxElement inConnectedElement =(BoxElement) m_elements.get(inConnectedCell);

                    if (inConnectedElement != null)
                    {
                        inConnectedElement.removeOut((String)inEdge.getUserObject());
                    }
                }
            }

            m_borealisGraphModel.remove(inEdgesArray);
            OutputPort outputPort = cell.getOutputPort();
            Set outEdgesSet = outputPort.getEdges();

            int outNum = outEdgesSet.size();
            index = 0;
            Object[] outEdgesArray = new Object[outNum];

            for (Iterator i = outEdgesSet.iterator(); i.hasNext();)
            {
                BorealisEdge outEdge = (BorealisEdge)i.next();
                outEdgesArray[index] = outEdge;
                index++;
                
                BorealisPort targetPort = (BorealisPort)outEdge.getTarget();
                
                GraphCell outConnectedCell = targetPort.getParentCell();

                if (outConnectedCell instanceof BoxGraphCell)
                {
                    BoxElement outConnectedElement =(BoxElement) m_elements.get(outConnectedCell);

                    if (outConnectedElement != null)
                    {
                        outConnectedElement.removeIn((String)outEdge.getUserObject());
                    }
                }
            }

            m_borealisGraphModel.remove(outEdgesArray);
            
            if (cell.isTableBox())
            {
                TablerefPort tablerefPort = cell.getTablerefPort();
                Set refEdgesSet = tablerefPort.getEdges();

                if (refEdgesSet.size()!=0)
                {
                    Object[] refEdgesArray = new Object[1];
                    Iterator i = refEdgesSet.iterator();
                    BorealisEdge refEdge = (BorealisEdge)i.next();
                    refEdgesArray[0] = refEdge;
                    
                    BorealisPort targetPort = (BorealisPort)refEdge.getTarget();
                    GraphCell refConnectedCell = targetPort.getParentCell();
                    TableElement refConnectedElement =(TableElement) m_elements.get(refConnectedCell);

                    if(refConnectedElement != null)
                    {
                        refConnectedElement.setAttribute(XmlConstants.TABLE_STREAM_ATTRIBUTE, "");
                    }

                    m_borealisGraphModel.remove(refEdgesArray);
                }
            }
            
            removeMapping(element, cell);
        }

        return ret;
    }
    
    /**
     * Remove a box element from the model.
     */
    public boolean removeBoxElement(BoxElement element)
    {
        BoxGraphCell cell = (BoxGraphCell) m_cells.get(element);

        if (cell == null)
        {
            return false;
        }

        boolean result = removeBox(element, cell);
        Object[] cells = {cell, cell.getInputPort(), cell.getOutputPort(), cell.getTablerefPort()};
        
        m_borealisGraphModel.remove(cells);
        m_editorPanel.getNetworkTreeModel().showTreeUpdate();

        return result;
    }
    
    /**
     * Remove a box element from the model.
     */
    public boolean removeBoxCell(BoxGraphCell cell)
    {
        BoxElement element = (BoxElement) m_elements.get(cell);
        
        if (element == null)
        {
            return false;
        }

        return removeBox(element, cell);
    }
    
    public boolean removeEdge(BorealisEdge edge)
    {
        BorealisPort sourcePort = (BorealisPort)edge.getSource();
        BorealisPort targetPort = (BorealisPort)edge.getTarget();
        
        if (sourcePort!=null&& targetPort!=null)
        {
            GraphCell sourceConnectedCell = sourcePort.getParentCell();

            if(sourceConnectedCell instanceof BoxGraphCell)
            {
                BoxElement sourceConnectedElement =(BoxElement) m_elements.get(sourceConnectedCell);

                if(sourceConnectedElement != null)
                {
                    sourceConnectedElement.removeOut((String)edge.getUserObject());
                }
            }

            GraphCell targetConnectedCell = targetPort.getParentCell();

            if (targetConnectedCell instanceof BoxGraphCell)
            {
                BoxElement targetConnectedElement =(BoxElement) m_elements.get(targetConnectedCell);

                if (targetConnectedElement != null)
                {
                    targetConnectedElement.removeIn((String)edge.getUserObject());
                }

            }
            else if(targetConnectedCell instanceof TableGraphCell)
            {
                TableElement targetConnectedElement =
                             (TableElement)m_elements.get(targetConnectedCell);

                targetConnectedElement.setAttribute(
                             XmlConstants.TABLE_STREAM_ATTRIBUTE, "");
                
                BoxElement sourceConnectedElement =
                             (BoxElement)m_elements.get(sourceConnectedCell);

                sourceConnectedElement.removeTableref();
            }
        }

        return true;
    }


    private boolean removeTable(TableElement element, TableGraphCell cell)
    {
        boolean ret = m_doc.removeTable(element.getNameValue());

        if (ret)
        {
            Set edgesSet = cell.getTablePort().getEdges();

            if (edgesSet.size()!= 0)
            {
                Object[] edgesArray = new Object[1];
                
                Iterator i = edgesSet.iterator();
                BorealisEdge edge = (BorealisEdge)i.next();
                edgesArray[0] = edge;
                
                BorealisPort sourcePort = (BorealisPort)edge.getSource();
                BoxGraphCell connectedCell = (BoxGraphCell) sourcePort.getParentCell();
                BoxElement connectedElement =(BoxElement) m_elements.get(connectedCell);

                if (connectedElement != null)
                {
                    connectedElement.removeTableref();
                }
                
                m_borealisGraphModel.remove(edgesArray);
            }

            removeMapping(element, cell);
        }

        return ret;
    }
    
    /**
     * Remove a table element from the model.
     */
    public boolean removeTableElement(TableElement element)
    {
        TableGraphCell cell = (TableGraphCell) m_cells.get(element);

        if (cell == null)
        {
            return false;
        }
        
        boolean result = removeTable(element, cell);
        
        Object[] cells = {cell, cell.getTablePort()};
        m_borealisGraphModel.remove(cells);
        m_editorPanel.getNetworkTreeModel().showTreeUpdate();
        return result;
    }
    
    /**
     * Remove a table cell from the model.
     */
    public boolean removeTableCell(TableGraphCell cell)
    {
        TableElement element = (TableElement) m_elements.get(cell);
        
        if (element == null)
        {
            return false;
        }
        
        return removeTable(element, cell);
    }
    
    public boolean removeTablerefElement(TablerefElement element)
    {
        BoxElement boxElement = (BoxElement)element.getParentElement();
        BoxGraphCell boxCell =(BoxGraphCell) getCell(boxElement);
        
        Set edgesSet = boxCell.getTablerefPort().getEdges();
        Object[] edgesArray = new Object[1];

        if (edgesSet.size()!=0)
        {
            Iterator i = edgesSet.iterator();
            BorealisEdge edge = (BorealisEdge)i.next();
            edgesArray[0] = edge;
        }
        
        m_borealisGraphModel.remove(edgesArray);
        boolean result = boxElement.removeTableref();
        m_editorPanel.getNetworkTreeModel().showTreeUpdate();
        return result;
    }
    
    public void connect(PortView source, PortView target)
    {
        BorealisGraphCell sourceCell = (BorealisGraphCell)source.getParentView().getCell();
        BorealisGraphCell targetCell = (BorealisGraphCell)target.getParentView().getCell();
        String edgeName = "stream_name";
        
        connect(edgeName, sourceCell, targetCell);
    }
    
    public boolean removeInElement(InElement inElement)
    {
        BoxElement boxElement   = (BoxElement)inElement.getParentElement();
        BoxGraphCell boxCell = (BoxGraphCell)m_cells.get(boxElement);
        InputPort inputPort  = boxCell.getInputPort();

        String edgeName = inElement.getStream();
        
        Set edgesSet = inputPort.getEdges();
        Object[] edgesArray = new Object[1];
        
        for(Iterator i = edgesSet.iterator(); i.hasNext();)
        {
            BorealisEdge edge = (BorealisEdge)i.next();

            if (edge.getUserObject().equals(edgeName))
            {
                edgesArray[0] = edge;
                
                BorealisPort sourcePort = (BorealisPort)edge.getSource();
                GraphCell connectedCell = sourcePort.getParentCell();

                if (connectedCell instanceof BoxGraphCell)
                {
                    BoxElement connectedElement =(BoxElement) m_elements.get(connectedCell);

                    if (connectedElement != null)
                    {
                        connectedElement.removeOut((String)edge.getUserObject());
                    }
                }
            }
        }

        boxElement.removeIn(edgeName);
        m_borealisGraphModel.remove(edgesArray);
        m_borealisGraph.repaint();
        m_editorPanel.getNetworkTreeModel().showTreeUpdate();

        return true;
    }
    
    public boolean removeOutElement(OutElement outElement)
    {
        BoxElement boxElement = (BoxElement)outElement.getParentElement();
        BoxGraphCell boxCell = (BoxGraphCell)m_cells.get(boxElement);
        OutputPort outputPort = boxCell.getOutputPort();
        String edgeName = outElement.getStream();
        
        Set edgesSet = outputPort.getEdges();
        Object[] edgesArray = new Object[1];
        
        for(Iterator i = edgesSet.iterator(); i.hasNext();)
        {
            BorealisEdge edge = (BorealisEdge)i.next();

            if (edge.getUserObject().equals(edgeName))
            {
                edgesArray[0] = edge;
                
                BorealisPort targetPort = (BorealisPort)edge.getTarget();
                GraphCell connectedCell = targetPort.getParentCell();

                if (connectedCell instanceof BoxGraphCell)
                {
                    BoxElement connectedElement =(BoxElement) m_elements.get(connectedCell);

                    if (connectedElement != null)
                    {
                        connectedElement.removeIn((String)edge.getUserObject());
                    }
                }
                
            }
        }

        boxElement.removeOut(edgeName);
        m_borealisGraphModel.remove(edgesArray);
        m_borealisGraph.repaint();
        m_editorPanel.getNetworkTreeModel().showTreeUpdate();
        return true;
    }
    
    /**
     * Remove a schema element from the model.
     */
    public boolean removeSchemaElement(SchemaElement element)
    {
        String oldValue = element.getNameValue();
        changeSchemaElementName(oldValue, "");
        m_editorPanel.getNetworkTreeModel().showTreeUpdate();
        return m_doc.removeSchema(element.getNameValue());
    }
    
    
    ////////////////////////////////////////////////////////////
    //add a cell or element to the query network
    /**
     * Add a element and a cell from the model.
     */
    public boolean add(BorealisElement element, BorealisGraphCell cell)
    {
        if ((element instanceof InputElement) &&
                (cell instanceof InputGraphCell))
        {
            return addInput((InputElement) element, null,(InputGraphCell) cell);
        }
        else if ((element instanceof OutputElement) &&
                (cell instanceof OutputGraphCell))
        {
            return addOutput((OutputElement) element, (OutputGraphCell) cell);
        }
        else if ((element instanceof BoxElement) &&
                (cell instanceof BoxGraphCell))
        {
            return addBox((BoxElement) element, null, (BoxGraphCell) cell);
        }
        else if ((element instanceof TableElement) &&
                (cell instanceof TableGraphCell))
        {
            return addTable((TableElement) element, null, null, (TableGraphCell) cell);
        }
        else if (element instanceof SchemaElement)
        {
            return addSchema((SchemaElement) element);
        }
        else if (element instanceof QueryElement)
        {
            return addQuery((QueryElement) element);
        }
        
        m_editorPanel.getNetworkTreeModel().showTreeUpdate();
        System.out.println("Error invalid BorealisModel.add(): " + element + "&&" + cell);
        return false;
    }
    
    /**
     * Add a schema element to the model.
     */
    public boolean addSchema(SchemaElement element)
    {
        if (m_doc.getAllSchemaNames().contains(element.getNameValue()))
        {
            JOptionPane.showConfirmDialog(null,
                    "The schema name has already been used. Please change the name.",
                    "Change Schema Name", JOptionPane.CLOSED_OPTION,
                    JOptionPane.WARNING_MESSAGE);
        }

        return m_doc.addSchema(element);
    }


    /**
     * Add a input element to the model.
     */
    public boolean addInput(InputElement   inputElement,
                            SchemaElement  schemaElement,
                            InputGraphCell cell)
    {
        if (m_doc.getAllInputNames().contains(inputElement.getStream()))
        {
            JOptionPane.showConfirmDialog(null,
                    "The input name has already been used. Please change the name.",
                    "Change Input Name", JOptionPane.CLOSED_OPTION,
                    JOptionPane.WARNING_MESSAGE);
        }
 
        if(schemaElement == null)
        {
            List schemaList = m_doc.getAllSchemas();

            if (schemaList.size()!=0)
            {
                schemaElement = (SchemaElement)schemaList.get(0);
            }
            else
            {
                schemaElement = new SchemaElement();
                m_doc.addSchema(schemaElement);
            }
        }

        inputElement.setSchema(schemaElement.getNameValue());
        boolean ret = m_doc.addInput(inputElement);

        if (ret)
        {
            cell.setUserObject(inputElement.getStream());
            m_borealisGraphModel.insertCell(cell);
            addMapping(inputElement, cell);
        }

        return ret;
    }


    /**
     * Add an output element to the model.
     */
    public boolean addOutput(OutputElement outputElement, OutputGraphCell cell)
    {
        if (m_doc.getAllOutputNames().contains(outputElement.getStream()))
        {
            JOptionPane.showConfirmDialog(null,
                    "The output name has already been used. Please change the name.",
                    "Change Output Name", JOptionPane.CLOSED_OPTION,
                    JOptionPane.WARNING_MESSAGE);
        }
        
        boolean ret = m_doc.addOutput(outputElement);

        if (ret)
        {
            cell.setUserObject(outputElement.getStream());
            m_borealisGraphModel.insertCell(cell);
            addMapping(outputElement, cell);
        }
        
        return ret;
    }
    
    /**
     * Add a query element to the model.
     */
    public boolean addQuery(QueryElement element)
    {
        if (m_doc.getAllQueryNames().contains(element.getNameValue()))
        {
            JOptionPane.showConfirmDialog(null,
                    "The query name has already been used. Please change the name.",
                    "Change Query Name", JOptionPane.CLOSED_OPTION,
                    JOptionPane.WARNING_MESSAGE);
        }

        return m_doc.addQuery(element);
    }
    
    /**
     * Remove a query element from the model.
     */
    public boolean removeQueryElement(QueryElement element)
    {
        Vector boxNames = element.getAllBoxNames();

        for (Iterator i = boxNames.iterator(); i.hasNext();)
        {
            String boxName =(String) i.next();
            BoxElement boxElement = m_doc.getBox(boxName);

            if (!removeBoxElement(boxElement))
            {
                return false;
            }
        }
        
        Vector tableNames = element.getAllTableNames();

        for (Iterator j = tableNames.iterator(); j.hasNext();)
        {
            String tableName =(String) j.next();
            TableElement tableElement = m_doc.getTable(tableName);

            if (!removeTableElement(tableElement))
            {
                return false;
            }
        }

        m_editorPanel.getNetworkTreeModel().showTreeUpdate();
        return m_doc.removeQuery(element.getNameValue());
    }
    
    /**
     * Add an box element to the model.
     */
    public boolean addBox(BoxElement    boxElement,
                          QueryElement  queryElement,
                          BoxGraphCell  cell)
    {
        DebugTrace.trace(boxElement.getNameValue());

        if (m_doc.getAllBoxNames().contains(boxElement.getNameValue()))
        {
            JOptionPane.showConfirmDialog(null,
                "The box name has already been used. Please change the name.",
                "Change Box Name", JOptionPane.CLOSED_OPTION,
                JOptionPane.WARNING_MESSAGE);
        }
        
        if (queryElement == null)
        {
            List queryList = m_doc.getAllQueries();

            if (queryList.size() != 0)
            {
                queryElement = (QueryElement)queryList.get(0);
            }
            else
            {   queryElement = new QueryElement();
                m_doc.addQuery(queryElement);
            }
        }

        boolean ret = m_doc.addBox(boxElement, queryElement);

        if (ret)
        {
            cell.setUserObject(boxElement.getNameValue());
            m_borealisGraphModel.insertCell(cell);
            addMapping(boxElement, cell);
        }
        
        return ret;
    }
    
    
    /**
     * Add a table element to the model.
     */
    public boolean addTable(TableElement   tableElement,
                            QueryElement   queryElement,
                            SchemaElement  schemaElement,
                            TableGraphCell cell)
    {
        if (m_doc.getAllTableNames().contains(tableElement.getNameValue()))
        {
            JOptionPane.showConfirmDialog(null,
                "The table name has already been used. Please change the name.",
                "Change Table Name", JOptionPane.CLOSED_OPTION,
                JOptionPane.WARNING_MESSAGE);
        }
        
        if (queryElement == null)
        {
            List queryList = m_doc.getAllQueries();

            if (queryList.size()!=0)
            {
                queryElement = (QueryElement)queryList.get(0);
            }
            else
            {
                queryElement = new QueryElement();
                m_doc.addQuery(queryElement);
            }
        }
        
        if (schemaElement == null)
        {
            List schemaList = m_doc.getAllSchemas();

            if (schemaList.size()!=0)
            {
                schemaElement = (SchemaElement)schemaList.get(0);
            }
            else
            {
                schemaElement = new SchemaElement();
                m_doc.addSchema(schemaElement);
            }
        }

        tableElement.setSchema(schemaElement.getNameValue());
        
        boolean ret = m_doc.addTable(tableElement, queryElement);

        if (ret)
        {
            cell.setUserObject(tableElement.getNameValue());
            m_borealisGraphModel.insertCell(cell);
            addMapping(tableElement, cell);
        }
        
        return ret;
    }
    
    
    public void connect(String             edgeName,
                        BorealisGraphCell  sourceCell,
                        BorealisGraphCell  targetCell)
    {
        if (sourceCell instanceof InputGraphCell)
        {
            if (((InputGraphCell)sourceCell).getOutputPort().getEdges().size()!=0)
            {
                JOptionPane.showConfirmDialog(null,
                    "The input cell has only one stream.",
                    "Error on input cell", JOptionPane.CLOSED_OPTION,
                    JOptionPane.WARNING_MESSAGE);
            }
            
            edgeName = (String)sourceCell.getUserObject();

            if (targetCell instanceof BoxGraphCell)
            {
                BoxElement  targetBoxElement = (BoxElement)getElement(targetCell);
                InElement   inElement = new InElement(edgeName);
                targetBoxElement.addIn(inElement);
                
                m_borealisGraphModel.connect(sourceCell.getOutputPort(), targetCell.getInputPort(), edgeName);
            }
            
        }
        else if (sourceCell instanceof BoxGraphCell)
        {
            if (targetCell instanceof OutputGraphCell)
            {
                if (((OutputGraphCell)targetCell).getInputPort().getEdges().size()!=0)
                {
                    JOptionPane.showConfirmDialog(null,
                        "The output cell can only has one stream.",
                        "Warning on output cell", JOptionPane.CLOSED_OPTION,
                        JOptionPane.WARNING_MESSAGE);
                }
                
                edgeName = (String)targetCell.getUserObject();
                
                BoxElement sourceBoxElement = (BoxElement)getElement(sourceCell);
                OutElement outElement = new OutElement(edgeName);
                sourceBoxElement.addOut(outElement);
                
                m_borealisGraphModel.connect(sourceCell.getOutputPort(), targetCell.getInputPort(), edgeName);
            }
            else if (targetCell instanceof BoxGraphCell)
            {
                edgeName = "stream_name" + edgeNameCount++;
                
                BoxElement targetBoxElement = (BoxElement)getElement(targetCell);
                InElement inElement = new InElement(edgeName);
                targetBoxElement.addIn(inElement);
                
                BoxElement sourceBoxElement = (BoxElement)getElement(sourceCell);
                OutElement outElement = new OutElement(edgeName);
                sourceBoxElement.addOut(outElement);
                
                m_borealisGraphModel.connect(sourceCell.getOutputPort(), targetCell.getInputPort(), edgeName);
            }
            else if (targetCell instanceof TableGraphCell)
            {
                if (((TableGraphCell)targetCell).getTablePort().getEdges().size()!=0)
                {
                    JOptionPane.showConfirmDialog(null,
                            "The table cell can only has one stream.",
                            "Warning on table cell", JOptionPane.CLOSED_OPTION,
                            JOptionPane.WARNING_MESSAGE);
                }
                
                if (((BoxGraphCell)sourceCell).isTableBox())
                {
                    edgeName = "stream_name" + edgeNameCount++;
                    
                    TableElement tableElement =
                                     (TableElement)getElement(targetCell);

                    tableElement.setAttribute(
                        XmlConstants.TABLE_STREAM_ATTRIBUTE, edgeName);
                    
                    BoxElement sourceBoxElement =
                                     (BoxElement)getElement(sourceCell);
                    
                    TablerefElement tablerefElement =
                       new TablerefElement((String)targetCell.getUserObject());

                    sourceBoxElement.addTableref(tablerefElement);
                }
                
                m_borealisGraphModel.connect(((BoxGraphCell)sourceCell).getTablerefPort(),((TableGraphCell)targetCell).getTablePort(), edgeName);
            }
        }
    }
    
    
    //////////////////////////////////////////////////////////////////////////////
    //Change Query Network XML Tree
    public void changeSchemaElementName(String oldValue, String newValue)
    {
        m_doc.changeSchemaName(oldValue, newValue);
        m_editorPanel.getNetworkTreeModel().showTreeUpdate();
    }
    
    public void changeInputElementStream(InputElement  inputElement,
                                      String        oldValue,
                                      String        newValue)
    {
        InputGraphCell inputGraphCell = (InputGraphCell)m_cells.get(inputElement);
        inputGraphCell.setUserObject(newValue);
        
        changeInputPort(inputGraphCell, oldValue, newValue);
        
        m_borealisGraph.repaint();
    }
    
    public void changeInputCellName(InputGraphCell inputGraphCell, String oldValue, String newValue)
    {
        Vector inputNames = m_doc.getAllInputNames();

        if (inputNames.contains(newValue))
        {
            JOptionPane.showConfirmDialog(null,
                    "The input cell name has already exist.",
                    "Warning", JOptionPane.CLOSED_OPTION,
                    JOptionPane.WARNING_MESSAGE);
        }

        InputElement inputElement = (InputElement) m_elements.get(inputGraphCell);
        inputElement.setAttribute(XmlConstants.INPUT_STREAM_ATTRIBUTE, newValue);
        changeInputPort(inputGraphCell, oldValue, newValue);
    }
    
    private void changeInputPort(InputGraphCell inputGraphCell,String oldValue, String newValue)
    {
        OutputPort outputPort = inputGraphCell.getOutputPort();
        Set edgesSet = outputPort.getEdges();

        if (edgesSet.size()!=0)
        {
            Iterator i = edgesSet.iterator();
            BorealisEdge edge = (BorealisEdge)i.next();
            edge.setUserObject(newValue);
            BorealisPort targetPort = (BorealisPort)edge.getTarget();
            
            GraphCell connectedCell = targetPort.getParentCell();

            if (connectedCell instanceof BoxGraphCell)
            {
                BoxElement connectedElement =(BoxElement) m_elements.get(connectedCell);

                if (connectedElement != null)
                {
                    //connectedElement.removeIn((String) cell.getUserObject());
                    connectedElement.changeIn(oldValue, newValue);
                }
            }
        }

        m_editorPanel.getNetworkTreeModel().showTreeUpdate();
    }
    
    public void changeOutputElementStream(OutputElement  outputElement,
                                          String         oldValue,
                                          String         newValue)
    {
        OutputGraphCell outputGraphCell = (OutputGraphCell)m_cells.get(outputElement);
        outputGraphCell.setUserObject(newValue);
        changeOutputPort(outputGraphCell, oldValue, newValue);
        m_borealisGraph.repaint();
    }
    
    public void changeOutputCellName(OutputGraphCell outputGraphCell, String oldValue, String newValue)
    {
        Vector outputNames = m_doc.getAllOutputNames();

        if (outputNames.contains(newValue))
        {
            JOptionPane.showConfirmDialog(null,
                "The output cell name has already exist.",
                "Warning", JOptionPane.CLOSED_OPTION,
                JOptionPane.WARNING_MESSAGE);
        }
        
        OutputElement outputElement =
                          (OutputElement) m_elements.get(outputGraphCell);

        outputElement.setAttribute(
                          XmlConstants.OUTPUT_STREAM_ATTRIBUTE, newValue);

        changeOutputPort(outputGraphCell, oldValue, newValue);
    }
    
    private void changeOutputPort(OutputGraphCell outputGraphCell,String oldValue, String newValue)
    {
        InputPort inputPort = outputGraphCell.getInputPort();
        Set edgesSet = inputPort.getEdges();

        if (edgesSet.size()!=0)
        {
            Iterator i = edgesSet.iterator();
            BorealisEdge edge = (BorealisEdge)i.next();
            edge.setUserObject(newValue);
            
            BorealisPort sourcePort = (BorealisPort)edge.getSource();
            GraphCell connectedCell = sourcePort.getParentCell();

            if (connectedCell instanceof BoxGraphCell)
            {
                BoxElement connectedElement = (BoxElement)m_elements.get(connectedCell);

                if (connectedElement != null)
                {
                    connectedElement.changeOut(oldValue, newValue);
                }
            }
        }

        m_editorPanel.getNetworkTreeModel().showTreeUpdate();
    }


    public void changeElementSchemaAttribute(String newValue)
    {
        SchemaElement schemaElement = m_doc.getSchema(newValue);

        if (schemaElement == null)
        {
            JOptionPane.showConfirmDialog(null,
                "There is no schema named '" + newValue + "'. Please use 'Add Schema' to add it.",
                "No existing schema name", JOptionPane.CLOSED_OPTION,
                JOptionPane.WARNING_MESSAGE);
        }
    }
    
    public void changeBoxElementName(BoxElement  boxElement,
                                     String      newValue)
    {
        BoxGraphCell boxGraphCell = (BoxGraphCell)m_cells.get(boxElement);
        boxGraphCell.setUserObject(newValue);

        m_editorPanel.getNetworkTreeModel().showTreeUpdate();
        m_borealisGraph.repaint();
    }
    
    public void changeBoxCellName(BoxGraphCell boxGraphCell, String newValue)
    {
        Vector boxNames = m_doc.getAllBoxNames();

        if (boxNames.contains(newValue))
        {
            JOptionPane.showConfirmDialog(null,
                "The box cell name has already exist.",
                "Warning", JOptionPane.CLOSED_OPTION,
                JOptionPane.WARNING_MESSAGE);
        }
        
        BoxElement boxElement = (BoxElement)m_elements.get(boxGraphCell);
        boxElement.setAttribute(XmlConstants.NAME_ATTRIBUTE, newValue);
        m_editorPanel.getNetworkTreeModel().showTreeUpdate();
        
    }

    public void changeInElementStream(BoxElement    boxElement,
                                      String        oldValue,
                                      String        newValue)
    {
        BoxGraphCell boxGraphCell = (BoxGraphCell)m_cells.get(boxElement);
        InputPort inputPort = boxGraphCell.getInputPort();
        
        Set edgesSet = inputPort.getEdges();

        for (Iterator i = edgesSet.iterator(); i.hasNext();)
        {
            BorealisEdge edge = (BorealisEdge)i.next();

            if (edge.getUserObject().equals(oldValue))
            {
                if (((BorealisPort)edge.getSource()).getParentCell() instanceof InputGraphCell)
                {
                    JOptionPane.showConfirmDialog(null,
                            "You'd better change the input stream from graph.",
                            "Warning", JOptionPane.CLOSED_OPTION,
                            JOptionPane.WARNING_MESSAGE);
                }
                else
                {
                    edge.setUserObject(newValue);
                    changeEdgeSource(edge, oldValue, newValue);
                }
            }
        }

        m_borealisGraph.repaint();
    }
    
    public void changeOutElementStream(BoxElement   boxElement,
                                       String       oldValue,
                                       String       newValue)
    {
        BoxGraphCell boxGraphCell = (BoxGraphCell)m_cells.get(boxElement);
        OutputPort outputPort = boxGraphCell.getOutputPort();
        
        Set edgesSet = outputPort.getEdges();

        for (Iterator i = edgesSet.iterator(); i.hasNext();)
        {
            BorealisEdge edge = (BorealisEdge)i.next();

            if (edge.getUserObject().equals(oldValue))
            {
                edge.setUserObject(newValue);
                changeEdgeTarget(edge, oldValue, newValue);
            }
        }

        m_borealisGraph.repaint();
    }
    
    public void changeEdgeCellName(BorealisEdge edge, String oldValue, String newValue)
    {
        Set cells = m_elements.keySet();
        int cellNumber = cells.size();
        Object[] allCells = new Object[cellNumber];
        int index = 0;

        for (Iterator j = cells.iterator(); j.hasNext();)
        {
            allCells[index] = j.next();
            index++;
        }

        int twice = 0;
        Set edgesSet = DefaultGraphModel.getEdges(m_borealisGraphModel,allCells);

        for (Iterator i = edgesSet.iterator(); i.hasNext();)
        {
            BorealisEdge existEdge = (BorealisEdge) i.next();

            if (existEdge.getUserObject().equals(newValue))
            {
                twice = twice + 1;

                if (twice==2)
                {
                    JOptionPane.showConfirmDialog(null,
                        "The edge cell name has already exist.",
                        "Warning", JOptionPane.CLOSED_OPTION,
                        JOptionPane.WARNING_MESSAGE);
                }
            }
        }
        
        if (((BorealisPort)edge.getTarget()).getParentCell() instanceof TableGraphCell)
        {
            BorealisPort targetPort = (BorealisPort)edge.getTarget();
            TableGraphCell connectedCell =(TableGraphCell) targetPort.getParentCell();
            TableElement tableElement = (TableElement)m_elements.get(connectedCell);
            tableElement.setAttribute(XmlConstants.TABLE_STREAM_ATTRIBUTE, newValue);
        }
        else
        {
            changeEdgeSource(edge, oldValue, newValue);
            changeEdgeTarget(edge, oldValue, newValue);
        }
    }
    
    private void changeEdgeSource(BorealisEdge edge, String oldValue, String newValue)
    {
        BorealisPort sourcePort = (BorealisPort)edge.getSource();
        GraphCell connectedCell = sourcePort.getParentCell();

        if (connectedCell instanceof BoxGraphCell)
        {
            BoxElement connectedElement =(BoxElement) m_elements.get(connectedCell);

            if (connectedElement != null)
            {
                connectedElement.changeOut(oldValue, newValue);
            }
        }
        else if(connectedCell instanceof InputGraphCell)
        {
            ((InputGraphCell)connectedCell).setUserObject(newValue);
            InputElement inputElement = (InputElement) m_elements.get(connectedCell);
            inputElement.setAttribute(XmlHandler.INPUT_STREAM_ATTRIBUTE, newValue);
        }
    }
    
    private void changeEdgeTarget(BorealisEdge edge, String oldValue, String newValue)
    {
        BorealisPort targetPort = (BorealisPort)edge.getTarget();
        GraphCell connectedCell = targetPort.getParentCell();

        if (connectedCell instanceof BoxGraphCell)
        {
            BoxElement connectedElement =(BoxElement) m_elements.get(connectedCell);

            if (connectedElement != null)
            {
                connectedElement.changeIn(oldValue, newValue);
            }
        }
        else if (connectedCell instanceof OutputGraphCell)
        {
            ((OutputGraphCell)connectedCell).setUserObject(newValue);
            OutputElement outputElement = (OutputElement) m_elements.get(connectedCell);
            outputElement.setAttribute(XmlHandler.OUTPUT_STREAM_ATTRIBUTE, newValue);
        }
    }

    
    public void changeTableElementName(TableElement   tableElement,
                                       String         newValue)
    {
        TableGraphCell tableGraphCell = (TableGraphCell)m_cells.get(tableElement);
        tableGraphCell.setUserObject(newValue);
        
        changeTableref(tableGraphCell, newValue);
        m_borealisGraph.repaint();
    }
    

    public void changeTableCellName(TableGraphCell cell, String newValue)
    {
        Vector tableNames = m_doc.getAllTableNames();

        if (tableNames.contains(newValue))
        {
            JOptionPane.showConfirmDialog(null,
                "The table cell name has already exist.",
                "Warning", JOptionPane.CLOSED_OPTION,
                JOptionPane.WARNING_MESSAGE);
        }
        
        TableElement element = (TableElement)m_elements.get(cell);
        element.setAttribute(XmlConstants.NAME_ATTRIBUTE, newValue);
        changeTableref(cell, newValue);
    }
    
    private void changeTableref(TableGraphCell tableGraphCell, String newValue)
    {
        Set edgesSet = tableGraphCell.getTablePort().getEdges();

        if (edgesSet.size()!=0)
        {
            Iterator i = edgesSet.iterator();
            BorealisEdge edge = (BorealisEdge)i.next();
            BorealisPort sourcePort = (BorealisPort)edge.getSource();
            
            BoxGraphCell connectedCell = (BoxGraphCell) sourcePort.getParentCell();
            BoxElement connectedElement =(BoxElement) m_elements.get(connectedCell);

            if (connectedElement != null)
            {
                connectedElement.changeTableref(newValue);
            }
        }

        m_editorPanel.getNetworkTreeModel().showTreeUpdate();
    }
    
    public void changeTableElementStream(TableElement   tableElement,
                                         String         newValue)
    {
        TableGraphCell tableGraphCell = (TableGraphCell)m_cells.get(tableElement);
        Set edgesSet = tableGraphCell.getTablePort().getEdges();

        if (edgesSet.size()!=0)
        {
            Iterator i = edgesSet.iterator();
            BorealisEdge edge = (BorealisEdge)i.next();
            edge.setUserObject(newValue);
        }
    }
    
    
    ////////////////////////////////////////////////////////////////////////
    //change deploy XML Tree
    //
    public void changeNodeElementEndpoint(String oldValue, String newValue)
    {
        Object[] allCells = m_borealisGraph.getRoots();

        for (int i = 0; i<allCells.length; i++)
        {
            if (allCells[i] instanceof HasNodeCell)
            {
                HasNodeElement element = (HasNodeElement) m_elements.get(allCells[i]);

                if (element.getNodeAttribute().equals(oldValue))
                {
                    element.setNodeAttribute(newValue);
                }
            }
        }
    }
}


/////////////////////////  end BorealisModel.java  /////////////////////////////
