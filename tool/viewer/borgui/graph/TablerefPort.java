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
 * A class that represent a Borealis tableref port.
 */
public class TablerefPort extends BorealisPort {
    
    /**
     * Constructor.
     */
    public TablerefPort(GraphCell parentCell, Rectangle2D bounds) {
        super( parentCell, bounds, Color.black);
    }
    
    /**
     * Default constructor.
     */
    public TablerefPort(GraphCell parentCell) {
        super(parentCell, new Rectangle(0, 0, 10, 10), Color.black);
        
    }
    

    
}


