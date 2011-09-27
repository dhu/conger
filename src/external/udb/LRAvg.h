#ifndef  LRAVG_H
#define  LRAVG_H

#include "Aggregate.h"
#include <ext/hash_map>

BOREALIS_NAMESPACE_BEGIN

using __gnu_cxx::hash_map;

/**
 * This function computes the average, discarding anything where its first argument ("car count") is 0
 * LRAvg(-1,-1,-1,-1,-1) is -1
 *
*/
class LRAvg : public Aggregate
{
    struct my_window : public AggregateWindow
    {

        int     _cnt; // running count
        single  _sum; // running total

        hash_map<int, int> _tracker;

        void init();

        void incr( const EvalContext  &ctxt ) throw( EvalException );

        void final( char  *out );
    };


    AggregateWindow *open_window() { return( new my_window ); }


    void setup( const ExprContext  &ctxt )
         throw( AuroraException );
};


BOREALIS_NAMESPACE_END

#endif                     // LRAVG_H
