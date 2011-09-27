#include "HeadClient.h"
#include "Diagram.h"

using namespace Borealis;

#define   HEAD_NODE   "localhost"


////////////////////////////////////////////////////////////////////////////////
//
int  main( int  argc, char  **argv )
{
    //string  borealis = "cp2test.xml";
    //string            deploy   = "";

    //string            borealis = "BorealisDiagram.xml";
    //string            deploy   = "DeployDiagram.xml";

    //string            borealis = "BorealisPlanet.xml";
    //string            deploy   = "DeployPlanet.xml";

    string            borealis = "BorealisRegion.xml";
    string            deploy   = "DeployRegion.xml";

    int        error = -1;

    HeadClient  *client;
    RPC<void>  rpc;
//
//..............................................................................


if ( !deploy.empty() )
{   NOTICE << "borealis=" << borealis << "  deploy=" << deploy;
}
else
{   NOTICE << "borealis=" << borealis;
}

DEBUG  <<  "HEAD_NODE=" << HEAD_NODE << " HEAD_PORT=" << DEFAULT_HEAD_PORT;

client = (HeadClient *)new HeadClient( InetAddress( HEAD_NODE,
                                                    DEFAULT_HEAD_PORT ));

rpc = client->deploy_xml_file( borealis );
NOTICE << "deployXmlFile( " << borealis << " ): " << rpc;
Status  status = rpc.stat();

if (( status )  &&  ( !deploy.empty() ))
{   DEBUG  << "===========================================================";

    rpc = client->deploy_xml_file( deploy );
    NOTICE << "deployXmlFile( " << deploy << " ): " << rpc;
    status = rpc.stat();
}

if ( status )
{   NOTICE << "Headtest succeeded.";
    error = 0;
}
else
{   NOTICE << "Headtest failed:  " << status;
}

return( error );
}
