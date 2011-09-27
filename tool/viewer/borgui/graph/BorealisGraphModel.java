package borgui.graph;
import java.util.*;
import org.jgraph.event.GraphModelEvent;
import org.jgraph.event.GraphModelListener;
import org.jgraph.graph.*;


/**
 * A class that defines a suitable data model for a BorealisGraph.
 */
public class BorealisGraphModel extends DefaultGraphModel
{
    protected boolean        m_buffering = false;

    protected Vector<Object> m_bufferedCells = new Vector<Object>();

    protected Map<Object, AttributeMap>  m_bufferedMap =
                                     new Hashtable<Object, AttributeMap>();

    protected ConnectionSet  m_bufferedConnections = new ConnectionSet();
    
    
    /**
     * Constructor.
     */
    public BorealisGraphModel() {}

    
    /**
     * Set wheather all the cells are buffered temperarilly.
     */
    public void setBuffering(boolean val)
    {
        m_buffering = val;
    }

    
    /**
     * Add all the cells in the buffer to the model.
     */
    public void flushBuffer()
    {
        insert(m_bufferedCells.toArray(),
                m_bufferedMap,
                m_bufferedConnections, null, null);

        clearBuffer();
    }

    
    /**
     * Clear the buffer.
     */
    public void clearBuffer()
    {
        m_bufferedCells.clear();
        m_bufferedCells = new Vector<Object>();
        m_bufferedMap.clear();
        m_bufferedMap = new Hashtable<Object, AttributeMap>();
        m_bufferedConnections = new ConnectionSet();
    }

    
    /**
     * Insert a cell to the model.
     */
    public void insertCell(BorealisGraphCell cell)
    {
        Map<Object, AttributeMap> attrMap
            = new Hashtable<Object, AttributeMap>();

        attrMap.put(cell, cell.getAttributes());

        //attrMap.putAll(cell.getPortMap());
        AttributeMap  attributes = cell.getPortMap();

        for(Iterator i = attributes.keySet().iterator(); i.hasNext(); )
        {
            Object  port = i.next();

            attrMap.put(port, (AttributeMap)attributes.get(port));
        }

        Vector<BorealisGraphCell> toinsert = new Vector<BorealisGraphCell>();

        toinsert.add(cell);
//      toinsert.add(cell.getChildren());
//          Object[] insert = new Object[] { cell };
//          insert(insert, attrMap,  new ConnectionSet(), new ParentMap(), null);

        if (m_buffering)
        {
            m_bufferedCells.addAll(toinsert);
            m_bufferedMap.putAll(attrMap);
        }
        else
        {   insert(toinsert.toArray(), attrMap,  new ConnectionSet(), null, null);
            toBack(toinsert.toArray());
        }
    }

    
    /**
     * Insert a edge to the model.
     */
    public void insertEdge(Edge edge)
    {
        Map<Edge, AttributeMap>  attrMap = new Hashtable<Edge, AttributeMap>();
        attrMap.put(edge, edge.getAttributes());

        // Create ConnectionSet for Insertion
        ConnectionSet cs = new ConnectionSet(edge, edge.getSource(), edge.getTarget());

        // Add Edge and Connections to the Model
        Vector<Edge>  insert = new Vector<Edge>();
        insert.add(edge);
        
        if (m_buffering)
        {
            m_bufferedCells.addAll(insert);
            m_bufferedMap.putAll(attrMap);
            m_bufferedConnections.connect(edge, edge.getSource(), edge.getTarget());
        }
        else
        {   insert(insert.toArray(), attrMap, cs, null, null);
        }
    }

    
    public void connect(Port sourcePort, Port targetPort, String edgeName)
    {

        if (sourcePort!=null && targetPort!=null)
        {
            //Create Edge
            BorealisEdge edge = new BorealisEdge(edgeName);

            if (acceptsSource(edge, sourcePort) && acceptsTarget(edge, targetPort))
            {
                // Create a Map thath holds the attributes for the edge
                edge.getAttributes().applyMap(createEdgeAttributes());

                // Insert the Edge and its Attributes
                edge.setSource(sourcePort);
                edge.setTarget(targetPort);
                insertEdge(edge);
            }
        }
    }
    
    
    /**
     * Connect two components (a tableref port and a table port) in the model.
     */
    public void connect(TablerefPort out, TablePort in)
    {
        BorealisEdge edge = new BorealisEdge();
        edge.setSource(out);
        edge.setTarget(in);
        insertEdge(edge);
    }

    
    // Hook for subclassers
    public Map createEdgeAttributes()
    {
        Map map = new Hashtable();

        // Add a Line End Attribute
        GraphConstants.setLineEnd(map, GraphConstants.ARROW_SIMPLE);

        // Add a label along edge attribute
        GraphConstants.setLabelAlongEdge(map, true);
        GraphConstants.setEditable(map, true);
        GraphConstants.setBendable(map, false);
        
        return map;
    }
    
    
    /**
     * Returns true if port is a valid target for edge.
     */
    public boolean acceptsTarget(Object edge, Object port)
    {
        return true;
    }

    
    /**
     * Returns true if port is a valid source for edge.
     */
    public boolean acceptsSource(Object edge, Object port)
    {
        return true;
    }
}


//////////////////////  end graph/BorealisGraphModel.java  /////////////////////
