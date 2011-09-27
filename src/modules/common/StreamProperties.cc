#include "StreamProperties.h"

BOREALIS_NAMESPACE_BEGIN

// Returns the name of the stream.
//
string StreamProperties::get_name() const
{   return( _name );
}

// Returns the schema of the stream.
//
TupleDescription StreamProperties::get_schema() const
{   return( _schema );
}

BOREALIS_NAMESPACE_END
