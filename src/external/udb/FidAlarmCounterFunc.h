#ifndef  FIDALARMCOUNTERFUNC_H
#define  FIDALARMCOUNTERFUNC_H

#include "NArgs.h"

#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>

BOREALIS_NAMESPACE_BEGIN

class FidAlarmCounterFunc : public Function
{
  public:
    ptr<Expression> makeExpression( const NArgs  &args )
                             throw( ExprException );

    class Impl : public TypedExpression<int32>
    {
      public:
        Impl( ptr<Expression>  iam, ptr<Expression>  threshold ) :
            _iam( iam ),
            _threshold( threshold ),
            _was_quenched( false ),
            _num_alarms( 0 ) {}


        int32 evalImpl( EvalAs<int32>, const EvalContext  &ctxt )
                 throw( EvalException );

      private:
        ptr<Expression> _iam, _threshold;
        bool  _was_quenched;
        int   _num_alarms;
    };
};


BOREALIS_NAMESPACE_END

#endif                     // FIDALARMCOUNTERFUNC_H
