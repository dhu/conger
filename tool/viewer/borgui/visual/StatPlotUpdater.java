package borgui.visual;

import java.util.*;
import java.awt.*;
import java.awt.event.*;

import javax.swing.event.*;

import borgui.common.*;
import borgui.visual.*;
import borgui.editor.*;
import borgui.client.*;
import borgui.graph.*;


/**
 *
 */
public class StatPlotUpdater
{
    // Plots to be displayed.
    //
    protected Vector<StatPlot>   m_statplots = new Vector<StatPlot>();

    //protected ParseStats         m_parseStatsCpu;
    //protected ParseStats         m_parseStatsLatency;
    //protected ParseStats         m_parseStatsTuple;

    // Sample statistics from each node.
    //
    protected Vector<XmlRpcStatHandler>  m_rpcHandler = new Vector<XmlRpcStatHandler>();


    // A composite statistics data sample collected from all nodes.
    //
    protected ParseStats           m_parseStatistics;

    //protected XmlRpcStatHandler  m_rpcHandlerCpu     = null;
    //protected XmlRpcStatHandler  m_rpcHandlerLatency = null;
    //protected XmlRpcStatHandler  m_rpcHandlerTuple   = null;

 
    public StatPlotUpdater() {}


    ////////////////////////////////////////////////////////////////////////////
    //
    //  Spcify the nodes from which statistics will be collected.
    //
    public void setStatisticsNode(// Save Endpoints for the nodes to be polled.
                                  //
                                  Vector<String>  endpoint)
    {
        String   node;
        String   ip;
        String   port;
    //..........................................................................


        m_rpcHandler.clear();

        for (Iterator i = endpoint.iterator(); i.hasNext();)
        {
            node = (String)i.next();
            DebugTrace.trace(node);
            ip   = HostEndpoint.getHost(node);
            port = HostEndpoint.getPort(node);

            // For now we just get the cpu utilization.
            // RPC<double> QueryProcessor::get_cpu_statistics()
            //
            m_rpcHandler.add(new XmlRpcStatHandler(ip, port,
                                   "QueryProcessor.get_cpu_statistics",
                                                   new Vector()));
        }

        return;
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    // Set the plot to display.  For now there is only one plot.
    //
    public void setStatPlot(StatPlot p)
    {
    //..........................................................................


        m_statplots.clear();
        m_statplots.add(p);
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    //  Called periodically to poll for statistics and advance the plot.
    //
    public void updateStatistics()
    {
    //..........................................................................


        final SwingWorker worker = new SwingWorker()
        {
            // Collect statistics from all nodes.
            //
            public Object construct()
            {
                synchronized(m_rpcHandler)
                {
                    Vector<Vector<Object>>   node = new Vector<Vector<Object>>();
                    Iterator<XmlRpcStatHandler> i = m_rpcHandler.iterator();

                    // Poll each node for statistics.
                    //
                    while (i.hasNext())
                    {
                        XmlRpcStatHandler  handler = i.next();

                        // Sample statistics from each node and wrap it up.
                        //
                        node.add(handler.collectNodeStatistics());
                    }

                    m_parseStatistics = new ParseStats(node);
                }

                return  null;
            }


            // Runs on the event-dispatching thread.
            // safely update GUI here
            //
            public void finished()
            {
                synchronized(m_rpcHandler)
                {
                    Iterator<StatPlot>  i = m_statplots.iterator();

                    // Draw each chart.
                    //
                    while (i.hasNext())
                    {
                        //  s = AllPlotPanel(PlotData(VisualPanel()))
                        //
                        StatPlot  s = i.next();

                        DebugTrace.trace("StatPlot size = " + m_statplots.size());

                        // Plot in AllPlotPanel.updateStatistics
                        //
                        s.updateStatistics(m_parseStatistics);
                    }
                }
            }
        };

        worker.start();
    }




    /******************************
    //  Set to collect stats.  Limited to retreive from a single node???
    //
    public void setNodeIpPort(String connectIpPort)
    {
        if (m_rpcHandlerCpu == null) {   // only on the first use.
        try
        {   String connectIp   = HostEndpoint.getHost(connectIpPort);
            String connectPort = HostEndpoint.getPort(connectIpPort);
            DebugTrace.trace(connectIpPort);

            // RPC<double> QueryProcessor::get_cpu_statistics()
            //
            m_rpcHandlerCpu = new XmlRpcStatHandler(connectIp,
                                                    connectPort,
                                    "QueryProcessor.get_cpu_statistics",
                                                    new Vector());
        }
        catch(Exception e)
        {   DebugTrace.trace("can not initialize rpcHandler");
        }
    }





    public void addStatPlot(StatPlot p)
    {
        m_statplots.add(p);
    }
    ********************/


    /**********************
    //  Set to collect stats.  Limited to retreive from a single node???
    //
    public void setNodeIpPort(String connectIpPort)
    {if (m_rpcHandlerCpu == null) {   // only on the first use.
        try
        {   String connectIp   = HostEndpoint.getHost(connectIpPort);
            String connectPort = HostEndpoint.getPort(connectIpPort);
            DebugTrace.trace(connectIpPort);

            // RPC<double> QueryProcessor::get_cpu_statistics()
            //
            m_rpcHandlerCpu = new XmlRpcStatHandler(connectIp,
                                                    connectPort,
                                    "QueryProcessor.get_cpu_statistics",
                                                    new Vector());

            m_rpcHandlerCpu = new XmlRpcStatHandler(connectIp,
                                                    connectPort,
                                    "QueryProcessor.get_region_cpu_statistics",
                                                    new Vector());

            m_rpcHandlerLatency = new XmlRpcStatHandler(connectIp,
                                                        connectPort,
                                        "QueryProcessor.get_latency_statistics",
                                                        new Vector());

            m_rpcHandlerTuple = new XmlRpcStatHandler(connectIp,
                                                      connectPort,
                                      "QueryProcessor.get_tuple_delivery_statistics",
                                                      new Vector());
        }
        catch(Exception e)
        {   DebugTrace.trace("can not initialize rpcHandler");
        }
    }}
    ********************/

    /**********************************
    public void updateStats()
    {
        updateCpuStats();
        //updateLatencyStats();
        //updateTupleStats();
    }

   
    // Make rpcs and update stats in a separate thread
    //
    private void updateCpuStats()
    {
        final SwingWorker worker = new SwingWorker()
        {
            public Object construct()
            {
                synchronized(m_rpcHandlerCpu)
                {
                    // Collect cpu utilization from each node and wrap it up.
                    //
                    m_parseStatsCpu = new ParseStats(
                          m_rpcHandlerCpu.pollTestStatistics());
                }

                return  null;
            }

            // Runs on the event-dispatching thread.
            // safely update GUI here
            //
            public void finished()
            {
                synchronized(m_rpcHandlerCpu)
                {
                    // Draw each line on the cpu chart.
                    //  s = AllPlotPanel(PlotData(VisualPanel()))
                    for (Iterator i = m_statplots.iterator(); i.hasNext();)
                    {
                        StatPlot s = (StatPlot)i.next();
                        DebugTrace.trace("StatPlot size = " + m_statplots.size());
                        s.updateCpuStats(m_parseStatsCpu);
                    }
                }
            }
        };

        worker.start();
    }
    *********************************/

    /*********************************
    // Make rpcs and update stats in a separate thread
    //
    private void updateCpuStats()
    {
        final SwingWorker worker = new SwingWorker()
        {
            public Object construct()
            {
                synchronized(m_rpcHandlerCpu)
                {
                    m_parseStatsCpu =
                         new ParseStats(m_rpcHandlerCpu.getStats());
                }

                return null;
            }

            //Runs on the event-dispatching thread.
            // safely update GUI here
            //
            public void finished()
            {
                synchronized(m_rpcHandlerCpu)
                {
                    for (Iterator i = m_statplots.iterator(); i.hasNext();)
                    {
                        StatPlot s = (StatPlot)i.next();
                        s.updateCpuStats(m_parseStatsCpu);
                    }
                }
            }
        };

        worker.start();
    }


    //Make rpcs and update stats in a separate thread
    private void updateLatencyStats()
    {
        final SwingWorker worker = new SwingWorker()
        {
            public Object construct()
            {
                synchronized(m_rpcHandlerLatency)
                {
                    m_parseStatsLatency = new ParseStats(m_rpcHandlerLatency.getStats());
                    DebugTrace.trace("Latency Ststs: " + m_parseStatsLatency.toString());
                }

                return null;
            }

            //Runs on the event-dispatching thread.
            // safely update GUI here
            public void finished()
            {
                synchronized(m_rpcHandlerLatency)
                {
                    for (Iterator i = m_statplots.iterator(); i.hasNext();)
                    {
                        StatPlot s = (StatPlot) i.next();
                        s.updateLatencyStats(m_parseStatsLatency);
                    }
                }
            }
        };

        worker.start();
    }

  
    //Make rpcs and update stats in a separate thread
    private void updateTupleStats()
    {
        final SwingWorker worker = new SwingWorker()
        {
            public Object construct()
            {
                synchronized(m_rpcHandlerTuple)
                {
                    m_parseStatsTuple = new ParseStats(m_rpcHandlerTuple.getStats());
                }

                return null;
            }

            //Runs on the event-dispatching thread.
            // safely update GUI here
            public void finished()
            {
                synchronized(m_rpcHandlerTuple)
                {
                    for (Iterator i = m_statplots.iterator(); i.hasNext();)
                    {
                        StatPlot s = (StatPlot) i.next();
                        s.updateTupleStats(m_parseStatsTuple);
                    }
                }
            }
        };

        worker.start();
    }
    **********************************/
}


////////////////////////  end  StatPlotUpdater.java  ///////////////////////////
