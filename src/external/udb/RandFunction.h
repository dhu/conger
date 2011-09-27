#ifndef  RANDFUNCTION_H
#define  RANDFUNCTION_H

#include "Expression.h"
#include "ExprUtil.h"

BOREALIS_NAMESPACE_BEGIN

class RandFunction : public Function
{
  public:
    ptr<Expression> makeExpression( const NArgs  &args )
                             throw( ExprException );


    class SingleRandFunction : public TypedExpression<single>, NArgs
    {
      public:
        SingleRandFunction( const NArgs  &args ) : NArgs( args ) {}

        single evalImpl( EvalAs<single>, const EvalContext  &ctxt )
                  throw( EvalException );
    };


    class DoubleRandFunction : public TypedExpression<double>, NArgs
    {
      public:
        DoubleRandFunction( const NArgs  &args ) : NArgs( args ) {}

        double evalImpl( EvalAs<double>, const EvalContext  &ctxt )
                  throw( EvalException );
    };


    class Int32RandFunction : public TypedExpression<int32>, NArgs
    {
      public:
        Int32RandFunction( const NArgs  &args ) : NArgs( args ) {}

        int32 evalImpl( EvalAs< int32 >, const EvalContext  &ctxt )
                 throw(EvalException );
    };


    class Int16RandFunction : public TypedExpression<int16>, NArgs
    {
      public:
        Int16RandFunction( const NArgs  &args ) : NArgs( args ) {}

        int16 evalImpl( EvalAs< int16 >, const EvalContext  &ctxt )
                 throw(EvalException );
    };


    class Int8RandFunction : public TypedExpression<int8>, NArgs
    {
      public:
        Int8RandFunction( const NArgs  &args ) : NArgs( args ) {}

        int8 evalImpl( EvalAs< int8 >, const EvalContext  &ctxt )
                 throw(EvalException );
    };


    class Int64RandFunction : public TypedExpression<int64>, NArgs
    {
      public:
        Int64RandFunction( const NArgs  &args ) : NArgs( args ) {}

        int64 evalImpl( EvalAs< int64 >, const EvalContext  &ctxt )
                 throw( EvalException );
    };
};


BOREALIS_NAMESPACE_END

#endif                     // RANDFUNCTION_H
