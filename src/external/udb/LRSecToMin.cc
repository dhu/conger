#include "LRSecToMin.h"

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
ptr<Expression>  LRSecToMinFunction::makeExpression( const NArgs  &args )
                                              throw( ExprException )
{
//..............................................................................


    args.requireNumArgs(1);

    if ( args.size() != 1  ||  args[0]->getType() != DataType::INT )
    {   Throw( ExprException, "LRSecToMin takes one integer argument" );
    }

    return( ptr<Expression>( new LRSecToMinFunctionImpl( args )));
}



////////////////////////////////////////////////////////////////////////////////
//
int32  LRSecToMinFunction::LRSecToMinFunctionImpl::evalImpl(EvalAs<int32>,
                                                      const EvalContext  &ctxt
                                                           )
                                                      throw( EvalException )
{
//..............................................................................


    int s = arg(0)->eval<int32>( ctxt );
    double sd = s + 0.0;

    return( (int32)( floor( sd / 60.0 ) + 1.0 ));
}


BOREALIS_NAMESPACE_END
