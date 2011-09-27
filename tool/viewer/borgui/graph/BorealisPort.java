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
 * A class that represent a Borealis port.
 */
public class BorealisPort  extends DefaultPort
{
    GraphCell    m_parentCell;


    /**
     * Constructor.
     */
    public BorealisPort(GraphCell    parentCell,
                        Rectangle2D  bounds,
                        Color        color)
    {
        super();

        m_parentCell = parentCell;

        AttributeMap  map = new AttributeMap();

        GraphConstants.setBounds(map, bounds);
        GraphConstants.setBackground(map, color);
        GraphConstants.setGradientColor(map, Color.lightGray);
        GraphConstants.setOpaque(map, true);

        setAttributes(map);
    }
   
 
    public BorealisPort()
    {
        super();
    }
    

    public GraphCell getParentCell()
    {
        return  m_parentCell;
    }
}


///////////////////////  end  BorealisPort.java  ///////////////////////////////
