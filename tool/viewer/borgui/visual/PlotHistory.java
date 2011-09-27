/*
 * PlotHistory.java
 *
 * Created on June 6, 2005, 4:49 PM
 *
 * To change this template, choose Tools | Options and locate the template under
 * the Source Creation and Management node. Right-click the template and choose
 * Open. You can then make changes to the template in the Source Editor.
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
public class PlotHistory extends JPanel implements Controllable{
    
    
    //SUBPLOT_COUNT is the number of the subplots.
    public static final int SUBPLOT_COUNT = 3;
    public static final int BEFORE_TIME = 80000;
    
    //varables for the whole plot.
    private CombinedDomainXYPlot m_plot;
    private TimeSeriesCollection[] m_datasetsAverage = new TimeSeriesCollection[SUBPLOT_COUNT];
    private double[] m_lastValueAverage = new double[SUBPLOT_COUNT];
    
    //TimeSeriesCollection for every subplot
    protected TimeSeriesCollection m_sub0Datasets = new TimeSeriesCollection();
    protected TimeSeriesCollection m_sub1Datasets = new TimeSeriesCollection();
    protected TimeSeriesCollection m_sub2Datasets = new TimeSeriesCollection();
    
    //Render for every subplot
    protected XYItemRenderer m_sub0Renderer;
    protected XYItemRenderer m_sub1Renderer;
    protected XYItemRenderer m_sub2Renderer;
    
    //Color for subplot
    protected PlotColor m_plotColor;
    
    //for History
    protected PlotData m_plotData; //= new PlotData();
    
    protected Vector m_totalStreamVector = new Vector();
    
    
    
    
    /** Creates a new instance of PlotHistory */
    public PlotHistory(PlotData plotData) {
        super(new BorderLayout());
        
        m_plotData = plotData;
        
        //initial the whole Plot:
        m_plot = new CombinedDomainXYPlot( new DateAxis("") );
        
        
        //initial sub0Plot : cpu
        this.m_lastValueAverage[0] = 100.0;
        TimeSeries series0 = new TimeSeries( "CPU avg.", Millisecond.class );
        this.m_datasetsAverage[0] = new TimeSeriesCollection(series0);
        
        NumberAxis rangeAxis0 = new NumberAxis("CPU ( % )");
       // rangeAxis0.setAutoRangeIncludesZero(true);
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
        
	//wenjuan: add latency
        m_plot.add(m_sub1Plot);
        
        
        
        //initial sub2Plot : tuple delivery
        this.m_lastValueAverage[2] = 100.0;
        TimeSeries series2 = new TimeSeries( "Tuple avg.", Millisecond.class );
        this.m_datasetsAverage[2] = new TimeSeriesCollection(series2);
        
        NumberAxis rangeAxis2 = new NumberAxis("Tuple ( % )");
        //rangeAxis2.setAutoRangeIncludesZero(true);
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
    
    public void setPlotColor(PlotColor plotColor) {
        m_plotColor = plotColor;
    }
    
    
    
    public void refreshStreamVector(Vector totalStreamVector) {
        m_totalStreamVector = totalStreamVector;
    }
    
    
    
    public void showTime(java.util.Date time) {
        
        try {
            Date givenTime = time;
            Date beforeDate = new Date(givenTime.getTime() - BEFORE_TIME);
            
            
            Millisecond before = new Millisecond(beforeDate);
            Millisecond now = new Millisecond(givenTime);
            
            //for average
            for (int i = 0; i<SUBPLOT_COUNT; i++) {
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
            
            for (Iterator i = oldSub0Series.iterator(); i.hasNext();) {
                TimeSeries oldSeries = (TimeSeries) i.next();
                TimeSeries newSeries = oldSeries.createCopy(before, now);
                String ipPort = oldSeries.getName();
                m_sub0Datasets.addSeries(newSeries);
                m_sub0Renderer.setSeriesPaint(rendererCpuIndex,  m_plotColor.getNodeColor(ipPort));
                rendererCpuIndex = rendererCpuIndex + 1;
                
            }
            
            
            //for sub1;
            int rendererLatencyIndex = 0;
            
            if(m_totalStreamVector.size() == 0) {
               /*
                m_sub1Datasets.removeAllSeries();
                Vector oldSub1Series = m_plotData.m_sub1Series;
                for (Iterator i = oldSub1Series.iterator(); i.hasNext();) {
                    TimeSeries oldSeries = (TimeSeries) i.next();
                    TimeSeries newSeries = oldSeries.createCopy(before, after);
                    String name = oldSeries.getName();
                    newSeries.setName(" ");
                    m_sub1Datasets.addSeries(newSeries);
                    m_sub1Renderer.setSeriesPaint(rendererLatencyIndex, m_plotColor.getStreamColor(name));
                    rendererLatencyIndex = rendererLatencyIndex + 1;
                }
                */
            }
            
            else {
                m_sub1Datasets.removeAllSeries();
                Vector oldSub1Series = m_plotData.m_sub1Series;
                for (Iterator i = oldSub1Series.iterator(); i.hasNext();) {
                    TimeSeries oldSeries = (TimeSeries) i.next();
                    String oldSeriesName = oldSeries.getName();
                    boolean isInShowVector = false;
                    for(Iterator j = m_totalStreamVector.iterator(); j.hasNext();) {
                        String showStream = (String) j.next();
                        if(oldSeriesName.equals(showStream)) {
                            isInShowVector = true;
                        }
                    }
                    
                    if(isInShowVector) {
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
            
            if(m_totalStreamVector.size() == 0) {
                /*
                m_sub2Datasets.removeAllSeries();
                Vector oldSub2Series = m_plotData.m_sub2Series;
                for (Iterator i = oldSub2Series.iterator(); i.hasNext();) {
                    TimeSeries oldSeries = (TimeSeries) i.next();
                    String name = oldSeries.getName();
                    TimeSeries newSeries = oldSeries.createCopy(before, now);
                    m_sub2Datasets.addSeries(newSeries);
                    m_sub2Renderer.setSeriesPaint(rendererTupleIndex, m_plotColor.getStreamColor(name)));
                    rendererTupleIndex = rendererTupleIndex + 1;
                 
                }
                 */
            } else {
                m_sub2Datasets.removeAllSeries();
                Vector oldSub2Series = m_plotData.m_sub2Series;
                for (Iterator i = oldSub2Series.iterator(); i.hasNext();) {
                    TimeSeries oldSeries = (TimeSeries) i.next();
                    
                    String oldSeriesName = oldSeries.getName();
                    boolean isInShowVector = false;
                    for(Iterator j = m_totalStreamVector.iterator(); j.hasNext();) {
                        String showStream = (String) j.next();
                        if(oldSeriesName.equals(showStream)) {
                            isInShowVector = true;
                        }
                    }
                    
                    if(isInShowVector) {
                        TimeSeries newSeries = oldSeries.createCopy(before, now);
                        String name = oldSeries.getName();
                        m_sub2Datasets.addSeries(newSeries);
                        m_sub2Renderer.setSeriesPaint(rendererTupleIndex, m_plotColor.getStreamColor(name));
                        rendererTupleIndex = rendererTupleIndex + 1;
                        
                    }
                }
            }
            
            
        } catch(Exception e) {
            //e.printStackTrace();
            System.out.println("History copy wrong");
        }
    }
    
    
}




