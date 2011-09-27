#ifndef  FIDCOMPLIANCELOOKTHROUGHANDAGG_H
#define  FIDCOMPLIANCELOOKTHROUGHANDAGG_H

#include "Aggregate.h"

BOREALIS_NAMESPACE_BEGIN

class FidComplianceLookthroughAndAgg : public Aggregate
{
    struct my_window : public AggregateWindow
    {
        bool result;

        void init() { result = false; }

        void incr(const EvalContext& ctxt) throw (EvalException)
        {
            int  arg0 = arg(0).eval<int32>( ctxt );
            result = ( result  &&  (bool)arg0 );
        }

        void final( char  *out )
        {   *(int*)out = (result)? 1 : 0;
        }
    };

    AggregateWindow *open_window() { return( new my_window ); }

    void setup( const ExprContext  &ctxt )
         throw( AuroraException );
};


BOREALIS_NAMESPACE_END

#endif                     // FIDCOMPLIANCELOOKTHROUGHANDAGG_H
