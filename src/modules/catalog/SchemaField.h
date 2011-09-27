#ifndef   SCHEMAFIELD_H
#define   SCHEMAFIELD_H

#include  "common.h"
#include  "DataType.h"
#include  "SchemaArray.h"

BOREALIS_NAMESPACE_BEGIN

class SchemaField
{
  public:
    SchemaField() {}

    SchemaField( string  name,
               DataType  type,
                 uint16  size,
                 uint16  offset = 0,
            SchemaArray *array = NULL              
               )
               : _name(name),
                 _type(type),
                 _size(size),
                 _offset(offset),
                 _array(array)
    {}


    string  as_string() const
    {   return( "Field{" + _name + " " + _type + "(" + _size + " at " + _offset + ")}" );
    }


    string  get_name() const
    {   return( _name );
    }


    void  set_name( string   name )
    {   _name = name;
    }


    DataType  get_type() const
    {   return( _type );
    }


    uint16  get_size() const
    {   return( _size );
    }


    void  set_type_size( DataType  type, uint16  size )
    {   _type = type;
        _size = size;
    }


    uint16  get_offset() const
    {   return( _offset );
    }



    void  set_offset( uint16  offset )
    {   _offset = offset;
    }


    SchemaArray  *get_array() const
    {   if (_type == DataType::ARRAY)
        {   return(_array);
        }

        return(NULL);
    }


    void  set_array(SchemaArray  *array)
    {   _type  = DataType::ARRAY;
        _array = array;
    }



    NMSTL_SIMPLY_SERIALIZABLE( SchemaField,
                               << _name << _type << _size << _offset );

  private:

    /// Name of the schema field.
    ///
    string        _name;

    /// Type of the schema field.
    ///
    DataType      _type;

    /// Number of bytes in the field or the maximum string size.
    ///
    uint16        _size;

    /// Tuple offset to this field.
    ///
    uint16        _offset;

    /// If _type is ARRAY, at the array definition; else undefined.
    ///
    SchemaArray  *_array;
};

BOREALIS_NAMESPACE_END
#endif                           // SCHEMAFIELD_H
