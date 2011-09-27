package borgui.xml;

import org.jdom.Element;
import java.util.List;
import borgui.common.*;

/**
 * A class that represent a Output Borealis Element
 */
public class OutputElement  extends BorealisElement
                         implements StreamAttributeElement,
                                    HasSchemaElement
{
    // This is the location of the output element.
    // Note that it needs to be explicitly set as XML output elements
    // do not have node attributes.
    //
    String     m_node;

    public final static String NAME = XmlConstants.OUTPUT_ELEMENT;


    public OutputElement(String stream, String schema)
    {
        super(NAME);
        this.setStream(stream);
        this.setSchema(schema);
    }


    public OutputElement(String stream)
    {
        super(NAME);
        this.setStream(stream);
    }


    public OutputElement(Element element)
    {
        super(NAME, element);
    }


    public String getNodeAttribute()
    {
        return m_node;
    }


    public void setNodeAttribute(String value)
    {
        m_node = value;
    }


    public String getStream()
    {
        return this.getAttributeValue(XmlConstants.OUTPUT_STREAM_ATTRIBUTE);
    }


    public void setStream(String value)
    {
        this.setAttribute(XmlConstants.OUTPUT_STREAM_ATTRIBUTE, value);
    }


    public String getSchema()
    {
        return this.getAttributeValue(XmlConstants.OUTPUT_SCHEMA_ATTRIBUTE);
    }


    public boolean hasSchema()
    {
        if(this.getAttribute(XmlConstants.OUTPUT_SCHEMA_ATTRIBUTE) != null)
        {    return true;
        }

        return false;
    }


    public void setSchema(String value)
    {
        this.setAttribute(XmlConstants.OUTPUT_SCHEMA_ATTRIBUTE, value);
    }
}


//////////////////////////  end  OutputElement.java  ///////////////////////////
