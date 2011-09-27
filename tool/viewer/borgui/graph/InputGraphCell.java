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
 * A class that represent a Borealis input Graph Cell.
 */
public class InputGraphCell  extends BorealisGraphCell
                          implements HasSchemaCell, HasNodeCell
{
    
    protected OutputPort   m_output;
    
    
    /**
     * Default Constructor.
     */
    public InputGraphCell()
    {
        super(new Rectangle(0, 0, 70, 70),
                Color.GRAY, (Icon) new ImageIcon(borgui.editor.EditorConstants.INPUT_ICON));

        m_output = new OutputPort(this);
        add(m_output);
    }
    
     public InputGraphCell(String name)
     {
        super(name, new Rectangle(0, 0, 70, 70),
                Color.GRAY, (Icon) new ImageIcon(borgui.editor.EditorConstants.INPUT_ICON));

        m_output = new OutputPort(this);
        add(m_output);
    }
    
    /**
     * Get its output port
     */
    public OutputPort getOutputPort()
    {
        return m_output;
    }
    
    /**
     * Get the AttributeMap of its output port.
     */
    public AttributeMap getPortMap()
    {
        Map<OutputPort, Map>  map = new Hashtable<OutputPort, Map>();

        int u = GraphConstants.PERMILLE;
        
        GraphConstants.setOffset(m_output.getAttributes(),
                                 new Point(u, u / 2));

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


//////////////////////////  end InputGraphCell.java  ///////////////////////////
