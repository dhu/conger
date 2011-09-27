#ifndef  CATALOGTABLE_H
#define  CATALOGTABLE_H

#include  "CatalogQuery.h"

BOREALIS_NAMESPACE_BEGIN

class  CatalogQuery;

/// Configuration data for a table.
///
class CatalogTable
{
  public:
    /// A collection of tables
    typedef  vector<CatalogTable *>    TableStar;

    //CatalogTable() {}
    //CatalogTable(const CatalogTable &table);

    void shallow_copy(const CatalogTable &table);

    /// Name of a table as a string.
    ///
    Name  get_table_name() const
    {  return( _name );
    }

    /// Name of a table schema.
    ///
    Name  get_table_schema_name() const
    {  return( _schema->get_schema_name() );
    }

    CatalogSchema  *get_table_schema() const
    {  return( _schema );
    }

    const vector<Name>  *get_table_key_const() const
    {   return( &_key );
    }

    const vector<Name>  *get_table_index_const() const
    {   return( &_index );
    }

    vector<Name>  *get_table_key()
    {   return( &_key );
    }

    vector<Name>  *get_table_index()
    {   return( &_index );
    }

    BoxParameter  *get_table_parameters()
    {   return &_parameter;
    }

    BoxParameter::ParameterMap  *get_table_parameter_map()
    {   return( _parameter.get_parameter_map() );
    }

    const BoxParameter::ParameterMap *get_table_parameter_map_const() const;

    void  set_table_name_schema( Name    name,
                  CatalogSchema   *schema )
    {
        _name   = name;
        _schema = schema;

        _query  = NULL;
        _node   = "";             // Empty for now.  Needs to be set???
    }

    void  set_table_schema(CatalogSchema *schema);

    void  set_table_query( CatalogQuery   *query )
    {   _query  = query;
    }

    void  set_table_node( string   node )
    {   _node = node;
    }

    string  get_table_node() const;

    void  add_table_key( Name   key )
    {   _key.push_back( key );
    }

    void  add_table_index( Name   index )
    {   _index.push_back( index );
    }


    ///  Create an XML string for a table.
    ///
    string  form_table_xml();


    string as_string() const
    {
        string r;
        r = _name.as_string();
        return r;
    }


    NMSTL_SERIALIZABLE(CatalogTable);
    void freeze(::NMSTL::OSerial &out) const;
    void shallow_freeze(::NMSTL::OSerial &out) const;
    void unfreeze(::NMSTL::ISerial &in);
    NMSTL_DUMMY_SERIALREG(CatalogTable);


  private:
    Name            _name;               // Table name
    CatalogQuery   *_query;              // Query containing the table.
    CatalogSchema  *_schema;             // Schema of the table contents.
    string          _node;               // Location of the table.

    vector<Name>    _key;                // Fields that make up the key.
    vector<Name>    _index;              // Indexed table fields.
    BoxParameter    _parameter;          // Table parameters.
};

BOREALIS_NAMESPACE_END
#endif                       // CATALOGTABLE_H
