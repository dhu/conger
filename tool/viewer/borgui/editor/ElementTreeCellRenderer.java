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
public class ElementTreeCellRenderer extends DefaultTreeCellRenderer
{
    /**
     * Constructor.
     */
    public ElementTreeCellRenderer()
    {
        super();
    }

    
    /**
     * Configures the renderer based on the passed in components.
     */
    public Component getTreeCellRendererComponent(JTree    tree,
                                                  Object   value,
                                                  boolean  sel,
                                                  boolean  expanded,
                                                  boolean  leaf,
                                                  int      row,
                                                  boolean  hasFocus)
    {
        String str = new String("<html>");
        
        if (value instanceof Attribute)
        {
            Attribute attr = (Attribute) value;

            str = str + "<font color=red>" + attr.getName() + "</font>"
                      + "<font color=black>=</font>"
                      + "<font color=green>" + attr.getValue() + "</font>";
            
        }
        else if (value instanceof Element)
        {
            Element elem = (Element) value;
            String name = elem.getName();
            str = str + "<font color=blue>" + name + ": ";
            
            if (name.equals(BoxElement.NAME))
            {
                str += ((BoxElement) elem).getNameValue();
            }
            else if (name.equals(QueryElement.NAME))
            {
                str += ((QueryElement) elem).getNameValue();
            }
            else if (name.equals(SchemaElement.NAME))
            {
                str += ((SchemaElement) elem).getNameValue();
            }
            else if (name.equals(InputElement.NAME))
            {
                str += ((InputElement) elem).getStream();
            }
            else if (name.equals(OutputElement.NAME))
            {
                str += ((OutputElement) elem).getStream();
            }
            else if (name.equals(InElement.NAME))
            {
                str += ((InElement) elem).getStream();
            }
            else if (name.equals(OutElement.NAME))
            {
                str += ((OutElement) elem).getStream();
            }
            else if (name.equals(ParameterElement.NAME))
            {
                str += ((ParameterElement) elem).getNameValue();
            }
            else if (name.equals(FieldElement.NAME))
            {
                str += ((FieldElement) elem).getNameValue();
            }
            else if (name.equals(TablerefElement.NAME))
            {
                str += ((TablerefElement) elem).getTableAttribute();
            }
            else if(name.equals(TableElement.NAME))
            {
                str += ((TableElement) elem).getNameValue();
            }

            str += "</font>";
        }
        
        str = str + "</html>";
        
        JLabel label = new JLabel(str);
        JPanel panel = new JPanel();
        panel.setLayout(new BoxLayout(panel, BoxLayout.X_AXIS));
        panel.add(label);

        // HACK!!!! CHANGE
        panel.setPreferredSize(new Dimension(200, 15));
        
        // if selected show that it is
        if (sel)
        {
            panel.setBackground(Color.lightGray);
        }
        else
        {
            panel.setBackground(tree.getBackground());
        }
        
        return panel;
    }
}


///////////////////////  end  ElementTreeCellRenderer.java  ////////////////////
