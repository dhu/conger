#ifndef NODETABLE_H
#define NODETABLE_H

#include "QBox.h"


////////////////////////////////////////////////////////////////////////////////
//  This default version is built when tables are not supported.
//..............................................................................

BOREALIS_NAMESPACE_BEGIN

class Table
{
  public:
    Table() throw( AuroraException ) {
        Throw(AuroraException, "No table support configured in this build.");
    }
};

/// Map a set of tables by name.
class TableMap
{
  public:
    /// Copy tables in a node's Topology to another.
    void   copy_from( /// Node from which to copy tables.
                     const TableMap  &other
                   )
    {};


    /// Dump the names of the tables in a node.
    void   repr( /// Append output to a string.
                 string  &out
               ) const
    {};


    /// Count the number of tables with a given name (returns 0).
    int32  count( /// See if this table is mapped.
                  string  name
                ) const;

    /// Clear the table map.
    void clear()
    {};
};


/// Container for AuroraNode Table transactions.
class NodeTable
{
  public:
    /// Constuctor links _table_map to a node's Topology.
    ///
    NodeTable( /// Initial set of mapped tables for the node.
               TableMap  &init
             )
    {};



    /// Default destructor.
    ~NodeTable()
    {};



    /// Attach a table to a newly added box.
    ///
    void  parse_add_box_xml( /// XML for the box node to add.
                                     const DOMElement  *box_node,

                                     /// Tables in this node's Topology.
                                     TableMap  &top_tables,

                                     /// Schema name for the box being added.
                                     string   box_name,

                                     /// Instance to fill in for the box.
                                     ptr<QBox>   box
                        )
        throw( AuroraException )
    {};



    /// Add a table to the schema for a node.
    ///
    void  parse_add_xml( /// Add the table to this node.
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
        throw( AuroraException );



    /// If a box contains a table then delete the table.
    ///
    void  parse_remove_box_xml( /// XML for the box node being removed.
                             const DOMElement  *remove_box_node,

                             /// Schema name of the box node being removed.
                             const string   box_name,

                             /// Instance for the box.
                             ptr<QBox>   box
                           )
        throw( AuroraException )
    {};



    ///  NYI:  Remove all tables from a node.
    ///
    void  parse_remove_xml()
                   throw( AuroraException );


    /// Add a table implementation directly from the catalog.
    void add_table( /// Add the table to this node.
                    AuroraNode& node,

                    /// Catalog representation of table
                    CatalogTable* table,

                    // Reference to table in the scheduler task's topology
                    TableMap& top_tables)
        throw( AuroraException );


    /// Check if a table implementation exists in the engine.
    bool has_table( // Name of table.
                    string table_name, 

                    // List of engine's tables.
                    TableMap& top_tables )
    { return false; }


    /// Retrieves a Table implementation from the given map.
    ptr<Table> get_table( // Name of table.
                          string table_name,
                          
                          // List of engine's tables.
                          TableMap& top_tables ) const
    { return ptr<Table>(); }


    /// Links a table to a box.
    void set_table( string table_name,
                    ptr<QBox> box,
                    TableMap& top_tables )
    {}


    /// Flush any cached table information.
    ///
    void  shutdown()
    {};
};

BOREALIS_NAMESPACE_END
#endif                            // NODETABLE_H
