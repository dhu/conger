package borgui.xml;

import org.jdom.Element;
import java.util.List;
import borgui.common.*;

/**
 * A class that represent a Schema Borealis Element
 */
public class SchemaElement extends BorealisElement
{
    public final static String NAME = XmlConstants.SCHEMA_ELEMENT;


    public SchemaElement()
    {
        super(NAME);

        this.setNameValue("NewName0");
    }

    public SchemaElement(String name)
    {
        super(NAME);
        this.setNameValue(name);
    }


    public SchemaElement(Element element)
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


    public List getAllFields()
    {
        return this.getChildren(XmlConstants.FIELD_ELEMENT);
    }


    public FieldElement getField(String name)
    {
        return (FieldElement)XmlHandler.findElement(this,
                                                    XmlConstants.FIELD_ELEMENT,
                                                    XmlConstants.NAME_ATTRIBUTE,
                                                    name);
    }


    public boolean addField(FieldElement field)
    {
        this.addContent(field);
        return true;
    }


    public boolean removeField(String name)
    {
        Element element = this.getField(name);

        if (element == null)
        {   return false;
        }

        return this.removeContent(element);
    }
}


/////////////////////////  end  SchemaElement.java  ////////////////////////////
