#include <NMSTL/tcprpc>
#include <NMSTL/text>
#include <NMSTL/thread>

#include  "common.h"
#include  "HeadServer.h"


////////////////////////////////////////////////////////////////////////////////
//
//  The Head deploys a Borealis network to a set of Borealis nodes.
//  It maintains a global catalog to check the consistency of the network.
//  Application Xml is submitted to the Head for deployment.
//
//  The Head can be run with either static or dynamic networks.
//  When launching a static network the Head reads XML files defining the
//  network, deploys the network and terminates.  The XML file paths are
//  given as command line arguments to the Head.
//
//  To launch a dynamic network the Head is run with no files listed on the
//  command.  It continuously waits for applications to submit XML with RPC
//  calls.  The call deployXmlString transmits the XML as a string to the
//  Head.  XML can be passed in files with the deploy_xml_file call.  See
//  the HeadServer and HeadClient classes for additional details.
//..............................................................................


using namespace Borealis;

    IOEventLoop     loop;        // Event loop to feed XML to the Head.
    HeadServer      server;      // RPC calls to process received XML.


/****************
////////////////////////////////////////////////////////////////////////////////
//
//  Set up, but do not run the Head server.
//
Status  open_server(int    port)
{
    Status       status = true;
    RPCBindings  bindings;
//..............................................................................


    DEBUG  << "port = " << port;
    bindings.bind(server, "BigGiantHead");
    TCPRPCAcceptor acceptor(loop, port, bindings);

    if (!acceptor)
    {   status = "Unable to accept TCP/RPC connections: "
               + to_string(acceptor.stat());

        ERROR << status;
    }
    else
    {   NOTICE << "Accepting BigGiantHead TCP/RPC connections on "
               <<  acceptor.get_socket().getsockname();

        server.set_head(port);
    }

    return  status;
}
****************/

////////////////////////////////////////////////////////////////////////////////
//
//  Set up and run the Head server.
//
Status  run_server(int    port)
{
    Status       status = true;
    RPCBindings  bindings;
//..............................................................................


    DEBUG  << "port = " << port;
    bindings.bind(server, "BigGiantHead");
    TCPRPCAcceptor acceptor(loop, port, bindings);

    if (!acceptor)
    {   status = "Unable to accept TCP/RPC connections: "
               + to_string(acceptor.stat());

        ERROR << status;
    }
    else
    {   NOTICE << "Accepting BigGiantHead TCP/RPC connections on "
               <<  acceptor.get_socket().getsockname();

        server.set_head(port);
        loop.run();
        NOTICE << "terminating...";
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Run deployment in a thread.
//
void  deploy_diagram(void  *dummy)
{
    Status          status = true;
//
//   When there is a persistent Head it need to be running before deployment.
//   Deployment sends out an XML Head element to each node; which establishes
//   a connection to the Head server.  It must be running to connect.
//..............................................................................


    //Thread::sleep(Time::msecs(3000));       // Allow the Head server to start up.
    DeployDiagram  &diagram = server.get_global_catalog();

    if (diagram.get_head().empty())
    {   status = "The Head server is not yet running.";
        DEBUG << status;
        // This should wait until it is running???
    }
    else
    {   DEBUG << "Head set to " << diagram.get_head();
        status = diagram.deploy();
    }

    if (!status)
    {   ERROR << "Deployment failed:  " << status;
        loop.terminate();
    }
    
    return;
}



/////////////////////////////  BigGiantHead.cc  ////////////////////////////////
//
//  Front-end for Borealis.
//
int main(int argc, const char *argv[])
{
    string   xml_file;            // Borealis network definition file.

    int      result = 0;
    int      port   = DEFAULT_HEAD_PORT;
    Status   status = true;

    Boolean  deployed = False;    // At least one diagram has been deployed.
    Boolean  initial  = False;    // At lead one XML file was parsed.
    Boolean  persist  = False;    // The Head is persistent.

    pthread_t  deploy_thread;
//
//  Launch order:  Borealis nodes, BigGiantHead, application
//
//  BigGiantHead  [-l]  [-p]  [<file>.xml  ...]
//
//    If no files are given then the Head is run as a persistent server.
//    This mode is used to modify the query network as the system is running.
//    An application connects to the Head using the HeadClient class.
//    RPC calls are used to pass XML strings or files via the HeadServer class.
//    The persistent Head always uses the port specified by DEFAULT_HEAD_PORT.
//
//    Any files listed are used to deploy queries to Borealis nodes.
//    After deployment the Head terminates and queries can not be modified.
//    If no deployment file is given, default deployments will be used.
//
//      -l  Override all host addresses with the host for the Head.
//          This is used to test a distributed deployment on a single computer.
//
//      -p  The head should persist after deploying Application XML.
//          This flag is only needed when XML files are specified.
//          When no XML files are given, the Head runs persistently by default.
//..............................................................................


    // Handle options and parse any XML files into the Global catalog.
    //
    DeployDiagram  &diagram = server.get_global_catalog();

    for (int  i = 1; i < argc; i++)
    {   xml_file = to_string(argv[i]);

        if (xml_file == "-l")
        {   if (deployed)
            {   ERROR << "-l must be first on the command line.";
                result = -1;
                break;
            }

            diagram.deploy_local();
            NOTICE << "Deploying all components locally.";
        }
        else if (xml_file == "-p")
        {   persist = True;
        }
        else
        {   NOTICE << "Processing xml:  " << xml_file;
            initial = True;
            status  = diagram.parse_file(xml_file);

            if (!status)
            {   ERROR << "Parsing error:  " << status;
                result = -1;
                break;
            }

            if (!diagram.is_borealis_diagram())
            {   deployed = True;
            }
        }
    }


    // Deploy the diagram (if any) to Borealis nodes and launch the server
    // process for the Head (if persistent).
    //
    if (result == 0)
    {   if (initial)
        {   if (!deployed)
            {   DEBUG << "BigGiantHead: generate deployment ***************************";
                status = diagram.generate_deployment();
            }

            DEBUG  << "BigGiantHead:  deploy  ================================";

            if (persist)
            {   //if (status)
                //{   status = open_server(port);
                //}

                if (status)
                {   // Run deployment in it's own thread.
                    //
                    if (pthread_create(&deploy_thread,
                                       0,
                                       (void*(*)(void*))deploy_diagram,
                                       (void*)NULL))
                    {
                        status = "Can not start the deploy_diagram thread.";
                    }
                }

                if (status)
                {   DEBUG  << "Run the Head server event loop ...";
                    status = run_server(port);
                    //loop.run();
                    //NOTICE << "terminating...";
                }
            }
            else if (status)
            {   // Deploy the diagram and exit the Head.
                //
                status = diagram.deploy();
            }
        }
        else
        {   // No XML files were given; run persistently.
            // Return after the Head is killed.
            //
            //status = open_server(port);
            status = run_server(port);
 
            //if (status)
            //{   loop.run();
            //    NOTICE << "terminating...";
            //}
        }
    }

    if (!status)
    {   result = -1;
    }

    if (result)
    {   ERROR << "Error status = " << to_string(result)
              << " : " << status;
    }

    diagram.deploy_conger("hi, this is from conger;");
    return  result;
}

//////////////////////////  end BigGiantHead.cc  ///////////////////////////////
