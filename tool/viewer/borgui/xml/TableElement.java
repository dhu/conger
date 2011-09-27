package borgui.xml;

import org.jdom.Element;
import java.util.List;


public class TableElement  extends BorealisElement
                        implements HasQueryElement,
                                   HasSchemaElement,
                                   HasNodeElement
{
    public static final String NAME = XmlConstants.TABLE_ELEMENT;


    public TableElement(String name, String schema, String stream)
    {
        super(NAME);

        this.setNameValue(name);
        this.setSchema(schema);
        this.setStream(stream);
    }


    public TableElement(Element element)
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


    public String getSchema()
    {
        return this.getAttributeValue(XmlConstants.TABLE_SCHEMA_ATTRIBUTE);
    }


    public void setSchema(String value)
    {
        this.setAttribute(XmlConstants.TABLE_SCHEMA_ATTRIBUTE, value);
    }


    public String getStream()
    {
        return this.getAttributeValue(XmlConstants.TABLE_STREAM_ATTRIBUTE);
    }


    public void setStream(String value)
    {
        this.setAttribute(XmlConstants.TABLE_STREAM_ATTRIBUTE, value);
    }


    public List getAllKeys()
    {
        return this.getChildren(XmlConstants.KEY_ELEMENT);
    }


    public KeyElement getKey(String attrVal)
    {
        return (KeyElement)XmlHandler.findElement(this,
                                     XmlConstants.KEY_ELEMENT,
                                     XmlConstants.KEY_FIELD_ATTRIBUTE,
                                                  attrVal);
    }


    public boolean addKey(KeyElement key)
    {
        this.addContent(key);
        return true;
    }


    public boolean removeKey(String attribute)
    {
        Element element = this.getKey(attribute);

        if (element == null)
        {   return false;
        }

        return this.removeContent(element);
    }


    public List getAllParameters()
    {
        return this.getChildren(XmlConstants.PARAMETER_ELEMENT);
    }


    public ParameterElement getParameter(String name)
    {
        return (ParameterElement)XmlHandler.findElement(this,
                                           XmlConstants.PARAMETER_ELEMENT,
                                           XmlConstants.NAME_ATTRIBUTE, name);
    }


    public boolean addParameter(ParameterElement parameter)
    {
        this.addContent(parameter);
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


    public QueryElement getQueryElement()
    {
        return (QueryElement)this.getParentElement();
    }


    public String getQueryName()
    {
        return ((QueryElement)this.getParentElement()).getNameValue();
    }


    public String getNodeAttribute()
    {
        return this.getAttributeValue(XmlConstants.TABLE_NODE_ATTRIBUTE);
    }


    public void setNodeAttribute(String value)
    {
        this.setAttribute(XmlConstants.TABLE_NODE_ATTRIBUTE, value);
    }
}


//////////////////////////  end  TableElement.java  ////////////////////////////
