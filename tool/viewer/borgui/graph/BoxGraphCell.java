package borgui.graph;

import javax.swing.*;
import java.awt.*;
import java.awt.geom.Rectangle2D;
import java.awt.event.*;
import java.util.*;
import org.jgraph.graph.*;
import borgui.xml.*;
import borgui.common.*;


/**
 * A class that represent a Borealis Box Graph Cell.
 */
public class BoxGraphCell  extends BorealisGraphCell
                        implements HasQueryCell, HasNodeCell
{
    protected InputPort     m_input;
    protected OutputPort    m_output;
    
    protected TablerefPort  m_tableref;
    protected boolean       m_tablebox = false;

    
    /**
     * Default Constructor.
     */
    public BoxGraphCell()
    {
        super(new Rectangle(0, 0, 70, 70), Color.GRAY, null);
        m_tableref = new TablerefPort(this);
        
        m_input = new InputPort(this);
        add(m_input);
        m_output = new OutputPort(this);
        add(m_output);
    }

    
    /**
     * Get its input port by the supplied number.
     */
    public InputPort getInputPort()
    {
        return m_input;
    }

    
    /**
     * Get its output port by the supplied number.
     */
    public OutputPort getOutputPort()
    {
        return m_output;
    }

    
    /**
     * Set its icon.
     */
    public void setIcon(Icon icon)
    {
        GraphConstants.setIcon((Map) getAttributes(), icon);
    }
   
 
    public void setTableBox(boolean value)
    {
        if (value && !m_tablebox)
        {
            add(m_tableref);
        }
        else if (!value && m_tablebox)
        {
            remove(m_tableref);
        }

        m_tablebox = value;
    }

    
    /**
     * Return wheather it is a table box.
     */
    public boolean isTableBox()
    {
        return m_tablebox;
    }
    

    /**
     * Get its tableref port
     */
    public TablerefPort getTablerefPort()
    {
        if (m_tablebox)
        {
            return m_tableref;
        }
        
        return null;
    }
    

    /**
     * Get the AttributeMap of all its ports.
     */
    public AttributeMap  getPortMap()
    {
        Map<BorealisPort, Map>  map = new Hashtable<BorealisPort, Map>();
         
        int u = GraphConstants.PERMILLE;
        GraphConstants.setOffset(m_input.getAttributes(), new Point(0, u / 2));
        map.put(m_input, m_input.getAttributes());
        
        GraphConstants.setOffset(m_output.getAttributes(), new Point(u, u / 2));
        map.put(m_output, m_output.getAttributes());
        
        AttributeMap  portMap = new AttributeMap(map);
        return portMap;
    }

    
    public void changeNodeColor(Color nodeColor)
    {
        AttributeMap map = getAttributes();
        GraphConstants.setBackground(map, nodeColor);
    }
}


///////////////////////////  end  BoxGraphCell.java  ///////////////////////////
