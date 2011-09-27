/*
 * DeployElement.java
 *
 * Created on February 6, 2006, 4:45 PM
 *
 * To change this template, choose Tools | Options and locate the template under
 * the Source Creation and Management node. Right-click the template and choose
 * Open. You can then make changes to the template in the Source Editor.
 */

package borgui.xml;

import  org.jdom.Element;


/**
 *
 * @author vivian
 */
public class DeployElement extends BorealisElement
{
    public final static String NAME = XmlConstants.DEPLOY_ELEMENT;


    /** Creates a new instance of DeployElement */
    public DeployElement()
    {
        super(NAME);
    }


    public DeployElement(Element element)
    {
        super(NAME, element);
    }


    public void addNode(NodeElement nodeElement)
    {
        this.addContent(nodeElement);
    }
}


/////////////////////////  end  DeployElement.java  ////////////////////////////
