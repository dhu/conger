package borgui.xml;

import org.jdom.Element;
import java.util.List;
import borgui.common.*;

/**
 * A class that represent a Input Borealis Element
 */

public class InputElement  extends BorealisElement
                        implements StreamAttributeElement,
                                   HasSchemaElement,
                                   HasNodeElement
{
    public final static String  NAME = XmlConstants.INPUT_ELEMENT;


    public InputElement(String stream, String schema, String node)
    {
        super(NAME);

        this.setStream(stream);
        this.setSchema(schema);
        this.setNodeAttribute(node);
    }


    public InputElement(Element element)
    {
        super(NAME, element);
    }


    public String getStream()
    {
        return this.getAttributeValue(XmlConstants.INPUT_STREAM_ATTRIBUTE);
    }


    public void setStream(String value)
    {
        this.setAttribute(XmlConstants.INPUT_STREAM_ATTRIBUTE, value);
    }


    public String getSchema()
    {
        return this.getAttributeValue(XmlConstants.INPUT_SCHEMA_ATTRIBUTE);
    }


    public void setSchema(String value)
    {
        this.setAttribute(XmlConstants.INPUT_SCHEMA_ATTRIBUTE, value);
    }

    
    public String getNodeAttribute()
    {
        return this.getAttributeValue(XmlConstants.INPUT_NODE_ATTRIBUTE);
    }


    public void setNodeAttribute(String value)
    {
        this.setAttribute(XmlConstants.INPUT_NODE_ATTRIBUTE, value);
    }
}


////////////////////////  end  InputElement.java  //////////////////////////////
