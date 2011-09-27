#include "DeployDiagram.h"
#include "WriteRegion.h"

using namespace Borealis;

#define  BOREALIS_XML  "BorealisCatalog.xml"
#define  CATALOG_XML   "catalog.out"


////////////////////////////////////////////////////////////////////////////////
//
int  main( int  argc, char  **argv )
{
    DeployDiagram    diagram;
    int              error = 0;
//..............................................................................


Status  status = diagram.parse_file( BOREALIS_XML );

//if ( status )
//{   status = diagram.generate_deployment();
//}

if ( status )
{   WriteRegion::write_region( diagram, CATALOG_XML );
}
else
{   NOTICE << status;
    error = 1;
}

return( error );
}
