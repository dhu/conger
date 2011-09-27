/*
 * ReviewTreeCellRender.java
 *
 * Created on August 4, 2005, 5:36 PM
 */

package borgui.generator;

import javax.swing.*;
import java.awt.*;
import javax.swing.tree.*;
import org.jdom.*;


/**
 * A class used to render the tree 
 *
 * @author Wenjuan Xing (vivian@cs.brown.edu)
 * version 1.0 10/10/2005
 */
public class ReviewTreeCellRenderer extends DefaultTreeCellRenderer{
    
    /** 
     * Creates a new instance of ReviewTreeCellRender 
     */
    public ReviewTreeCellRenderer() {
        super();
    }
    
    /**
     * Return a rendered tree panel.
     */
    public Component getTreeCellRendererComponent(JTree tree, Object value,
            boolean sel, boolean expanded,
            boolean leaf, int row, boolean hasFocus) {
        String str = new String("<html>");
        
        
        if(value instanceof Element) {
            Element elem = (Element) value;
            String name = elem.getName();
            str = str + "<font color=blue>" + name + ": " +"&nbsp;&nbsp;";
            
            if (name.equals(GenConstants.DIST_ELEM_NAME)) {
                Attribute typeAttr = elem.getAttribute(GenConstants.DIST_TYPE_ATTR);
                str +="<font color=red>" + typeAttr.getName() + "</font>"
                        + "<font color=black>=</font>"
                        + "<font color=green>" + typeAttr.getValue() + "</font>";
                
            } else if (name.equals(GenConstants.PARAM_DIST_ELEM_NAME)) {
                Attribute nameAttr = elem.getAttribute(GenConstants.DIST_NAME_ATTR);
                str +="<font color=red>" + nameAttr.getName() + "</font>"
                        + "<font color=black>=</font>"
                        + "<font color=green>" + nameAttr.getValue() + "</font>";
                
                Attribute typeAttr = elem.getAttribute(GenConstants.DIST_TYPE_ATTR);
                str +="&nbsp;&nbsp;&nbsp;"+"<font color=red>" + typeAttr.getName() + "</font>"
                        + "<font color=black>=</font>"
                        + "<font color=green>" + typeAttr.getValue() + "</font>";
                
            } else if (name.equals(GenConstants.PARAM_ELEM_NAME)) {
                Attribute nameAttr = elem.getAttribute(GenConstants.DIST_NAME_ATTR);
                str +="<font color=red>" + nameAttr.getName() + "</font>"
                        + "<font color=black>=</font>"
                        + "<font color=green>" + nameAttr.getValue() + "</font>";
                
                Attribute valueAttr = elem.getAttribute(GenConstants.PARAM_VALUE_ATTR);
                str +="&nbsp;&nbsp;&nbsp;"+"<font color=red>" + valueAttr.getName() + "</font>"
                        + "<font color=black>=</font>"
                        + "<font color=green>" + valueAttr.getValue() + "</font>";
                
            } 
            
            str += "</font>";
            
        }
        
        str = str + "</html>";
        
        JLabel label = new JLabel(str);
        JPanel panel = new JPanel();
        
        panel.setLayout(new BoxLayout(panel, BoxLayout.X_AXIS));
        panel.add(label);
        // HACK!!!! CHANGE
        panel.setPreferredSize(new Dimension(500, 15));
        
        if(sel)
        {
	    setForeground(getTextSelectionColor());
        }
	else
        {
	    setForeground(getTextNonSelectionColor());
        }
        
        return panel;
    }
    
}
