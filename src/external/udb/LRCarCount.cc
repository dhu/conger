#include "LRCarCount.h"

BOREALIS_NAMESPACE_BEGIN

using __gnu_cxx::hash_map;

/**
 * This function is COUNT()ing how many reports have come in
 *  but not counting duplicates (based on carid) !
 * It is also tracking the average speed, making sure that
 *  when a car is seen twice, the speed for that car is the average of both the numbers
 * Note - when you see no cars, the average speed is "-1"
*/



////////////////////////////////////////////////////////////////////////////////
//
void  LRCarCount::my_window::init()
{
//..............................................................................

    _cnt = 0;
    _sum = 0;

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  LRCarCount::my_window::incr( const EvalContext  &ctxt )
                            throw( EvalException )
{
//..............................................................................


    // Retrieve carid of report
    int32 carid = arg(0).eval<int32>( ctxt );
    int32 speed = arg(1).eval<int32>( ctxt );

    if ( carid == -1 ) return;      // Ignore warmup tuples

    if ( _tracker.find( carid ) != _tracker.end() )
    {   // We found him, dupe record
        // ignore _cnt
        // but for _sum, subtract his previous reading, and add the average

        int lastspeed = _tracker[carid];
        _sum = _sum - lastspeed + ((lastspeed + speed + 0.0) / 2.0);

        // No need to update the hash because we only see a duplicate one time!!!!!!

    }
    else
    {   // First time we see this car
        ++_cnt;
        _sum += speed;
        _tracker[carid] = speed;
    }

    return;
}


////////////////////////////////////////////////////////////////////////////////
//
void  LRCarCount::my_window::final( char *out )
{
//..............................................................................

    // Output cnt
    *(int*)out = _cnt;

    if ( _cnt == 0 )
    {   *(single*)(out+4) = -1.0;
    }
    else
    {   *(single*)(out+4) = ( _sum + 0.0 ) / ( _cnt + 0.0 );
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  LRCarCount::setup( const ExprContext  &ctxt )
                  throw( AuroraException )
{
//..............................................................................


    if ( num_args() != 2)
    {   Throw(aurora_typing_exception, "LRCarCount takes 2 argument");
    }

    if (( arg(0)->getType() != DataType::INT )  &&
        ( arg(1)->getType() != DataType::INT ))
    {
        Throw( aurora_typing_exception,
               "LRCarCount requires 2 int arguments" );
    }

    // Output type
    add_int_32output_field();
    add_single_output_field();
}


BOREALIS_NAMESPACE_END
