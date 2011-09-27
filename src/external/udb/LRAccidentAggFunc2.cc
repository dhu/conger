#include "LRAccidentAggFunc2.h"

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
void  LRAccidentAggFunc2::my_window::init()
{
//..............................................................................


    _first    = true;
    _accident = true;
    _carid    = -1;

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  LRAccidentAggFunc2::my_window::incr( const EvalContext  &ctxt )
                                    throw( EvalException )
{
//..............................................................................


    // Retrieve carid from tuple
    int32  carid = arg(0).eval<int32>( ctxt );

    if ( _first )
    {
        _carid = carid;
        _first = false;
    }
    else
    {   _accident = ( _accident  &&
                     _carid != carid );
    }

    return;
}

////////////////////////////////////////////////////////////////////////////////
//
void  LRAccidentAggFunc2::my_window::final( char *out )
{
//..............................................................................


    // Output accident flag
    *(int32*)out = (_accident)? 1 : 0;

     return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  LRAccidentAggFunc2::setup( const ExprContext  &ctxt )
                          throw( AuroraException )
{
//..............................................................................


    if ( num_args() != 1 )
    {   Throw( aurora_typing_exception,
               "LRAccidentAggFunc2 takes one arguments" );
    }
    else if ( arg(0)->getType() != DataType::INT )
    {   Throw( aurora_typing_exception,
               "LRAccidentAggFunc2 requires one int arguments" );
    }

    // Output type
    add_int_32output_field();

    return;
}


BOREALIS_NAMESPACE_END
