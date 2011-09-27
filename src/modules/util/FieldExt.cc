#include "FieldExt.h"
#include "nmstl_util.h"
#include "TupleDescription.h"

BOREALIS_NAMESPACE_BEGIN

FieldExt::FieldExt( int32 port, char type, int32 offset, int32 size )
{
    _port = port;
    _offset = offset + HEADER_SIZE;
    _size = size;
    _type = type;
}


FieldExt::FieldExt ( const string  ident_string )
{
    init_from_string( ident_string.c_str() );
}


FieldExt::FieldExt ( const char  *ident_string )
{
    init_from_string( ident_string );
}


void FieldExt::init_from_string( const char  *ident_string )
{
    // String will look like ":port:offset:size:type:"
    char *tmp;
    char *pos = const_cast<char *>(ident_string);
    string error = "unknown error";
    int32  val;

    if ( *pos != ':' )
    {   error = "missing first colon";
        goto die;
    }

    pos++; // Skip :
    val = strtol( pos, &tmp, 10 );

    if ( tmp == pos || val > INT_MAX || val < 0 )
    {   error = "bad port";
        goto die;
    }

    pos = tmp;
    _port = static_cast<int32>(val);

    if ( *pos != ':' )
    {   error = "missing second colon, at \"" + string(pos) + "\"";
        goto die;
    }

    pos++; // Skip :
    // TODO: check_type
    _type = *pos;

    if ( _type != 'i' && _type != 'L'  &&
         _type != '1' && _type != '2'  &&
         _type != 's' && _type != 't'  &&
         _type != 'f' )                           // bb:  Depricated
    {
        error = "invalid field type";
        goto die;
    }

    pos++; // Skip type.

    if ( *pos != ':' )
    {   error = "missing third colon";
        goto die;
    }

    pos++; // Skip :
    val = strtol( pos, &tmp, 10 );
    // offset can be less than 0 -- used for fake FieldExt for ORDER_BY_TUPLENUM

    if ( tmp == pos || val > INT_MAX )                // || val < 0)
    {   error = "bad offset";
        goto die;
    }

    pos = tmp;
    _offset = static_cast<int32>(val) + HEADER_SIZE;

    if ( *pos != ':' )
    {   error = "missing fourth colon";
        goto die;
    }

    pos++; // Skip :
    val = strtol( pos, &tmp, 10 );

    if ( tmp == pos || val > INT_MAX || val < 0 )
    {   error = "bad size";
        goto die;
    }

    pos = tmp;
    // NOTE!!!!!!!!!!! I THINK _size SHOULD BE + 1 WHEN THE TYPE IS "s" (string)
    // SINCE evaluate() RETURNS THE STRING AND A NULL CHARACTER!
    // - eddie
    _size = static_cast<int32>( val );

    if ( *pos != ':' )
    {   error = "missing fifth colon";
        goto die;
    }

    pos++; // skip last :

    if ( *pos != '\0' )
    {   error = "string doesn't end after fifth colon";
        goto die;
    }

    return;

  die:
    // TODO: Throw exception or something more proper.
    Throw( AuroraException,
           string() + "Bad format string for field ('"
                    + ident_string + "'): " + error );
}


FieldExt::~FieldExt() {}

char* FieldExt::evaluate(char* tuple1, char *tuple2)
{
    if ( _port == 0 )
    {   return( evaluate( tuple1 ));
    }
    else
    {   return( evaluate( tuple2 ));
    }
}


char* FieldExt::evaluate( char*  tuple )
{
    char  *result;
    int32  i = 0;

    if ( _type == 's' )
    {   result = new char [ _size + 1 ];
    }
    else
    {   result = new char [ _size ];
    }

    while ( i < _size )
    {   result[ i ] = tuple[ _offset + i ];
        i++;
    }

    if ( _type == 's' )
    {   result[_size] = '\0';
    }

    return( result );
}

// Will return as a stream of chars THAT ARE NOT NULL TERMINATED!!!
// It will also tell the size of the returned
char* FieldExt::evaluate_as_char( char  *tuple, int32  &return_size )
{
    return( evaluate_as_char( tuple, return_size, false ));
}


// This version can, optionally, null terminate the returned char*
char* FieldExt::evaluate_as_char( char   *tuple,
                                int32  &return_size,
                                bool    addnull
                              )
{
    char* result;

    // For the string type, easy, we know the size in bytes, just copy
    if ( _type == 's' )
    {
        result = (char*) malloc((addnull) ? _size + 1 :_size);
        //result = new char [_size];
        memcpy(result,&tuple[_offset],_size);

        if (addnull) result[_size] = '\0';
        return_size = (addnull) ? _size + 1 : _size;
    }
    else
    {
        // Why 384? precision of 12 for doubles and the left
        //  number maximum of say 310 digits (see IEEE max double value
        //  is about 1.8 * 10^308)... so 384 chars should be enough
        //
        result = (char*) malloc(384);

        // Length of the sprintf output.
        // Note that sprintf does not count the trailing zero.
        //
        int32  nchars = 0;

        if ( _type == 'i' )
        {   nchars = sprintf( result, "%d", (int)*((int32*)&tuple[ _offset ]));
        }
        else if ( _type == 'L' )
        {   nchars = sprintf( result, "%" I64(d), *((int64*)&tuple[ _offset ]));
        }
        else if ( _type == 'S' )
        {   nchars = sprintf( result, "%d", *((int16*)&tuple[ _offset ]));
        }
        else if ( _type == 'B' )
        {   nchars = sprintf( result, "%d", *((int8*)&tuple[ _offset ]));
        }
        else if ( _type == '1' )
        {   nchars = sprintf( result, "%.6f", *((single*)&tuple[ _offset ]));
        }
        else if ( _type == '2' )
        {   nchars = sprintf( result, "%.12f", *((double*)&tuple[ _offset ]));
        }
        else if ( _type == 't' )
        {   Tuple  t = Tuple::factory(&tuple[_offset]);

            nchars = sprintf(result, "%d,%d", (int)t.get_timestamp().tv_sec,
                                              (int)t.get_timestamp().tv_usec);

            // hard coded???
            //nchars = sprintf( result, "%d,%d",
            //                  (int)((timeval*) &tuple[_offset])->tv_sec,
            //                  (int)((timeval*) &tuple[_offset])->tv_usec );
        }

        // Now take just what we need...
        result = (char*) realloc( result, (addnull) ? nchars + 1 : nchars );
        if (addnull) result[ nchars ] = '\0';
        return_size = (addnull) ? nchars + 1 : nchars;
    }

    return( result );
}


// Evaluate the FieldExt and put the value into the position
// specified by loc. Loc should be large enough.
// This is used by SimpleAggBox.
//
void FieldExt::evaluate_into_location( char  *tuple, char  *loc )
{
    memcpy( loc, tuple + _offset, _size );
}


// NOTE!!!!!!!!!!! I THINK _size SHOULD BE + 1 WHEN THE TYPE IS "s" (string)
// SINCE evaluate() RETURNS THE STRING AND A NULL CHARACTER!
// NOTE 2: I HAVE SEEN, HOWEVER, ASSUMPTIONS IN OTHER PLACES OF CODE THAT ADD ONE
//  TO THE RETURN VALUE FROM HERE SO........
// BUT MY REASONING AND ANGER STANDS!!!!! evaluate() ADDS THE \0 !!! SO YOU MUST NOT
// LIE HERE BY SAYING 1 less THAN WHAT IT REALLY IS!!!!!!!!
//
int32 FieldExt::get_returned_size()
{   return( _size );
}


char FieldExt::getType()
{   return( _type );
}


int32 FieldExt::get_offset()
{   return( _offset );
}


void FieldExt::set_port( int32  i )
{   _port = i;
}


vector<DataType> FieldExt::get_return_types()
{
    if      ( _type == 'i' ) return vector<DataType>( 1, DataType::INT );
    else if ( _type == 'L' ) return vector<DataType>( 1, DataType::LONG );
    else if ( _type == 'S' ) return vector<DataType>( 1, DataType::SHORT );
    else if ( _type == 'B' ) return vector<DataType>( 1, DataType::BYTE );
    else if ( _type == '1' ) return vector<DataType>( 1, DataType::SINGLE );
    else if ( _type == '2' ) return vector<DataType>( 1, DataType::DOUBLE );
    else if ( _type == 's' ) return vector<DataType>( 1, DataType::STRING );
    else if ( _type == 't' ) return vector<DataType>( 1, DataType::TIMESTAMP);
    else
    {
        cout << "FieldExt : bad type on getReturnTypes() call" << endl;
        exit( 1 );
    }
}


string FieldExt::as_string() const
{
    return( "FieldExt{" + to_string( _offset ) + " " +
            to_string( _type ) + "(" + to_string( _size ) + ")}" );
}


BOREALIS_NAMESPACE_END
