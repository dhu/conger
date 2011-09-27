#ifndef TUPLEDESCRIPTION_H
#define TUPLEDESCRIPTION_H

#include <cassert>

#include "Exceptions.h"
#include "DataType.h"
#include <nmstl_util.h>

#include "xercesDomFwd.h"

#include "TupleHeader.h"
#include "Catalog.h"
#include  <NMSTL/net>

BOREALIS_NAMESPACE_BEGIN

class Tuple;

/// A type of tuple, containing zero or more fields (each encapsulated
/// as a TupleDescription::Field object).
///
/// Every TupleDescription includes HEADER_SIZE bytes of padding
/// at the beginning to account for a timestamp and reserved field.
///
/// TupleDescriptions are immutable: once constructed they may not be
/// changed.
///
class TupleDescription
{
  public:

    enum flags
    {
        NO_HEADER = 1
    };


    /// Returns the offset of the field within the schema.
    ///
    uint16  get_offset(uint16  field_index) const
    {   return(_rep->_local_schema->get_field_offset(field_index));
    }


    void set_offset(uint16  off,
                    uint16  field_index)
    {
        _rep->_local_schema->set_field_offset(field_index, off);
    }

    /// Constructs a TupleDescription from a sequence of Field objects.
    ///
    template <typename iterator>
    TupleDescription(string    name,
                     iterator  begin,
                     iterator  end,
                     flags     flags = flags()
                    ) :
        _rep(new Rep(name, flags))
    {
        while (begin != end)
        {   _rep->_local_schema->_field.push_back( *begin );
            _rep->_local_schema->_field.back().set_offset( _rep->_size );
            _rep->_size += begin->get_size();

            begin++;
        }
    }


    /// Constructs a TupleDescription from a sequence of Field objects.
    ///
    template <typename iterator>
    TupleDescription(iterator  begin,
                     iterator  end,
                        flags  flags = flags()
                    ) :
        _rep(new Rep(string(), flags))
    {
        while (begin != end)
        {   begin->set_offset(_rep->_size);

            _rep->_local_schema->_field.push_back(*begin);
            _rep->_local_schema->_field.back().set_offset(_rep->_size);
            _rep->_size += begin->get_size();

            begin++;
        }
    }


    /// Construct a TupleDescription from a CatalogSchema structure.
    ///
    explicit  TupleDescription(CatalogSchema   *schema,
                               flags            flags = flags());


    TupleDescription(const TupleDescription& t)
    {
        if (t._rep)
        {
            string name = t._rep->_local_schema->get_schema_name();
            _rep = ptr<Rep>(new Rep(name));

            copy( t._rep->_local_schema->get_schema_field().begin(),
                  t._rep->_local_schema->get_schema_field().end(),
                  back_inserter( _rep->_local_schema->get_schema_field() ));

            _rep->_size = t._rep->_size;
        }
    }


    /// Creates a TupleDescription on which no method may be invoked.
    ///
    TupleDescription() {}


    /// Returns a non-null value if the object was not created with
    /// the no-argument constructor, i.e., this is a meaningful
    /// TupleDescription.
    ///
    /// Useful in the following manner:
    ///
    /// <pre>
    /// TupleDescription td = callSomeMethod();
    /// if (td)    {
    ///     NOTICE << "someMethod returned a valid TupleDescription: " << td;
    /// } else    {
    ///     NOTICE << "no such TupleDescription";
    /// }
    /// </pre>
    operator const void *() const { return _rep; }


    CatalogSchema *get_schema() const
    {   return( _rep->_local_schema );
    }


    /// Return the Fields object description of a field in the schema.
    ///
    /// @param field_num the zero-relative index of the field.  Must be
    ///     valid or an assertion will fail
    ///
    const SchemaField  *get_schema_field( uint16  field_num ) const
    {
        assert( field_num < _rep->_local_schema->get_schema_field().size() );

        return( _rep->_local_schema->get_schema_field( field_num ));
    }


    /// Return the Fields object description of a field in the schema, given
    /// its field name. -- tatbul
    ///
    const SchemaField  *get_schema_field(string field) const
    {
        return(_rep->_local_schema->get_schema_field(field));
    }


    /// Returns a reference to a vector of all fields in the schema.
    ///
    const vector<SchemaField>  *get_schema_fields() const
    {   return( &_rep->_local_schema->get_schema_field() );
    }

    /// Returns the index of the field with a particular name, or -1
    /// if there is no such field.
    ///
    int index_of_field( string  name ) const;


    /// Returns the name of the schema.
    ///
    string get_name() const
    {   return( to_string( _rep->_local_schema->get_schema_name() ));
    }


    /// Returns the number of fields in the schema.
    ///
    unsigned int get_num_fields() const
    {   return( _rep->_local_schema->get_schema_field().size() );
    }


    /// Returns the size of the schema including the header, as an exact sum
    /// of all field lengths.
    ///
    size_t get_size() const;


    /// Returns a string representation of the schema.
    ///
    string as_string() const;


    /// Generates a tuple matching this description, including the
    /// header (with the timestamp set to the current time).
    ///
    string generate_tuple() const;


    /// Generates a human-readable description of a tuple matching
    /// this description.
    ///
    string tuple_to_string( const void  *buf ) const;


    /// Generates a human-readable description of a tuple matching
    /// this description.
    ///
    string tuple_to_string( const Tuple &tup ) const;

    /// Generates a human-readable description of a tuple, without a header,
    /// matching this description.
    ///
    string tuple_data_to_string( const void *buf ) const;

    /// Returns an XML representation of this schema.
    ///
    /// @param name a name to use if the schema is anonymous (ignored
    ///    if the schema has a name)
    ///
    string as_xml( string name = string() ) const;


    /// Returns true iff this schema has the same field types in the
    /// same order as another TupleDescription.
    ///
    bool same_field_types( const TupleDescription&  other ) const;


    /// Parses a semicolon-separated list of tuples.
    ///
    string tuples_from_string( string  in ) const
                      throw( AuroraException );

    NMSTL_SIMPLY_SERIALIZABLE( TupleDescription, << _rep );


private:

    /// Prohibit comparison
    ///
    bool operator == (const TupleDescription&  other) const;
    bool operator != (const TupleDescription&  other) const;

    struct Rep
    {
        CatalogSchema      *_local_schema;
        uint16              _size;     // move into CatalogSchema???


        Rep( string name = string(),
             flags flags = flags()
           ) :
             _size(flags & NO_HEADER ? 0 : get_tuple_header_size())
        {
            _local_schema = new CatalogSchema;
            _local_schema->set_schema_name( name );
        }

        Rep( CatalogSchema  *schema,
             flags   flags = flags()
           ) :
            _size(flags & NO_HEADER ? 0 : get_tuple_header_size())
        {
            _local_schema = schema;
        }

      NMSTL_SIMPLY_SERIALIZABLE(Rep,
                    << _local_schema->_name << _local_schema->_field << _size);
    };

    ptr<Rep>   _rep;
    flags      _flags;
};

#ifndef DOXYGEN_SKIP
template<typename T> class data_type_for;
template<> class data_type_for<int8>   { public: operator DataType() const { return DataType::BYTE; } };
template<> class data_type_for<int16>  { public: operator DataType() const { return DataType::SHORT; } };
template<> class data_type_for<int32>  { public: operator DataType() const { return DataType::INT; } };
template<> class data_type_for<int64>  { public: operator DataType() const { return DataType::LONG; } };
template<> class data_type_for<single> { public: operator DataType() const { return DataType::SINGLE; } };
template<> class data_type_for<double> { public: operator DataType() const { return DataType::DOUBLE; } };
template<> class data_type_for<string> { public: operator DataType() const { return DataType::STRING; } };


//  Change to SchemaField???
template<typename T>
class Field : public SchemaField
{
  public:
    Field( string name = string() ) :
        SchemaField( name, data_type_for<T>(), sizeof( T ))
    {}
};


template<>
class Field<string> : public SchemaField
{
  public:
    Field( string  name, unsigned int  length ) :
        SchemaField( name, DataType::STRING, length )
    {}

    Field( unsigned int  length ) :
        SchemaField( string(), DataType::STRING, length )
    {}
};

#endif                       // DOXYGEN_SKIP

BOREALIS_NAMESPACE_END
#endif                       // TUPLEDESCRIPTION_H
