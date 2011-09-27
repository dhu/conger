/*
 * UnSubscribeElement.java
 *
 * Created on February 6, 2006, 5:52 PM
 *
 * To change this template, choose Tools | Options and locate the template under
 * the Source Creation and Management node. Right-click the template and choose
 * Open. You can then make changes to the template in the Source Editor.
 */

package borgui.xml;

import  org.jdom.*;


/**
 *
 * @author vivian
 */
public class UnSubscribeElement  extends BorealisElement
{
    public static final String NAME = XmlConstants.UNSUBSCRIBE_ELEMENT;


    /** Creates a new instance of UnSubscribeElement */
    public UnSubscribeElement(String  stream)
    {
        super(NAME);
        this.setStream(stream);
    }


    public UnSubscribeElement(Element  element)
    {
        super(NAME, element);
    }


    public String getStream()
    {
        return this.getAttributeValue(XmlConstants.UNSUBSCRIBE_STREAM_ATTRIBUTE);
    }


    public void setStream(String  stream)
    {
        this.setAttribute(XmlConstants.UNSUBSCRIBE_STREAM_ATTRIBUTE, stream);
    }
}


/////////////////////////  end  UnSubscribeElement.java  ///////////////////////
