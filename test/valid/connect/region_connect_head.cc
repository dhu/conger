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


#define  SLEEP_TIME    10000     // Delay between file writes in ms.
#define  NODE1         15100

using namespace Borealis;
using namespace NMSTL;

    RegionServer  *server;
    IOEventLoop    loop;
    uint8          call = 0;
    string         xml;
