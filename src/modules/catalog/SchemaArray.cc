#include  "SchemaArray.h"
#include  "CatalogSchema.h"
#include  "parseutil.h"

BOREALIS_NAMESPACE_BEGIN



////////////////////////////////////////////////////////////////////////////////
//
// Add each dimension from left to right.
//
SchemaArray::DimensionInformation   SchemaArray::parse_dimension
                                (
                                 // Min bound:  ['+' | '-'] <digit>+
                                 // "0" if a sparse dimension.
                                 //
                                 string    lower,

                                 // Max bound:  { (['+' | '-'] <digit>+)
                                 // Unbounded:  | '*'
                                 //    Sparse:  | ('int' | 'string:' <digit>+)
                                 //             }
                                 //
                                 string    upper
                                )
                           throw(AuroraBadEntityException)
{
    int32    lo;
    int32    up = 0;
    int16    sparse = 0;

    string::size_type  index;

    string   message;
//
//  Exit:  _variable_size is changed to True if a sparse of unbounded dimension.
//         It is unchanged if this is a dense dimension.
//..............................................................................


    try
    {   lo = string_to_int(lower);

        if (upper == "int")
        {   lo     = 0;
            sparse = -1;

            _variable_size = True;
        }
        else if (upper.substr(0, 7) == "string:")
        {   lo    = 0;
            index = 7;
            while (isdigit(upper[index])) { index++; }

            sparse = string_to_int(upper.substr(7, index - 7));
            _variable_size = True;
        }
        else if (upper != "*")
        {   up = string_to_int(upper);
        }
        else if (!_variable_size)
        {   up = lo;
            _variable_size = True;
        }
        else if (_array_specification.empty())
        {   message = "Only the leftmost upper bound can be variable.";
            DEBUG << message;
            Throw(AuroraBadEntityException, message);
        }
        else
        {   message = "An array can not have a variable size and have sparse dimensions.";
            DEBUG << message;
            Throw(AuroraBadEntityException, message);
        }
 
        //_array_specification.push_back(DimensionInformation(lo, up, sparse));
    }
    catch (exception &e)
    {   message = "An array bound must be an signed integer or a sparse index type.";
        Throw(AuroraBadEntityException, message);
    }

    return  DimensionInformation(lo, up, sparse);
}



////////////////////////////////////////////////////////////////////////////////
//
// Return the number of bytes used to store array data.
//
uint32  SchemaArray::array_size()
{
    uint32  size   = 1;           // Total number of dense elements.
    uint32  sparse = 0;           // Total width of all sparse indecies.

    vector<DimensionInformation>::iterator  d;
//..............................................................................


    //DEBUG << "type=" << _element_type << " size=" << _element_size;

    // Determine the number of elements in the array.
    //
    for (d  = _array_specification.begin();
         d != _array_specification.end(); d++)
    {
        if (d->_sparse == 0)
        {   size *= (uint32)(d->_upper - d->_lower) + 1;   // Dense dimension.
        }
        else if (d->_sparse < 0)
        {   sparse = sparse + 4;            // Width of a sparse integer index.
        }
        else
        {   sparse = sparse + d->_sparse;   // Width of a sparse string index.
        }
    }

    // Return the total number of bytes.
    //
    size = size * _element_size + sparse;
    DEBUG << "_element_size=" << _element_size << " sparse=" << sparse << " size=" << size;

    return  size;
}



////////////////////////////////////////////////////////////////////////////////
//
// Return the array type with bounds as:  <type> ( <bound>, ... )
//
string  SchemaArray::array_type()
{
    string  array;

    vector<DimensionInformation>::iterator  d;
//..............................................................................


    if (_element_type == DataType::NONE)
    {   array = _element_tuple->get_schema_name();
    }
    else
    {   array = to_string(_element_type);
    }

    array += "(";

    for (d  = _array_specification.begin();
         d != _array_specification.end(); d++)
    {
        array += to_string(d->_lower) + "..." + to_string(d->_upper) + ", ";
    }

    array = array.erase(array.size() - 2, 2);
    array += ")";
    DEBUG << "array=" << array;

    return  array;
}


BOREALIS_NAMESPACE_END
