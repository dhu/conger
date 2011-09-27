/*
 * DeployDocument.java
 *
 * Created on February 7, 2006, 10:49 AM
 *
 * To change this template, choose Tools | Options and locate the template under
 * the Source Creation and Management node. Right-click the template and choose
 * Open. You can then make changes to the template in the Source Editor.
 */

package borgui.xml;

import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Vector;

import org.jdom.*;
import org.jdom.filter.ElementFilter;

import borgui.common.*;


/**
 *
 * @author vivian
 */
public class DeployDocument extends Document
{

    /** Creates a new instance of DeployDocument */
    public DeployDocument()
    {
        super();
        DeployElement root = new DeployElement();
        this.setDeploy(root);
    }


    public DeployDocument(Document doc)
    {
        super();

        XmlHandler.jdomDocToDeployDoc(doc, this);
    }


    public void setDocument(DeployDocument doc)
    {
        DeployElement root = doc.getDeploy();
        doc.removeContent(root);
        setDeploy(root);
    }


    public DeployElement getDeploy()
    {
        return (DeployElement) this.getRootElement();
    }


    public boolean setDeploy(DeployElement deploy)
    {
        this.setRootElement(deploy);
        return true;
    }


    public HashSet<String>  getDistinctIpPorts()
    {
        Vector<String>   oldIpPorts = getIpPorts();
        HashSet<String>  set = new HashSet<String>();

        for (Iterator i = oldIpPorts.iterator(); i.hasNext();)
        {
            String ipPort = (String)i.next();
            set.add(ipPort);
        }

        return set;
    }


    /**
     * Return a vector of the node ip info from a XML document.
     */
    public Vector<String>  getIpPorts()
    {
        DeployElement deployElement = (DeployElement)getRootElement().clone();

        List elementList = deployElement.removeContent(
                               new ElementFilter(XmlConstants.NODE_ELEMENT));

        if (elementList.size() == 0)
        {   return new Vector<String>();
        }

        Vector<String>  nodesIpPort = getIpPortsFromElementList(elementList);
        return nodesIpPort;
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    //  Get the ip and port from the element.
    //
    protected static Vector<String> getIpPortsFromElementList(List elements)
    {
        Vector<String> result = new Vector<String>();
        String         ipPort;
    //..........................................................................


        for (Iterator i = elements.iterator(); i.hasNext(); )
        {
            Element element = (Element) i.next();

            try
            {
                String nodeAttrPrimary = element.getAttributeValue(
                           XmlConstants.NODE_ENDPOINT_ATTRIBUTE).trim();

                ipPort = HostEndpoint.formEndpoint(nodeAttrPrimary,
                                      XmlConstants.DEFAULT_BOREALIS_PORT);

                //DebugTrace.trace("add:  " + ipPort);

                result.add(ipPort);
            }
            catch(Exception e)
            {
                DebugTrace.trace("Error in getting ip and port from deploy xml: " + e.getMessage());
            }
        }

        return result;
    }


    ////////////////////////////////////////////////////////////////////////////
    //
    public void addNodes(Vector<String>  ipPorts)
    {
        HashSet<String>  existNodesSet = getDistinctIpPorts();

        for (Iterator i = ipPorts.iterator(); i.hasNext();)
        {
            String  ipPort = (String)i.next();

            if (!existNodesSet.contains(ipPort))
            {
                NodeElement nodeElement = new NodeElement(ipPort, "");
                getRootElement().addContent(nodeElement);
            }
        }
    }
}

////////////////////////////  end DeployDocument.java  /////////////////////////
