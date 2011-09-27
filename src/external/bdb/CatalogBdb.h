#ifndef CATALOGBDB_H
#define CATALOGBDB_H

#include  "CatalogQuery.h"
#include  "CatalogBox.h"

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
///
///    Type inferencing methods for Bdb boxes.
///
class CatalogBdb
{
//..............................................................................


  public:

    ///  Infer the out stream schema for an sql update box.
    ///
    static  Status  infer_update_out(
                             /// The update box.
                             CatalogBox   *box,

                             /// Only In stream.
                             CatalogStream  *in,

                             ///  In - Optional Out stream; schema is optional.
                             /// Out - Unchanged or schema is added.
                             CatalogStream  *out,

                             /// Map to save the composite schema.
                             CatalogSchema::SchemaMap  *schema_map);


    ///  Infer the out stream schema for an sql update box.
    ///
    static  Status  infer_insert_out(
                             /// The insert box.
                             CatalogBox   *box,

                             /// Only In stream.
                             CatalogStream  *in,

                             ///  In - Optional Out stream; schema is optional.
                             /// Out - Unchanged or schema is added.
                             CatalogStream  *out);


    ///  Infer the out stream schema for an sql select box.
    ///
    static  Status  infer_select_out(
                             /// The select box.
                             CatalogBox   *box,

                             /// Select In stream.
                             CatalogStream  *in,

                             ///  In - First Out stream; schema is optional.
                             /// Out - Unchanged or schema is added.
                             CatalogStream  *out,

                             /// Map to save the composite schema.
                             CatalogSchema::SchemaMap  *schema_map);


    ///  Infer the out stream schema for an sql delete box.
    ///
    static  Status infer_delete_out(
                             /// The delete box.
                             CatalogBox   *box,

                             /// Delete In stream.
                             CatalogStream *in,
                                   
                             ///  In - First Out stream; schema is optional.
                             /// Out - Unchanged or schema is added.
                             CatalogStream *out,
                                    
                             /// Map to save the composite schema.
                             CatalogSchema::SchemaMap  *schema_map);
};


BOREALIS_NAMESPACE_END

#endif                      // CATALOGBDB_H
