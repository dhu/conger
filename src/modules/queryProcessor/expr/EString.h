#ifndef ESTRING_H
#define ESTRING_H

#include "common.h"
#include "EStringPool.h"

//using namespace Borealis;
BOREALIS_NAMESPACE_BEGIN


/// A lightweight string class that performs no memory management.
/// Creating or assigning an EString never makes a copy of string data.
///
/// This class may be especially useful in conjuction with EStringPool.
///
class EString
{
  public:
    /// Constructs an empty EString.  data() will return a null pointer.
    EString() : _data( "" ), _length( 0 ) {}

    /// Constructs an EString with the given content.  A copy of the
    /// data is <i>not</i> maintained, i.e., this EString will become
    /// invalid when the data pointer becomes invalid.
    EString( const char *data, unsigned int length ) :
             _data( data ), _length( length ) {}

    /// Constructs an EString with the given content.  A copy of the
    /// data is <i>not</i> maintained: this EString will become invalid
    /// as soon as the str object is destroyed.
    explicit EString( const string  &str ) :
              _data( str.data() ), _length( str.length() ) {}

    /// Returns a pointer to the EString's character data.
    const char *data() { return( _data ); }

    /// Returns the length of the EString.
    unsigned int length() { return( _length ); }

    /// Returns the contents of the EString as a string object.  (The
    /// string object <i>does</i> make a copy of the EString data.)
    string as_string() const
    {   return( _length ? string(_data, _length) : string()); }

    /// Performs a three-way lexicographical comparison with other.
    /// Returns -1 if this EString's contents are less than other's,
    /// 0 if the EStrings are equivalent, or 1 if this EString's contents
    /// are greater than other's.
    int compare( const EString  &other );

    /// Concatenates this EString and other.  Copies the
    /// concatentation into the given pool and returns the resultant
    /// EString.  This EString will remain valid until the pool is
    /// destroyed or reset.
    EString concat( const EString  &other, EStringPool  &pool );

    /// Copies a string object into the given pool and returns the
    /// resultant EString, which will remain valid until the pool is
    /// destroyed or reset.
    static EString save( const string  &str, EStringPool  &pool );

    /// Copies this EString into the given pool and returns the
    /// resultant EString, which will remain valid until the pool is
    /// destroyed or reset.
    EString save( EStringPool  &pool );

    /// Returns the index'th character in the EString.
    char operator[] ( unsigned int  index ) const;

    /// Returns true iff the EStrings' contents are identical.
    bool operator == ( const EString  &other )
    {   return( compare( other ) == 0); }

    /// Returns true iff the EStrings's contents are not identical.
    bool operator != ( const EString  &other )
    {   return( compare( other ) != 0 ); }

    /// Returns true iff this EString's contents are lexicographically
    /// less than other's.
    bool operator < ( const EString  &other )
    {   return( compare( other ) < 0 ); }

    /// Returns true iff this EString's contents are lexicographically
    /// greater than other's.
    bool operator > ( const EString  &other )
    {   return( compare( other ) > 0 ); }

    /// Returns true iff this EString's contents are lexicographically
    /// less than or equal to other's.
    bool operator <= ( const EString  &other )
    {   return( compare( other ) <= 0 ); }

    /// Returns true iff this EString's contents are lexicographically
    /// greater than or equal to other's.
    bool operator >= ( const EString  &other )
    {   return( compare( other ) >= 0 ); }

    /// Creates a string representation of an integer in the given
    /// pool and returns the resultant EString, which will remain
    /// valid until the pool is destroyed or reset.
    static EString coerce( bool  b, EStringPool  &pool );

    /// Creates a string representation of an integer in the given
    /// pool and returns the resultant EString, which will remain
    /// valid until the pool is destroyed or reset.
    static EString coerce( int8  i, EStringPool  &pool );

    /// Creates a string representation of an integer in the given
    /// pool and returns the resultant EString, which will remain
    /// valid until the pool is destroyed or reset.
    static EString coerce( int16  i, EStringPool  &pool );

    /// Creates a string representation of an integer in the given
    /// pool and returns the resultant EString, which will remain
    /// valid until the pool is destroyed or reset.
    static EString coerce( int32  i, EStringPool  &pool );

    /// Creates a string representation of an integer in the given
    /// pool and returns the resultant EString, which will remain
    /// valid until the pool is destroyed or reset.
    static EString coerce( int64  i, EStringPool  &pool );

    /// Creates a string representation of a floating-point value in
    /// the given pool and returns the resultant EString, which will
    /// remain valid until the pool is destroyed or reset.
    static EString coerce( single  f, EStringPool  &pool );

    /// Creates a string representation of a floating-point value in
    /// the given pool and returns the resultant EString, which will
    /// remain valid until the pool is destroyed or reset.
    static EString coerce( double  f, EStringPool  &pool );

  private:
    const char    *_data;
    unsigned int   _length;
};

BOREALIS_NAMESPACE_END
#endif                         // ESTRING_H
