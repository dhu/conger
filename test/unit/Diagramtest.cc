#include "DeployDiagram.h"

using namespace Borealis;


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

    //string            borealis = "sunions.xml";
    //string            deploy   = "";

    //string            borealis = "BorealisDiagram.xml";
    //string            deploy   = "DeployReplica.xml";

    string            borealis = "example_query.xml";
    string            deploy   = "example_deploy.xml";

    DeployDiagram     diagram;
    int               error = -1;
//..............................................................................


if ( !deploy.empty() )
{   NOTICE << "borealis=" << borealis << "  deploy=" << deploy;
}
else
{   NOTICE << "borealis=" << borealis;
}

Status  status = diagram.parse_file( borealis );

if ( status )
{   DEBUG  << "===========================================================";

    if (!deploy.empty())
    {   status = diagram.parse_file(deploy);
    }
    else
    {   status = diagram.generate_deployment();
    }
}

if ( status )
{   DEBUG  << "===========================================================";

    // Uncomment if you want to test deployment to Borealis nodes.
    //
    if ( diagram.deploy() )
    {   error = 0;
    }
}

if ( error )
{   NOTICE << "Diagramtest failed:  " << error;
}
else
{   NOTICE << "Diagramtest succeeded.";
}

return( error );
}
