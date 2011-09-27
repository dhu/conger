#include  "TupleHeader.h"

BOREALIS_NAMESPACE_BEGIN


// The size of the tuple header changes depending on which optional
// components are in use.  By default all optional fields are included.
//
uint8    HEADER_SIZE = 53;


// When True the header size can not be reduced.  The size can not be
// reduced after the first ime it has been set or accessed.
//
Boolean  header_is_irreducable = False;



// Reduce the size of the tuple header.  The size can be reduced once
// before any streams are processed by the application by sending a
// component XML element to the QueryProcessor via the Head.
// 
void  reduce_tuple_header_size(uint8  size)
{
    if (header_is_irreducable)
    {   FATAL << "Can not reduce the tuple header after it has been accessed.";
    }

    header_is_irreducable = True;

    HEADER_SIZE = size;

    return;
}



// Get the size of the tuple header.  This can only be accessed after the
// QueryProcessor has had an opportunity to reduce the header size.
//
uint8  get_tuple_header_size()
{
    header_is_irreducable = True;

    return(HEADER_SIZE);
}


BOREALIS_NAMESPACE_END
