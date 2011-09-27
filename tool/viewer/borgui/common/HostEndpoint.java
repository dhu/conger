package borgui.common;

import java.net.InetAddress;
import java.net.UnknownHostException;

import java.io.*;
import java.util.*;

import borgui.xml.XmlConstants;



////////////////////////////////////////////////////////////////////////////////
//
//  Canonical naming for host machines and endpoints.
//  These methods correspond to those in:   borealis/src/modules/utility/Util.cc
//
public class HostEndpoint
{
    ////////////////////////////////////////////////////////////////////////////
    //
    // Determine the dotted IP address of a host machine.
    //
    public static String  getHostAddress(// Name of the host machine.
                                         // Empty or null for the local host.
                                         //
                                         String   host )
    {
        String     hostname = host;
    //
    // The default port is a static assignment for now.
    // It could be assigned by scanning available channels???
    //..........................................................................


        if ((host.equals("127.0.0.1"))  ||  (host.equals(""))  ||
            (host.equals("localhost"))  ||  (host == null))
        {
            try
            {   InetAddress localhost = InetAddress.getLocalHost();
                hostname = localhost.getHostAddress();
            }
            catch (UnknownHostException  e)
            {   hostname = "127.0.0.1";    //       Default IP.
                DebugTrace.trace("host " + host + ": " + e.getMessage());
            }
        }
        else
        {   try
            {   //DebugTrace.trace("==>" + host + "<== " + host.length());
                InetAddress  ip = InetAddress.getByName(host);
                hostname = ip.getHostAddress();
            }
            catch (UnknownHostException  e)
            {   hostname = "127.0.0.1";    //       Default IP.
                DebugTrace.trace("host " + host + ": " + e.getMessage());
            }
        }

        //DebugTrace.trace("-->" + host + "--> " + hostname);
        return  hostname;
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    // Compose a complete cononical endpoint:  <dotted ip address>:<port>
    //
    public static String  formEndpoint(// An endpoint as:  [<host>][:[<port>]]
                                       //
                                       String   endpoint,

                                       // A default port number.
                                       //
                                       int      port)
    {
        String   form;
        int      index;
    //
    // The default port is a static assignment for now.
    // It could be assigned by scanning available channels???
    //..........................................................................


        if (endpoint == null)
        {   form  = "";
            index = -1;                          // No port.
        }
        else
        {   form  = endpoint;
            index = endpoint.indexOf(':');       // Check for an optional port.
        }

        if (index != -1)
        {   form = form.substring(0, index);     // Trim off any colon/port.
        }

        form = getHostAddress(form);             // Get the dotted host address.

        if ((index == -1)  ||                    // IF no colon or no port,
            (index == endpoint.length() - 1))
        {
            form += ":" + port;                  // Add the default port number.
        }
        else
        {   form += endpoint.substring(index);   // Use the given port number.
        }

        //DebugTrace.trace("Complete address is:  " + form);
        return  form;
    }


    
    ////////////////////////////////////////////////////////////////////////////
    //
    // Extract the host from an endpoint.
    //
    public static String  getHost(// An endpoint as:  [<host>][:[<port>]]
                                  //
                                  String   endpoint)
    {
        String   host;
        int      index;
    //
    //..........................................................................


        if (endpoint == null)
        {   host  = "";
            index = -1;                          // No port.
        }
        else
        {   host  = endpoint;
            index = endpoint.indexOf(':');       // Check for an optional port.
        }

        if (index != -1)
        {   host = host.substring(0, index);     // Trim off any colon/port.
        }

        host = getHostAddress(host);             // Get the dotted host address.

        //DebugTrace.trace("The host is:  " + host);
        return  host;
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    // Extract the port from an endpoint.  Emty string if no port.
    //
    public static String  getPort(// An endpoint as:  [<host>][:[<port>]]
                                  //
                                  String   endpoint)
    {
        String   port = "";
        int      index;
    //
    //..........................................................................


        if (endpoint != null)
        {   index = endpoint.indexOf(':');       // Check for an optional port.

            if (index != -1)
            {   port = endpoint.substring(index + 1);   // Trim off any host/colon.
            }
        }

        // DebugTrace.trace("The port is:  " + port);
        return  port;
    }


    ////////////////////////////////////////////////////////////////////////////
    //
    //  Derive a display name of an endpoint or a host with no port.
    //
    public static String displayEndpoint(// An endpoint as:  [<host>][:[<port>]]
                                         //
                                         String   host)
    {
        String  name;
        String  local;
        String  port;

        int     index;
    //..........................................................................


        name = getHost(host);

        try
        {   name  = InetAddress.getByName(name).getCanonicalHostName();
            local = InetAddress.getLocalHost().getCanonicalHostName();
            index = local.indexOf('.');
            //DebugTrace.trace("name = " + name + "  local = " + local);

            if (index != -1)
            {   local = local.substring(index + 1);

                if (name.endsWith(local.substring(index + 1)))
                {
                    index = name.indexOf('.');

                    if (index != -1)
                    {   name = name.substring(0, index);
                    }
                }
            }
        }
        catch (UnknownHostException e)
        {   DebugTrace.trace(name + ": " + e.getMessage());
        }

        port = getPort(host);

        if (!port.equals(""))
        {   name = name + ":" + port;
        }

        //DebugTrace.trace("display name = " + name);
        return  name;
    }
    


    ////////////////////////////////////////////////////////////////////////////
    //
    // Launch an external program.
    //
    public static Process launchProgram(// Program path on remote machine.
                                        String   program,

                                        // An endpoint as:  [<host>][:[<port>]]
                                        String   host)
            throws Exception
    {
        String   command;
        Process  process;
    //
    //..........................................................................


        command = program;

        if (!HostEndpoint.getHost("").equals(host))
        {   String  lib = XmlConstants.BOREALIS_SOFTWARE + "bdb/lib:"
                        + XmlConstants.BOREALIS_SOFTWARE + "nmstl/lib:"
                        + XmlConstants.BOREALIS_SOFTWARE + "xerces/lib";

            command = "\"setenv LD_LIBRARY_PATH " + lib + ";" + command + "\"";
            command = "ssh " + host + " " + command;
        }

        DebugTrace.trace(command);

        process = Runtime.getRuntime().exec(command);
        DebugTrace.trace("launched " + program + " on " + host);

        /********
        catch (SecurityException e)
        {   DebugTrace.trace("Security error:  " + e.getMessage());
        }
        catch (java.io.IOException e)
        {   DebugTrace.trace("IO error:  " + e.getMessage());
        }
        catch (NullPointerException e)
        {   DebugTrace.trace("No argument:  " + e.getMessage());
        }
        catch (IllegalArgumentException e)
        {   DebugTrace.trace("Empty argument:  " + e.getMessage());
        }
        ***********/

        return  process;
    }

    /*******************
    class StreamGobbler extends Thread
    {
        InputStream is;
        String type;
        OutputStream os;
    
        StreamGobbler(InputStream is, String type)
        {
            this(is, type, null);
        }


        StreamGobbler(InputStream is, String type, OutputStream redirect)
        {
            this.is = is;
            this.type = type;
            this.os = redirect;
        }

    
        public void run()
        {
            try
            {
                PrintWriter pw = null;

                if (os != null)
                {   pw = new PrintWriter(os);
                }
                
                InputStreamReader isr = new InputStreamReader(is);
                BufferedReader br = new BufferedReader(isr);
                String line=null;

                while ( (line = br.readLine()) != null)
                {
                    if (pw != null)
                    {   pw.println(line);
                    }

                    DebugTrace.trace(type + ">" + line);    
                }
                if (pw != null)
                {    pw.flush();
                }
            }
            catch (IOException ioe)
            {
                ioe.printStackTrace();  
            }
        }
    }
    *****************/
}


/////////////////////////  end  HostEndpoint.java  /////////////////////////////
