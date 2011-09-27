#include <NMSTL/xmlrpc>
#include <NMSTL/tcprpc>
#include <NMSTL/text>

#include  "RegionServer.h"
#include  "WriteRegion.h"
#include  "HeadClient.h"
#include  "Diagram.h"


////////////////////////////////////////////////////////////////////////////////
//
//  This test regional component receives update Xml from a Borealis node
//  and periodically writes the contents of the regional catalog.
//..............................................................................


#define  SLEEP_TIME    5000              // Delay between file writes in ms.
#define  GLOBAL_NODE   "localhost"
#define  HEAD_NODE     "localhost"

#define  BOREALIS_NETWORK  "Global.xml"
#define  BOREALIS_DEPLOY   "GlobalDeploy.xml"

using namespace Borealis;
using namespace NMSTL;

    RegionServer   *server;
    string         node;

    Remote<HeadServer>  head_server;

    HeadClient    *client;
    int            result;


////////////////////////////////////////////////////////////////////////////////
//
void  write_global_catalog()
{
    Status     status = true;
    RPC<void>  rpc;
//..............................................................................


    if (server->is_region_loaded())
    {   WriteRegion::write_region( server->get_regional_catalog(),
                                   "global.out" );

        NOTICE << "wrote the global catalog.  Terminating ...";
        client->terminate();
    }
    else 
    {   DEBUG << "Waiting for the global catalog to load.";

        // The callback is enqueued with a timer.
        // We only callback with a timer because this is looping.
        //
        ( new CallbackTimer( client->get_loop(),
                            &write_global_catalog ))
                  ->arm( Time::now() + Time::msecs( SLEEP_TIME ));
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
int main(int  argc, const char  *argv[])
{
    int          port = DEFAULT_GLOBAL_PORT;
    string       ip   = GLOBAL_NODE;

    string       borealis = BOREALIS_NETWORK;
    string       deploy   = BOREALIS_DEPLOY;

    Status       status = true;
    string       xml;

    RPCBindings  bindings;
    RPC<void>    rpc;
//..............................................................................


    // Connect to the Head.
    //
    client = new HeadClient(InetAddress(HEAD_NODE, DEFAULT_HEAD_PORT));
    server = new RegionServer(port);

    bindings.bind(*server, "GlobalServer");

    ip = Util::get_host_address(ip);

    InetAddress listening(ip, port);
    TCPRPCAcceptor acceptor(client->get_loop(), listening, bindings);

    if (!acceptor)
    {   status = "Unable to accept RPC connections: "
               + to_string(acceptor.stat());

        NOTICE << status;
        result = 1;
    }
    else
    {   NOTICE << "Accepting Regional RPC connections on "
               << acceptor.get_socket().getsockname();

        // Request the Head to deploy the XML 
        //
        rpc = client->deploy_xml_file(borealis);
        NOTICE << "deployXmlFile( " << borealis << " ): " << rpc;
        status = rpc.stat();
 
        if (!status)
        {   result = 2;
        }
    }

    if ((status)  &&  (!deploy.empty()))
    {   DEBUG  << "===========================================================";

        rpc = client->deploy_xml_file( deploy );
        NOTICE << "deployXmlFile( " << deploy << " ): " << rpc;
        status = rpc.stat();

        if (!status)
        {   result = 3;
        }
    }

    // Request to fill the global catalog.
    //
    if (status)
    {   xml = "<global  endpoint=\"" + ip + ":" + to_string(port) + "\" />";

        rpc = client->add_xml_string(xml);
        Status status = rpc.stat();
        NOTICE  << "status = " << status;
  
        if (!status)
        {   result = 4;
        }
    }

    // Loop to write the catalog.
    //
    if (status)
    {   write_global_catalog();
    }

    NOTICE  << "run the client event loop...";
    client->run();

    NOTICE << "terminating ...";

    return(result);
}
