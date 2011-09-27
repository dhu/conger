#ifndef BorealisCatalogReader_H
#define BorealisCatalogReader_H

#include <RegionalCatalog.h>
#include "LMCommon.h"

BOREALIS_NAMESPACE_BEGIN

/**
 * An BorealisCatalogReader class implements an agent that
 * reads borealis nodes information and query graph deployment informaton from a 
 * regional catalog and then update the LMNodeManager and LMQueryNetwork
 */
class BorealisCatalogReader
{

public:

    /**
     * Reads information from the catalog and creates LMBoxes and LMArcs in the
     * LMQueryNetwork 
     * @param catalog the region catalog
     * @return false if no Borealis node is running
     */
    static bool read_catalog(RegionalCatalog& catalog);

};


BOREALIS_NAMESPACE_END
#endif


