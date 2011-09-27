package borgui.editor;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.tree.*;
import org.jdom.Element;
import org.jdom.Attribute;
import borgui.xml.*;

/**
 * Displays an entry in a tree.
 */
public class NetworkTreeCellRenderer extends DefaultTreeCellRenderer
{
    
    /**
     * Constructor.
     */
    public NetworkTreeCellRenderer()
    {
        super();
    }

    
    /**
     * Configures the renderer based on the passed in components.
     */
    public Component getTreeCellRendererComponent(JTree     tree,
                                                  Object    value,
                                                  boolean   sel,
                                                  boolean   expanded,
                                                  boolean   leaf,
                                                  int       row,
                                                  boolean   hasFocus)
    {
        String str = new String("");
        
        if (value instanceof String)
        {
            str += (String) value;
        }
        else if (value instanceof Element)
        {
            Element element = (Element) value;
            String name = element.getName();
            
            str += name + ": ";
            
            if (name.equals(NetworkElement.NAME))
            {
                //str += ((NetworkElement) element).getOwner();
            }

            if (name.equals(BoxElement.NAME))
            {
                str += ((BoxElement) element).getNameValue();
            }
            else if (name.equals(QueryElement.NAME))
            {
                str += ((QueryElement) element).getNameValue();
            }
            else if (name.equals(SchemaElement.NAME))
            {
                str += ((SchemaElement) element).getNameValue();
            }
            else if (name.equals(InputElement.NAME))
            {
                str += ((InputElement) element).getStream();
            }
            else if (name.equals(OutputElement.NAME))
            {
                str += ((OutputElement) element).getStream();
            }
            else if(name.equals(TableElement.NAME))
            {
                str += ((TableElement) element).getNameValue();
            }
        }
        else
        {   System.out.println("Invalid cell");
        }
        
        
        JLabel label = new JLabel(str);
        JPanel panel = new JPanel();
        panel.setLayout(new BoxLayout(panel, BoxLayout.X_AXIS));
        panel.add(label);
        panel.setPreferredSize(new Dimension(200, 15));
        
        if (sel)
        {
            panel.setBackground(Color.lightGray);
        }
        else
        {   panel.setBackground(tree.getBackground());
        }
        
        return panel;
    }
}


////////////////////////  end  NetworkTreeCellRenderer.java  ///////////////////
