#include "LRAccidentAggFunc1.h"

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
void  LRAccidentAggFunc1::my_window::init()
{
//..............................................................................


    _first      = true;
    _stopped    = true;
    _expressway = -1;
    _direction  = -1;
    _position   = -1;

    return;
}


////////////////////////////////////////////////////////////////////////////////
//
void  LRAccidentAggFunc1::my_window::incr( const EvalContext  &ctxt )
                                    throw( EvalException )
{
//..............................................................................


    // Retrieve values from tuple
    int32 expressway = arg(0).eval<int32>( ctxt );
    int32 direction  = arg(1).eval<int32>( ctxt );
    int32 position   = arg(2).eval<int32>( ctxt );

    if ( _first )
    {
        _expressway = expressway;
        _direction  = direction;
        _position   = position;
        _first      = false;
    }
    else
    {   _stopped = ( _stopped                      &&
                   ( expressway == _expressway )  &&
                   ( direction  == _direction  )  &&
                   ( position   == _position   ));
    }

    return;
}


////////////////////////////////////////////////////////////////////////////////
//
void  LRAccidentAggFunc1::my_window::final(char  *out)
{
//..............................................................................


    // Output expressway, direction, position, stopped
    *(int32*)out        = _expressway;
    *(int32*)(out + 4)  = _direction;
    *(int32*)(out + 8)  = _position;
    *(int32*)(out + 12) = (_stopped) ? 1 : 0;

    return;
}


////////////////////////////////////////////////////////////////////////////////
//
void  LRAccidentAggFunc1::setup( const ExprContext  &ctxt )
                          throw( AuroraException )
{
//..............................................................................


    if ( num_args() != 3 )
    {   Throw( aurora_typing_exception,
               "LRAccidentAggFunc1 takes 3 arguments" );
    }

    if ( arg(0)->getType() != DataType::INT  &&
         arg(1)->getType() != DataType::INT  &&
         arg(2)->getType() != DataType::INT )
    {
        Throw( aurora_typing_exception,
               "LRAccidentAggFunc1 requires three int arguments" );
    }

    // Output type
    add_int_32output_field();
    add_int_32output_field();
    add_int_32output_field();
    add_int_32output_field();
}


BOREALIS_NAMESPACE_END
