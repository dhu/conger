/*
 * PublishElement.java
 *
 * Created on February 6, 2006, 5:41 PM
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
public class PublishElement  extends BorealisElement
{
    public static final String NAME = XmlConstants.PUBLISH_ELEMENT;

    /** Creates a new instance of PublishElement */
    public PublishElement(String stream, String node)
    {
        super(NAME);
        this.setStream(stream);
        this.setNode(node);
    }


    public PublishElement(Element elem)
    {
        super(NAME, elem);
    }


    public String getStream()
    {
        return this.getAttributeValue(XmlConstants.PUBLISH_STREAM_ATTRIBUTE);
    }


    public void setStream(String stream)
    {
        this.setAttribute(XmlConstants.PUBLISH_STREAM_ATTRIBUTE, stream);
    }


    public String getNode()
    {
        return this.getAttributeValue(XmlConstants.PUBLISH_NODE_ATTRIBUTE);
    }


    public void setNode(String node)
    {
        this.setAttribute(XmlConstants.PUBLISH_NODE_ATTRIBUTE, node);
    }
}


/////////////////////////  end  PublishElement.java  ///////////////////////////
