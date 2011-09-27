package borgui.xml;

import org.jdom.Element;
import java.util.List;
import borgui.common.*;


/**
 * A class that represent a key Borealis Element
 */
public class KeyElement extends BorealisElement
{
    public static final String NAME = XmlConstants.KEY_ELEMENT;


    public KeyElement(String field)
    {
        super(NAME);
        this.setField(field);
    }

    public KeyElement(Element element)
    {
        super(NAME, element);
    }

    public String getField()
    {
        return this.getAttributeValue(XmlConstants.KEY_FIELD_ATTRIBUTE);
    }

    public void setField(String value)
    {
        this.setAttribute(XmlConstants.KEY_FIELD_ATTRIBUTE, value);
    }
}


///////////////////////////  end  KeyElement.java  /////////////////////////////
