#include "EString.h"

//using namespace Borealis;
BOREALIS_NAMESPACE_BEGIN

/// A lightweight string class that performs no memory management.
/// Creating or assigning an EString never makes a copy of string data.
///
/// This class may be especially useful in conjuction with EStringPool.

/// Performs a three-way lexicographical comparison with other.
/// Returns -1 if this EString's contents are less than other's,
/// 0 if the EStrings are equivalent, or 1 if this EString's contents
/// are greater than other's.
///
int EString::compare( const EString  &other )
{
    int ret = __builtin_memcmp( _data, other._data,
                                min(_length, other._length ));

    if ( ret ) return( ret );

    if ( _length == other._length ) return( 0 );

    return( _length < other._length ? -1 : 1 );
}

/// Concatenates this EString and other.  Copies the
/// concatentation into the given pool and returns the resultant
/// EString.  This EString will remain valid until the pool is
/// destroyed or reset.
///
EString EString::concat( const EString  &other, EStringPool  &pool )
{
    char *out = pool.alloc( _length + other._length );
    unsigned int outlen = _length + other._length;

    __builtin_memcpy( out, _data, _length );
    __builtin_memcpy( out + _length, other._data, other._length );

    return EString( out, outlen );
}

/// Copies a string object into the given pool and returns the
/// resultant EString, which will remain valid until the pool is
/// destroyed or reset.
///
EString EString::save( const string  &str, EStringPool  &pool )
{   return( EString( str ).save( pool ));
}

/// Copies this EString into the given pool and returns the
/// resultant EString, which will remain valid until the pool is
/// destroyed or reset.
///
EString EString::save( EStringPool  &pool )
{
        char *out = pool.alloc(_length);
        __builtin_memcpy(out, _data, _length);
        return EString(out, _length);
}

/// Returns the index'th character in the EString.
char EString::operator[] ( unsigned int  index ) const
{   return( _data[ index ] );
}

/// Creates a string representation of an boolean in the given
/// pool and returns the resultant EString, which will remain
/// valid until the pool is destroyed or reset.
///
EString EString::coerce( bool  b, EStringPool  &pool )
{
    const int32 BUF_SIZE = 32;

    char *out = pool.alloc( BUF_SIZE );
    int outlen = snprintf( out, BUF_SIZE, "%d", b );
    if ( outlen < 0 || outlen >= BUF_SIZE ) outlen = 0;

    return( EString( out, outlen ));
}



/// Creates a string representation of an integer in the given
/// pool and returns the resultant EString, which will remain
/// valid until the pool is destroyed or reset.
///
EString EString::coerce( int8  i, EStringPool  &pool )
{
    const int BUF_SIZE = 32;

    char *out = pool.alloc( BUF_SIZE );
    int outlen = snprintf( out, BUF_SIZE, "%d", i );
    if ( outlen < 0 || outlen >= BUF_SIZE ) outlen = 0;

    return( EString( out, outlen ));
}



/// Creates a string representation of an integer in the given
/// pool and returns the resultant EString, which will remain
/// valid until the pool is destroyed or reset.
///
EString EString::coerce( int16  i, EStringPool  &pool )
{
    const int BUF_SIZE = 32;

    char *out = pool.alloc( BUF_SIZE );
    int outlen = snprintf( out, BUF_SIZE, "%d", i );
    if ( outlen < 0 || outlen >= BUF_SIZE ) outlen = 0;

    return( EString( out, outlen ));
}



/// Creates a string representation of an integer in the given
/// pool and returns the resultant EString, which will remain
/// valid until the pool is destroyed or reset.
///
EString EString::coerce( int32  i, EStringPool  &pool )
{
    const int BUF_SIZE = 32;

    char *out = pool.alloc( BUF_SIZE );

    // The cast fixes a gcc bug???
    int outlen = snprintf( out, BUF_SIZE, "%d", (int)i );

    if ( outlen < 0 || outlen >= BUF_SIZE ) outlen = 0;

    return( EString( out, outlen ));
}



/// Creates a string representation of an integer in the given
/// pool and returns the resultant EString, which will remain
/// valid until the pool is destroyed or reset.
///
EString EString::coerce( int64  i, EStringPool  &pool )
{
    const int BUF_SIZE = 32;

    char *out = pool.alloc( BUF_SIZE );
    int outlen = snprintf( out, BUF_SIZE, "%" I64(d) , i );
    if ( outlen < 0 || outlen >= BUF_SIZE ) outlen = 0;

    return( EString( out, outlen ));
}


/// Creates a string representation of a floating-point value in
/// the given pool and returns the resultant EString, which will
/// remain valid until the pool is destroyed or reset.
///
EString EString::coerce( single  f, EStringPool  &pool )
{
    const int BUF_SIZE = 32;

    char *out = pool.alloc( BUF_SIZE );
    int outlen = snprintf( out, BUF_SIZE, "%f", double( f ));
    if ( outlen < 0 || outlen >= BUF_SIZE ) outlen = 0;

    return( EString( out, outlen ));
}


/// Creates a string representation of a floating-point value in
/// the given pool and returns the resultant EString, which will
/// remain valid until the pool is destroyed or reset.
EString EString::coerce( double  f, EStringPool  &pool )
{
    const int BUF_SIZE = 32;

    char *out = pool.alloc( BUF_SIZE );
    int outlen = snprintf( out, BUF_SIZE, "%f", f );
    if ( outlen < 0 || outlen >= BUF_SIZE ) outlen = 0;

    return( EString( out, outlen ));
}

BOREALIS_NAMESPACE_END
