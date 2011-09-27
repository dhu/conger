#ifndef   CATALOGSCHEMA_H
#define   CATALOGSCHEMA_H

#include  "SchemaField.h"
#include  "util.h"

BOREALIS_NAMESPACE_BEGIN

class CatalogSchema
{
  public:
    typedef  map<Name, CatalogSchema >    SchemaMap;

    CatalogSchema() {}

    CatalogSchema(const CatalogSchema  &other)
    {   _name = other._name;

        copy( other._field.begin(), other._field.end(),
              back_inserter( _field ) );
    }


    // make a deep copy of another schema.
    void clone( const CatalogSchema  &other )
    {
        _name = other._name;

        copy( other._field.begin(), other._field.end(),
              back_inserter( _field ) );
    }


    string get_schema_name() const
    {   return( to_string( _name ));
    }


    void  set_schema_name(Name   name)
    {   _name = name;
        return;
    }


    const SchemaField  *get_schema_field( uint16  index )  const
    {   return( &_field[ index ] );
    }

    // Get schema field by name.
    //
    const SchemaField *get_schema_field(string field) const
    {
        for (unsigned int i = 0; i < _field.size(); i++)
        {
            if (_field[i].get_name() == field)
            {
                return(&_field[i]);
            }
        }

        // DEBUG << "No such field : " << field;
        return NULL;
    }

    uint16  get_field_offset( uint16  index ) const
    {   return( _field[ index ].get_offset()  );
    }


    void set_field_offset( uint16  index, uint16  offset )
    {   _field[ index ].set_offset( offset );
        return;
    }


    string  get_field_name( uint16  index ) const
    {   return( _field[ index ].get_name() );
    }


    /// Return the number of bytes in a schema.
    ///
    uint16  get_size() const
    {   vector<SchemaField>::const_iterator field_it = _field.begin();
        vector<SchemaField>::const_iterator field_end = _field.end();
        int size = 0;

        for (; field_it != field_end; field_it++)
        {   size += field_it->get_size();
        }

        return( size );
    }


    vector<SchemaField>  &get_schema_field()
    {   return( _field );
    }


    int32  index_of_field( string  field ) const
    {   for ( uint32  i = 0; i < _field.size(); ++i )
        {   if ( _field[i].get_name() == field )
            {   return( i );
            }
        }

        return( -1 );
    }


    ///  Return True if two schemas have the same field names and types.
    ///
    Boolean  equal_schema(// Schema to compare.
                          CatalogSchema  *Match);


    ///  Create an XML string for a stream schema.
    ///
    string  form_schema_xml();

    string  as_string() const
    {
        string fields;
        vector<SchemaField>::const_iterator field_it = _field.begin();
        vector<SchemaField>::const_iterator field_end = _field.end();
        
        for (; field_it != field_end; ++field_it)
            fields += (fields.empty()? string() : ", ") + field_it->as_string();
        
        return( "CatalogSchema{name=" + to_string( _name )
                     + "; fields=" + fields
                     + "; size=" + get_size() + "}" );
    }


    NMSTL_SIMPLY_SERIALIZABLE( CatalogSchema, << _name << _field );


  // Nmstl serialization in TupleDescription.h can't use getters.
  //protected:

    Name                   _name;         // Name of the schema.
    Name                   _key;          // Name of a field for queue updates.
    vector<SchemaField>    _field;        // Fields in the schema.
};


BOREALIS_NAMESPACE_END
#endif                       // CATALOGSCHEMA_H
