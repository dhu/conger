#ifndef  LRSEGMENTSTATISTICSFUNC2_H
#define  LRSEGMENTSTATISTICSFUNC2_H

#include "Aggregate.h"

BOREALIS_NAMESPACE_BEGIN

class LRSegmentStatisticsFunc2 : public Aggregate
{
    struct my_window : public AggregateWindow
    {
        int32  _sum;
        int32  _lcnt;

        void init();

        void incr( const EvalContext  &ctxt ) throw( EvalException );

        void final( char  *out );
    };

    AggregateWindow *open_window() { return( new my_window ); }

    void setup( const ExprContext  &ctxt )
         throw( AuroraException );
};


BOREALIS_NAMESPACE_END

#endif                     // LRSEGMENTSTATISTICSFUNC2_H
