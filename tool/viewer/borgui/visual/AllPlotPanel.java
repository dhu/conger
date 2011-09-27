/**
 * AllPlotPanel.java
 *
 * Created on 2005
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

import org.jfree.chart.*;
import org.jfree.chart.axis.*;
import org.jfree.chart.plot.CombinedDomainXYPlot;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.renderer.xy.StandardXYItemRenderer;
import org.jfree.chart.title.LegendTitle;
import org.jfree.data.time.*;
import org.jfree.ui.*;
import org.jfree.util.UnitType;
import org.jfree.chart.renderer.xy.*;

import borgui.common.DebugTrace;
import borgui.common.HostEndpoint;


/**
 * A class that represents a plot panel for latenshowing all the statistic data, include all the nodes.
 *
 * @author Wenjuan Xing (vivian@cs.brown.edu)
 * version 1.0 10/13/2005
 *
 */
public class AllPlotPanel  extends JPanel
                        implements ActionListener, StatPlot
{
    //SUBPLOT_COUNT is the number of the subplots.
    public static final int SUBPLOT_COUNT = 3;
    public static final int AROUND_TIME = 70000;
    
    //varables for the whole plot.
    private CombinedDomainXYPlot  m_plot;
    public TimeSeriesCollection[] m_datasetsAverage  = new TimeSeriesCollection[SUBPLOT_COUNT];
    private double[]              m_lastValueAverage = new double[SUBPLOT_COUNT];
    
    //TimeSeriesCollection for every subplot
    protected TimeSeriesCollection m_sub0Datasets = new TimeSeriesCollection();
    protected TimeSeriesCollection m_sub1Datasets = new TimeSeriesCollection();
    protected TimeSeriesCollection m_sub2Datasets = new TimeSeriesCollection();
    
    // Render for every subplot
    // the visual representation of a single (x, y) item on an XYPlot. 
    //
    protected XYItemRenderer m_sub0Renderer;
    protected XYItemRenderer m_sub1Renderer;
    protected XYItemRenderer m_sub2Renderer;
    
    //Color for subplot
    protected PlotColor  m_plotColor;
    protected PlotData   m_plotData;
    
    protected Vector m_totalStreamVector = new Vector();
    
    //test
    protected Legend m_legend;
    protected JFreeChart m_chart;
    

    //  Creates a new instance of AllPlotPanel
    //
    public AllPlotPanel(PlotData plotData)
    {
        super(new BorderLayout());
        
        m_plotData = plotData;
        
        //initial the whole Plot:
        m_plot = new CombinedDomainXYPlot( new DateAxis("") );
        
        
        //initial sub0Plot : cpu
        this.m_lastValueAverage[0] = 100.0;
        TimeSeries series0 = new TimeSeries("CPU avg.", Millisecond.class);
        this.m_datasetsAverage[0] = new TimeSeriesCollection(series0);
        
        NumberAxis rangeAxis0 = new NumberAxis("CPU ( % )");
        //rangeAxis0.setAutoRangeIncludesZero(true);
        rangeAxis0.setRange(0.0, 105.0);
        
        XYPlot m_sub0Plot = new XYPlot(this.m_datasetsAverage[0], null, rangeAxis0,new DefaultXYItemRenderer());
        m_sub0Plot.setBackgroundPaint(Color.lightGray);
        m_sub0Plot.setDomainGridlinePaint(Color.white);
        m_sub0Plot.setRangeGridlinePaint(Color.white);
        
        m_sub0Plot.setDataset(1, m_sub0Datasets);
        m_sub0Plot.setRenderer(0, new DefaultXYItemRenderer());
        m_sub0Plot.setRenderer(1, new StandardXYItemRenderer());
        
        XYItemRenderer sub0Render = m_sub0Plot.getRenderer(0);
        sub0Render.setPaint(Color.blue);
        m_sub0Renderer = m_sub0Plot.getRenderer(1);
        
        m_plot.add(m_sub0Plot);
        
        
        //initial sub1Plot : Latency
        this.m_lastValueAverage[1] = 100.0;
        TimeSeries series1 = new TimeSeries("Latency avg.", Millisecond.class);
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
        
        //wenjuan: display latency
         m_plot.add(m_sub1Plot);
        
        //initial sub2Plot : tuple delivery
        this.m_lastValueAverage[2] = 100.0;
        TimeSeries series2 = new TimeSeries("Tuple avg.", Millisecond.class);
        this.m_datasetsAverage[2] = new TimeSeriesCollection(series2);
        
        NumberAxis rangeAxis2 = new NumberAxis("Tuple ( % )");
        rangeAxis2.setRange(0.0, 105.0);
//        rangeAxis2.setAutoRangeIncludesZero(true);
        
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
        
        
        //add all the subplot into m_chart
        m_chart = new JFreeChart("", m_plot);
        
        LegendTitle legend = (LegendTitle) m_chart.getSubtitle(0);
        legend.setPosition(RectangleEdge.RIGHT);
        legend.setMargin( new RectangleInsets(UnitType.ABSOLUTE, 0, 0, 0, 8) );
        
        //test
        m_legend = m_chart.getLegend();
        
        m_chart.setBorderPaint(Color.black);
        m_chart.setBorderVisible(true);
        m_chart.setBackgroundPaint(Color.white);
        
        m_plot.setBackgroundPaint(Color.lightGray);
        m_plot.setDomainGridlinePaint(Color.white);
        m_plot.setRangeGridlinePaint(Color.white);
        m_plot.setAxisOffset(new RectangleInsets(4, 4, 4, 4));
        
        ValueAxis axis = m_plot.getDomainAxis();
        axis.setAutoRange(true);
        axis.setFixedAutoRange(60000.0);  // 60 seconds
        
        ChartPanel chartPanel = new ChartPanel(m_chart);
        add(chartPanel);
        
        chartPanel.setPreferredSize(new java.awt.Dimension(400, 180));
        chartPanel.setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));
    }

    
    // Update the current stream vector.
    //
    public void refreshStreamVector(Vector totalStreamVector)
    {
        m_totalStreamVector = totalStreamVector;
    }
    


    //  Set the plotColor. Each stream's color will be set according to this sequence.
    //
    public void setPlotColor(PlotColor plotColor)
    {
        m_plotColor = plotColor;
    }


    

    ////////////////////////////////////////////////////////////////////////////
    //
    // Update only the CPU utilization for now; extent to all statistics per node.
    //
    public void updateStatistics(ParseStats cpu)
    {
    //..........................................................................


        // Convert the CPU utilization samples to time series.
        //
        m_plotData.updateStatistics(cpu);
        
        try
        {   Date nowDate    = new Date();
            Date beforeDate = new Date(nowDate.getTime() - AROUND_TIME);
            
            Millisecond before = new Millisecond(beforeDate);
            Millisecond now    = new Millisecond(nowDate);
            
            //for average
            //for (int i = 0; i<SUBPLOT_COUNT; i++) {
            TimeSeries seriesAverage = m_datasetsAverage[0].getSeries(0);

            seriesAverage.clear();
            seriesAverage = m_plotData.m_datasetsAverage[0].getSeries(0).createCopy(before, now);

            m_datasetsAverage[0].removeAllSeries();
            m_datasetsAverage[0].addSeries(seriesAverage);
            //}
            
            
            //for sub0;
            m_sub0Datasets.removeAllSeries();
            Vector oldSub0Series = m_plotData.m_sub0Series;

            // the series index (zero-based).
            //
            int  rendererCpuIndex = 0;
            Iterator i = oldSub0Series.iterator();

            while (i.hasNext())
            {
                TimeSeries  oldSeries = (TimeSeries)i.next();
                String      ipPort    = oldSeries.getName();
                String      node      = HostEndpoint.formEndpoint(ipPort, 0);

                DebugTrace.trace("ipPort = " + ipPort + "  node = " + node);

                if (m_plotData.m_currentCpuIpPort.contains(ipPort))
                {
                    TimeSeries newSeries = oldSeries.createCopy(before, now);
                    m_sub0Datasets.addSeries(newSeries);

                    //DebugTrace.trace("rendererCpuIndex=" + rendererCpuIndex);

                    // Sets the paint used for a series and sends a
                    // RendererChangeEvent to all registered listeners. 
                    //
                    m_sub0Renderer.setSeriesPaint(rendererCpuIndex,
                                      m_plotColor.getNodeColor(node));

                    rendererCpuIndex = rendererCpuIndex + 1;
                }
            }
        }
        catch(Exception exception)
        {   DebugTrace.trace(exception.getMessage());
        }
    }
    

    

    /*******************************************
    // Update the CPU utilization
    //
    public void updateCpuStats(ParseStats cpu)
    {
        m_plotData.updateCpuStats(cpu);
        
        try
        {
            Date nowDate = new Date();
            Date beforeDate = new Date(nowDate.getTime() - AROUND_TIME);
            
            Millisecond before = new Millisecond(beforeDate);
            Millisecond now = new Millisecond(nowDate);
            
            //for average
            //for (int i = 0; i<SUBPLOT_COUNT; i++) {
            TimeSeries seriesAverage = m_datasetsAverage[0].getSeries(0);
            seriesAverage.clear();
            seriesAverage = m_plotData.m_datasetsAverage[0].getSeries(0).createCopy(before, now);
            m_datasetsAverage[0].removeAllSeries();
            m_datasetsAverage[0].addSeries(seriesAverage);
            
            //}
            
            
            //for sub0;
            m_sub0Datasets.removeAllSeries();
            Vector oldSub0Series = m_plotData.m_sub0Series;

            // the series index (zero-based).
            //
            int rendererCpuIndex = 0;
            
            for (Iterator i = oldSub0Series.iterator(); i.hasNext();)
            {
                TimeSeries oldSeries = (TimeSeries)i.next();
                String ipPort = oldSeries.getName();
                DebugTrace.trace("ipPort = " + ipPort);

                if (m_plotData.m_currentCpuIpPort.contains(ipPort))
                {
                    TimeSeries newSeries = oldSeries.createCopy(before, now);
                    m_sub0Datasets.addSeries(newSeries);

                    //DebugTrace.trace("rendererCpuIndex=" + rendererCpuIndex);

                    // Sets the paint used for a series and sends a
                    // RendererChangeEvent to all registered listeners. 
                    //
                    m_sub0Renderer.setSeriesPaint(rendererCpuIndex,
                                      m_plotColor.getNodeColor(ipPort));

                    rendererCpuIndex = rendererCpuIndex + 1;
                }
            }
        }
        catch(Exception exception)
        {
            //exception.printStackTrace();
            DebugTrace.trace("History copy wrong");
        }
    }
    

    // Update the statistics of Latency
    //
    public void updateLatencyStats(ParseStats latency)
    {
        
        m_plotData.updateLatencyStats(latency);
        
        try
        {
            Date nowDate = new Date();
            Date beforeDate = new Date(nowDate.getTime() - AROUND_TIME);
            
            Millisecond before = new Millisecond(beforeDate);
            Millisecond now = new Millisecond(nowDate);
            
            //for average
            //for (int i = 0; i<SUBPLOT_COUNT; i++) {
            TimeSeries seriesAverage = m_datasetsAverage[1].getSeries(0);
            seriesAverage.clear();
            seriesAverage = m_plotData.m_datasetsAverage[1].getSeries(0).createCopy(before, now);
            m_datasetsAverage[1].removeAllSeries();
            m_datasetsAverage[1].addSeries(seriesAverage);
            
            //}
            
            
            //for sub1;
            int rendererLatencyIndex = 0;
            
            if (m_totalStreamVector.size() == 0)
            {
                //m_sub1Datasets.removeAllSeries();
                 
                //Vector oldSub1Series = m_plotData.m_sub1Series;

                //for (Iterator i = oldSub1Series.iterator(); i.hasNext();)
                //{
                //    TimeSeries oldSeries = (TimeSeries) i.next();
                //    TimeSeries newSeries = oldSeries.createCopy(before, now);
                //    String name = oldSeries.getName();
                //    newSeries.setName(" ");
                //    m_sub1Datasets.addSeries(newSeries);
                //    m_sub1Renderer.setSeriesPaint(rendererLatencyIndex, m_plotColor.getStreamColor(name));
                //    rendererLatencyIndex = rendererLatencyIndex + 1;
                //}
            }
            else
            {
                rendererLatencyIndex = 0;
                m_sub1Datasets.removeAllSeries();
                Vector oldSub1Series = m_plotData.m_sub1Series;

                for (Iterator i = oldSub1Series.iterator(); i.hasNext();)
                {
                    TimeSeries oldSeries = (TimeSeries) i.next();
                    String oldSeriesName = oldSeries.getName();
                    boolean isInShowVector = false;

                    for (Iterator j = m_totalStreamVector.iterator(); j.hasNext();)
                    {
                        String showStream = (String) j.next();

                        if (oldSeriesName.equals(showStream))
                        {
                            isInShowVector = true;
                        }
                    }
                    
                    if (isInShowVector)
                    {
                        TimeSeries newSeries = oldSeries.createCopy(before, now);
                        String name = oldSeries.getName();
                        m_sub1Datasets.addSeries(newSeries);
                        m_sub1Renderer.setSeriesPaint(rendererLatencyIndex, m_plotColor.getStreamColor(name));
                        rendererLatencyIndex = rendererLatencyIndex + 1;
                    }
                }
                
            }
        }
        catch(Exception exception)
        {
            //exception.printStackTrace();
            DebugTrace.trace("History copy wrong");
        }
    }
  
    
    // Update the statistics of Tuple
    //
    public void updateTupleStats(ParseStats tuple)
    {
        
        m_plotData.updateTupleStats(tuple);
        
        try
        {
            Date nowDate = new Date();
            Date beforeDate = new Date(nowDate.getTime() - AROUND_TIME);
            
            Millisecond before = new Millisecond(beforeDate);
            Millisecond now = new Millisecond(nowDate);
            
            TimeSeries seriesAverage = m_datasetsAverage[2].getSeries(0);
            seriesAverage.clear();
            seriesAverage = m_plotData.m_datasetsAverage[2].getSeries(0).createCopy(before, now);
            m_datasetsAverage[2].removeAllSeries();
            m_datasetsAverage[2].addSeries(seriesAverage);
            
            
            //for sub2;
            
            int rendererTupleIndex = 0;
            
            if (m_totalStreamVector.size() == 0)
            {
                //m_sub2Datasets.removeAllSeries();
                //Vector oldSub2Series = m_plotData.m_sub2Series;

                //for (Iterator i = oldSub2Series.iterator(); i.hasNext();)
                //{
                //    TimeSeries oldSeries = (TimeSeries) i.next();
                //    TimeSeries newSeries = oldSeries.createCopy(before, now);
                //    String name = oldSeries.getName();
                //    m_sub2Datasets.addSeries(newSeries);
                //    m_sub2Renderer.setSeriesPaint(rendererTupleIndex, m_plotColor.getStreamColor(name));
                //    rendererTupleIndex = rendererTupleIndex + 1;
                //}
            }
            else
            {
                m_sub2Datasets.removeAllSeries();
                Vector oldSub2Series = m_plotData.m_sub2Series;

                for (Iterator i = oldSub2Series.iterator(); i.hasNext();)
                {
                    TimeSeries oldSeries = (TimeSeries) i.next();
                    String oldSeriesName = oldSeries.getName();
                    boolean isInShowVector = false;

                    for (Iterator j = m_totalStreamVector.iterator(); j.hasNext();)
                    {
                        String showStream = (String) j.next();

                        if (oldSeriesName.equals(showStream))
                        {
                            isInShowVector = true;
                        }
                    }
                    
                    if (isInShowVector)
                    {
                        TimeSeries newSeries = oldSeries.createCopy(before, now);
                        String name = oldSeries.getName();
                        m_sub2Datasets.addSeries(newSeries);
                        m_sub2Renderer.setSeriesPaint(rendererTupleIndex, m_plotColor.getStreamColor(name));
                        rendererTupleIndex = rendererTupleIndex + 1;
                    }
                }
            }
            
        }
        catch(Exception exception)
        {
            //exception.printStackTrace();
            DebugTrace.trace("History copy wrong");
        }
    }
    *************************/


    //  Update the statistics of each plot.
    //
    public void actionPerformed(ActionEvent   e)
    {
        DebugTrace.trace("wenjuan: no use");

        /**************
        try
        {
            Date nowDate = new Date();
            Date beforeDate = new Date(nowDate.getTime() - AROUND_TIME);
            
            Millisecond before = new Millisecond(beforeDate);
            Millisecond now = new Millisecond(nowDate);
            
            // for average
            //
            for (int i = 0; i<SUBPLOT_COUNT; i++)
            {
                TimeSeries seriesAverage = m_datasetsAverage[i].getSeries(0);

                seriesAverage.clear();
                seriesAverage = m_plotData.m_datasetsAverage[i].getSeries(0).createCopy(before, now);
                m_datasetsAverage[i].removeAllSeries();
                m_datasetsAverage[i].addSeries(seriesAverage);
            }


            //for sub0;
            m_sub0Datasets.removeAllSeries();
            Vector oldSub0Series = m_plotData.m_sub0Series;
            
            int rendererCpuIndex = 0;
            
            for (Iterator i = oldSub0Series.iterator(); i.hasNext();)
            {
                TimeSeries oldSeries = (TimeSeries) i.next();
                String ipPort = oldSeries.getName();

                if (m_plotData.m_currentCpuIpPort.contains(ipPort))
                {
                    TimeSeries newSeries = oldSeries.createCopy(before, now);
                    m_sub0Datasets.addSeries(newSeries);
                    m_sub0Renderer.setSeriesPaint(rendererCpuIndex, m_plotColor.getNodeColor(ipPort));
                    rendererCpuIndex = rendererCpuIndex + 1;
                }
            }


            //for sub1;
            int rendererLatencyIndex = 0;
            
            if (m_totalStreamVector.size() == 0)
            {
                //m_sub1Datasets.removeAllSeries();
                //Vector oldSub1Series = m_plotData.m_sub1Series;

                //for (Iterator i = oldSub1Series.iterator(); i.hasNext();)
                //{
                //    TimeSeries oldSeries = (TimeSeries) i.next();
                //    TimeSeries newSeries = oldSeries.createCopy(before, now);
                //    String name = oldSeries.getName();
                //    newSeries.setName(" ");
                //    m_sub1Datasets.addSeries(newSeries);
                //    m_sub1Renderer.setSeriesPaint(rendererLatencyIndex, m_plotColor.getStreamColor(name));
                //    rendererLatencyIndex = rendererLatencyIndex + 1;
                //}
            }
            else
            {
                rendererLatencyIndex = 0;
                m_sub1Datasets.removeAllSeries();
                Vector oldSub1Series = m_plotData.m_sub1Series;

                for (Iterator i = oldSub1Series.iterator(); i.hasNext();)
                {
                    TimeSeries oldSeries = (TimeSeries) i.next();
                    String oldSeriesName = oldSeries.getName();
                    boolean isInShowVector = false;

                    for (Iterator j = m_totalStreamVector.iterator(); j.hasNext();)
                    {
                        String showStream = (String) j.next();

                        if (oldSeriesName.equals(showStream))
                        {
                            isInShowVector = true;
                        }
                    }
                    
                    if (isInShowVector)
                    {
                        TimeSeries newSeries = oldSeries.createCopy(before, now);
                        String name = oldSeries.getName();
                        m_sub1Datasets.addSeries(newSeries);
                        m_sub1Renderer.setSeriesPaint(rendererLatencyIndex, m_plotColor.getStreamColor(name));
                        rendererLatencyIndex = rendererLatencyIndex + 1;
                    }
                }
            }


            //for sub2;
            int rendererTupleIndex = 0;
            
            if (m_totalStreamVector.size() == 0)
            {
                //m_sub2Datasets.removeAllSeries();
                //Vector oldSub2Series = m_plotData.m_sub2Series;
                //for (Iterator i = oldSub2Series.iterator(); i.hasNext();)
                //{
                //    TimeSeries oldSeries = (TimeSeries) i.next();
                //    TimeSeries newSeries = oldSeries.createCopy(before, now);
                //    String name = oldSeries.getName();
                //    m_sub2Datasets.addSeries(newSeries);
                //    m_sub2Renderer.setSeriesPaint(rendererTupleIndex, m_plotColor.getStreamColor(name));
                //    rendererTupleIndex = rendererTupleIndex + 1;
                //}
            }
            else
            {   m_sub2Datasets.removeAllSeries();
                Vector oldSub2Series = m_plotData.m_sub2Series;

                for (Iterator i = oldSub2Series.iterator(); i.hasNext();)
                {
                    TimeSeries oldSeries = (TimeSeries) i.next();
                    String oldSeriesName = oldSeries.getName();
                    boolean isInShowVector = false;

                    for (Iterator j = m_totalStreamVector.iterator(); j.hasNext();)
                    {
                        String showStream = (String) j.next();

                        if(oldSeriesName.equals(showStream))
                        {
                            isInShowVector = true;
                        }
                    }
                    
                    if (isInShowVector)
                    {
                        TimeSeries newSeries = oldSeries.createCopy(before, now);
                        String name = oldSeries.getName();
                        m_sub2Datasets.addSeries(newSeries);
                        m_sub2Renderer.setSeriesPaint(rendererTupleIndex, m_plotColor.getStreamColor(name));
                        rendererTupleIndex = rendererTupleIndex + 1;
                        
                    }
                }
            }
        }
        catch(Exception exception)
        {
            DebugTrace.trace("History copy wrong");
        }
********/
    }
}


/////////////////////  end  AllPlotPanel.java  /////////////////////////////////
