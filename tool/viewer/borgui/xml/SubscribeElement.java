/*
 * SubscribeElement.java
 *
 * Created on February 6, 2006, 5:46 PM
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
public class SubscribeElement  extends BorealisElement
{
    public static final String NAME = XmlConstants.SUBSCRIBE_ELEMENT;


    /** Creates a new instance of SubscribeElement */
    public SubscribeElement(String stream, String endpoint, String gap)
    {
        super(NAME);

        this.setStream(stream);
        this.setEndpoint(endpoint);
        this.setGap(gap);
    }


    public SubscribeElement(Element element)
    {
        super(NAME, element);
    }


    public String getStream()
    {
        return this.getAttributeValue(XmlConstants.SUBSCRIBE_STREAM_ATTRIBUTE);
    }


    public void setStream(String stream)
    {
        this.setAttribute(XmlConstants.SUBSCRIBE_STREAM_ATTRIBUTE, stream);
    }


    public String getEndpoint()
    {
        return this.getAttributeValue(XmlConstants.SUBSCRIBE_ENDPOINT_ATTRIBUTE);
    }


    public void setEndpoint(String endpoint)
    {
        this.setAttribute(XmlConstants.SUBSCRIBE_ENDPOINT_ATTRIBUTE, endpoint);
    }


    public String getGap()
    {
        return this.getAttributeValue(XmlConstants.SUBSCRIBE_GAP_ATTRIBUTE);
    }


    public void setGap(String gap)
    {
        this.setAttribute(XmlConstants.SUBSCRIBE_GAP_ATTRIBUTE, gap);
    }
}


///////////////////////  end  SubscribeElement.java  ///////////////////////////
