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
 * A class that represent Borealis Table Graph cell.
 */
public class TableGraphCell extends BorealisGraphCell implements HasSchemaCell, HasQueryCell, HasNodeCell {
    
    
    protected TablePort m_tablePort;
    
    
    
    /**
     * Default Constructor.
     */
    public TableGraphCell() {
        super(new Rectangle(0, 0, 70, 70),
                Color.GRAY, (Icon) new ImageIcon(borgui.editor.EditorConstants.TABLE_ICON));
        m_tablePort = new TablePort(this);
        add(m_tablePort);
    }
    
    public TableGraphCell(String name) {
        super(name, new Rectangle(0, 0, 70, 70),
                Color.GRAY, (Icon) new ImageIcon(borgui.editor.EditorConstants.TABLE_ICON));
        m_tablePort = new TablePort(this);
        add(m_tablePort);
    }
    
    /**
     * Get its table port
     */
    public TablePort getTablePort() {
        return m_tablePort;
    }
    
    /**
     * Get the AttributeMap of its table port.
     */
    public AttributeMap getPortMap() {
        
        return new AttributeMap();
    }
    
    public void changeNodeColor(Color nodeColor) {
        AttributeMap map = getAttributes();
        GraphConstants.setBackground(map, nodeColor);
    }
}


