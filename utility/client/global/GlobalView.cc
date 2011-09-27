#include  "GlobalView.h"
#include  "WriteRegion.h"
#include  "Diagram.h"

using namespace Borealis;

#define  SLEEP_TIME     1000              // Delay between file writes in ms.

int8   loaded = 0;


////////////////////////////////////////////////////////////////////////////////
//
//  The GlobalView program manages a global catalog for a Borealis system.
//  Clients may request the catalog and subsequent updates using XML/RPC.
//..............................................................................


RegionServer      *GlobalView::_global_server;


////////////////////////////////////////////////////////////////////////////////
//
//  Launch the Global client and server; request the catalog from the Head.
//
Status  GlobalView::open(// Used by the Global client to connect to the Head.
                         string       head_point,

                         // Port for the Viewer to receive updates from the head.
                         //
                         string       view_point)
{
    string       ip = "";
    string       xml;
    Status       status = true;

    InetAddress  head;
    RPC<void>    rpc;
//..............................................................................


    // The Global component listens for requests (see tool/head/HeadServer.cc).
    //
    _global_server = new RegionServer(Util::form_endpoint(view_point,
                                                          DEFAULT_GLOBAL_PORT));


    // Run the Global server event loop in it's own thread.
    //
    if (pthread_create(&_server_thread,
                       0,
                       (void*(*)(void*))start_server,
                       (void*)&(_global_server->get_ioevent_loop())) < 0)
    {
        status = "Can not start the Global server thread.";
    }
    else    
    {   // Connection to send to the Head.
        //
        head = InetAddress(Util::form_endpoint(head_point, DEFAULT_HEAD_PORT));

        _head_client = new HeadClient(head);

        if (!_head_client->get_endpoint()->is_connected())
        {
            NOTICE  << "I can't find my Head.  Looking for my Head ...";

            while (!_head_client->get_endpoint()->is_connected())
            {   sleep(3);

                _head_client->get_endpoint()->reset_socket(TCPSocket(head));

                DEBUG  << "Reset socket:  " << head;
            }

            DEBUG  << "Found it!";
        }

        // not synched:  wait until the thread starts running???
        sleep(1);

        // Request the Head to fill the global catalog.
        //
        xml = "<global  endpoint=\"" + view_point + "\" />";
        rpc = _head_client->add_xml_string(xml);
        status = rpc.stat();

        if (!status)
        {   NOTICE  << "ERROR:  The Head at " << head << " is not running.";
        }
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Periodic client operation.
//
void  GlobalView::write_global_catalog()
{
    Status     status = true;
    RPC<void>  rpc;
//..............................................................................


    if (loaded)
    {   NOTICE << "already loaded.";
    }
    else if (_global_server->is_region_loaded())
    {   WriteRegion::write_region(_global_server->get_regional_catalog(),
                                  "global.out");

        NOTICE << "wrote the global catalog."; //  Terminating ...";
        loaded = 1;
        //_head_client->terminate();
    }
    else 
    {   DEBUG << "Waiting for the global catalog to load.";
    }

    // The callback is enqueued with a timer.
    // We only callback with a timer because this is looping.
    //
    (new CallbackTimer(_head_client->get_loop(),
                       wrap(this,
                            &GlobalView::write_global_catalog)))
                    ->arm(Time::now() + Time::msecs(SLEEP_TIME));

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Start the event loop for the Global client.
//
void  GlobalView::run()
{
//..............................................................................


    NOTICE  << "run the client event loop...";
    _head_client->run();

    NOTICE << "terminating ...";
    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  GlobalView  -d <Request Poll>  -h <Head Location>
//
//     Request Poll - Endpoint to use for the GlobalView program to listen for
//                    requests from an application for the catalog via the
//                    RegionServer::load_regional_catalog call.
//                    The StreamVisualizer polls for catalog updates this way.
//
//                    The Head also uses this channel to send any Catalog
//                    changes to the GlobalView program via the
//                    RegionServer::add_xml_string call.
//
//                    The default host is the local host and the default
//                    port is the default global component port (55000).
//
//    Head Location - Endpoint used by the Head for a request from the
//                    GlobalView program to reflect update XML.
//
//                    The default host is the local host and the default
//                    port is the default Head port (35000).
//
int  main(int  argc, const char  *argv[])
{
    string      view_point = ":" + to_string(DEFAULT_GLOBAL_PORT);
    string      head_point = ":" + to_string(DEFAULT_HEAD_PORT);

    Status      status = true;
    int32       result;
    GlobalView  global;
//
//..............................................................................
 

    status = global.open(head_point, view_point);

    // Thread loop to write the catalog.
    //
    if (status)
    {   global.write_global_catalog();
        global.run();
    }
    else
    {   WARN << status;
        result = 1;
    }

    return  result;
}



//////////////////////////  end  GlobalView.cc  ////////////////////////////////
