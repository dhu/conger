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
 * A class that represent a Borealis output Graph Cell.
 */
public class OutputGraphCell  extends BorealisGraphCell
                           implements HasSchemaCell
{
    protected InputPort  m_input;
    
    
    /**
     * Default Constructor.
     */
    public OutputGraphCell()
    {
        super(new Rectangle(0, 0, 70, 70),
              Color.GRAY,
              (Icon)new ImageIcon(borgui.editor.EditorConstants.OUTPUT_ICON));

        m_input = new InputPort(this);
        add(m_input);
    }

    
    public OutputGraphCell(String name)
    {
        super(name,
              new Rectangle(0, 0, 70, 70),
              Color.GRAY,
              (Icon) new ImageIcon(borgui.editor.EditorConstants.OUTPUT_ICON));

        m_input = new InputPort(this);
        add(m_input);
    }
    
    
    /**
     * Get its input port
     */
    public InputPort getInputPort()
    {
        return m_input;
    }
   
 
    /**
     * Get the AttributeMap of its input port.
     */
    public AttributeMap getPortMap()
    {
        Map<InputPort, Map>  map = new Hashtable<InputPort, Map>();
        
        int u = GraphConstants.PERMILLE;
        
        GraphConstants.setOffset(m_input.getAttributes(), new Point(0, u / 2));
        map.put(m_input, m_input.getAttributes());
        
        AttributeMap portMap = new AttributeMap(map);
        return portMap;
    }


    public void changeNodeColor(Color nodeColor)
    {
        DebugTrace.trace("set output color");
        AttributeMap map = getAttributes();
        GraphConstants.setBackground(map, nodeColor);
    }
}


//////////////////////////  end  OutputGraphCell.java  /////////////////////////
