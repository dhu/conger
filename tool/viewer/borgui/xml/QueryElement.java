package borgui.xml;

import java.util.Iterator;
import org.jdom.Element;
import java.util.List;
import java.util.Vector;


/**
 * A class that represent a Query Borealis Element
 */
public class QueryElement extends BorealisElement
{
    public static final String NAME = XmlConstants.QUERY_ELEMENT;


    public QueryElement()
    {
        super(NAME);
        this.setNameValue("NewName0");
    }


    public QueryElement(String name)
    {
        super(NAME);
        this.setNameValue(name);
    }

    public QueryElement(Element element)
    {
        super(NAME, element);
    }


    public String getNameValue()
    {
        return this.getAttributeValue(XmlConstants.NAME_ATTRIBUTE);
    }


    public void setNameValue(String name)
    {
        this.setAttribute(XmlConstants.NAME_ATTRIBUTE, name);
    }


//    public List<QueryElement>  getAllBoxes()
//    {
//        return this.getChildren(XmlConstants.BOX_ELEMENT);
//    }


    public List<BoxElement>  getAllBoxes()
    {
        List<BoxElement>  elements = new Vector<BoxElement>();
        List   boxes = this.getChildren(XmlConstants.BOX_ELEMENT);

        for(Iterator i = boxes.iterator(); i.hasNext();)
        {
            elements.add((BoxElement)i.next());
        }

        return elements;
    }


    public Vector<String> getAllBoxNames()
    {
        Vector<String>  names = new Vector<String>();
        List            boxes = this.getChildren(XmlConstants.BOX_ELEMENT);

        for(Iterator i = boxes.iterator(); i.hasNext();)
        {
            BoxElement child = (BoxElement)i.next();
            names.add(child.getNameValue());
        }

        return names;
    }


    public BoxElement getBox(String name)
    {
        return (BoxElement) XmlHandler.findElement(this,
                                      XmlConstants.BOX_ELEMENT,
                                      XmlConstants.NAME_ATTRIBUTE,
                                                   name);
    }


    public boolean addBox(BoxElement box)
    {
        this.addContent(box);
        return true;
    }


    public boolean removeBox(String name)
    {
        Element elem = this.getBox(name);

        if (elem == null)
        {
            return false;
        }

        return this.removeContent(elem);
    }


    //public List getAllTables()
    //{
    //    return this.getChildren(XmlConstants.TABLE_ELEMENT);
    //}


    public List<TableElement>  getAllTables()
    {
        List<TableElement>  elements = new Vector<TableElement>();
        List   tables = this.getChildren(XmlConstants.TABLE_ELEMENT);

        for(Iterator i = tables.iterator(); i.hasNext();)
        {
            elements.add((TableElement)i.next());
        }

        return elements;
    }


    public Vector<String> getAllTableNames()
    {
        Vector<String>  names = new Vector<String>();
        List tables = this.getChildren(XmlConstants.TABLE_ELEMENT);

        for (Iterator i = tables.iterator(); i.hasNext();)
        {
            TableElement child = (TableElement)i.next();
            names.add(child.getNameValue());
        }

        return names;
    }


    public TableElement getTable(String name)
    {
        return (TableElement)XmlHandler.findElement(this,
                                                    XmlConstants.TABLE_ELEMENT,
                                                    XmlConstants.NAME_ATTRIBUTE,
                                                    name);
    }


    public boolean addTable(TableElement table)
    {
        this.addContent(table);
        return true;
    }


    public boolean removeTable(String name)
    {
        Element element = this.getTable(name);

        if (element == null)
        {
            return false;
        }

        return this.removeContent(element);
    }
}

//////////////////////////  end QueryElement.java  /////////////////////////////
