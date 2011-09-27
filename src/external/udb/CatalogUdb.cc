#include  "CatalogQuery.h"
#include  "CatalogBox.h"

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
//  Infer the type of Box out streams from in streams.
//  If an out stream already has a schema it is validated.
//  Upon entry, all in streams must have schemas and the box has been validated.
//
Status  CatalogBox::infer_udb_out(// Map to save the composite schema.
                                  CatalogSchema::SchemaMap  *schema_map,

                                  CatalogStream  *in,

                                  CatalogStream  *out)
{
    Status    status = false;
    string    type;
//..............................................................................


    type = get_box_type();

    if (type == "delay")                            // in = out
    {   status = match_box_schema(in, out);
    }
    else if (type == "delay_sel")                   // in = out
    {   status = match_box_schema(in, out);
    }
    else if (type == "combine")                     // NYI
    {   status = "Unimplmented inferencing for a box.";
    }
    else if (type == "LRWaitFor")
    {   status = "Unimplmented inferencing for a box.";
    }

    return  status;
}


BOREALIS_NAMESPACE_END
