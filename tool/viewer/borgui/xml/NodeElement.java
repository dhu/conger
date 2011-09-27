/*
 * NodeElement.java
 *
 * Created on February 6, 2006, 5:27 PM
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
public class NodeElement extends BorealisElement
{
    public static final String NAME = XmlConstants.NODE_ELEMENT;


    /** Creates a new instance of NodeElement */
    public NodeElement(String endpoint, String query)
    {
        super(NAME);

        this.setEndpoint(endpoint);
        this.setQuery(query);
    }


    public NodeElement(Element element)
    {
        super(NAME, element);
    }


    public String getEndpoint()
    {
        return this.getAttributeValue(XmlConstants.NODE_ENDPOINT_ATTRIBUTE);
    }


    public void setEndpoint(String endpoint)
    {
        this.setAttribute(XmlConstants.NODE_ENDPOINT_ATTRIBUTE, endpoint);
    }


    public String getQuery()
    {
        return this.getAttributeValue(XmlConstants.NODE_QUERY_ATTRIBUTE);
    }


    public void setQuery(String query)
    {
        this.setAttribute(XmlConstants.NODE_QUERY_ATTRIBUTE, query);
    }
}


////////////////////////////  end  NodeElement.java  ///////////////////////////
