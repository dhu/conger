/*
 * PlotColor.java
 *
 * Created on June 7, 2005, 8:10 PM
 *
 * To change this template, choose Tools | Options and locate the template under
 * the Source Creation and Management node. Right-click the template and choose
 * Open. You can then make changes to the template in the Source Editor.
 */

package borgui.visual;

import java.util.Vector;
import java.awt.Color;

import borgui.common.DebugTrace;


/**
 *
 *
 * @author Wenjuan Xing (vivian@cs.brown.edu)
 * version 1.0 10/13/2005
 *
 */
public class PlotColor
{
    private Vector<Color>     m_color = new Vector<Color>();

    protected Vector<String>  m_nodesIpPort = new Vector<String>();
    protected Vector<String>  m_streamNames = new Vector<String>();

    /** Creates a new instance of PlotColor */

    public PlotColor(Vector<String>  nodesIpPort)
    {
        m_nodesIpPort = nodesIpPort;

        m_color.add( 0, new java.awt.Color(255,   0,   0));
        m_color.add( 1, new java.awt.Color(102, 255,   0));
        m_color.add( 2, new java.awt.Color(255, 204,   0));
        m_color.add( 3, new java.awt.Color(255,   0, 255));
        m_color.add( 4, new java.awt.Color(204, 255, 102));
        m_color.add( 5, new java.awt.Color( 51, 204, 255));
        m_color.add( 6, new java.awt.Color(255, 255,   0));
        m_color.add( 7, new java.awt.Color(102, 204,  51));
        m_color.add( 8, new java.awt.Color(160, 255, 255));
        m_color.add( 9, new java.awt.Color(102,   0, 160));
        m_color.add(10, new java.awt.Color(102, 160,   0));
        m_color.add(11, new java.awt.Color(160,   0, 255));
        m_color.add(12, new java.awt.Color(204,   0, 204));
        m_color.add(13, new java.awt.Color(  0, 102,   0));
        m_color.add(14, new java.awt.Color(  0,   0, 255));
    }


    public Color getNodeColor(String nodeIpPort)
    {
        int index = m_nodesIpPort.indexOf(nodeIpPort);

        if (index < 0)
        {   DebugTrace.trace("warning: IpPort " + nodeIpPort
                           + " is not in total IpPort Vector.");

            return  Color.GRAY;
        }

        // DebugTrace.trace("index = " + index + "  size = " + m_color.size());
        return (Color)m_color.get(index % m_color.size());
    }


    public void refreshShowStreamVector(Vector<String>  showStreamNameVector)
    {
        m_streamNames = showStreamNameVector;
    }


    public Color getStreamColor(String streamName)
    {
        int index = m_streamNames.indexOf(streamName);

        if (index < 0)
        {   DebugTrace.trace("warning: streamName " + streamName
                           + " is not in total streamName Vector.");
        }
        else
        {  return (Color)m_color.get(index % m_color.size());
        }

        return Color.BLACK;
    }
}

//////////////////////////  end PlotColor.java  ////////////////////////////////
