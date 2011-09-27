package borgui.graph;

import javax.swing.*;
import java.awt.*;
import java.awt.geom.Rectangle2D;
import java.awt.event.*;
import java.util.*;
import org.jgraph.graph.*;


/**
 * A class that represent Borealis Graph cell.
 */
public class BorealisGraphCell extends DefaultGraphCell
{
    /**
     * Constructor.
     */
    public BorealisGraphCell(Rectangle2D bounds, Color color, Icon icon)
    {
        super();
        setup(bounds, color, icon);
    }

    
    public BorealisGraphCell(String name, Rectangle2D bounds, Color color, Icon icon)
    {
        super(name);
        setup(bounds, color, icon);
    }
    
    
    /**
     * To be used in constructor. Set bound, color and icon.
     */
    private void setup(Rectangle2D bounds, Color color, Icon icon)
    {
        AttributeMap map = new AttributeMap();
        
        GraphConstants.setBounds(map, bounds);
        GraphConstants.setBackground(map, color);
        GraphConstants.setGradientColor(map, Color.lightGray);
        //  GraphConstants.setBorder(map, );
        GraphConstants.setBorderColor(map, Color.black);
        GraphConstants.setOpaque(map, true);
//      GraphConstants.setLabelAlongEdge(map, true);
        GraphConstants.setLabelPosition(map, new Point(0, 0));
        
        if (icon != null)
        {
            GraphConstants.setIcon(map, icon);
        }
        setAttributes(map);
    }

    
    /**
     * Get the icon of the cell.
     */
    public Icon getIcon()
    {
        return GraphConstants.getIcon((Map) getAttributes());
    }
   
 
    /**
     * Set the icon of the cell.
     */
    public void setIcon(Icon icon)
    {
        GraphConstants.setIcon((Map) getAttributes(), icon);
    }
   
 
    /**
     * Get the Background of the cell.
     */
    public Color getBackground()
    {
        return GraphConstants.getBackground((Map) getAttributes());
    }


    /**
     * Set the Background of the cell.
     */
    public void setBackground(Color color)
    {
        GraphConstants.setBackground((Map) getAttributes(), color);
    }
   
 
    /**
     * Get the bound of the cell.
     */
    public Rectangle2D getBounds()
    {
        return GraphConstants.getBounds((Map) getAttributes());
    }
   
 
    /**
     * Set the bound of the cell.
     */
    public void setBounds(Rectangle2D bounds)
    {
        GraphConstants.setBounds((Map) getAttributes(), bounds);
    }
   
 
    /**
     * Get attribute map of the port
     */
    public AttributeMap getPortMap()
    {
        AttributeMap portMap = new AttributeMap();
        return portMap;
    }
   
 
    /**
     * get a copy of the cell.
     */
    public Object clone()
    {
        BorealisGraphCell cell = (BorealisGraphCell) super.clone();

        cell.setup(GraphConstants.getBounds(getAttributes()),
                GraphConstants.getBackground(getAttributes()),
                GraphConstants.getIcon(getAttributes()));

        return (Object) cell;
    }
    

    public BorealisPort getInputPort()
    {
        return null;
    }
   
 
    public BorealisPort getOutputPort()
    {
        return null;
    }
}


//////////////////////////  end  BorealisGraphCell.java  ///////////////////////
