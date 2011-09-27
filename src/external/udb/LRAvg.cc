#include "LRAvg.h"

BOREALIS_NAMESPACE_BEGIN

using __gnu_cxx::hash_map;

/**
 * This function computes the average, discarding anything where its first argument ("car count") is 0
 * LRAvg(-1,-1,-1,-1,-1) is -1
 *
*/


////////////////////////////////////////////////////////////////////////////////
//
void  LRAvg::my_window::init()
{
//..............................................................................


    _cnt = 0;
    _sum = 0;

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  LRAvg::my_window::incr( const EvalContext  &ctxt )
                       throw( EvalException )
{
//..............................................................................


    // Retrieve carid of report
    single  avgspeed = arg(0).eval<single>(ctxt);

    if ( avgspeed == -1 ) return; // Do not include data points that represent "undefined" average speeds

    ++_cnt;
    _sum += avgspeed;

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  LRAvg::my_window::final( char  *out )
{
//..............................................................................


    if ( _cnt == 0 )
    {   *(single*)out = -1.0; // No valid data points --> -1 lav
    }
    else
    {   *(single*)out = ( _sum + 0.0 ) / ( _cnt + 0.0 );
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  LRAvg::setup( const ExprContext  &ctxt )
             throw( AuroraException )
{
//..............................................................................


    if ( num_args() != 1 )
    {   Throw( aurora_typing_exception, "LRAvg takes 1 argument" );
    }
    else if ( arg(0)->getType() != DataType::SINGLE )
    {   Throw( aurora_typing_exception, "LRAvg requires one float argument" );
    }

    // Output type
    add_single_output_field();
}


BOREALIS_NAMESPACE_END
