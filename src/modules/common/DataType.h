#ifndef DATATYPE_H
#define DATATYPE_H

#include <NMSTL/serial>

#include "nmstl_util.h"
#include "common.h"
#include "Exceptions.h"

BOREALIS_NAMESPACE_BEGIN

/// A type of data in a tuple.  Valid values are
///
/// - DataType::NONE (sentinel "null" value)
/// - DataType::BYTE
/// - DataType::SHORT
/// - DataType::INT
/// - DataType::LONG
/// - DataType::SINGLE
/// - DataType::DOUBLE
/// - DataType::STRING
/// - DataType::TIMESTAMP
/// - DataType::ARRAY
///
class DataType
{
  public:
    enum Rep
    {
        NONE      =  0,
        BYTE      =  5,
        SHORT     =  6,
        INT       =  7,
        LONG      =  8,
        SINGLE    =  9,
        DOUBLE    = 10,
        STRING    = 11,
        TIMESTAMP = 12,
        BOOL      = 13,
        ARRAY     = 14
    };

    DataType() : _type(NONE) {}
    DataType(Rep type) : _type(type) {}

    bool operator == (const DataType& t) const { return _type == t._type; }
    bool operator != (const DataType& t) const { return _type != t._type; }
    bool operator <  (const DataType& t) const { return _type <  t._type; }


    ///  Convert the repesentation of a datatype to a lowercase string.
    ///
    string  as_string() const;


    ///  Set a DataType representation from it's type name.
    ///  Return the field width.
    ///
    uint8  from_string(// Name of the type (case insensitive).
                       string  field)
                 throw(AuroraBadEntityException);

    ///  Convert the repesentation of a datatype to a c type.
    ///
    string  c_type() const;
	
	// get the DataType's size
	uint16 get_size() const;

  private:

    int    _type;
    NMSTL_SIMPLY_SERIALIZABLE(DataType, << _type);
};


inline bool operator == (const DataType& a, DataType::Rep b)
            { return a == DataType( b ); }

inline bool operator == (DataType::Rep b, const DataType& a)
            { return a == DataType( b ); }

BOREALIS_NAMESPACE_END
#endif                        // DATATYPE_H
