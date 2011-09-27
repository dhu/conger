/*
 * StatisticsHandler.java
 *
 * Created on May 23, 2005, 3:20 PM
 */

package borgui.visual;

import medusaXmlRpc.*;

import java.util.Vector;
import java.util.Iterator;

import java.text.DecimalFormat;

import borgui.common.DebugTrace;


//  utility/client/java/medusaXmlRpc/lib/medusaXmlRpc.jar  created
//  utility/client/java/medusaXmlRpc/lib/xmlrpc.jar  downloaded from apache.

/**
 * 
 *
 * @author Wenjuan Xing (vivian@cs.brown.edu)
 * version 1.0 10/13/2005
 *
 */
public class XmlRpcStatHandler
{
    protected ServerNode  m_serverNode;
    protected String      m_methodName = "";
    protected Vector      m_parameter  = new Vector();
    
    
    ////////////////////////////////////////////////////////////////////////////
    //
    // Creates a new instance of StatisticsHandler
    //
    public XmlRpcStatHandler(String   ipAddress,
                             String   portNumber,
                             String   methodName,
                             Vector   parameter)
    {
    //..........................................................................


        m_serverNode = new ServerNode(ipAddress, portNumber);
        m_methodName = methodName;
        m_parameter  = parameter;
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    // Gather statistics from a single node.
    //
    public Vector<Object>  collectNodeStatistics()
    {
        Double    cpu = 0.0;

        Vector<Object>  pair = new Vector<Object>();
    //..........................................................................


        try
        {   // DebugTrace.trace("invoke " + m_methodName + " ...");

            cpu = (Double)MedusaXmlRpc.invoke(m_serverNode.serverRPC(),
                                              m_methodName,
                                              m_parameter);

            pair.add(m_serverNode.serverAddr());
            pair.add(cpu);

            DecimalFormat form = new DecimalFormat("000.000#");

            DebugTrace.trace("node = " + m_serverNode.serverAddr()
                          + "  cpu = " + form.format(cpu));
        }
        catch (Exception e)
        {   DebugTrace.trace("ERROR: invoke " + m_methodName + ":  "
                                              + e.getMessage());
        }

        return  pair;
    }


/***********************
    // Gather statistics from a single node.
    //
    public Vector  pollTestStatistics()
    {
        Double  cpu  = 0.0;
        Vector<Vector<Object>>  node = new Vector<Vector<Object>>();

        try
        {   DebugTrace.trace("invoke " + m_methodName + " ...");

            cpu = (Double)MedusaXmlRpc.invoke(m_serverNode.serverRPC(),
                                              m_methodName,
                                              m_parameter);

            Vector<Object>  pair = new Vector<Object>();
            pair.add(m_serverNode.serverAddr());
            pair.add(cpu);

            node.add(pair);

            DecimalFormat df = new DecimalFormat("000.000#");

            DebugTrace.trace("node = " + m_serverNode.serverAddr()
                          + "  cpu = " + df.format(cpu));
        }
        catch (Exception e)
        {   DebugTrace.trace("ERROR: invoke " + m_methodName + ":  "
                                              + e.getMessage());
        }

        return  node;
    }


    public Vector  getStats()
    {
        Vector  result = new Vector();

        try
        {   DebugTrace.trace("invoke '" + m_methodName + "' ...");

            Vector Stats = (Vector)MedusaXmlRpc.invoke(m_serverNode.serverRPC(),
                                                       m_methodName,
                                                       m_parameter);

            if (Stats.size() != 0)
            {
                DebugTrace.trace("invoke XML RPC worked.");
            }
            else
            {   DebugTrace.trace("invoke works, but Stats.size = 0");
            }

            result = Stats;
        }
        catch(Exception e)
        {
            //e.printStackTrace();
            DebugTrace.trace("ERROR: invoke '" + m_methodName + "'");
            DebugTrace.trace(e.getMessage());
        }

        return result;
    }
*************************/
}


/////////////////////// end XmlRpcStatHandler.java  ////////////////////////////
