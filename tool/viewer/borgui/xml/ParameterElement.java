package borgui.xml;

import org.jdom.Element;
import java.util.List;
import borgui.common.*;

/**
 * A class that represent a Parameter Borealis Element
 */

public class ParameterElement extends BorealisElement
{
    public static final String NAME = XmlConstants.PARAMETER_ELEMENT;


    public ParameterElement(String name, String value)
    {
        super(NAME);
        this.setNameValue(name);
        this.setValue(value);
    }


    public ParameterElement(Element element)
    {
        super(NAME, element);
    }


    public String getNameValue()
    {
        return this.getAttributeValue(XmlConstants.NAME_ATTRIBUTE);
    }


    public void setNameValue(String value)
    {
        this.setAttribute(XmlConstants.NAME_ATTRIBUTE, value);
    }


    public String getValue()
    {
        return this.getAttributeValue(XmlConstants.PARAMETER_VALUE_ATTRIBUTE);
    }


    public void setValue(String value)
    {
        this.setAttribute(XmlConstants.PARAMETER_VALUE_ATTRIBUTE, value);
    }
}


/////////////////////  end  ParameterElement.java  /////////////////////////////
