package borgui.xml;

import org.jdom.Element;
import java.util.List;
import borgui.common.*;

/**
 * A class that represent a In Borealis Element.
 */
public class InElement  extends BorealisElement
                     implements StreamAttributeElement
{
    public final static String NAME = XmlConstants.IN_ELEMENT;


    public InElement(String stream)
    {
        super(NAME);
        this.setStream(stream);
    }

    public InElement(Element element)
    {
        super(NAME, element);
    }

    public String getStream()
    {
        return this.getAttributeValue(XmlConstants.IN_STREAM_ATTRIBUTE);
    }


    public void setStream(String value)
    {
        this.setAttribute(XmlConstants.IN_STREAM_ATTRIBUTE, value);
    }
}


///////////////////////  end  InElement.java  //////////////////////////////////
