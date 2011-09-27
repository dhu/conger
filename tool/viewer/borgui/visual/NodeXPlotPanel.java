/**
 * NodeXPlotPanel.java
 *
 * Created on June 6, 2005, 12:58 PM
 *
 */

package borgui.visual;


import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.*;

import javax.swing.BorderFactory;
import javax.swing.JPanel;

import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.axis.DateAxis;
import org.jfree.chart.axis.NumberAxis;
import org.jfree.chart.axis.ValueAxis;
import org.jfree.chart.plot.CombinedDomainXYPlot;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.renderer.xy.StandardXYItemRenderer;
import org.jfree.chart.title.LegendTitle;
import org.jfree.data.time.Millisecond;
import org.jfree.data.time.TimeSeries;
import org.jfree.data.time.TimeSeriesCollection;
import org.jfree.ui.ApplicationFrame;
import org.jfree.ui.RectangleEdge;
import org.jfree.ui.RectangleInsets;
import org.jfree.ui.RefineryUtilities;
import org.jfree.util.UnitType;
import org.jfree.chart.renderer.xy.DefaultXYItemRenderer;
import org.jfree.chart.renderer.xy.*;



/**
 *
 *
 * @author Wenjuan Xing (vivian@cs.brown.edu)
 * version 1.0 10/13/2005
 *
 */
public class NodeXPlotPanel extends JPanel implements ActionListener, StatPlot
{
    //nodeX ipPort
    protected String m_nodeXIpPort = new String("");
    protected int m_cpuIndex = 0;
    
    //SUBPLOT_COUNT is the number of the subplots.
    public static final int SUBPLOT_COUNT = 3;
    
    //varables for the whole plot.
    private CombinedDomainXYPlot m_plot;
    private TimeSeriesCollection[] m_datasetsAverage = new TimeSeriesCollection[SUBPLOT_COUNT];
    private double[] m_lastValueAverage = new double[SUBPLOT_COUNT];
    
    //TimeSeries for every subplot
    protected Vector<TimeSeries> m_sub1Series = new Vector<TimeSeries>();
    protected Vector<TimeSeries> m_sub2Series = new Vector<TimeSeries>();
    
    //TimeSeriesCollection for every subplot
    protected TimeSeriesCollection m_sub1Datasets = new TimeSeriesCollection();
    protected TimeSeriesCollection m_sub2Datasets = new TimeSeriesCollection();
    
    //Render for every subplot
    protected XYItemRenderer m_sub1Renderer;
    protected XYItemRenderer m_sub2Renderer;
    
    //XmlRpcHandler
    protected XmlRpcStatHandler m_rpcHandlerCpu;
    protected XmlRpcStatHandler m_rpcHandlerLatency;
    protected XmlRpcStatHandler m_rpcHandlerTuple;
    
    protected Vector<String>  m_totalTupleStreamName   = new Vector<String>();
    protected Vector<String>  m_currentTupleStreamName = new Vector<String>();
    protected int m_totalTupleStreamSize = 0;
    
    // protected Vector<String>  m_currentAllNodeTupleStreamName = new Vector<String>();
    
    protected Vector<String>  m_totalLatencyStreamName     = new Vector<String>();
    protected Vector<String>  m_currentLatencyStreamName   = new Vector<String>();
    protected Vector<String>  m_inUseLatencyStreamName     = new Vector<String>();
    protected Vector<String>  m_totalShowStreamVector      = new Vector<String>();
    protected Vector<String>  m_averageTupleStreamVector   = new Vector<String>();
    protected Vector<String>  m_averageLatencyStreamVector = new Vector<String>();
    
    protected int m_totalLatencyStreamSize = 0;
    
    //Connect IpPort
    protected String m_connectIpPort = "";
    
    protected ParseStats m_parseStatsCpu;
    protected ParseStats m_parseStatsLatency;
    protected ParseStats m_parseStatsTuple;
    
    
    //Color for subplot
    protected PlotColor m_plotColor;
    XYItemRenderer m_sub0Render;
    
    /** Creates a new instance of NodeXPlotPanel */
    public NodeXPlotPanel(String nodeIpPort, int cpuIndex)
    {
        super(new BorderLayout());
        
        m_nodeXIpPort = nodeIpPort;
        m_cpuIndex = cpuIndex;
        
        //initial the whole Plot:
        m_plot = new CombinedDomainXYPlot( new DateAxis("") );
        
        
        //initial sub0Plot : cpu
        this.m_lastValueAverage[0] = 100.0;
        TimeSeries series0 = new TimeSeries( "CPU", Millisecond.class );
        this.m_datasetsAverage[0] = new TimeSeriesCollection(series0);
        series0.setHistoryCount(80000);
        
        NumberAxis rangeAxis0 = new NumberAxis("CPU ( % )");
        //rangeAxis0.setAutoRangeIncludesZero(true);
        rangeAxis0.setRange(0.0, 105.0);
        
        
        XYPlot m_sub0Plot = new XYPlot(this.m_datasetsAverage[0], null, rangeAxis0,new DefaultXYItemRenderer());
        m_sub0Plot.setBackgroundPaint(Color.lightGray);
        m_sub0Plot.setDomainGridlinePaint(Color.white);
        m_sub0Plot.setRangeGridlinePaint(Color.white);
        
        m_sub0Plot.setRenderer(0, new DefaultXYItemRenderer());
        
        //move due to the color
        m_sub0Render = m_sub0Plot.getRenderer(0);
        //sub0Render.setPaint(m_plotColor.getNodeColor(m_nodeXIpPort));
        
        m_plot.add(m_sub0Plot);
        
        
        //initial sub1Plot : Latency
        this.m_lastValueAverage[1] = 100.0;
        TimeSeries series1 = new TimeSeries( "Latency avg.", Millisecond.class );
        this.m_datasetsAverage[1] = new TimeSeriesCollection(series1);
        
        NumberAxis rangeAxis1 = new NumberAxis("Latency");
        rangeAxis1.setAutoRangeIncludesZero(true);
        
        XYPlot m_sub1Plot = new XYPlot(this.m_datasetsAverage[1], null, rangeAxis1,new DefaultXYItemRenderer());
        m_sub1Plot.setBackgroundPaint(Color.lightGray);
        m_sub1Plot.setDomainGridlinePaint(Color.white);
        m_sub1Plot.setRangeGridlinePaint(Color.white);
        
        m_sub1Plot.setDataset(1, m_sub1Datasets);
        m_sub1Plot.setRenderer(0, new DefaultXYItemRenderer());
        m_sub1Plot.setRenderer(1, new StandardXYItemRenderer());
        
        XYItemRenderer sub1Render = m_sub1Plot.getRenderer(0);
        sub1Render.setPaint(Color.blue);
        m_sub1Renderer = m_sub1Plot.getRenderer(1);
        
        //wenjuan: dislay latency
        m_plot.add(m_sub1Plot);
        
        
        
        //initial sub2Plot : tuple delivery
        this.m_lastValueAverage[2] = 100.0;
        TimeSeries series2 = new TimeSeries( "Tuple avg.", Millisecond.class );
        this.m_datasetsAverage[2] = new TimeSeriesCollection(series2);
        
        NumberAxis rangeAxis2 = new NumberAxis("Tuple ( % )");
//        rangeAxis2.setAutoRangeIncludesZero(true);
        rangeAxis2.setRange(0.0, 105.0);
        
        XYPlot m_sub2Plot = new XYPlot(this.m_datasetsAverage[2], null, rangeAxis2,new DefaultXYItemRenderer());
        m_sub2Plot.setBackgroundPaint(Color.lightGray);
        m_sub2Plot.setDomainGridlinePaint(Color.white);
        m_sub2Plot.setRangeGridlinePaint(Color.white);
        
        m_sub2Plot.setDataset(1, m_sub2Datasets);
        m_sub2Plot.setRenderer(0, new DefaultXYItemRenderer());
        m_sub2Plot.setRenderer(1, new StandardXYItemRenderer());
        
        XYItemRenderer sub2Render = m_sub2Plot.getRenderer(0);
        sub2Render.setPaint(Color.blue);
        m_sub2Renderer = m_sub2Plot.getRenderer(1);
        
        m_plot.add(m_sub2Plot);
        
        
        //add all the subplot into chart
        JFreeChart chart = new JFreeChart("", m_plot);
        
        LegendTitle legend = (LegendTitle) chart.getSubtitle(0);
        legend.setPosition(RectangleEdge.RIGHT);
        legend.setMargin( new RectangleInsets(UnitType.ABSOLUTE, 0, 0, 0, 8) );
        
        chart.setBorderPaint(Color.black);
        chart.setBorderVisible(true);
        chart.setBackgroundPaint(Color.white);
        
        m_plot.setBackgroundPaint(Color.lightGray);
        m_plot.setDomainGridlinePaint(Color.white);
        m_plot.setRangeGridlinePaint(Color.white);
        m_plot.setAxisOffset(new RectangleInsets(4, 4, 4, 4));
        
        ValueAxis axis = m_plot.getDomainAxis();
        axis.setAutoRange(true);
        axis.setFixedAutoRange(60000.0);  // 60 seconds
        
        ChartPanel chartPanel = new ChartPanel(chart);
        add(chartPanel);
        
        chartPanel.setPreferredSize(new java.awt.Dimension(400, 180));
        chartPanel.setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));
    }
    
    public void setPlotColor(PlotColor plotColor)
    {
        m_plotColor = plotColor;
        
        m_sub0Render.setPaint(m_plotColor.getNodeColor(m_nodeXIpPort));
    }

    /*********************************
    public void setNodeIpPorts(Vector nodeIpPorts)
    {
        m_connectIpPort = (String) nodeIpPorts.get(0);
        
        initialRpcHandler();
    }
    
    protected void initialRpcHandler()
    {
        try
        {   String connectIp   = m_connectIpPort.substring(0, m_connectIpPort.indexOf(':'));
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
        catch(Exception e)
        {
            System.out.println("from 'NodeXPlotPanel'-'initialRpcHandler'---can not initial rpcHandler");
        }
    }
    **********************************/


    //for CPU
    protected void fillSub0Plot()
    {
        Double lastValue =  m_parseStatsCpu.getCpuStats(m_nodeXIpPort) * 100;
        m_lastValueAverage[0] = lastValue;

        if (m_lastValueAverage[0]!=0.0)
        {
            this.m_datasetsAverage[0].getSeries(0).add( new Millisecond(), m_lastValueAverage[0]);
        }
        
    }
    
//for Tuple
    protected void createCurrentTupleStreamName()
    {
        try
        {   m_currentTupleStreamName.clear();
            m_averageTupleStreamVector.clear();
            int nodeXTupleStreamSize = m_parseStatsTuple.getTupleStreamSize(m_nodeXIpPort);

            for (int i = 0; i<nodeXTupleStreamSize; i++)
            {
                String nodeXTupleStreamName = m_parseStatsTuple.getTupleStreamName(m_nodeXIpPort,i);
                m_averageTupleStreamVector.add(nodeXTupleStreamName);

                if (m_totalShowStreamVector.contains(nodeXTupleStreamName))
                {
                    m_currentTupleStreamName.add(nodeXTupleStreamName);
                }
            }
        }
        catch(Exception e)
        {   System.out.println("from NodeXPlotPanle.java: wrong with createCurrentTupleStreamName()");
        }
    }
    
    //create totalTupleStream Vector and create sub2Plot
    protected void createSub2Plot()
    {
        try
        {
            Vector totalTupleCopy =(Vector) m_totalTupleStreamName.clone();

            for (Iterator i = m_currentTupleStreamName.iterator(); i.hasNext();)
            {
                String currentStream = (String) i.next();

                if (!totalTupleCopy.contains(currentStream))
                {
                    m_totalTupleStreamName.add(currentStream);
                    TimeSeries tupleSeries = new TimeSeries(currentStream, Millisecond.class);
                    tupleSeries.setHistoryCount(80000);
                    m_sub2Series.add(tupleSeries);
                    m_sub2Datasets.addSeries(tupleSeries);
                    m_sub2Renderer.setSeriesPaint(m_totalTupleStreamSize, m_plotColor.getStreamColor(currentStream));
                    m_totalTupleStreamSize = m_totalTupleStreamSize + 1;
                }
            }
        }
        catch(Exception e)
        {
            System.out.println("from NodeXPlotPanle.java: wrong in createSub2Plot");
        }
    }
    
    protected void fillSub2Plot()
    {
        try
        {   Double lastValue = new Double(0.0);
            Vector sub2SeriesCopy = (Vector) m_sub2Series.clone();
            int sizeOfAverage  = 0;
            
            for (Iterator i = m_totalTupleStreamName.iterator(); i.hasNext();)
            {
                String totalStream = (String) i.next();

                if (m_currentTupleStreamName.contains(totalStream))
                {
                    lastValue =  m_parseStatsTuple.getTupleValueByName(totalStream);

                    for (Iterator j = sub2SeriesCopy.iterator(); j.hasNext();)
                    {
                        TimeSeries series = (TimeSeries)j.next();

                        if (series.getName().equals(totalStream))
                        {
                            series.add(new Millisecond(),lastValue);
                        }
                    }
                }
                else
                {
                    for (Iterator j = sub2SeriesCopy.iterator(); j.hasNext();)
                    {
                        TimeSeries series = (TimeSeries)j.next();

                        if (series.getName().equals(totalStream))
                        {
                            m_sub2Series.remove(series);
                        }
                    }
                }
            }
            
            double currentTotalTupleValue = 0.0;

            for (Iterator i = m_averageTupleStreamVector.iterator(); i.hasNext();)
            {
                String streamName = (String)i.next();
                lastValue = m_parseStatsTuple.getTupleValueByName(streamName);
                currentTotalTupleValue = currentTotalTupleValue + lastValue;
                sizeOfAverage = sizeOfAverage + 1;
            }
            
            m_lastValueAverage[2] = currentTotalTupleValue/sizeOfAverage;

            if (m_lastValueAverage[2]!=0)
            {
                this.m_datasetsAverage[2].getSeries(0).add( new Millisecond(), m_lastValueAverage[2]);
            }
        }
        catch(Exception e)
        {
            System.out.println("from NodeXPlotPanel.java: wrong in fillSub2Plot");
        }
        
    }
//for Latency
    
    
    public void refreshStreamVector(Vector<String>  totalStreamVector)
    {
        m_totalShowStreamVector = totalStreamVector;
    }
    
    protected void createInUseLatencyStreamName()
    {
        try
        {
            m_inUseLatencyStreamName.clear();
            m_averageLatencyStreamVector.clear();
            int nodeXLatencyStreamSize = m_parseStatsLatency.getLatencyStreamSize(m_nodeXIpPort);

            for (int i = 0; i<nodeXLatencyStreamSize; i++)
            {
                String nodeXLatencyStreamName = m_parseStatsLatency.getLatencyStreamName(m_nodeXIpPort,i);

                if (m_averageTupleStreamVector.contains(nodeXLatencyStreamName))
                {
                    m_averageLatencyStreamVector.add(nodeXLatencyStreamName);
                }

                if (m_totalShowStreamVector.contains(nodeXLatencyStreamName))
                {
                    m_inUseLatencyStreamName.add(nodeXLatencyStreamName);
                }
            }
        }
        catch(Exception e)
        {
            //e.printStackTrace();
            System.out.println("from NodeXPlotPanle.java: wrong with createInUseLatencyStreamName()");
        }
    }

    protected void createSub1Plot()
    {
        try
        {
            //Vector  totalLatencyCopy = (Vector)m_totalLatencyStreamName.clone();
            Vector<String>  totalLatencyCopy = new Vector<String>(m_totalLatencyStreamName);

            for (Iterator i = m_inUseLatencyStreamName.iterator(); i.hasNext();)
            {
                String inUseStream = (String) i.next();

                if (!totalLatencyCopy.contains(inUseStream))
                {
                    m_totalLatencyStreamName.add(inUseStream);
                    TimeSeries LatencySeries = new TimeSeries(inUseStream, Millisecond.class);
                    LatencySeries.setHistoryCount(80000);
                    m_sub1Series.add(LatencySeries);
                    m_sub1Datasets.addSeries(LatencySeries);
                    m_sub1Renderer.setSeriesPaint(m_totalLatencyStreamSize, m_plotColor.getStreamColor(inUseStream));
                    m_totalLatencyStreamSize = m_totalLatencyStreamSize + 1;
                }
            }
        }
        catch(Exception e)
        {
            System.out.println("from PlotData.java: wrong with creatSub1Plot");
        }
    }
    
    protected void fillSub1Plot()
    {
        try
        {
            Double lastValue = new Double(0.0);
            //Vector sub1SeriesCopy = (Vector)m_sub1Series.clone();
            Vector<TimeSeries> sub1SeriesCopy = new Vector<TimeSeries>(m_sub1Series);
            
            for (Iterator i = m_totalLatencyStreamName.iterator(); i.hasNext();)
            {
                String totalStream = (String) i.next();

                if (m_inUseLatencyStreamName.contains(totalStream))
                {
                    lastValue = (Double) m_parseStatsLatency.getLatencyValueByName(totalStream);

                    if (!(lastValue==-1.0))
                    {
                        for (Iterator j = sub1SeriesCopy.iterator(); j.hasNext();)
                        {
                            TimeSeries series =(TimeSeries) j.next();

                            if (series.getName().equals(totalStream))
                            {
                                series.add(new Millisecond(),lastValue);
                            }
                        }
                    }
                }
                else
                {   for (Iterator k = sub1SeriesCopy.iterator(); k.hasNext();)
                    {
                        TimeSeries series =(TimeSeries) k.next();

                        if (series.getName().equals(totalStream))
                        {
                            m_sub1Series.remove(series);
                        }
                    }
                }
            }
            
            double totalLatencyValue = 0.0;
            int sizeOfAverage = 0;

            for (Iterator i = m_averageLatencyStreamVector.iterator(); i.hasNext();)
            {
                String streamName = (String) i.next();
                lastValue = (Double) m_parseStatsLatency.getLatencyValueByName(streamName);

                if (lastValue!=-1)
                {
                    totalLatencyValue = totalLatencyValue + lastValue;
                    sizeOfAverage = sizeOfAverage + 1;
                }
            }
            
            m_lastValueAverage[1] = totalLatencyValue/sizeOfAverage;

            if (m_lastValueAverage[1]!=0)
            {
                this.m_datasetsAverage[1].getSeries(0).add( new Millisecond(), m_lastValueAverage[1]);
            }
        }
        catch(Exception e)
        {
            System.out.println("from NodeXPlotPanel.java: wrong with 'fillSub1Plot'");
        }
    }

    
    /***************************
    public void updateStats(ParseStats cpu,
                            ParseStats latency,
                            ParseStats tuple)
    {
        m_parseStatsCpu = cpu;
        m_parseStatsLatency = latency;
        m_parseStatsTuple = tuple;
 
        fillSub0Plot();
 
        createCurrentTupleStreamName();
        createSub2Plot();
        fillSub2Plot();
 
        createInUseLatencyStreamName();
        createSub1Plot();
        fillSub1Plot();
    }
    ****************/
    
    
    public void updateStatistics(ParseStats cpu)
    {
        m_parseStatsCpu = cpu;
        fillSub0Plot();
        
    }


    /**********************
    public void updateCpuStats(ParseStats cpu)
    {
        m_parseStatsCpu = cpu;
        fillSub0Plot();
        
    }

    
    public void updateLatencyStats(ParseStats latency)
    {
        m_parseStatsLatency = latency;
        
        createInUseLatencyStreamName();
        createSub1Plot();
        fillSub1Plot();
    }

    
    public void updateTupleStats(ParseStats tuple)
    {
        m_parseStatsTuple = tuple;
        
        createCurrentTupleStreamName();
        createSub2Plot();
        fillSub2Plot();
    }
    *********************/
    
    
    public void actionPerformed(ActionEvent e)
    {
        /*************************
        m_parseStatsCpu = new ParseStats( m_rpcHandlerCpu.getStats());
        m_parseStatsLatency = new ParseStats( m_rpcHandlerLatency.getStats());
        m_parseStatsTuple = new ParseStats( m_rpcHandlerTuple.getStats());
         
        fillSub0Plot();
         
        createCurrentTupleStreamName();
        createSub2Plot();
        fillSub2Plot();
         
        createInUseLatencyStreamName();
        createSub1Plot();
        fillSub1Plot();
        */
    }
}


/////////////////////////  end NodeXPlotPanel.java  ////////////////////////////
