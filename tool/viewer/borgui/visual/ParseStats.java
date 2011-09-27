/*
 * ParseStats.java
 *
 * Created on June 8, 2005, 8:50 PM
 */

package borgui.visual;

import java.util.*;

import borgui.common.DebugTrace;



/**
 *
 *
 * @author Wenjuan Xing (vivian@cs.brown.edu)
 * version 1.0 10/13/2005
 *
 */
public class ParseStats
{
    // Each element contains statistics for a node.
    // The element is a pair:  {String <Ip:Port>, <Statistic>}
    //
    // For cpu utilization Statistic is a Double.
    //
    // For stream latency and tuple rates Statistic is a vector with
    // an element for each stream.  Each stream element is a pair:
    //     {String <Stream Name>, Double <Latency | Rate>}
    //
    protected Vector m_stats = new Vector();


    // Creates a new instance of ParseStats
    //
    public ParseStats(Vector  stats)
    {
        m_stats = stats;
    }

    public String toString()
    {
        return m_stats.toString();
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    //  CPU Utilization
    //..........................................................................


    // Get the number of nodes from which cpu utilization is collected.
    //
    public int getCpuNodeSize()
    {
        int result = 0;
        result = m_stats.size();
        //DebugTrace.trace("from 'getCpuNodeSize()':-- currentCpuNodeSize is " + result);

        return result;
    }


    // Get the Node Ip and Port by index for cpu statistics.
    //
    public String getCpuNodeIpPort(int NodeIndex)
    {
        String result;
        Vector Vectorx = (Vector)m_stats.get(NodeIndex);

        result = (String)Vectorx.get(0);
        DebugTrace.trace("NodeIpPort = " + result);

        return result;
    }


    // Get the cpu utilization by ip and port
    //
    public Double getCpuStats(String inputIpPort)
    {
        Double result = new Double(0);

        for (Iterator i = m_stats.iterator();i.hasNext();)
        {
            Vector current = (Vector)i.next();
            String getIpPort = (String)current.get(0);

            if (getIpPort.trim().equals(inputIpPort.trim()))
            {
                result = (Double)current.get(1);
                //DebugTrace.trace("Cpu from " + inputIpPort +" is "+ result);

                break;
            }
        }

        return  result;
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    //  Stream latencies
    //..........................................................................


    // Get the number of nodes for which stream latencies are collected.
    //
    public int getLatencyNodeSize()
    {
        int result = 0;
        result = m_stats.size();
        //DebugTrace.trace("from 'getLatencyNodeSize()':-- LatencyNodeSize is " + result);

        return result;
    }


    // Get the names of all streams on all nodes.
    //
    public Vector<String>  getLatencyAllStreamName()
    {
        Vector<String> result = new Vector<String>();

        // Iterate over each node.
        //
        for (Iterator i = m_stats.iterator(); i.hasNext();)
        {
            Vector Vectorx = (Vector) i.next();
            Vector Vectorx1 = (Vector) Vectorx.get(1);

            // Iterate over each stream on a node.
            //
            for (Iterator j = Vectorx1.iterator(); j.hasNext();)
            {
                Vector Vectorx1x = (Vector) j.next();
                String name = (String) Vectorx1x.get(0);

                result.add(name);
            }
        }

        //DebugTrace.trace("new from 'getLatencyAllStreamName()':-- LatencyStream Size is " + result.size());
        return result;
    }


    // Get the latency on a single stream.
    //
    public Double getLatencyValueByName(String streamName)
    {
        Double result = new Double(0.0);

        for (Iterator i = m_stats.iterator(); i.hasNext();)
        {
            Vector Vectorx = (Vector) i.next();
            Vector Vectorx1 = (Vector) Vectorx.get(1);

            for (Iterator j = Vectorx1.iterator(); j.hasNext();)
            {
                Vector Vectorx1x = (Vector) j.next();
                String name = (String) Vectorx1x.get(0);

                if (name.equals(streamName))
                {
                    result = (Double) Vectorx1x.get(1);
                    //DebugTrace.trace("from 'getLatencyValueByName()':-- Latency Value for Stream " + streamName +" is " + result);
                }
            }
        }

        return result;
    }


    // Get the latencies for all streams on a node.
    //
    public Vector<Double>  getAllLatencyValueInNodeX(String ipPort)
    {
        Vector<Double>  result = new Vector<Double>();

        Vector Vectorx = getLatencyVector(ipPort);

        if (Vectorx.size()!=0)
        {
            Vector Vectorx1 = (Vector) Vectorx.get(1);

            for (Iterator i = Vectorx1.iterator(); i.hasNext();)
            {
                Vector Vectorx1x = (Vector) i.next();
                Double value = (Double) Vectorx1x.get(1);
                result.add(value);
            }
        }

        return result;
    }


    // Get the streams and latencies for all streams on a node.
    // A 2 element vector is returned with:  {String <Ip:Port>, <Statistic>}
    //
    // Statistic is a vector with an element for each stream.
    // Each stream element is a pair {String <Stream Name>, Double <Latency>}
    // 
    protected Vector getLatencyVector(String inputIpPort)
    {
        Vector result = new Vector();

        for (Iterator i = m_stats.iterator();i.hasNext();)
        {
            Vector current = (Vector)i.next();
            String getIpPort = (String)current.get(0);

            if (getIpPort.trim().equals(inputIpPort.trim()))
            {
                result = current;
                //DebugTrace.trace("new from 'getLatencyVector()': --inputIpPort " + inputIpPort);
                return result;
            }
        }

        return result;
    }


    // Get the number of streams on a node for which latencies are collected.
    //
    public int getLatencyStreamSize(String IpPort)
    {
        int result = 0;
        Vector Vectorx = getLatencyVector(IpPort);

        if (Vectorx.size()!=0)
        {
            Vector Vectorx1 = (Vector) Vectorx.get(1);
            result = Vectorx1.size();
            //DebugTrace.trace("new from 'getLatencyStreamSize()':--LatencyStreamSize from IpPort " + IpPort +" is " + result);
        }

        return result;
    }


    // Get the name of a stream on a node and stream index.
    //
    public String getLatencyStreamName(String IpPort, int Index)
    {
        String result = new String("");
        Vector Vectorx = getLatencyVector(IpPort);

        if (Vectorx.size() != 0)
        {
            Vector Vectorx1 = (Vector) Vectorx.get(1);
            Vector Vectorx1x = (Vector) Vectorx1.get(Index);
            result = (String) Vectorx1x.get(0);
            //DebugTrace.trace("new from 'getLatencyStreamName()': -- LatencyStreamName of Index " + Index + " is " + result);
        }

        return result;
    }


    // Get all stream latencies on a node.
    // 
    public Vector<Double>  getAllTupleValueInNodeX(String ipPort)
    {
        Vector<Double>  result = new Vector<Double>();

        Vector Vectorx = getTupleVector(ipPort);

        if (Vectorx.size() != 0)
        {
            Vector Vectorx1 = (Vector)Vectorx.get(1);

            for (Iterator i = Vectorx1.iterator(); i.hasNext();)
            {
                Vector Vectorx1x = (Vector) i.next();
                Double value = (Double) Vectorx1x.get(1);
                result.add(value);
            }
        }

        return result;
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    //  Tuple rates
    //..........................................................................


    // Get the number of nodes for which tuple rates are collected.
    //
    public int getTupleNodeSize()
    {
        int result = 0;
        result = m_stats.size();
        //DebugTrace.trace("from 'getTupleNodeSize()':-- TupleNodeSize is " + result);

        return result;
    }


    // Get the streams and tuple rates for all streams on a node.
    // A 2 element vector is returned with:  {String <Ip:Port>, <Statistic>}
    //
    // Statistic is a vector with an element for each stream.
    // Each stream element is a pair {String <Stream Name>, Double <Rate>}
    // 
    public Vector getTupleVector(String inputIpPort)
    {
        Vector result = new Vector();


        // Do over each node.
        //
        for (Iterator i = m_stats.iterator();i.hasNext();)
        {
            Vector current = (Vector)i.next();
            String getIpPort = (String)current.get(0);

            if (getIpPort.trim().equals(inputIpPort.trim()))
            {
                result = current;
                //DebugTrace.trace("from 'getTupleVector()': --inputIpPort " + inputIpPort);
                return result;
            }
        }

        return result;
    }


    // get Tuple StreamSize by IpPort
    //
    public int getTupleStreamSize(String IpPort)
    {
        int result = 0;
        Vector Vectorx = getTupleVector(IpPort);

        if (Vectorx.size() != 0)
        {
            Vector Vectorx1 = (Vector) Vectorx.get(1);
            result = Vectorx1.size();
            //DebugTrace.trace("from 'getTupleStreamSize()':--TupleStreamSize from IpPort " + IpPort +" is " + result);
        }

        return result;
    }


    // Get the name of a stream on a node and stream index.
    //
    public String getTupleStreamName(String IpPort, int Index)
    {
        String result = "";

        Vector Vectorx = getTupleVector(IpPort);

        if (Vectorx.size()!=0)
        {
            Vector Vectorx1 = (Vector) Vectorx.get(1);
            Vector Vectorx1x = (Vector) Vectorx1.get(Index);
            result = (String) Vectorx1x.get(0);
            //DebugTrace.trace("from 'getTupleStreamName()': -- TupleName of Index " + Index + " is " + result);
        }

        return result;
    }


    // Get the names of all streams on all nodes.
    //
    public Vector<String>  getTupleAllStreamName()
    {
        Vector<String>  result = new Vector<String>();

        // Do over each node.
        //
        for (Iterator i = m_stats.iterator(); i.hasNext();)
        {
            Vector Vectorx = (Vector) i.next();
            Vector Vectorx1 = (Vector) Vectorx.get(1);

            // Do over each stream on each node.
            //
            for(Iterator j = Vectorx1.iterator(); j.hasNext();)
            {
                Vector Vectorx1x = (Vector) j.next();
                String name = (String) Vectorx1x.get(0);
                result.add(name);
            }
        }

        //DebugTrace.trace("from 'getTupleAllStreamName()':-- TupleStream Size is " + result.size());
        return result;
    }


    // Get the tuple rate on a single stream.
    //
    public Double getTupleValueByName(String streamName)
    {
        Double result = new Double(0.0);

        for (Iterator i = m_stats.iterator(); i.hasNext();)
        {
            Vector Vectorx = (Vector) i.next();
            Vector Vectorx1 = (Vector) Vectorx.get(1);

            for (Iterator j = Vectorx1.iterator(); j.hasNext();)
            {
                Vector Vectorx1x = (Vector) j.next();
                String name = (String) Vectorx1x.get(0);

                if (name.equals(streamName))
                {
                    result = (Double) Vectorx1x.get(1);
                //  DebugTrace.trace("new from 'getTupleValueByName()':-- Tuple Value for Stream " + streamName +" is " + result);
                }
            }
        }

        return result;
    }
}


//////////////////////////  end ParseStats.java  ///////////////////////////////
