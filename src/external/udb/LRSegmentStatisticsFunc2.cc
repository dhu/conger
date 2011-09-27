#include "LRSegmentStatisticsFunc2.h"

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
void  LRSegmentStatisticsFunc2::my_window::init()
{
//..............................................................................


    _sum  = -1;
    _lcnt = -1;

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  LRSegmentStatisticsFunc2::my_window::incr( const EvalContext  &ctxt )
                                          throw( EvalException )
{
//..............................................................................


    // Retrieve values from tuple
    int32  sum  = arg(0).eval<int32>( ctxt );
    int32  lcnt = arg(1).eval<int32>( ctxt );

    _sum += sum;
    _lcnt = lcnt;

   return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  LRSegmentStatisticsFunc2::my_window::final( char  *out )
{
//..............................................................................


   // Output LAV (sum / 5), lcnt
   *(int32*)out = _sum / 5;
   *(int32*)(out + 4) = _lcnt;

   return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  LRSegmentStatisticsFunc2::setup( const ExprContext  &ctxt )
                                throw( AuroraException )
{
//..............................................................................


    if ( num_args() != 2 )
    {   Throw( aurora_typing_exception,
               "LRSegmentStatisticsFunc2 takes 2 arguments" );
    }
    else if (( arg(0)->getType() != DataType::INT )  &&
             ( arg(1)->getType() != DataType::INT ))
    {
        Throw( aurora_typing_exception,
               "LRSegmentStatisticsFunc2 requires 2 int arguments" );
    }

    // Output type
    add_int_32output_field();
    add_int_32output_field();

    return;
}


BOREALIS_NAMESPACE_END
