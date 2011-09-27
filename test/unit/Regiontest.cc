#include <NMSTL/xmlrpc>
#include <NMSTL/tcprpc>
#include <NMSTL/text>

#include  "RegionServer.h"
#include  "WriteRegion.h"
#include  "MedusaClient.h"
//#include  "QueryProcessor.h"


////////////////////////////////////////////////////////////////////////////////
//
//  This test regional component receives update Xml from a Borealis node
//  and periodically writes the contents of the regional catalog.
//..............................................................................


#define  SLEEP_TIME    10000     // Delay between file writes in ms.

using namespace Borealis;
using namespace NMSTL;

    RegionServer   *server;
    IOEventLoop    loop;


////////////////////////////////////////////////////////////////////////////////
//
void  write_regional_catalog()
{
    string     xml;

    set<string>                 *node_set;
    set<string>::const_iterator  node;
//..............................................................................


    WriteRegion::write_region( server->get_regional_catalog(),
                              "Regiontest.xml" );

    NOTICE << "wrote the catalog.";

    node_set = server->get_regional_catalog().get_region_node();

    for (node = node_set->begin(); node != node_set->end(); node++)
    {    ptr<MedusaClient> client = (ptr<MedusaClient>) new MedusaClient(*node);

         Remote<QueryProcessor> query_processor =
                                    client->qp_for(MedusaID(*node));

         xml  = "<statistic>\n";
         xml += " <parameter name=\"activate_latency\" value=\"1\" />\n";
         xml += "</statistic>\n";

         DEBUG << "xml=" << xml;
         query_processor.add_xml_string(xml);
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

    return( 0 );
}
