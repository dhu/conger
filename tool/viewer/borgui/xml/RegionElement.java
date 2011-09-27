/*
 * Region.java
 *
 * Created on February 6, 2006, 5:32 PM
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
public class RegionElement extends BorealisElement
{
    public static final String NAME = XmlConstants.REGION_ELEMENT;

    /** Creates a new instance of Region */
    public RegionElement(String endpoint, String node)
    {
        super(NAME);

        this.setEndpoint(endpoint);
        this.setNode(node);
    }


    public RegionElement(Element element)
    {
        super(NAME, element);
    }


    public String getEndpoint()
    {
        return this.getAttributeValue(XmlConstants.REGION_ENDPOINT_ATTRIBUTE);
    }


    public void setEndpoint(String endpoint)
    {
        this.setAttribute(XmlConstants.REGION_ENDPOINT_ATTRIBUTE, endpoint);
    }


    public String getNode()
    {
        return this.getAttributeValue(XmlConstants.NODE_QUERY_ATTRIBUTE);
    }


    public void setNode(String node)
    {
        this.setAttribute(XmlConstants.REGION_NODE_ATTRIBUTE, node);
    }
}


//////////////////////  end  RegionElement.java  ///////////////////////////////
