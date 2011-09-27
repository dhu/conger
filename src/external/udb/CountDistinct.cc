#include "CountDistinct.h"

BOREALIS_NAMESPACE_BEGIN

using __gnu_cxx::hash_map;

/**
 * This function counts distinct values seen on a given field
 *
*/


////////////////////////////////////////////////////////////////////////////////
//
const string CountDistinctWindow::as_string() const
{
    string  s;
    bool    first = true;
//..............................................................................


    NOTICE << "hi";

    s += "CountDistinct[";
    s += AggregateWindow::as_string();
    s += ", unique values = {";

    for (map<string, int>::const_iterator i = _unique_values.begin(); i != _unique_values.end(); i++)
    {
        if (first)
        {   first = false;
        }
        else
        {   s += ", ";
        }

        s += "(";
        s += i->first;
        s += ", ";
        s += to_string(i->second);
        s += ")";
    }

    s += "}]";

    return s;
}



////////////////////////////////////////////////////////////////////////////////
//
void CountDistinct::setup( const ExprContext  &ctxt )
                    throw( AuroraException )
{
//..............................................................................


    if ( num_args() != 1 )
    {   Throw( aurora_typing_exception, "Distinct count takes 1 argument" );
    }

    // Output type is integer
    add_int_32output_field( "count_distinct" );
}


NMSTL_SERIAL_DEFINE( CountDistinct, 10121);
NMSTL_SERIAL_DEFINE( CountDistinctWindow, 10131);

BOREALIS_NAMESPACE_END
