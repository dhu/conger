#include "TupleDescription.h"
#include "tupleGenerator.h"
#include "Tuple.h"

#include <cmath>

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
// Construct a TupleDescription from a CatalogSchema structure.
//

TupleDescription::TupleDescription(CatalogSchema   *schema,
                                   flags            flags)
    : _rep(new Rep(schema, flags))
{
    vector<SchemaField>  *field;
//..............................................................................


    field = &schema->_field;

    for (uint32  j = 0; j < field->size(); ++j)
    {
        //DEBUG  << "add field (" << (*field)[ j ].get_name() << ", "
        //                        << (*field)[ j ].get_type() << ", "
        //                        << (*field)[ j ].get_size() << ")";

        //DEBUG << "rep size = " << _rep->_size;

        _rep->_local_schema->set_field_offset(j, _rep->_size);
        _rep->_size += (*field)[j].get_size();
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
size_t TupleDescription::get_size() const
{
//..............................................................................


    return(_rep->_size);
}



////////////////////////////////////////////////////////////////////////////////
//
string TupleDescription::as_string() const
{
    string fields = "";
//..............................................................................


    vector<SchemaField>::iterator field_it  = _rep->_local_schema->_field.begin();
    vector<SchemaField>::iterator field_end = _rep->_local_schema->_field.end();

    for( ; field_it != field_end; ++field_it)
    fields += field_it->as_string()+" ";

    return("TupleDescription{name=" + to_string(_rep->_local_schema->_name)
                      + "; fields=" + fields
                        + "; size=" + _rep->_size + "}");
}



////////////////////////////////////////////////////////////////////////////////
//
int TupleDescription::index_of_field(string  name) const
{
//..............................................................................


    for ( unsigned int i = 0; i < _rep->_local_schema->_field.size(); ++i )
    {   if ( _rep->_local_schema->get_field_name( i ) == name )
        {   return( i );
        }
    }

    return(-1);
}



////////////////////////////////////////////////////////////////////////////////
//
string TupleDescription::generate_tuple() const
{
//..............................................................................


    char *data = TupleGenerator::generate_tuple(
                     const_cast<TupleDescription*>(this));

    string ret(data, get_size());
    free(data);

    return(ret);
}



////////////////////////////////////////////////////////////////////////////////
//
string  TupleDescription::tuple_to_string(const void  *buf) const
{
    string  out = "(";
    uint16  field_index = 0;

    vector<SchemaField>::const_iterator i;
//..............................................................................


    Tuple  tuple = Tuple::factory(buf);

    out << "time="         << tuple.get_timestamp().tv_sec
                    << '+' << tuple.get_timestamp().tv_usec;

    out << " local_time="  << tuple.get_local_timestamp().tv_sec
                    << '+' << tuple.get_local_timestamp().tv_usec;

    out << " tuple_id="    << tuple.get_tuple_id();

    out << " tuple_type="  << tuple.get_tuple_type();

    out << " revision_id=" << tuple.get_revision_id();

    out << " quality="     << tuple.get_quality();

    out << " win_spec="    << tuple.get_win_spec();

    out << " fake_flag="   << tuple.get_fake_flag();

    out << " array_flag="  << tuple.get_array_flag();

    out << " rev_field="   << tuple.get_revised();

    out << " stime="       << tuple.get_tuple_stime().tv_sec
                    << '+' << tuple.get_tuple_stime().tv_usec;
 

    for (i  = _rep->_local_schema->_field.begin();
         i != _rep->_local_schema->_field.end(); ++i)
    {
        out << ",";
        out << i->get_name() << "=";
            
        const void *data = ((const char*)buf) + get_offset( field_index++ );

        // HACK: Display revised_value as int for demo purposes
        //
        if(i->get_name() == "revised_value")
        {
            out += to_string( *(int32*)data);
        }
        else
        {   if (i->get_type() == DataType::INT) 
            {   out += to_string(*(int32*)data);
            }
            else if (i->get_type() == DataType::LONG)
            {   out += to_string(*(int64*)data);
            }
            else if (i->get_type() == DataType::SHORT)
            {   out += to_string(*(int16*)data);
            }
            else if (i->get_type() == DataType::BYTE)
            {   out += to_string((int32)(*(int8*)data));
            }
            else if (i->get_type() == DataType::SINGLE)
            {   out += to_string(*(single*)data);
            }
            else if (i->get_type() == DataType::DOUBLE)
            {   out += to_string(*(double*)data);
            }
            else if (i->get_type() == DataType::TIMESTAMP)
            {   out << ((timeval*)data)->tv_sec << '+'
                    << ((timeval*)data)->tv_usec;
            }
            else if (i->get_type() == DataType::ARRAY)
            {   out << to_string(*(int32*)data);
            }
            else
            {   const char *ch = (const char *)data;
                const char *end = ch + i->get_size();

                while (( ch != end )  &&  *ch ) ++ch;

                out << "\"" << to_escaped_string(data, ch - (const char *)data)
                    << "\"";
            }
        }
    }

    out += ")";

    return( out );
}



////////////////////////////////////////////////////////////////////////////////
//
string TupleDescription::tuple_to_string( const Tuple  &tup ) const
{
//..............................................................................


    return( tuple_to_string( tup.get_data() ));
}



////////////////////////////////////////////////////////////////////////////////
//
string TupleDescription::tuple_data_to_string( const void* buf ) const
{
    string  out = "(";
    uint16  field_index = 0;

    vector<SchemaField>::const_iterator  i;
//..............................................................................


    for (i  = _rep->_local_schema->_field.begin();
         i != _rep->_local_schema->_field.end(); ++i)
    {
        out << i->get_name() << "=";
            
        const void *data = ((const char*)buf) + get_offset( field_index++ );

        // HACK: Display revised_value as int for demo purposes
        //
        if(i->get_name() == "revised_value")
        {
            out += to_string( *(int32*)data);
        }
        else
        {   if ( i->get_type() == DataType::INT )
            {   out += to_string( *(int32*)data );
            }
            else if ( i->get_type() == DataType::LONG )
            {   out += to_string( *(int64*)data );
            }
            else if ( i->get_type() == DataType::SHORT )
            {   out += to_string( *(int16*)data );
            }
            else if ( i->get_type() == DataType::BYTE )
            {   out += to_string( (int32)(*(int8*)data) );
            }
            else if ( i->get_type() == DataType::SINGLE )
            {   out += to_string( *(single*)data );
            }
            else if ( i->get_type() == DataType::DOUBLE )
            {   out += to_string( *(double*)data );
            }
            else if ( i->get_type() == DataType::TIMESTAMP )
            {   out << ((timeval*)data)->tv_sec << '+'
                    << ((timeval*)data)->tv_usec;
            }
            else
            {   const char *ch = (const char *)data;
                const char *end = ch + i->get_size();

                while (( ch != end )  &&  *ch ) ++ch;

                out << "\"" << to_escaped_string(data, ch - (const char *)data)
                    << "\"";
            }
        }
        out << ",";
    }

    out[out.length() - 1] = ')';

    return( out );
}



////////////////////////////////////////////////////////////////////////////////
//
string TupleDescription::as_xml(string  name) const
{
    string  out;
//..............................................................................


    out << "<schema";

    name = to_string( _rep->_local_schema->_name ).empty()
             ? name : to_string(_rep->_local_schema->_name );

    if ( !name.empty() )
    {   out << " name=\"" << xml_escape(name) << "\"";
    }

    out << ">";

    for ( vector<SchemaField>::const_iterator i = _rep->_local_schema->_field.begin();
          i != _rep->_local_schema->_field.end(); ++i )
    {
        out << "<field name=\"" << xml_escape(i->get_name() ) << "\" type=\""
            << i->get_type() << "\"";

        if ( i->get_type() == DataType::STRING )
        {   out << " size=\"" << i->get_size() << "\"";
        }

        out << "/>";
    }

    out << "</schema>";

    return( out );
}



////////////////////////////////////////////////////////////////////////////////
//
bool TupleDescription::same_field_types(const TupleDescription  &other) const
{
    vector<SchemaField>::const_iterator i, j;
//..............................................................................


    if (_rep->_local_schema->_field.size() != other._rep->_local_schema->_field.size())
    {   return false;
    }

    for (i  = _rep->_local_schema->_field.begin(),
         j  = other._rep->_local_schema->_field.begin();
         i != _rep->_local_schema->_field.end();
             ++i, ++j)
    {
        ASSERT(j != other._rep->_local_schema->_field.end());

        if (i->get_type()  != j->get_type() ||
            i->get_size() != j->get_size())
        {
            return(false);
        }
    }

    ASSERT(j == other._rep->_local_schema->_field.end());

    return(true);
}



////////////////////////////////////////////////////////////////////////////////
//
string TupleDescription::tuples_from_string(string  in) const
                                      throw(AuroraException)
{
    string  out;
    int32   bytes = get_size();
    char    buf[bytes];

    vector<string>  tuples;
    vector<string>  fields;
//..............................................................................


    split(in, ';', tuples);

    for (vector<string>::const_iterator i = tuples.begin();
         i != tuples.end(); ++i)
    {
        if (i->empty()) continue;

        split(*i, ',' , fields);

        if (fields.size() > get_num_fields())
        {   Throw(AuroraException,
                  "Too many fields, expected " + to_string(get_num_fields())
                      + " but got " + to_string( fields.size()));
        }

        memset(buf, 0, bytes);

        for (unsigned int i = 0; i < fields.size(); ++i)
        {
            int offset = get_offset(i);
            DataType type = _rep->_local_schema->_field[i].get_type();

            if (type == DataType::INT)
            {   if (!to_scalar(fields[i], *(int32*)&buf[offset]))
                {   Throw(AuroraException,
                          "Invalid int \"" + to_escaped_string(fields[i])
                                           + "\"");
                }
            }
            else if (type == DataType::LONG)
            {   if (!to_scalar(fields[i], *(int64*)&buf[offset]))
                {   Throw(AuroraException,
                          "Invalid long \"" + to_escaped_string(fields[i])
                                            + "\"");
                }
            }
            else if (type == DataType::SHORT)
            {   if (!to_scalar(fields[i], *(int16*)&buf[offset]))
                {   Throw(AuroraException,
                          "Invalid short \"" + to_escaped_string(fields[i])
                                             + "\"");
                }
            }
            else if (type == DataType::BYTE)
            {   if (!to_scalar(fields[i], *(int8*)&buf[offset]))
                {   Throw(AuroraException,
                          "Invalid byte \"" + to_escaped_string(fields[i])
                                            + "\"");
                }
            }
            else if (type == DataType::SINGLE)
            {   if (!to_scalar(fields[i], *(single*)&buf[offset]))
                {    Throw(AuroraException,
                           "Invalid single \"" + to_escaped_string(fields[i])
                                               + "\"");
                }
            }
            else if (type == DataType::DOUBLE)
            {   if (!to_scalar(fields[i], *(double*)&buf[offset]))
                {    Throw(AuroraException,
                           "Invalid double \"" + to_escaped_string(fields[i])
                                               + "\"");
                }
            }
            else if (type == DataType::STRING)
            {   memcpy(&buf[offset], fields[i].data(),
                       min(fields[i].length(),
                           (size_t)_rep->_local_schema->_field[i].get_size()));
            }
            else if (type == DataType::TIMESTAMP)
            {   double val;

                if (!to_scalar(fields[i], val))
                {   Throw(AuroraException,
                          "Invalid timestamp \""
                              + to_escaped_string(fields[i]) + "\"");
                }

                *(Timestamp*)&buf[offset] = Timestamp(long(val),
                                              long((val - floor(val)) * 1.0e6));
            }
            else
            {   Throw(AuroraException,
                      "Unknown datatype " + to_string(type));
            }
        }

        out.append(buf, bytes);
    }

    return(out);
}


BOREALIS_NAMESPACE_END
