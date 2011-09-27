/*
 * DeployTreeCellRenderer.java
 *
 * Created on February 13, 2006, 4:03 PM
 *
 * To change this template, choose Tools | Options and locate the template under
 * the Source Creation and Management node. Right-click the template and choose
 * Open. You can then make changes to the template in the Source Editor.
 */

package borgui.editor;

import javax.swing.*;
import java.awt.*;
import javax.swing.tree.*;
import org.jdom.Element;
import org.jdom.Attribute;
import borgui.xml.*;


/**
 *
 * @author vivian
 */
public class DeployTreeCellRenderer extends DefaultTreeCellRenderer
{
    /** Creates a new instance of DeployTreeCellRenderer */
    public DeployTreeCellRenderer()
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
            Element element = (Element) value;
            String name = element.getName();
            str = str + "<font color=blue>" + name + ": ";
            
            if (name.equals(NodeElement.NAME))
            {
                str += ((NodeElement) element).getEndpoint();
            }
            else if (name.equals(ReplicSetElement.NAME))
            {
                str += ((ReplicSetElement)element).getNameValue();
            }
            else if (name.equals(RegionElement.NAME))
            {
                str += ((RegionElement) element).getEndpoint();
            }
            else if (name.equals(PublishElement.NAME))
            {
                str += ((PublishElement) element).getStream();
            }
            else if (name.equals(SubscribeElement.NAME))
            {
                str += ((SubscribeElement) element).getStream();
            }
            else if (name.equals(UnSubscribeElement.NAME))
            {
                str += ((UnSubscribeElement) element).getStream();
            }
            else if (name.equals(MoveElement.NAME))
            {
                str += ((MoveElement) element).getBox();
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


//////////////////////  end  DeployTreeCellRenderer.java  //////////////////////
