#include  "CatalogQuery.h"
#include  "CatalogBox.h"
#include  "Expression.h"
#include  "SQLSelect.h"
#include  "Aggregate.h"
#include  "util.h"

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
//  Infer the type of Box out streams from in streams.
//  If an out stream already has a schema it is validated.
//  Upon entry, all in streams must have schemas and the box has been validated.
//
Status  CatalogBox::infer_array_out(// Map to save the composite schema.
                                    CatalogSchema::SchemaMap  *schema_map,

                                    CatalogStream  *in,

                                    CatalogStream  *out)
{
//..............................................................................


    return  false;
}


BOREALIS_NAMESPACE_END
