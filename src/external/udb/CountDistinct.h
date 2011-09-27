#ifndef  COUNTDISTINCT_H
#define  COUNTDISTINCT_H

#include "Aggregate.h"
#include <ext/hash_map>

BOREALIS_NAMESPACE_BEGIN

using __gnu_cxx::hash_map;


/**
 * This function counts distinct values seen on a given field
 *
*/

class CountDistinctWindow : public AggregateWindow
{
    map<string, int> _unique_values;

    void init()
    {   _unique_values.clear();
    }

    void incr( const EvalContext  &ctxt ) throw( EvalException )
    {
        // Retrieve carid of report
        string  value = arg(0).eval<string>( ctxt );
        _unique_values[ value ] += 1;
    }

    void final( char  *out )
    {   *(int*)out = _unique_values.size();
    }

    virtual const string as_string() const;

    NMSTL_SERIAL_SUBCLASS(CountDistinctWindow, AggregateWindow, << _unique_values);
};


class CountDistinct : public Aggregate
{

  public:
    AggregateWindow *open_window()
    {   return( new CountDistinctWindow );
    }

    void setup( const ExprContext  &ctxt )
         throw( AuroraException );

    NMSTL_SERIAL_SUBCLASS(CountDistinct, Aggregate, );

};


BOREALIS_NAMESPACE_END

#endif                     // COUNTDISTINCT_H
