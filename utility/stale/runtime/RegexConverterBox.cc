#include "RegexConverterBox.h"

BOREALIS_NAMESPACE_BEGIN

RegexConverterBox::RegexConverterBox( TupleDescription  schema,
                                                  char  delim,
                                                string  regex
                                    )
                               throw( AuroraException )
                             : SimpleConverterBox( schema, delim ),
                               _regex( regex ),
                               _preg( new regex_t )
{
    int stat = regcomp( _preg.get(), regex.c_str(), REG_EXTENDED );

    if ( stat )
    {
        char buf[1024];
        regerror( stat, _preg.get(), buf, sizeof buf );
        _preg.reset();

        Throw( AuroraException,
               "Bad regular expression /" + regex + "/: " + buf );
    }
}

RegexConverterBox::~RegexConverterBox()
{
    if ( _preg )
    {   regfree( _preg.get() );
    }
}

bool RegexConverterBox::convert_tuple( const char  *data,
                                    unsigned int   length,
                                            char  *out
                                    )
                               throw( AuroraException )
{
    string datastr( data, length );

    static const unsigned int PMATCH_COUNT = 16;
    regmatch_t pmatch[PMATCH_COUNT];

    int stat = regexec( _preg.get(), datastr.c_str(),
                        PMATCH_COUNT, pmatch, 0 );

    DEBUG << "Trying to match \"" + to_escaped_string( data, length )
                                  + "\" to /" + _regex + "/";

    if ( stat == REG_NOMATCH )
    {
        INFO << "Input tuple \"" + to_escaped_string( data, length )
                                 + "\" didn't match regex /" + _regex + "/";

        return( false );
    }

    if ( stat )
    {   char buf[1024];

        regerror( stat, _preg.get(), buf, sizeof buf );

        Throw( AuroraException,
               "Unable to match input tuple \""
                   + to_escaped_string( data, length ) + "\" to regex /"
                   + _regex + "/: " + buf );
    }

    DEBUG << " - matched";

    _subexprs.clear();

    for ( unsigned int i = 1; i < PMATCH_COUNT && pmatch[i].rm_so != -1; ++i )
    {
        _subexprs.push_back( Subexpr( data + pmatch[i].rm_so,
                                      data + pmatch[i].rm_eo ));

        DEBUG << "   - subexpr #" << i << " is " << _subexprs.back();
    }

    return( convert_tuple_from_regex( Subexpr( data + pmatch[0].rm_so,
                                            data + pmatch[0].rm_eo ),
                                   _subexprs,
                                   out ));

    return( false );
}


bool RegexConverterBox::Subexpr::to_string_field( char  *field,
                                        unsigned int   size
                                              ) const
{
    string::size_type to_copy = min( string::size_type( size ),
                                     string::size_type( end - begin ));

    copy( begin, begin + to_copy, field );
    fill( field + to_copy, field + size, ' ' );

    return( int( size ) >= end - begin );
}


int32 RegexConverterBox::Subexpr::to_int() const
                                  throw( AuroraException )
{
    int32  ret;

    if ( !( to_scalar( string( begin, end ), ret )))
    {   Throw( AuroraException,
               "Invalid integer string \""
                   + to_escaped_string( NMSTL::to_string( *this )) + "\"" );
    }

    return( ret );
}


int64 RegexConverterBox::Subexpr::to_long() const
                                   throw( AuroraException )
{
    int64  ret;

    if ( !( to_scalar( string( begin, end ), ret )))
    {   Throw( AuroraException,
               "Invalid integer string \""
                   + to_escaped_string( NMSTL::to_string( *this )) + "\"" );
    }

    return( ret );
}


single RegexConverterBox::Subexpr::to_single() const
                                    throw( AuroraException )
{
    single  ret;

    if ( !( to_scalar( string( begin, end ), ret )))
    {   Throw( AuroraException,
               "Invalid single float string \""
                   + to_escaped_string( NMSTL::to_string( *this )) + "\"" );
    }

    return( ret );
}


double RegexConverterBox::Subexpr::to_double() const
                                    throw( AuroraException )
{
    double ret;

    if ( !( to_scalar( string( begin, end ), ret )))
    {   Throw( AuroraException,
               "Invalid double float string \""
                   + to_escaped_string( NMSTL::to_string( *this )) + "\"" );
    }

    return( ret );
}


BOREALIS_NAMESPACE_END
