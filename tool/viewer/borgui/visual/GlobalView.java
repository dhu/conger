package borgui.visual;

import java.util.*;
import java.lang.*;
import java.io.*;

import javax.swing.*;

import medusaXmlRpc.*;

import borgui.common.HostEndpoint;
import borgui.common.DebugTrace;
import borgui.common.SwingWorker;
import borgui.xml.XmlConstants;



////////////////////////////////////////////////////////////////////////////////
//
//  Connect to an active Borealis system, display the network diagram, monitor
//  it for changes to the diagram and update the display.
//
public class GlobalView
{
    protected Process    m_process;

    protected String     m_head_point = "";
    protected String     m_view_point = "";

              String     m_xml = "";

    public GlobalView() {}



    ////////////////////////////////////////////////////////////////////////////
    //
    //  Launch the GlobalView program on the same node as the Head.
    //  GlobalView get the Global catalog updated from the Head.
    //  It is polled by the view to get a new catalog when it changes.
    //
    public void  launchView()
    {
        String  program = "GlobalView";
        String  command;
        String  view_host;
    //
    //..........................................................................


        // The default host for the Head is the localhost.
        // At most this is what the user designates to connect to a system.
        // The GlobalView program normally runs on the same node as the Head.
        // If the Head does not run on the same node as the StreamViewer
        // then it needs to be designated.
        // The StreamViewer checks to see that a Head is running by???
        //
        // The default port for the head is the head port.
        // The default host for the viewer is the same as the head.
        // The default port for the viewer is the global port.
        // launch GV (uses defaults)
        // if remote host = this host -> use exec; else use ssh.
        // BOREALIS_SOFTWARE=/pro/borealis/software
        // ssh 128.148.37.27 "setenv LD_LIBRARY_PATH /pro/borealis/software/bdb/lib:/pro/borealis/software/nmstl/lib:/pro/borealis/software/xerces/lib; /u/bb/borealis/utility/client/global/GlobalView &"
        //
        if (m_head_point.equals(""))
        {   view_host = HostEndpoint.getHost(m_view_point);
            setHeadPoint(view_host);
        }
        else if (m_view_point.equals(""))
        {   view_host = HostEndpoint.getHost(m_head_point);
            setViewPoint(view_host);
        }
        else
        {   view_host = HostEndpoint.getHost(m_view_point);
        }

        try
        {   m_process = HostEndpoint.launchProgram(program, view_host);
        }
        catch (Exception  e)
        {
            DebugTrace.trace(e.getMessage());

            JOptionPane.showConfirmDialog(null,
                            "The GlobalView program could not be launched.\n"
                                + e.getMessage(),
                            "No GlobalView program",
                            JOptionPane.CLOSED_OPTION,
                            JOptionPane.ERROR_MESSAGE);
        }

        return;
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    public void  killView()
    {
        String  view_host;
        String  command;
    //
    //  NYI:  Use an RPC call so GlobalView can notify the head.
    //..........................................................................


        // Kill the process that launched the viewer.
        //
        m_process.destroy();

        view_host = HostEndpoint.getHost(m_view_point);

        try
        {   command = "\"skill -9 -c lt-GlobalView;skill -9 -c GlobalView\"";

            if (!HostEndpoint.getHost("").equals(m_view_point))
            {   command = "ssh " + view_host + " " + command;
            }

            Process process = Runtime.getRuntime().exec(command);
            int status = process.waitFor();
            DebugTrace.trace("exit = " + status);
        }
        catch (java.io.IOException  e)
        {
            DebugTrace.trace(e.getMessage());

            JOptionPane.showConfirmDialog(null,
                            "The GlobalView program could not be killed.\n"
                                + e.getMessage(),
                            "Immortal GlobalView program",
                            JOptionPane.CLOSED_OPTION,
                            JOptionPane.ERROR_MESSAGE);
        }
        catch (java.lang.InterruptedException  exception)
        {
            DebugTrace.trace(exception.getMessage());
        }

        DebugTrace.trace("killed GlobalView on " + m_view_point);

        return;
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    //  Retrieve the catalog as XML from the GlobalView program.
    //
    public String  loadCatalog()
    {
        String   xml    = "";
        String   method = "GlobalServer.load_regional_catalog";
    //
    // Entry:  The GlobalView program must have been launched.
    //..........................................................................


        ServerNode  serverNode = new ServerNode(HostEndpoint.getHost(m_view_point),
                                                HostEndpoint.getPort(m_view_point));

        try
        {   // serverRPC example:  http://128.148.37.27:55001/RPC2
            //
            xml = (String)MedusaXmlRpc.invoke(serverNode.serverRPC(),
                                              method,
                                              new Vector());

            if (!xml.equals(""))  DebugTrace.trace("xml=" + xml);
        }
        catch(Exception   exception)
        {
            DebugTrace.trace("ERROR: invoke " + method);
            DebugTrace.trace(exception.getMessage());
        }

        return  xml;
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    // The background task can provide intermediate results by invoking
    // SwingWorker.publish, causing SwingWorker.process to be invoked from
    // the event dispatch thread.
    //
    // The background task can define bound properties. Changes to these
    // properties trigger events, causing event-handling methods to be
    // invoked on the event dispatch thread. 
    //
    // final SwingWorker worker = new SwingWorker()
    // {  methods
    // };
    // worker.start();
    //
    // When you create the timer, you specify an action listener to be
    // notified when the timer "goes off". The actionPerformed method in
    // this listener should contain the code for whatever task you need
    // to be performed. When you create the timer, you also specify the
    // number of milliseconds between timer firings. If you want the timer
    // to go off only once, you can invoke setRepeats(false) on the timer.
    // To start the timer, call its start method. To suspend it, call stop. 
    //
    // Note that the Swing timer's task is performed in the event dispatch
    // thread. This means that the task can safely manipulate components,
    // but it also means that the task should execute quickly.
    //
    // timer = new Timer(speed, this);
    // timer.setInitialDelay(pause);
    // timer.start(); 
    //
    // m_timerAllPlot = new javax.swing.Timer(1000, new ActionListener()
    //     {
    //         public void actionPerformed(ActionEvent evt)
    //         {   m_statplotUpdater.updateStats();
    //         }
    //     };
    //  );
    //
    // Run periodically to contact the GlovalVIew program and see if the
    // catalog has been updated.  If so redraw the graph.
    // A special case is if a move happens and only the colors are changed.
    //
    public void updateView()
    {
        final SwingWorker worker = new SwingWorker()
        {
            public Object construct()
            {
                synchronized(m_xml)
                {
                    m_xml = loadCatalog();

                    DebugTrace.trace("xml = " + m_xml);
                }

                return null;
            }

            // Runs on the event-dispatching thread.
            // Safely update GUI here
            //
            public void finished()
            {
                //synchronized(m_rpcHandlerCpu)
                //{
                //        s.updateCpuStats(m_parseStatsCpu);
                //    }
                //}
            }
        };

        worker.start();
    } 



    ////////////////////////////////////////////////////////////////////////////
    //
    public void  setViewPoint(String   endpoint)
    {
    //..........................................................................


        m_view_point = HostEndpoint.formEndpoint(endpoint,
                                    XmlConstants.DEFAULT_GLOBAL_PORT);

        return;
    }


    ////////////////////////////////////////////////////////////////////////////
    //
    public void  setHeadPoint(String   endpoint)
    {
    //..........................................................................


        m_head_point = HostEndpoint.formEndpoint(endpoint,
                                    XmlConstants.DEFAULT_HEAD_PORT);

        return;
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    public String  getViewPoint()
    {
    //..........................................................................


        return  m_view_point;
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    public String  getHeadPoint()
    {
    //..........................................................................


        return  m_head_point;
    }
}


///////////////////////////  end  GlobalView.java  /////////////////////////////
