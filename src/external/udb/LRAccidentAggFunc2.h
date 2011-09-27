#ifndef  LRACCIDENTAGGFUNC2_H
#define  LRACCIDENTAGGFUNC2_H

#include "Aggregate.h"

BOREALIS_NAMESPACE_BEGIN

class LRAccidentAggFunc2 : public Aggregate
{
    struct my_window : public AggregateWindow
    {

        bool   _first;
        bool   _accident;
        int32  _carid;

        void init();

        void incr( const EvalContext  &ctxt ) throw( EvalException );

        void final( char *out );
    };

    AggregateWindow *open_window() { return( new my_window ); }

    void setup( const ExprContext  &ctxt )
         throw( AuroraException );
};


BOREALIS_NAMESPACE_END

#endif                     // LRACCIDENTAGGFUNC2_H
