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
 * A class that represents a Edge in Borealis Mondel.
 */
public class BorealisEdge extends DefaultEdge {

	/**
         * Constructor.
         */
	public BorealisEdge() {
		super();
		AttributeMap map = new AttributeMap();
		GraphConstants.setLineColor(map, Color.black);
		GraphConstants.setBendable(map, true);
		GraphConstants.setLineEnd(map, GraphConstants.ARROW_CLASSIC);
		//GraphConstants.setLineWidth(map, 1); 
		GraphConstants.setEndFill(map, true);
		setAttributes(map);
	}
	
        /**
         * Constructor.
         */
	public BorealisEdge(String edgeName) {
		super(edgeName);
		AttributeMap map = new AttributeMap();
		GraphConstants.setLineColor(map, Color.black);
		GraphConstants.setBendable(map, true);
		GraphConstants.setLineEnd(map, GraphConstants.ARROW_CLASSIC);
		GraphConstants.setLineWidth(map, 1); 
		//GraphConstants.setEndFill(map, true);
		setAttributes(map);
	}
        
        /**
         * Set color of the edge.
         */
	public void setColor(Color color) {
		GraphConstants.setLineColor(getAttributes(), color);
	}

        /**
         * Get the color of the edge.
         */
	public Color getColor() {
		return GraphConstants.getLineColor(getAttributes());
	}


}


