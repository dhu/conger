/*
 * ReplicSetElement.java
 *
 * Created on February 6, 2006, 5:12 PM
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
public class ReplicSetElement extends BorealisElement
{
    public static final String NAME = XmlConstants.REPLICA_SET_ELEMENT;


    /** Creates a new instance of ReplicSetElement */
    public ReplicSetElement(String name, String query)
    {
        super(NAME);

        this.setNameValue(name);
        this.setQuery(query);
    }


    public ReplicSetElement(Element element)
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


    public String getQuery()
    {
        return this.getAttributeValue(XmlConstants.REPLICA_QUERY_ATTRIBUTE);
    }


    public void setQuery(String query)
    {
        this.setAttribute(XmlConstants.REPLICA_QUERY_ATTRIBUTE, query);
    }
}


////////////////////////  end  ReplicSetElement.java  //////////////////////////
