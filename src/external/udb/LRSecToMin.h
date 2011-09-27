#ifndef  LRSECTOMIN_H
#define  LRSECTOMIN_H

#include "Expression.h"
#include "ExprUtil.h"
#include <cmath>

BOREALIS_NAMESPACE_BEGIN

class LRSecToMinFunction : public Function
{
  public:
    ptr<Expression> makeExpression( const NArgs  &args )
                             throw( ExprException );


    class LRSecToMinFunctionImpl : public TypedExpression<int32>, NArgs
    {
      public:
        LRSecToMinFunctionImpl( const NArgs  &args ) : NArgs( args ) {}

        int32 evalImpl( EvalAs<int32>,
                        const EvalContext  &ctxt
                      )
                 throw( EvalException );
    };
};


BOREALIS_NAMESPACE_END

#endif                     // LRSECTOMIN_H
