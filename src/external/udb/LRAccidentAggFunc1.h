#ifndef  LRACCIDENTAGGFUNC1_H
#define  LRACCIDENTAGGFUNC1_H

#include "Aggregate.h"

BOREALIS_NAMESPACE_BEGIN

class LRAccidentAggFunc1 : public Aggregate
{
    struct my_window : public AggregateWindow
    {
        bool   _first;
        bool   _stopped;
        int32  _expressway;
        int32  _direction;
        int32  _position;

        void init();

        void incr( const EvalContext  &ctxt ) throw( EvalException );

        void final( char  *out);
    };


    AggregateWindow *open_window() { return( new my_window ); }

    void setup( const ExprContext  &ctxt )
         throw( AuroraException );
};


BOREALIS_NAMESPACE_END

#endif                     // LRACCIDENTAGGFUNC1_H
