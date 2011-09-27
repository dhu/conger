#include "RandFunction.h"

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
ptr<Expression>  RandFunction::makeExpression( const NArgs  &args )
                                        throw( ExprException )
{
//..............................................................................


    args.requireNumArgs( 0, 1 );

    if ( args.size() == 0 )
    {   return( ptr<Expression>( new SingleRandFunction( args )));
    }
    else if ( args[0]->getType() == DataType::SINGLE )
    {   return( ptr<Expression>( new SingleRandFunction( args )));
    }
    else if ( args[0]->getType() == DataType::DOUBLE )
    {   return( ptr<Expression>( new DoubleRandFunction( args )));
    }
    else if ( args[0]->getType() == DataType::INT )
    {   return( ptr<Expression>( new Int32RandFunction( args )));
    }
    else if ( args[0]->getType() == DataType::BYTE )
    {   return( ptr<Expression>( new Int8RandFunction( args )));
    }
    else if ( args[0]->getType() == DataType::SHORT )
    {   return( ptr<Expression>( new Int16RandFunction( args )));
    }
    else if ( args[0]->getType() == DataType::LONG )
    {   return( ptr<Expression>( new Int64RandFunction( args )));
    }

    Throw( ExprException, "invalid type" );
}



////////////////////////////////////////////////////////////////////////////////
//
single  RandFunction::SingleRandFunction::evalImpl( EvalAs<single>,
                                              const EvalContext  &ctxt )
                                             throw( EvalException )
{
//..............................................................................


    single  ret = rand() / ( RAND_MAX + 1.0 );

    if ( !NArgs::empty() )
    {   ret *= arg(0)->eval<single>( ctxt );
    }

    return( ret );
}



////////////////////////////////////////////////////////////////////////////////
//
double  RandFunction::DoubleRandFunction::evalImpl( EvalAs<double>,
                                              const EvalContext  &ctxt )
                                             throw( EvalException )
{
//..............................................................................


    double  ret = rand() / ( RAND_MAX + 1.0 );

    if ( !NArgs::empty() )
    {   ret *= arg(0)->eval<double>( ctxt );
    }

    return( ret );
}



////////////////////////////////////////////////////////////////////////////////
//
int8  RandFunction::Int8RandFunction::evalImpl( EvalAs< int8 >,
                                            const EvalContext  &ctxt )
                                           throw(EvalException )
{
//..............................................................................


    return( rand() % arg(0)->eval<int8>( ctxt ));
}



////////////////////////////////////////////////////////////////////////////////
//
int16  RandFunction::Int16RandFunction::evalImpl( EvalAs< int16 >,
                                            const EvalContext  &ctxt )
                                           throw(EvalException )
{
//..............................................................................


    return( rand() % arg(0)->eval<int16>( ctxt ));
}



////////////////////////////////////////////////////////////////////////////////
//
int32  RandFunction::Int32RandFunction::evalImpl( EvalAs< int32 >,
                                            const EvalContext  &ctxt )
                                           throw(EvalException )
{
//..............................................................................


    return( rand() % arg(0)->eval<int32>( ctxt ));
}



////////////////////////////////////////////////////////////////////////////////
//
int64  RandFunction::Int64RandFunction::evalImpl( EvalAs< int64 >,
                                            const EvalContext  &ctxt )
                                           throw( EvalException )
{
//..............................................................................


    return((( (int64)rand() << 32 ) ^ (uint64)rand() )
                 % arg(0)->eval<int64>( ctxt ));
}


BOREALIS_NAMESPACE_END
