#ifndef  FIDLATETRACKERFUNC_H
#define  FIDLATETRACKERFUNC_H

#include "Aggregate.h"

BOREALIS_NAMESPACE_BEGIN

class FidLateTrackerFunc : public Aggregate
{
    struct my_window : public AggregateWindow
    {
        char _feed_store[1];
        char _exchange_store[2];
        int _cnt;

        void init() { _cnt = 0; }

        void incr( const EvalContext  &ctxt ) throw( EvalException );

        void final( char  *out );
    };

    AggregateWindow *open_window() { return( new my_window ); }

    void setup( const ExprContext  &ctxt )
         throw( AuroraException );
};

BOREALIS_NAMESPACE_END

#endif                     // FIDLATETRACKERFUNC_H
