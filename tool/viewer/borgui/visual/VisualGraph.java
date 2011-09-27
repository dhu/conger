/*
 * VisualGraph.java
 *
 * Created on April 25, 2005, 3:15 PM
 */

package borgui.visual;


import java.util.*;
import javax.swing.*;
import java.awt.*;
import java.awt.image.*;
import java.awt.geom.Rectangle2D;
import java.util.*;
import java.awt.event.*;
import java.io.File;
import java.io.FileReader;
import java.io.BufferedReader;

import borgui.common.*;
import borgui.visual.*;
import borgui.graph.*;
import borgui.editor.*;
import borgui.xml.*;

import org.jgraph.graph.*;
import org.jgraph.event.GraphSelectionListener;
import org.jgraph.event.GraphSelectionEvent;


/**
 * HACK CLASS for demo
 * do not use
 *
 */
public class VisualGraph extends BorealisGraph implements Controllable
{
    protected VisualPanel             m_visualPanel;
    protected ControllerPanel         m_controllerPanel;
    protected HashMap<String, Color>  m_nodes = new HashMap<String, Color>();

    /************
    protected int                     m_lastFilenum = 0;
    protected int                     m_curFilenum = 0;
    protected final static String     FILE_PREFIX = "/tmp/catalog";
    protected final static String     FILE_SUFFIX = ".xml";
    
    // not efficent, but this is  only a temporary class
    protected Vector<Long>            m_modtimes = new Vector<Long>();
    protected Vector<Integer>         m_filenums = new Vector<Integer>();
    
    protected javax.swing.Timer       m_fileTimer;
    ******************/

    /** Creates a new instance of VisualGraph */
    public VisualGraph(VisualPanel visualPanel, ControllerPanel controllerPanel)
    {
        m_visualPanel = visualPanel;
        m_controllerPanel = controllerPanel;
        setEditable(false);

        /*******************
        m_fileTimer = new javax.swing.Timer(5000, new ActionListener()
            {
                public void actionPerformed(ActionEvent evt)
                {
                    updateFiles();
                }
            }
        );

        updateFiles();
        m_fileTimer.start();
        *********************/
    }
    
    
    public void showTime(Date time)
    {
        /***************
        try
        {   updateFiles();
            int filenum = getFilenum(time);
            
            if (filenum >= 1 && filenum != m_curFilenum)
            {
                String filename = getFilename(filenum);
                m_visualPanel.reload(filename);
                changeNodeColor();

                //wenjuan
                //updateNodes();
                //      System.out.println("SHOW TIME : " + time.getTime() +
                //                 " FILENAME  : " + filename);

                m_curFilenum = filenum;
            }
        }
        catch(Exception e)
        {
            System.out.println(e.getMessage());
        }
        ***************/
    }


    /*************************
    //wenjuan no use
    public void addNode(String node, Color color)
    {
        m_nodes.put(node, color);
    }
    
    protected synchronized void updateFiles()
    {
        //  final SwingWorker worker = new SwingWorker() {
        
        //  public Object construct() {
        for(m_lastFilenum++ ; true ; m_lastFilenum++)
        {
            File file = new File(getFilename(m_lastFilenum));
            
            if (!file.exists())
            {
                m_lastFilenum--;
                break;
            }
            else
            {
                boolean contentDiffers = false;
                File prevfile = new File(getFilename(m_lastFilenum - 1));
                
                // quick check against prev file
                if (file.length() != prevfile.length())
                {
                    contentDiffers = true;
                }
                else
                {
                    // longer check
                    try
                    {
                        FileReader fr = new FileReader(file);
                        FileReader prevfr = new FileReader(prevfile);
                        BufferedReader br = new BufferedReader(fr);
                        BufferedReader prevbr = new BufferedReader(prevfr);
                        
                        try
                        {
                            String line = br.readLine();
                            String prevline = prevbr.readLine();

                            for(; line != null && prevline != null && !contentDiffers; )
                            {
                                if (!line.equals(prevline))
                                {
                                    contentDiffers = true;
                                }
                                else
                                {
                                    line = br.readLine();
                                    prevline = prevbr.readLine();
                                }
                            }

                            if (line != null || prevline != null)
                            {
                                contentDiffers = true;
                            }
                        }
                        catch (java.io.IOException e)
                        {
                            System.out.println(e.getMessage());
                        }
                        
                    }
                    catch(java.io.FileNotFoundException e)
                    {
                        System.out.println(e.getMessage());
                    }
                }

                // add to list of files if diff
                if (contentDiffers)
                {
                    //System.out.println("CONTENT DIFFERS: " + getFilename(m_lastFilenum));
                    m_modtimes.add(file.lastModified());
                    m_filenums.add(m_lastFilenum);

                    // if first file
                    if (m_lastFilenum == 1)
                    {
                        m_controllerPanel.setStartTime(new Date(file.lastModified()));
                    }

                    m_controllerPanel.addMark(new Date(file.lastModified()));
                }
                else
                {
                    //System.out.println("CONTENT DOES NOT DIFFERS: " + getFilename(m_lastFilenum));
                }
            }
        }
//        return null;
//  }
        
//  };
//  worker.start();
        
        
    }
    
    protected synchronized int getFilenum(Date time)
    {
        Integer  filenum = -1;
        Long     target = time.getTime();
        Integer  num = 0;

        for(Iterator<Long> i = m_modtimes.iterator(); i.hasNext(); num++)
        {
            Long  mod = i.next();

            if ((mod >= target) || (!i.hasNext()))
            {
                filenum = (m_filenums.get(num)).intValue();
                break;
            }
        }
        
        return filenum;
    }

    
    protected String getFilename(int num)
    {
        return FILE_PREFIX + Integer.toString(num) + FILE_SUFFIX;
    }
    **********************/


    ////////////////////////////////////////////////////////////////////////////
    //
    // Color graph cells by node with no additional nodes.
    //
    public void changeNodeColor()
    {
        BorealisDocument  document;
        PlotColor         plotColor;
    //..........................................................................


        document  = m_visualPanel.getModel().getDocument();
        plotColor = new PlotColor(document.getAllDistinctIpPortsString());

        changeNodeColor(plotColor);
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    // Color graph cells by node with an initial set of additional nodes.
    //
    public void changeNodeColor(// Node colors.
                                //
                                PlotColor plotColor)
    {
        String    node;
        Color     nodeColor;

        BorealisModel       model;
        BorealisGraphModel  graphModel;
    //
    // A box may be moved out of a region and leave an empty node.
    // The empty node will not be in the catalog.
    //..........................................................................


        Object[] allCells = getRoots();
        //DebugTrace.trace(allCells.length + " cells.");

        model      = m_visualPanel.getModel();
        graphModel = model.getGraphModel();

        for(int i = 0; i < allCells.length; i++)
        {
            if (allCells[i] instanceof HasNodeCell)
            {
                HasNodeElement element = (HasNodeElement)
                          model.getElementMap().get((HasNodeCell)allCells[i]);

                node = element.getNodeAttribute();
                
                if (node != null)
                {   node = HostEndpoint.formEndpoint(node,
                                        XmlConstants.DEFAULT_BOREALIS_PORT);

                    DebugTrace.trace("color node @ " + node);

                    nodeColor = plotColor.getNodeColor(node);
                    ((HasNodeCell)allCells[i]).changeNodeColor(nodeColor);
                    graphModel.flushBuffer();
                }
            }
            else if (allCells[i] instanceof OutputGraphCell)
            {   OutputElement  element = (OutputElement)
                                            model.getElementMap().get(
                                                (OutputGraphCell)allCells[i]);

                node = element.getNodeAttribute();

                if (node != null)
                {   node = HostEndpoint.formEndpoint(node,
                                        XmlConstants.DEFAULT_BOREALIS_PORT);
                
                    DebugTrace.trace("color output @ " + node);
                    nodeColor = plotColor.getNodeColor(node);

                    ((OutputGraphCell)allCells[i]).changeNodeColor(nodeColor);
                    graphModel.flushBuffer();
                }
            }
        }
    }


    /***********************
    protected void updateNodes()
    {
        BorealisModel model = m_visualPanel.getModel();
        BorealisGraphModel graphModel = model.getGraphModel();
        BorealisDocument doc = model.getDocument();
        java.util.List boxes = doc.getAllBoxes();
     
        Vector toinsert = new Vector();
        //  Vector toupdate = new Vector();
        Map attrmap = new Hashtable();
     
        for (Iterator i = boxes.iterator(); i.hasNext(); )
        {
            BoxElement elem = (BoxElement) i.next();
            String node = elem.getNodeAttribute();

            if (node != null)
            {
                Object obj = m_nodes.get(node);

                if (obj != null)
                {
                    BorealisGraphCell cell = (BorealisGraphCell) model.getCell(elem);
     
                    AttributeMap map = cell.getAttributes();
                    GraphConstants.setBackground(map, (Color) obj);
     
                    graphModel.flushBuffer();
                }
                else
                {   ;//System.out.println("NO Node color for : " + node);
                }
            }
            else
            { ;//System.out.println("NO Node for box : " + elem.getNameVal());
            }
        }
     
        // insert nodes
        //graphModel.insert(toinsert.toArray(), attrmap, null, null, null);
        // send to back
        //graphModel.toBack(toinsert.toArray());
        // update
//  graphModel.cellsChanged(toupdate.toArray());
     
    }
     */
}


///////////////////////////  end VisualGraph.java  /////////////////////////////
