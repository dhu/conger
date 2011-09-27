#include <NMSTL/xmlrpc>
#include <NMSTL/tcprpc>
#include <NMSTL/text>

#include  "RegionServer.h"
#include  "WriteRegion.h"
#include  "MedusaClient.h"


////////////////////////////////////////////////////////////////////////////////
//
//  This test regional component receives update Xml from a Borealis node
//  and periodically writes the contents of the regional catalog.
//..............................................................................


#define  SLEEP_TIME     5000     // Delay between file writes in ms.
#define  NODE0         15000
#define  NODE1         15100

using namespace Borealis;
using namespace NMSTL;

    RegionServer  *server;
    IOEventLoop    loop;
    uint8          call = 0;   // 0: wait to load; 1 - busy; 2 - send to 
    uint8          zero;       // Copy of call passed to the thread.

    string         move0;
    string         move1;

    string         node0;
    string         node1;

    Remote<QueryProcessor> query_processor0;
    Remote<QueryProcessor> query_processor1;

    ptr<MedusaClient> client;



////////////////////////////////////////////////////////////////////////////////
//
static void  send_move(uint8  *zip)
{
    RPC<void>  rpc;
    Status   status = true;
//
// A thread is used because the rpc call does not return until the multistage
// move process completes.  This can take a long time and clog the event loop.
// The move is also reflected by the node to the regional component; which
// causes a deadlock.
//..............................................................................


    if (*zip != 3)
    {   // Move the box from node 0 to node 1.
        //
        rpc = query_processor0.add_xml_string(move1);
        call = 3;
    }
    else
    {   // Move the box from node 1 to node 0.
        //
        rpc = query_processor1.add_xml_string(move0);
        call = 2;
    }

    status = rpc.stat();
    DEBUG << "status=(" << status << "):  ";
 
    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  write_regional_catalog()
{
    pthread_t     thread;
//..............................................................................


    WriteRegion::write_region(server->get_regional_catalog(),
                              "Regiontest.xml");

    NOTICE << "wrote the catalog.";

    if (call == 0  &&  !server->get_regional_catalog().get_box("box1"))
    {   DEBUG << "Waiting for deployment to complete.";
    }
    else if (call == 1)
    {   DEBUG << "Busy moving the box ...";
    }
    else
    {   // Tell the box to move.
        //
        DEBUG << "move from " << node1;
        zero = call;

        if (pthread_create(&thread, 0,
                           (void*(*)(void*))send_move,
                           (void*)&zero)
            < 0)
        {
            WARN << "Can not start the add_xml thread.";
        }

        call = 1;                        // Signal busy.
    }

    // The callback is enqueued with a timer.
    // We only callback with a timer because this is looping.
    //
    ( new CallbackTimer(loop,
                        &write_regional_catalog))
              ->arm(Time::now() + Time::msecs(SLEEP_TIME));

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


    server = new RegionServer(port);
    bindings.bind(*server, "RegionServer");

    ip = Util::get_host_address(ip);

    InetAddress listening(ip, port);
    TCPRPCAcceptor acceptor(loop, listening, bindings);

    if (!acceptor)
    {   FATAL << "Unable to accept RPC connections: " << acceptor.stat();
    }

    NOTICE << "Accepting Regional RPC connections on "
           << acceptor.get_socket().getsockname();

    node0 = Util::form_endpoint("localhost", NODE0);
    node1 = Util::form_endpoint("localhost", NODE1);

    move0 = "<move  box=\"box1\"  node=\"" + node0 + "\" />";
    move1 = "<move  box=\"box1\"  node=\"" + node1 + "\" />";

    // Connect to the qp for each node.
    //
    client = (ptr<MedusaClient>) new MedusaClient(node0);
    query_processor0 = client->qp_for(MedusaID(node0));
    query_processor1 = client->qp_for(MedusaID(node1));

    write_regional_catalog();

    DEBUG  << "run the client event loop...";
    loop.run();

    NOTICE << "terminating..";

    return  0;
}
