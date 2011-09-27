/*
 * MoveElement.java
 *
 * Created on February 6, 2006, 5:54 PM
 *
 * To change this template, choose Tools | Options and locate the template under
 * the Source Creation and Management node. Right-click the template and choose
 * Open. You can then make changes to the template in the Source Editor.
 */

package borgui.xml;
import org.jdom.*;
/**
 *
 * @author vivian
 */
public class MoveElement extends BorealisElement
{
    public static final String NAME = XmlConstants.MOVE_ELEMENT;


    /** Creates a new instance of MoveElement */
    public MoveElement(String box, String node, String table)
    {
        super(NAME);
        this.setBox(box);
        this.setNode(node);
        this.setTable(table);
    }


    public MoveElement(Element element)
    {
        super(NAME, element);
    }


    public String getBox()
    {
        return this.getAttributeValue(XmlConstants.MOVE_BOX_ATTRIBUTE);
    }


    public void setBox(String box)
    {
        this.setAttribute(XmlConstants.MOVE_BOX_ATTRIBUTE, box);
    }


    public String getNode()
    {
        return this.getAttributeValue(XmlConstants.MOVE_NODE_ATTRIBUTE);
    }


    public void setNode(String node)
    {
        this.setAttribute(XmlConstants.MOVE_NODE_ATTRIBUTE, node);
    }


    public String getTable()
    {
        return this.getAttributeValue(XmlConstants.MOVE_TABLE_ATTRIBUTE);
    }


    public void setTable(String table)
    {
        this.setAttribute(XmlConstants.MOVE_TABLE_ATTRIBUTE, table);
    }
}


///////////////////////  end  MoveElement.java  ////////////////////////////////
