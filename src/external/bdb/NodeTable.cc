#include  "NodeTable.h"
#include  <XmlTempString.h>

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
///
/// Count the number of tables with a given name (returns 0 or 1).
///
int32  TableMap::count( /// See if this table is mapped.
                        string  name ) const
{
//..............................................................................


    return( _map.count( name ));
}



////////////////////////////////////////////////////////////////////////////////
///
/// Copy a tables in a node's Topology to another.
///
void  TableMap::copy_from( /// Node from which to copy tables.
                          const TableMap  &other )
{
//..............................................................................


    copy( other._map.begin(), other._map.end(), inserter( _map, _map.begin() ));
    
    return;
}



////////////////////////////////////////////////////////////////////////////////
///
/// Dump the names of the tables in a node.
///
void  TableMap::repr(  /// Append output to a string.
                       string  &out )  const
{
//..............................................................................
    out << "Tables {\n";

    for ( TableMapType::const_iterator  i  = _map.begin();
                                        i != _map.end(); ++i )
    {
        out << "    " << *( i->second ) << "\n";
    }

    out << "}\n";

    return;
}


////////////////////////////////////////////////////////////////////////////////
///
/// Delete all tables in the map.
///
void TableMap::clear()
{
    _map.clear();
}


////////////////////////////////////////////////////////////////////////////////
///
/// Constuctor links _table_map to a node's Topology.
///
NodeTable::NodeTable( /// Initial set of mapped tables for the node.
                      TableMap  &map ) : _table_map( map ) {};
//..............................................................................



////////////////////////////////////////////////////////////////////////////////
///
/// Find and add table, if any.
///
void  NodeTable::parse_add_box_xml( /// XML for the box node to add.
                                 const DOMElement  *box_node,

                                 /// Tables in this node's Topology.
                                 TableMap  &top_tables,

                                 /// Schema name for the box being added.
                                 string   box_name,

                                 /// Instance to fill in for the box.
                                 ptr<QBox>   box
                               )
                          throw( AuroraException )
{
    DOMNodeList       *table_nodes;
    const DOMElement  *table_elt;
    string             table_name;
    ptr <Table>        table;
//..............................................................................


    XmlTempString xtable( "table" );
    table_nodes = box_node->getElementsByTagName( xtable.to_xml() );

    DEBUG << "Iterating over tables.";

    for ( uint32  i = 0; i < table_nodes->getLength(); ++i )
    {
        table_elt  = static_cast<const DOMElement *>( table_nodes->item( i ));
        table_name = xml_attribute( table_elt, "name", ATTR_NON_EMPTY );
        table      = lookup( top_tables._map, table_name );
        
        if ( !table )
        {   table = lookup( _table_map._map, table_name );
        }
        
        if ( !table )
        {   Throw( aurora_no_such_entity_exception,
                   "Reference to undefined table \"" + table_name
                   + "\" in box \"" + box_name + "\"" );
        }
        
    box->set_table( table.get() );
    }
    
    return;
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
                              TableMap   &top_tables
                            )
    throw( AuroraException )
{
    ptr<Table>  table;
//..............................................................................


    if ( !_table_env )
    {    DEBUG << "Creating a table environment for this node.";
        _table_env = ptr<TableEnvironment>( new TableEnvironment( node ));
    }

    table = ptr<Table>( new Table( table_name, desc, entityelt, _table_env ));

    top_tables._map[ table_name ] = table;
    
    DEBUG << "Added table " << table;
    
    return;
}



////////////////////////////////////////////////////////////////////////////////
///
/// If a box contains a table then delete the table.
///
void  NodeTable::parse_remove_box_xml( /// XML for the box node being removed.
                                    const DOMElement  *remove_box_node,

                                    /// Schema name of the box node being removed.
                                    const string   box_name,

                                    /// Instance for the box.
                                    ptr<QBox>   box
                                  )
                             throw( AuroraException )
{
    DOMNodeList       *table_nodes;
    const DOMElement  *table_elt;
    string             table_name;
//..............................................................................


    // Get box table, if any.
    XmlTempString xtable( "table" );
    table_nodes = remove_box_node->getElementsByTagName( xtable.to_xml() );
    
    if ( table_nodes->getLength() > 1 )
    {   Throw( AuroraBadEntityException,
               "Box " + box_name
               + " was associated with more than one table!  Aurora"
               + " allows this in its description for now....." );
    }
    
    if ( table_nodes->getLength() == 1 )
    {   table_elt  = static_cast<const DOMElement *>( table_nodes->item( 0 ));
        table_name = xml_attribute( table_elt, "name", ATTR_NON_EMPTY );
        
        if ( box->get_table()->get_name() != table_name )
        {   Throw( AuroraBadEntityException,
                   "Box " + box_name + " does not access a table named <"
                   + table_name + ">" );
        }
    }
    
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
/// Add a table directly from the catalog.
///
void NodeTable::add_table( AuroraNode&   node,

                           CatalogTable* table,

                           TableMap&     top_tables )
    throw( AuroraException )
{
    ptr<Table>  table_impl;
//..............................................................................


    if ( !_table_env )
    {    DEBUG << "Creating a table environment for this node.";
    _table_env = ptr<TableEnvironment>( new TableEnvironment( node ));
    }

    string table_name = table->get_table_name().as_string();

    table_impl = ptr<Table>( new Table( table, _table_env ));

    top_tables._map[ table_name ] = table_impl;

    DEBUG << "Added table " << table_impl;

    return;
}

////////////////////////////////////////////////////////////////////////////////
///
/// Checks if the table already exists.
///
bool NodeTable::has_table( // Name of table.
                string table_name, 

                // List of engine's tables.
                TableMap& top_tables )
{
    ptr<Table> table_impl = lookup(top_tables._map, table_name);

    if (table_impl)
        return true;

    return false;
}


////////////////////////////////////////////////////////////////////////////////
///
/// Retrieves the specified table if it exists.
///
ptr<Table> NodeTable::get_table( // Name of table.
                                string table_name,
                                
                                TableMap& top_tables ) const
{
    ptr<Table> table_impl = lookup(top_tables._map, table_name);
    
    if (table_impl)
        return table_impl;
    
    return ptr<Table>();
}

////////////////////////////////////////////////////////////////////////////////
///
/// Links a table to a box.
///
void NodeTable::set_table( string table_name,
                           ptr<QBox> box,
                           TableMap& top_tables )
{
    ptr<Table> table_impl = lookup(top_tables._map, table_name);

    if ( table_impl )
        box->set_table( table_impl.get() );

    return;
}
                           
////////////////////////////////////////////////////////////////////////////////
///
/// Flush any cached table information.
///
void  NodeTable::shutdown()
{
//..............................................................................


for ( TableMapType::iterator i = _table_map._map.begin();
      i != _table_map._map.end(); ++i )
{
    ptr<Table> t = i->second;
    t->sync();
}

return;
}


BOREALIS_NAMESPACE_END

////////////////////////////  end  NodeTable.cc  ///////////////////////////////
