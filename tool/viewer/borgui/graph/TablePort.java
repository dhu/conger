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
 * A class that represent a Borealis table port.
 */
public class TablePort extends BorealisPort {
    /**
     * Constructor.
     */
    public TablePort(GraphCell parentCell, Rectangle2D bounds) {
        super(parentCell, bounds, Color.black);
    }
    
    /**
     * Default constructor.
     */
    public TablePort(GraphCell parentCell) {
        super( parentCell, new Rectangle(0, 0, 10, 10), Color.black);
         
    }
    
}


