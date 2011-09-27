#include  "NodeTable.h"
#include  <XmlTempString.h>


////////////////////////////////////////////////////////////////////////////////
//  This default version is built when tables are not supported.
//..............................................................................


BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
///
/// Count the number of tables with a given name (returns 0 or 1).
///
int32  TableMap::count( /// See if this table is mapped.
                        string  name
                      ) const
{
//..............................................................................


    return( 0 );
}



////////////////////////////////////////////////////////////////////////////////
///
/// Add a table to the schema for a node.
///
void  NodeTable::parse_add_xml( /// Add the table to this node.
                              AuroraNode  &node,

                              /// Name of the table being added.
                              string   table_name,

                              /// Tuple containing the table.
                              TupleDescription   desc,

                              /// Xml for the table.
                              DOMElement  *entityelt,

                              /// Reference to tables in the node's Topology.
                              TableMap   top_tables
                            )
                       throw( AuroraException )
{
//..............................................................................


    FATAL << "INACTIVE:  Can not add table entities.  Rebuild Borealis with Bdb";

    return;
}



////////////////////////////////////////////////////////////////////////////////
///
///  NYI:  Remove all tables from a node.
///
void  NodeTable::parse_remove_xml()
                          throw( AuroraException )
{
//..............................................................................


    Throw( AuroraException, "Removing tables currently not supported!" );
}



////////////////////////////////////////////////////////////////////////////////
///
/// Add a table to the node.
///
void NodeTable::add_table( AuroraNode&   node,

                           CatalogTable* table,

                           TableMap&     top_tables )
                    throw( AuroraException )
{
//..............................................................................


    Throw( AuroraException,
           string("Cannot add table entities. ")
           + "Rebuild Borealis with Berkeley DB." );
}

BOREALIS_NAMESPACE_END

////////////////////////////  end  NodeTable.cc  ///////////////////////////////
