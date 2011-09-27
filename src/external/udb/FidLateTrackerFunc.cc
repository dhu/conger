#include "FidLateTrackerFunc.h"

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
void  FidLateTrackerFunc::my_window::incr( const EvalContext  &ctxt )
                                    throw( EvalException )
{
//..............................................................................


    EString arg0 = arg(0).eval<EString>( ctxt );
    _feed_store[0] = arg0[0];

    EString arg1 = arg(1).eval<EString>( ctxt );
    _exchange_store[0] = arg1[0];
    _exchange_store[1] = arg1[1];
    ++_cnt;

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void FidLateTrackerFunc::my_window::final( char  *out )
{
//..............................................................................


    *(int*)out = _cnt == 2 ? 1 : 0;
    *(char*)(out + 4) = _feed_store[0];
    *(char*)(out + 5) = _exchange_store[0];
    *(char*)(out + 6) = _exchange_store[1];

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  FidLateTrackerFunc::setup( const ExprContext  &ctxt )
                          throw( AuroraException )
{
//..............................................................................


    if ( num_args() != 2 )
    {    Throw( aurora_typing_exception,
                "FidLateTrackerFunc takes two arguments");
    }

    if (( arg(0)->getType() != DataType::STRING )  ||
        ( arg(1)->getType() != DataType::STRING ))
    {
        Throw( aurora_typing_exception,
               "FidLateTrackerFunc requires two string arguments" );
    }

    add_int_32output_field();
    add_string_output_field(1);
    add_string_output_field(2);
}


BOREALIS_NAMESPACE_END
