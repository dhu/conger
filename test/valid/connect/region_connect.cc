#include <NMSTL/xmlrpc>
#include <NMSTL/tcprpc>
#include <NMSTL/text>

#include  "RegionServer.h"
#include  "WriteRegion.h"
//#include  "MedusaClient.h"


////////////////////////////////////////////////////////////////////////////////
//
//  This test regional component receives update Xml from a Borealis node
//  and periodically writes the contents of the regional catalog.
//..............................................................................


#define  SLEEP_TIME    10000     // Delay between file writes in ms.
#define  NODE0         15000
#define  NODE1         15100

using namespace Borealis;
using namespace NMSTL;

    RegionServer  *server;
    IOEventLoop    loop;
    uint8          call = 0;
    string         xml;



////////////////////////////////////////////////////////////////////////////////
//
static void  send_connect(string  *xml)
{
//..............................................................................


    DEBUG << "xml=" << *xml;

    Status  status = server->get_regional_catalog().connect_box(*xml, "box1");
    DEBUG << "connect_box status=" << status;
 
    return;
}



////////////////////////////////////////////////////////////////////////////////
//
static void  send_disconnect(string  *stream)
{
//..............................................................................


    DEBUG << "disconnect stream=" << *stream;
    Status  status = server->get_regional_catalog().disconnect_stream(*stream);
    DEBUG << "disconnect_stream status=" << status;
 
    return;
}




////////////////////////////////////////////////////////////////////////////////
//
void  write_regional_catalog()
{
    Status        status = true;
    set<string>  *node_set;
    pthread_t     thread;
//..............................................................................


    WriteRegion::write_region(server->get_regional_catalog(),
                              "Regiontest.xml");

    NOTICE << "wrote the catalog.";

    node_set = server->get_regional_catalog().get_region_node();

    if ((call == 0)  &&  (node_set->size() == 2))
    {   call = 1;

        xml  = "<box  name=\"Insert\"  type=\"filter\" >\n";
        xml += " <in       stream=\"source\"       />\n";
        xml += " <out      stream=\"intermediate\" />\n";
        xml += " <parameter  name=\"expression.0\"  value=\"1=1\" />\n";
        xml += "</box>\n";

        DEBUG << "xml=" << xml;

        if (pthread_create(&thread, 0,
                           (void*(*)(void*))send_connect,
                           (void*)&xml)
            < 0)
        {
            WARN << "Can not start the add_xml thread.";
        }
    }
    else if (call == 1)
    {   call = 2;
        xml  = "intermediate";

        if (pthread_create(&thread, 0,
                           (void*(*)(void*))send_disconnect,
                           (void*)&xml)
            < 0)
        {
            WARN << "Can not start the add_xml thread.";
        }
    }
 
    // The callback is enqueued with a timer.
    // We only callback with a timer because this is looping.
    //
    ( new CallbackTimer( loop,
                        &write_regional_catalog ))
              ->arm( Time::now() + Time::msecs( SLEEP_TIME ));

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
int main( int argc, const char *argv[] )
{
    int            port = DEFAULT_REGION_PORT;
    string         ip   = "localhost";
    RPCBindings    bindings;
//..............................................................................


    server = new RegionServer( port );
    bindings.bind( *server, "RegionServer" );

    ip = Util::get_host_address( ip );

    InetAddress listening( ip, port);
    TCPRPCAcceptor acceptor( loop, listening, bindings );

    if (!acceptor)
    {   FATAL << "Unable to accept RPC connections: " << acceptor.stat();
    }

    NOTICE << "Accepting Regional RPC connections on "
           << acceptor.get_socket().getsockname();

    write_regional_catalog();

    DEBUG  << "run the client event loop...";
    loop.run();

    NOTICE << "terminating..";

    return(0);
}
