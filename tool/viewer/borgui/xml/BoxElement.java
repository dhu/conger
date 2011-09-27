package borgui.xml;

import org.jdom.Element;
import java.util.List;
import borgui.common.*;

/**
 * A class that represent a Box Borealis Element
 */
public class BoxElement  extends BorealisElement
                      implements HasQueryElement, HasNodeElement
{

    public static final String NAME = XmlConstants.BOX_ELEMENT;


    public BoxElement(String name, String type)
    {
        super(NAME);
        this.setNameValue(name);
        this.setType(type);
    }


    public BoxElement(Element element)
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


    public String getType()
    {
        return this.getAttributeValue(XmlConstants.BOX_TYPE_ATTRIBUTE);
    }


    public void setType(String value)
    {
        this.setAttribute(XmlConstants.BOX_TYPE_ATTRIBUTE, value);
    }


    public String getNodeAttribute()
    {
        return this.getAttributeValue(XmlConstants.BOX_NODE_ATTRIBUTE);
    }


    public void setNodeAttribute(String value)
    {
        this.setAttribute(XmlConstants.BOX_NODE_ATTRIBUTE, value);
    }


    public List getAllIns()
    {
        return this.getChildren(XmlConstants.IN_ELEMENT);
    }


    public InElement getIn(String name)
    {
        return (InElement) XmlHandler.findElement(this,
                                     XmlConstants.IN_ELEMENT,
                                     XmlConstants.IN_STREAM_ATTRIBUTE,
                                                  name);
    }


    public boolean addIn(InElement in)
    {
        this.addContent(in);
        return true;
    }


    public void changeIn(String oldValue, String newValue)
    {
        InElement in = getIn(oldValue);
        in.setAttribute(XmlHandler.IN_STREAM_ATTRIBUTE, newValue);
    }


    public boolean removeIn(String name)
    {
        Element element = this.getIn(name);

        if (element == null)
        {   return false;
        }

        return this.removeContent(element);
    }


    public List getAllOuts()
    {
        return this.getChildren(XmlConstants.OUT_ELEMENT);
    }


    public OutElement getOut(String name)
    {
        return (OutElement) XmlHandler.findElement(this,
                                   XmlConstants.OUT_ELEMENT,
                                   XmlConstants.OUT_STREAM_ATTRIBUTE, name);
    }


    public boolean addOut(OutElement out)
    {
        this.addContent(out);
        return true;
    }


    public boolean removeOut(String name)
    {
        Element element = this.getOut(name);

        if (element == null)
        {   return false;
        }

        return this.removeContent(element);
    }


    public void changeOut(String oldValue, String newValue)
    {
        OutElement out = getOut(oldValue);
        out.setAttribute(XmlHandler.OUT_STREAM_ATTRIBUTE, newValue);
    }


    public List getAllParameters()
    {
        return this.getChildren(XmlConstants.PARAMETER_ELEMENT);
    }


    public ParameterElement getParameter(String name)
    {
        return (ParameterElement) XmlHandler.findElement(this,
                                         XmlConstants.PARAMETER_ELEMENT,
                                         XmlConstants.NAME_ATTRIBUTE, name);
    }


    public boolean addParameter(ParameterElement param)
    {
        this.addContent(param);
        return true;
    }


    public boolean removeParameter(String name)
    {
        Element element = this.getParameter(name);

        if (element == null)
        {   return false;
        }

        return this.removeContent(element);
    }


    public TablerefElement getTableref()
    {
        return (TablerefElement)this.getChild(XmlConstants.ACCESS_ELEMENT);
        //return (TablerefElement) XmlHandler.findElement(this,
        //                                XmlConstants.TABLEREF_ELEMENT,
        //                                XmlConstants.TABLEREF_TABLE_ATTRIBUTE,
        //                                name);
    }


    public boolean addTableref(TablerefElement tableref)
    {
        this.addContent(tableref);
        return true;
    }


    public boolean changeTableref(String newValue)
    {
        this.getTableref().setAttribute(XmlConstants.ACCESS_TABLE_ATTRIBUTE, newValue);
        return true;
    }


    public boolean removeTableref()
    {
        Element element = this.getTableref();

        if (element == null)
        {
            return false;
        }

        return this.removeContent(element);
    }


    public QueryElement getQueryElement()
    {
        return (QueryElement)this.getParentElement();
    }


    public String getQueryName()
    {
        return ((QueryElement)this.getParentElement()).getNameValue();
    }
}


//////////////////////////  end  BoxElement.java  //////////////////////////////
