#include  "parseutil.h"
#include  "DataType.h"

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
//  Convert the repesentation of a datatype to a lowercase string.
//
string DataType::as_string() const
{
//..............................................................................


return( _type == NONE      ? "none"      :
        _type == INT       ? "int"       :
        _type == LONG      ? "long"      :
        _type == SHORT     ? "short"     :
        _type == BYTE      ? "byte"      :
        _type == SINGLE    ? "single"    :
        _type == DOUBLE    ? "double"    :
        _type == STRING    ? "string"    :
        _type == TIMESTAMP ? "timestamp" :
        _type == ARRAY     ? "array"     :
        "unknown[" + to_string( static_cast<int>(_type) ) + "]" );
}



////////////////////////////////////////////////////////////////////////////////
//
//  Set a DataType representation from it's type name.
//  Return the field width or zero if the type is a string.
//
uint8  DataType::from_string( // Name of the type (case insensitive).
                              string  field
                            )
                       throw( AuroraBadEntityException )
{
    uint8     size;
    string    lower;
//
//  The size of a string type is unknown and zero is returned.
//..............................................................................


    lower = field;
    lowercase(lower);

    if (lower == "int")
    {   size  = sizeof(int32);
        _type = DataType::INT;
    }
    else if (lower == "long")
    {   size  = sizeof(int64);
        _type = DataType::LONG;
    }
    else if (lower == "byte")
    {   size  = sizeof(int8);
        _type = DataType::BYTE;
    }
    else if (lower == "short")
    {   size  = sizeof(int16);
        _type = DataType::SHORT;
    }
    else if (lower == "single")
    {   size  = sizeof(single);
        _type = DataType::SINGLE;
    }
    else if (lower == "double")
    {   size  = sizeof(double);
        _type = DataType::DOUBLE;
    }
    else if (lower == "string")
    {   size  = 0;
        _type = DataType::STRING;
    }
    else if (lower == "array")
    {   size  = sizeof(int32);
        _type = DataType::ARRAY;
    }
    else if (lower == "timestamp")
    {   size  = sizeof(timeval);
        _type = DataType::TIMESTAMP;
    }
    else
    {   Throw(AuroraBadEntityException,
              "Unknown field type:  " + field);
    }

    return(size);
}



////////////////////////////////////////////////////////////////////////////////
//
//  Convert the repesentation of a datatype to a C type.
//
string DataType::c_type() const
{
//..............................................................................


    return(_type == NONE      ? "void"      :
           _type == INT       ? "int32"     :
           _type == LONG      ? "int64"     :
           _type == SHORT     ? "int16"     :
           _type == BYTE      ? "int8"      :
           _type == SINGLE    ? "single"    :
           _type == DOUBLE    ? "double"    :
           _type == STRING    ? "char"      :
           _type == ARRAY     ? "int32"     :
           _type == TIMESTAMP ? "timestamp" :
           "unknown[" + to_string( static_cast<int>(_type) ) + "]");
}

uint16 DataType::get_size() const
{
	return(_type == NONE      ? 0      :
           _type == INT       ? sizeof(int32)     :
           _type == LONG      ? sizeof(int64)     :
           _type == SHORT     ? sizeof(int16)     :
           _type == BYTE      ? sizeof(int8)      :
           _type == SINGLE    ? sizeof(single)    :
           _type == DOUBLE    ? sizeof(double)    :
           _type == STRING    ? sizeof(char)      :
           _type == ARRAY     ? sizeof(int32)     :
           _type == TIMESTAMP ? sizeof(timeval) :
           0);
}
BOREALIS_NAMESPACE_END
