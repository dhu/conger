/*
 * PlotData.java
 *
 * Created on June 7, 2005, 7:49 PM
 *
 * To change this template, choose Tools | Options and locate the template under
 * the Source Creation and Management node. Right-click the template and choose
 * Open. You can then make changes to the template in the Source Editor.
 */

package borgui.visual;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.*;



import borgui.common.SwingWorker;
import borgui.common.DebugTrace;
import borgui.common.HostEndpoint;

import org.jfree.chart.*;
import org.jfree.data.time.*;
import org.jfree.chart.renderer.xy.*;


/**
 *
 *
 * @author Wenjuan Xing (vivian@cs.brown.edu)
 * version 1.0 10/13/2005
 *
 */
public class PlotData  implements ActionListener, StatPlot
{
    //SUBPLOT_COUNT is the number of the subplots.
    public static final int SUBPLOT_COUNT = 3;

    //varables for the whole plot.
    public TimeSeriesCollection[] m_datasetsAverage = new TimeSeriesCollection[SUBPLOT_COUNT];
    private double[] m_lastValueAverage = new double[SUBPLOT_COUNT];

    //TimeSeries for every subplot
    public Vector<TimeSeries>   m_sub0Series = new Vector<TimeSeries>();
    public Vector<TimeSeries>   m_sub1Series = new Vector<TimeSeries>();
    public Vector<TimeSeries>   m_sub2Series = new Vector<TimeSeries>();

    //TimeSeriesCollection for every subplot
    protected TimeSeriesCollection m_sub0Datasets = new TimeSeriesCollection();
    protected TimeSeriesCollection m_sub1Datasets = new TimeSeriesCollection();
    protected TimeSeriesCollection m_sub2Datasets = new TimeSeriesCollection();


    //XmlRpcHandler
    protected XmlRpcStatHandler m_rpcHandlerCpu;
    protected XmlRpcStatHandler m_rpcHandlerLatency;
    protected XmlRpcStatHandler m_rpcHandlerTuple;

    //Nodes' information
    protected Vector<String>    m_totalCpuIpPort = new Vector<String>();
    public    Vector<String>    m_currentCpuIpPort = new Vector<String>();
    protected int               m_totalCpuNodeSize = 0;

    protected Vector<String>    m_totalTupleStreamName   = new Vector<String>();
    protected Vector<String>    m_currentTupleStreamName = new Vector<String>();
    protected int               m_totalTupleStreamSize   = 0;

    protected Vector<String>    m_totalLatencyStreamName   = new Vector<String>();
    protected Vector<String>    m_currentLatencyStreamName = new Vector<String>();
    protected Vector<String>    m_inUseLatencyStreamName   = new Vector<String>();
    protected int               m_totalLatencyStreamSize = 0;

    //Connect IpPort
    protected String            m_connectIpPort = "";
    //protected ParseStats        m_parseStatsCpu;
    protected ParseStats        m_parseStatsLatency;
    protected ParseStats        m_parseStatsTuple;

    protected Vector<String>    m_totalShowStreamVector = new Vector<String>();

    protected Vector<StatPlot>  m_statplots = new Vector<StatPlot>();
    protected VisualPanel       m_visualPanel;


    /** Creates a new instance of PlotData */
    public PlotData( VisualPanel visualPanel)
    {
        m_visualPanel = visualPanel;

        //initial subPlot1 : cpu
        this.m_lastValueAverage[0] = 100.0;
        TimeSeries series0 = new TimeSeries("CPU avg.", Millisecond.class);
        this.m_datasetsAverage[0] = new TimeSeriesCollection(series0);

        //initial subPlot2 : Latency
        this.m_lastValueAverage[1] = 100.0;
        TimeSeries series1 = new TimeSeries("Latency avg.", Millisecond.class);
        this.m_datasetsAverage[1] = new TimeSeriesCollection(series1);

        //initial subPlot3 : tuple delivery
        this.m_lastValueAverage[2] = 100.0;
        TimeSeries series2 = new TimeSeries("Tuple avg.", Millisecond.class);
        this.m_datasetsAverage[2] = new TimeSeriesCollection(series2);
    }

    public void refreshStreamVector(Vector<String>  totalStreamVector)
    {
        m_totalShowStreamVector = totalStreamVector;
    }


    
    ////////////////////////////////////////////////////////////////////////////
    ///////////////////////////  cpu utilization  //////////////////////////////
    ////////////////////////////////////////////////////////////////////////////



    ////////////////////////////////////////////////////////////////////////////
    //
    //  Record nodes included in collected cpu utilizations.
    //
    protected void createCurrentCpuIpPort(ParseStats cpu)
    {
        String   node;
        int      totalNode;
    //..........................................................................


        m_currentCpuIpPort.clear();
        totalNode = cpu.getCpuNodeSize();

        for (int i = 0; i < totalNode; i++)
        {
            node = cpu.getCpuNodeIpPort(i);
            m_currentCpuIpPort.add(HostEndpoint.displayEndpoint(node));
        }
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    // Convert the statistical samples to time series.
    // Update only CPU utilization for now.
    //
    public void updateStatistics(ParseStats cpu)
    {
    //..........................................................................


        //m_parseStatsCpu = cpu;

        //  Record nodes included in collected cpu utilizations.
        //
        createCurrentCpuIpPort(cpu);

        //   Create the time series for each node to plot for CPU utilization.
        //
        createSub0Plot();


        //  Get the CPU utilization for each node and compute the average.
        //
        fillSub0Plot(cpu);

        /*
        for(Iterator i = m_statplots.iterator(); i.hasNext(); )
        {
                StatPlot p = (StatPlot) i.next();
                p.updateCpuStats(cpu);
        }*/
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    //   Create the time series for each node to plot for CPU utilization.
    //
    protected void createSub0Plot()
    {
        String   currentIpPort;
        String   display;

        TimeSeries  cpuSeries;
    //..........................................................................


        try
        {   Vector totalCpuCopy = (Vector)m_totalCpuIpPort.clone();
            Iterator i = m_currentCpuIpPort.iterator();

            while (i.hasNext())
            {   currentIpPort = (String)i.next();

                if (!totalCpuCopy.contains(currentIpPort))
                {
                    display = HostEndpoint.displayEndpoint(currentIpPort);
                    m_totalCpuIpPort.add(display);

                    DebugTrace.trace("currentIpPort = " + currentIpPort
                                   + "      display = " + display);

                    cpuSeries = new TimeSeries(display, Millisecond.class);
                    m_sub0Series.add(cpuSeries);
                    m_sub0Datasets.addSeries(cpuSeries);
                    m_totalCpuNodeSize = m_totalCpuNodeSize + 1;
                }
            }
        }
        catch (Exception e)
        {   DebugTrace.trace("createSub0Plot:  " + e.getMessage());
        }
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    //  Get the CPU utilization for each node and compute the average.
    //
    protected void fillSub0Plot(ParseStats cpu)
    {
        int     index = 0;
        double  currentTotalCpuValue = 0.0;
        Double  lastValue;
        String  node;

        TimeSeries  series;

        Vector<TimeSeries>  sub0SeriesCopy;
    //..........................................................................


        try
        {   //Vector<TimeSeries>  sub0SeriesCopy = (Vector<TimeSeries>)m_sub0Series.clone();

            Iterator i = m_totalCpuIpPort.iterator();
            double  hack = 10.0;

            while (i.hasNext())
            {
                node = (String)i.next();
                //DebugTrace.trace("ip = " + node + "  size = " + m_totalCpuIpPort.size());
                Millisecond  now = new Millisecond();

                if (m_currentCpuIpPort.contains(node))
                {
                    // Get the cpu utilization stored for a node.
                    //
                    lastValue  = cpu.getCpuStats(HostEndpoint.formEndpoint(node, 0)) * 100;
                    lastValue  = hack + lastValue * 100;   // test hack???
                    hack += 30.0;
                    Iterator j = m_sub0Series.iterator();

                    while (j.hasNext())
                    {   series = (TimeSeries)j.next();

                        if (series.getName().equals(node))
                        {
                            DebugTrace.trace("lastValue = " + lastValue
                                               + "  now = " + now.getMillisecond()
                                               +  "  ip = " + node );

                            series.add(now, lastValue);
                            currentTotalCpuValue = currentTotalCpuValue + lastValue;
                        }
                    }
                }
                else
                {   sub0SeriesCopy = new Vector<TimeSeries>(m_sub0Series);
                    Iterator k = sub0SeriesCopy.iterator();

                    while (k.hasNext())
                    {   series = (TimeSeries)k.next();

                        if (series.getName().equals(node))
                        {
                            m_sub0Series.remove(series);

                            m_visualPanel.m_serverButtonPanel.deleteServer(
                                            HostEndpoint.formEndpoint(node, 0));
                        }
                    }
                }
            }

            m_lastValueAverage[0] = currentTotalCpuValue / m_currentCpuIpPort.size();
            this.m_datasetsAverage[0].getSeries(0).add( new Millisecond(), m_lastValueAverage[0]);
        }
        catch(Exception e)
        {   DebugTrace.trace(e.getMessage());
        }
    }


    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////  tuple rate  //////////////////////////////
    ////////////////////////////////////////////////////////////////////////////


    protected void createCurrentTupleStreamName()
    {
        m_currentTupleStreamName.clear();
        m_currentTupleStreamName = m_parseStatsTuple.getTupleAllStreamName();
    }


    protected void createSub2Plot()
    {
        try
        {
            //Vector<String> totalTupleCopy =(Vector<String>) m_totalTupleStreamName.clone();
            Vector<String>  totalTupleCopy = new Vector<String>(m_totalTupleStreamName);

            for (Iterator i = m_currentTupleStreamName.iterator(); i.hasNext();)
            {
                String currentStream = (String) i.next();

                if (!totalTupleCopy.contains(currentStream))
                {
                    m_totalTupleStreamName.add(currentStream);
                    TimeSeries tupleSeries = new TimeSeries(currentStream, Millisecond.class);
                    m_sub2Series.add(tupleSeries);
                    m_sub2Datasets.addSeries(tupleSeries);
                    m_totalTupleStreamSize = m_totalTupleStreamSize + 1;
                }
            }
        }
        catch (Exception e)
        {   DebugTrace.trace(e.getMessage());
        }
    }



    protected void fillSub2Plot()
    {
        try
        {
            double currentTotalTupleValue = 0.0;
            int sizeOfAverage = 0;

            for (Iterator i = m_totalTupleStreamName.iterator(); i.hasNext();)
            {
                String totalStream = (String) i.next();

                if (m_currentTupleStreamName.contains(totalStream))
                {
                    Double lastValue = (Double) m_parseStatsTuple.getTupleValueByName(totalStream);

                    for(Iterator j = m_sub2Series.iterator(); j.hasNext();)
                    {
                        TimeSeries series = (TimeSeries)j.next();

                        if (series.getName().equals(totalStream))
                        {
                            series.add(new Millisecond(),lastValue);
                            //                    if(m_totalShowStreamVector.contains(totalStream)) {
                            currentTotalTupleValue = currentTotalTupleValue + lastValue;
                            sizeOfAverage = sizeOfAverage + 1;
                            //                    }
                        }
                    }
                }
            }

            m_lastValueAverage[2] = currentTotalTupleValue/sizeOfAverage;

            if (m_lastValueAverage[2]!=0)
            {
                this.m_datasetsAverage[2].getSeries(0).add( new Millisecond(), m_lastValueAverage[2]);
            }
        }
        catch (Exception e)
        {   DebugTrace.trace(e.getMessage());
        }
    }


    //for Latency
    protected void createInUseLatencyStreamName()
    {
        m_inUseLatencyStreamName.clear();
        m_currentLatencyStreamName = m_parseStatsLatency.getLatencyAllStreamName();
        m_inUseLatencyStreamName = m_currentLatencyStreamName;
    }


    protected void createSub1Plot()
    {
        try
        {   //Vector<String> totalLatencyCopy = (Vector<String>)m_totalLatencyStreamName.clone();
            Vector<String> totalLatencyCopy = new Vector<String>(m_totalLatencyStreamName);

            for (Iterator i = m_inUseLatencyStreamName.iterator(); i.hasNext();)
            {
                String inUseStream = (String) i.next();

                if (!totalLatencyCopy.contains(inUseStream))
                {
                    m_totalLatencyStreamName.add(inUseStream);
                    TimeSeries LatencySeries = new TimeSeries(inUseStream, Millisecond.class);
                    m_sub1Series.add(LatencySeries);
                    m_sub1Datasets.addSeries(LatencySeries);
                    m_totalLatencyStreamSize = m_totalLatencyStreamSize + 1;
                }
            }
        }
        catch(Exception e)
        {   DebugTrace.trace(e.getMessage());
        }
    }


    protected void fillSub1Plot()
    {
        try
        {
            for (Iterator i = m_totalLatencyStreamName.iterator(); i.hasNext();)
            {
                String totalStream = (String) i.next();

                if (m_inUseLatencyStreamName.contains(totalStream))
                {
                    Double lastValue = (Double) m_parseStatsLatency.getLatencyValueByName(totalStream);

                    if (!(lastValue == -1.0))
                    {
                        for (Iterator j = m_sub1Series.iterator(); j.hasNext();)
                        {
                            TimeSeries series = (TimeSeries) j.next();

                            if (series.getName().equals(totalStream))
                            {
                                series.add(new Millisecond(),lastValue);
                            }
                        }
                    }
                }
            }

            double totalLatencyValue = 0.0;
            double lastValue = 0.0;
            int sizeOfAverage = 0;

            if (m_currentTupleStreamName.size()==0)
            {
               /*
                for(Iterator i = m_currentLatencyStreamName.iterator(); i.hasNext();) {
                    String streamName = (String)i.next();
                    lastValue = (Double) m_parseStatsLatency.getLatencyValueByName(streamName);
                    if(lastValue!=-1) {
                        totalLatencyValue = totalLatencyValue +lastValue;
                        sizeOfAverage = sizeOfAverage + 1;
                    }
                }
                */
            }
            else
            {   for (Iterator i = m_currentLatencyStreamName.iterator(); i.hasNext();)
                {
                    String streamName = (String)i.next();

                    if (m_currentTupleStreamName.contains(streamName))
                    {
                        lastValue = (Double) m_parseStatsLatency.getLatencyValueByName(streamName);

                        if (lastValue!=-1)
                        {
                            totalLatencyValue = totalLatencyValue +lastValue;
                            sizeOfAverage = sizeOfAverage + 1;
                        }
                    }
                }
            }

            m_lastValueAverage[1] = totalLatencyValue/sizeOfAverage;

            if (m_lastValueAverage[1]!=0)
            {   this.m_datasetsAverage[1].getSeries(0).add( new Millisecond(), m_lastValueAverage[1]);
            }
        }
        catch (Exception e)
        {   DebugTrace.trace(e.getMessage());
        }
    }


    public void actionPerformed(ActionEvent e)
    {
        DebugTrace.trace("Not in use.");

        /******************************
        synchronized(this)
        {
            m_parseStatsCpu = new ParseStats( m_rpcHandlerCpu.getStats());
            m_parseStatsLatency = new ParseStats( m_rpcHandlerLatency.getStats());
            m_parseStatsTuple = new ParseStats( m_rpcHandlerTuple.getStats());

            createCurrentCpuIpPort();
            createSub0Plot();
            fillSub0Plot();

            createCurrentTupleStreamName();
            createSub2Plot();
            fillSub2Plot();

            createInUseLatencyStreamName();
            createSub1Plot();
            fillSub1Plot();

            for(Iterator i = m_xplotpanels.iterator(); i.hasNext(); )
            {
                NodeXPlotPanel p = (NodeXPlotPanel) i.next();

                p.updateStats(m_parseStatsCpu,
                              m_parseStatsLatency,
                              m_parseStatsTuple);
            }
        }
        ********************/
    }

    /****
    public void addStatPlot(StatPlot p)
    {
        m_statplots.add(p);
    }
    ***/

    /********************
    public void updateCpuStats(ParseStats cpu)
    {
        m_parseStatsCpu = cpu;

        createCurrentCpuIpPort();
        createSub0Plot();
        fillSub0Plot();

        for(Iterator i = m_statplots.iterator(); i.hasNext(); )
        {
                StatPlot p = (StatPlot) i.next();
                p.updateCpuStats(cpu);
        }


    }

    public void updateLatencyStats(ParseStats latency)
    {
        m_parseStatsLatency = latency;

        createInUseLatencyStreamName();
        createSub1Plot();
        fillSub1Plot();

        for(Iterator i = m_statplots.iterator(); i.hasNext();)
        {
                StatPlot p = (StatPlot) i.next();
                p.updateLatencyStats(latency);
        }
    }


    public void updateTupleStats(ParseStats tuple)
    {
        m_parseStatsTuple = tuple;

        createCurrentTupleStreamName();
        createSub2Plot();
        fillSub2Plot();

        for(Iterator i = m_statplots.iterator(); i.hasNext(); )
        {
                StatPlot p = (StatPlot) i.next();
                p.updateTupleStats(tuple);
        }
    }
    ********************/
}



    /*********************
    public void setNodeIpPorts(Vector<String>  nodeIpPorts)
    {
        try
        {   m_connectIpPort = (String) nodeIpPorts.get(0);
            //DebugTrace.trace("connectIpPort: " + m_connectIpPort);

            initialRpcHandler();
        }
        catch(Exception e)
        {   DebugTrace.trace("setNodeIpPorts'---can not get connect ip and port");
        }
    }


    protected void initialRpcHandler()
    {
        try
        {
            String connectIp = m_connectIpPort.substring(0, m_connectIpPort.indexOf(':'));
            String connectPort = m_connectIpPort.substring(m_connectIpPort.indexOf(':')+1, m_connectIpPort.length());

            m_rpcHandlerCpu     = new XmlRpcStatHandler(connectIp,
                                                        connectPort,
                                        "QueryProcessor.get_region_cpu_statistics",
                                                        new Vector());

            m_rpcHandlerLatency = new XmlRpcStatHandler(connectIp,
                                                        connectPort,
                                        "QueryProcessor.get_latency_statistics",
                                                        new Vector());

            m_rpcHandlerTuple   = new XmlRpcStatHandler(connectIp,
                                                        connectPort,
                                        "QueryProcessor.get_tuple_delivery_statistics",
                                                        new Vector());
        }
        catch (Exception e)
        {   DebugTrace.trace("initialRpcHandler:  can not initial rpcHandler");
        }
    }
    **********************/



//////////////////////////  end PlotData.java  /////////////////////////////////
