#include <cmath>

#include "Expression.h"
#include "ExprUtil.h"

BOREALIS_NAMESPACE_BEGIN

class cos_function : public Function
{
    ptr<Expression> makeExpression( const NArgs  &args )
                             throw( ExprException )
    {
        args.requireNumArgs( 1 );
        args.require_arg_type( 0, DataType::DOUBLE, DataType::SINGLE );
        return( ptr<Expression>( new Impl<double>( args )));
    }

  public:
    template<class T>
    class Impl : public TypedExpression<T>, public NArgs
    {
      public:
        Impl(const NArgs& args) : NArgs(args) {}

        T evalImpl(EvalAs<T>, const EvalContext& ctxt) throw (EvalException)
        {
            T val = arg(0)->template eval<T>(ctxt);
            val = cos(val);
            return val;
        }
    };
};


class arccos_function : public Function
{
    ptr<Expression> makeExpression( const NArgs  &args )
                             throw( ExprException )
    {
        args.requireNumArgs( 1 );
        args.require_arg_type( 0, DataType::DOUBLE, DataType::SINGLE  );
    return( ptr<Expression>( new Impl<double>( args )));
    }

public:
    template<class T>
    class Impl : public TypedExpression<T>, public NArgs
    {
      public:
        Impl(const NArgs& args) : NArgs(args) {}

        T evalImpl(EvalAs<T>, const EvalContext& ctxt) throw (EvalException)
        {
      T val = arg(0)->template eval<T>(ctxt);
      val = acos(val);
      return val;
        }
    };
};



class hypcos_function : public Function
{
    ptr<Expression> makeExpression( const NArgs  &args )
                             throw( ExprException )
    {
        args.requireNumArgs( 1 );
        args.require_arg_type( 0, DataType::DOUBLE, DataType::SINGLE );
    return( ptr<Expression>( new Impl<double>( args )));
    }
public:
    template<class T>
    class Impl : public TypedExpression<T>, public NArgs
    {
      public:
        Impl(const NArgs& args) : NArgs(args) {}

        T evalImpl(EvalAs<T>, const EvalContext& ctxt) throw (EvalException)
        {
      T val = arg(0)->template eval<T>(ctxt);
      val = cosh(val);
      return val;
        }
    };
};



class tan_function : public Function
{
    ptr<Expression> makeExpression( const NArgs  &args )
                             throw( ExprException )
    {
        args.requireNumArgs( 1 );
        args.require_arg_type( 0, DataType::DOUBLE, DataType::SINGLE );
    return( ptr<Expression>( new Impl<double>( args )));

    }
public:
    template<class T>
    class Impl : public TypedExpression<T>, public NArgs
    {
      public:
        Impl(const NArgs& args) : NArgs(args) {}

        T evalImpl(EvalAs<T>, const EvalContext& ctxt) throw (EvalException)
        {
      T val = arg(0)->template eval<T>(ctxt);
      val = tan(val);
      return val;
        }
    };
};



class arctan_function : public Function
{
    ptr<Expression> makeExpression( const NArgs  &args )
                             throw( ExprException )
    {
        args.requireNumArgs( 1 );
        args.require_arg_type( 0, DataType::DOUBLE, DataType::SINGLE );
    return( ptr<Expression>( new Impl<double>( args )));
    }

public:
    template<class T>
    class Impl : public TypedExpression<T>, public NArgs
    {
      public:
        Impl(const NArgs& args) : NArgs(args) {}

        T evalImpl(EvalAs<T>, const EvalContext& ctxt) throw (EvalException)
        {
      T val = arg(0)->template eval<T>(ctxt);
      val = atan(val);
      return val;
        }
    };
};



class hyptan_function : public Function
{
    ptr<Expression> makeExpression( const NArgs  &args )
                             throw( ExprException )
    {
        args.requireNumArgs( 1 );
        args.require_arg_type( 0, DataType::DOUBLE, DataType::SINGLE );
    return( ptr<Expression>( new Impl<double>( args )));
    }

public:
    template<class T>
    class Impl : public TypedExpression<T>, public NArgs
    {
      public:
        Impl(const NArgs& args) : NArgs(args) {}

        T evalImpl(EvalAs<T>, const EvalContext& ctxt) throw (EvalException)
        {
      T val = arg(0)->template eval<T>(ctxt);
      val = tanh(val);
      return val;
        }
    };
};



class sin_function : public Function
{
    ptr<Expression> makeExpression( const NArgs  &args )
                             throw( ExprException )
    {
        args.requireNumArgs( 1 );
        args.require_arg_type( 0, DataType::DOUBLE, DataType::SINGLE );
    return( ptr<Expression>( new Impl<double>( args )));
    }

public:
    template<class T>
    class Impl : public TypedExpression<T>, public NArgs
    {
      public:
        Impl(const NArgs& args) : NArgs(args) {}

        T evalImpl(EvalAs<T>, const EvalContext& ctxt) throw (EvalException)
        {
      T val = arg(0)->template eval<T>(ctxt);
      val = sin(val);
      return val;
        }
    };
};



class arcsin_function : public Function
{
    ptr<Expression> makeExpression( const NArgs  &args )
                             throw( ExprException )
    {
        args.requireNumArgs( 1 );
        args.require_arg_type( 0, DataType::DOUBLE, DataType::SINGLE );
    return( ptr<Expression>( new Impl<double>( args )));
    }

public:
    template<class T>
    class Impl : public TypedExpression<T>, public NArgs
    {
      public:
        Impl(const NArgs& args) : NArgs(args) {}

        T evalImpl(EvalAs<T>, const EvalContext& ctxt) throw (EvalException)
        {
      T val = arg(0)->template eval<T>(ctxt);
      val = asin(val);
      return val;
        }
    };
};



class hypsin_function : public Function
{
    ptr<Expression> makeExpression( const NArgs  &args )
                             throw( ExprException )
    {
        args.requireNumArgs( 1 );
        args.require_arg_type( 0, DataType::DOUBLE, DataType::SINGLE );
    return( ptr<Expression>( new Impl<double>( args )));
    }

public:
    template<class T>
    class Impl : public TypedExpression<T>, public NArgs
    {
      public:
        Impl(const NArgs& args) : NArgs(args) {}

        T evalImpl(EvalAs<T>, const EvalContext& ctxt) throw (EvalException)
        {
      T val = arg(0)->template eval<T>(ctxt);
      val = sinh(val);
      return val;
        }
    };
};



class ln_function : public Function
{
    ptr<Expression> makeExpression( const NArgs  &args )
                             throw( ExprException )
    {
        args.requireNumArgs( 1 );
        args.require_arg_type( 0, DataType::DOUBLE, DataType::SINGLE );
    return( ptr<Expression>( new Impl<double>( args )));

    }

public:
    template<class T>
    class Impl : public TypedExpression<T>, public NArgs
    {
      public:
        Impl(const NArgs& args) : NArgs(args) {}

        T evalImpl(EvalAs<T>, const EvalContext& ctxt) throw (EvalException)
        {
      T val = arg(0)->template eval<T>(ctxt);
      val = log(val);
      return val;
        }
    };
};



class log_function : public Function
{
    ptr<Expression> makeExpression( const NArgs  &args )
                             throw( ExprException )
    {
        args.requireNumArgs( 1 );
        args.require_arg_type( 0, DataType::DOUBLE, DataType::SINGLE );
    return( ptr<Expression>( new Impl<double>( args )));
    }

public:
    template<class T>
    class Impl : public TypedExpression<T>, public NArgs
    {
      public:
        Impl(const NArgs& args) : NArgs(args) {}

        T evalImpl(EvalAs<T>, const EvalContext& ctxt) throw (EvalException)
        {
      T val = arg(0)->template eval<T>(ctxt);
      val = log10(val);
      return val;

        }
    };
};



class sqrt_function : public Function
{
    ptr<Expression> makeExpression( const NArgs  &args )
                             throw( ExprException )
    {
        args.requireNumArgs( 1 );
        args.require_arg_type( 0, DataType::DOUBLE, DataType::SINGLE );
    return( ptr<Expression>( new Impl<double>( args )));
    }

public:
    template<class T>
    class Impl : public TypedExpression<T>, public NArgs
    {
      public:
        Impl(const NArgs& args) : NArgs(args) {}

        T evalImpl(EvalAs<T>, const EvalContext& ctxt) throw (EvalException)
        {
      T val = arg(0)->template eval<T>(ctxt);
      val = sqrt(val);
      return val;

        }
    };
};



class exp_function : public Function
{
    ptr<Expression> makeExpression( const NArgs  &args )
                             throw( ExprException )
    {
        args.requireNumArgs( 1 );
        args.require_arg_type( 0, DataType::DOUBLE, DataType::SINGLE );
    return( ptr<Expression>( new Impl<double>( args )));
    }

public:
    template<class T>
    class Impl : public TypedExpression<T>, public NArgs
    {
      public:
        Impl(const NArgs& args) : NArgs(args) {}

        T evalImpl(EvalAs<T>, const EvalContext& ctxt) throw (EvalException)
        {
      T val = arg(0)->template eval<T>(ctxt);
      val = exp(val);
      return val;

        }
    };
};



class floor_function : public Function
{
    ptr<Expression> makeExpression( const NArgs  &args )
                             throw( ExprException )
    {
        args.requireNumArgs( 1 );
        args.require_arg_type( 0, DataType::DOUBLE, DataType::SINGLE );
    return( ptr<Expression>( new Impl<double>( args )));
    }

public:
    template<class T>
    class Impl : public TypedExpression<T>, public NArgs
    {
      public:
        Impl(const NArgs& args) : NArgs(args) {}

        T evalImpl(EvalAs<T>, const EvalContext& ctxt) throw (EvalException)
        {
      T val = arg(0)->template eval<T>(ctxt);
      val = floor(val);
      return val;

        }
    };
};



class ceil_function : public Function
{
    ptr<Expression> makeExpression( const NArgs  &args )
                             throw( ExprException )
    {
        args.requireNumArgs( 1 );
        args.require_arg_type( 0, DataType::DOUBLE, DataType::SINGLE );
    return( ptr<Expression>( new Impl<double>( args )));
    }

public:
    template<class T>
    class Impl : public TypedExpression<T>, public NArgs
    {
      public:
        Impl(const NArgs& args) : NArgs(args) {}

        T evalImpl(EvalAs<T>, const EvalContext& ctxt) throw (EvalException)
        {
      T val = arg(0)->template eval<T>(ctxt);
      val = ceil(val);
      return val;

        }
    };
};



class pow_function : public Function
{
    ptr<Expression> makeExpression( const NArgs  &args )
                             throw( ExprException )
    {
        args.requireNumArgs( 2 );
        args.require_arg_type( 0, DataType::DOUBLE, DataType::SINGLE );
    args.require_arg_type( 1, DataType::DOUBLE, DataType::SINGLE );
    return( ptr<Expression>( new Impl<double>( args )));
    }

public:
    template<class T>
    class Impl : public TypedExpression<T>, public NArgs
    {
      public:
        Impl(const NArgs& args) : NArgs(args) {}

        T evalImpl(EvalAs<T>, const EvalContext& ctxt) throw (EvalException)
        {
      T val0 = arg(0)->template eval<T>(ctxt);
      T val1 = arg(1)->template eval<T>(ctxt);
      T val = pow(val0, val1);
      return val;

        }
    };
};



class arctan_2function : public Function
{
    ptr<Expression> makeExpression( const NArgs  &args )
                             throw( ExprException )
    {
        args.requireNumArgs( 2 );
        args.require_arg_type( 0, DataType::DOUBLE, DataType::SINGLE );
        args.require_arg_type( 1, DataType::DOUBLE, DataType::SINGLE );

        return( ptr<Expression>( new Impl<double>( args )));
    }

public:
    template<class T>
    class Impl : public TypedExpression<T>, public NArgs
    {
      public:
        Impl(const NArgs& args) : NArgs(args) {}

        T evalImpl(EvalAs<T>, const EvalContext& ctxt) throw (EvalException)
        {
      T val0 = arg(0)->template eval<T>(ctxt);
      T val1 = arg(1)->template eval<T>(ctxt);
      T val = atan2(val0, val1); //returns atan(val0/val1);
      return val;

        }
    };
};



////////////////////////////////////////////////////////////////////////////////
//
Function  *Function::instantiate_math(const string&  name)
{
//..............................................................................


    if (name == "cos")         return(new cos_function);
    else if (name == "acos")   return(new arccos_function);
    else if (name == "cosh")   return(new hypcos_function);
    else if (name == "tan")    return(new tan_function);
    else if (name == "atan")   return(new arctan_function);
    else if (name == "tanh")   return(new hyptan_function);
    else if (name == "sin")    return(new sin_function);
    else if (name == "asin")   return(new arcsin_function);
    else if (name == "sinh")   return(new hypsin_function);
    else if (name == "atan2")  return(new arctan_2function);
    else if (name == "ln")     return(new ln_function);
    else if (name == "log")    return(new log_function);
    else if (name == "sqrt")   return(new sqrt_function);
    else if (name == "exp")    return(new exp_function);
    else if (name == "floor")  return(new floor_function);
    else if (name == "ceil")   return(new ceil_function);
    else if (name == "pow")    return(new pow_function);

    return(NULL);
}


BOREALIS_NAMESPACE_END
