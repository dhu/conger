#ifndef  LRCARCOUNT_H
#define  LRCARCOUNT_H

#include "Aggregate.h"
#include <ext/hash_map>

BOREALIS_NAMESPACE_BEGIN

using __gnu_cxx::hash_map;

/**
 * This function is COUNT()ing how many reports have come in
 *  but not counting duplicates (based on carid) !
 * It is also tracking the average speed, making sure that
 *  when a car is seen twice, the speed for that car is the average of both the numbers
 * Note - when you see no cars, the average speed is "-1"
*/
class LRCarCount : public Aggregate
{
    struct my_window : public AggregateWindow
    {
        int     _cnt; // running count of unique car reports
        single  _sum; // running total of speeds (accounts for uniques,
                      // exploiting knowledge that at most 2 reports per car)
                      // it's a single to not lose any precision when we take
                      // the average of 2 readings of the same car.

        hash_map<int, int> _tracker;


        void init();

        void incr( const EvalContext  &ctxt ) throw( EvalException );

        void final( char *out );
    };


    AggregateWindow *open_window() { return( new my_window ); }

    void setup( const ExprContext  &ctxt )
         throw( AuroraException );
};


BOREALIS_NAMESPACE_END

#endif                     // LRCARCOUNT_H
