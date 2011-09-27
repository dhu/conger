#include "Expression.h"
#include "ExprUtil.h"

BOREALIS_NAMESPACE_BEGIN

class pad_function : public Function
{
  public:
    ptr<Expression> makeExpression( const NArgs  &args )
                             throw( ExprException )
    {
        args.requireNumArgs( 2 );

        if ( args[0]->getType() != DataType::STRING )
        {    Throw( ExprException, "first argument must be a string" );
        }
        else if (( args[1]->getType() != DataType::INT  )  &&
                 ( args[1]->getType() != DataType::SHORT)  &&
                 ( args[1]->getType() != DataType::BYTE )  &&
                 ( args[1]->getType() != DataType::LONG ))
        {
            Throw( ExprException, "second argument must be an integer" );
        }

        return( ptr<Expression>( new Impl( args )));
    }

public:
    class Impl : public TypedExpression<EString>, NArgs
    {
      public:
        Impl( const NArgs  &args ) : NArgs( args ) {}

        int32 getStringLength() const
        {
            ptr<ConstantExpression<int32> > constant =
                arg(1).dynamic_cast_to<ConstantExpression<int32> >();

            if ( constant )
            {
                int32 val = constant->getValue();

                if ( val >= 0 && val < 1024*1024 )
                {   return( val );
                }
            }

            return( -1 );
        }

        EString evalImpl(EvalAs<EString>, const EvalContext& ctxt) throw (EvalException)
        {
            EString out = arg(0)->eval<EString>(ctxt);
            int32 size = arg(1)->eval<int32>(ctxt);

            if ( size < 0 || size > 1024*1024 )
            {   // XXX Should log something or notify somehow.
                size = 0;
            }

            if ( out.length() >= (unsigned int)size )
            {   return( EString( out.data(), size ));
            }

            char *buf = ctxt.alloc( size );
            memcpy( buf, out.data(), out.length() );
            memset( buf + out.length(), 0, size - out.length() );

            return( EString( buf, size ));
        }
    };
};


enum min_or_max { MIN, MAX };
template<min_or_max min_or_max>
class min_or_max_function : public Function
{
  public:
    ptr<Expression> makeExpression( const NArgs  &args )
                             throw( ExprException )
    {
        args.requireNumArgs( 2, NArgs::VARARGS );

        for ( unsigned int i = 1; i < args.size(); ++i )
        {   if ( args[i]->getType() != args[0]->getType() )
            {    Throw( ExprException,
                        "all arguments must have the same type" );
            }
        }

        if ( args[0]->getType() == DataType::SINGLE )
        {   return( ptr<Expression>( new Impl<single>( args )));
        }
        else if ( args[0]->getType() == DataType::DOUBLE )
        {   return( ptr<Expression>( new Impl<double>( args )));
        }
        else if ( args[0]->getType() == DataType::INT )
        {   return( ptr<Expression>( new Impl<int32>( args )));
        }
        else if ( args[0]->getType() == DataType::LONG )
        {   return( ptr<Expression>( new Impl<int64>( args )));
        }
        else if ( args[0]->getType() == DataType::SHORT )
        {   return( ptr<Expression>( new Impl<int16>( args )));
        }
        else if ( args[0]->getType() == DataType::BYTE )
        {   return( ptr<Expression>( new Impl<int8>( args )));
        }

        Throw(ExprException,
              "All arguments must be byte, short, int, long, single or double.");
    }

public:
    template<class T>
    class Impl : public TypedExpression<T>, NArgs
    {
      public:
        Impl(const NArgs& args) : NArgs(args) {}

        T evalImpl(EvalAs<T>, const EvalContext& ctxt) throw (EvalException)
        {
            T ret = arg(0)->template eval<T>(ctxt);

            for ( unsigned int i = 1; i < num_args(); ++i )
            {
                ret =
                    min_or_max == MIN ?
                    min(ret, arg(i)->template eval<T>(ctxt)) :
                    max(ret, arg(i)->template eval<T>(ctxt));
            }

            return( ret );
        }

      private:
        NArgs _args;
    };
};


class sequence_function : public Function
{
    ptr<Expression> makeExpression( const NArgs  &args )
                             throw( ExprException )
    {
        args.requireNumArgs( 0 );

        return( ptr<Expression>( new Impl ));
    }

public:
    class Impl : public TypedExpression<int32>
    {
        int32 _val;

      public:
        Impl() : _val(0) {}

        int32 evalImpl( EvalAs<int32>, const EvalContext  &ctxt )
                 throw( EvalException )
        {
            return( _val++ );
        }
    };
};


class str_len_function : public Function
{
    ptr<Expression> makeExpression( const NArgs  &args )
                             throw( ExprException )
    {
        args.requireNumArgs( 1 );
        args.require_arg_type( 0, DataType::STRING );

        return ptr<Expression>(new Impl(args));
    }

public:
    class Impl : public TypedExpression<int32>, NArgs
    {
    public:
        Impl(const NArgs& args) : NArgs(args) {}

        int32 evalImpl( EvalAs<int32>, const EvalContext  &ctxt )
                 throw( EvalException )
        {
            return( arg(0)->eval<EString>(ctxt).length() );
        }
    };
};



class int_function : public Function
{
    ptr<Expression> makeExpression( const NArgs  &args )
                             throw( ExprException )
    {
        args.requireNumArgs( 1 );

        args.require_arg_type( 0, DataType::INT,    DataType::LONG,
                                  DataType::SHORT,  DataType::BYTE,
                                  DataType::SINGLE, DataType::DOUBLE );

        return ptr<Expression>( new Impl( args ));
    }

public:
    class Impl : public TypedExpression<int32>, NArgs
    {
      public:
        Impl( const NArgs  &args ) : NArgs( args ) {}

        int32 evalImpl( EvalAs<int32>, const EvalContext  &ctxt )
                 throw( EvalException )
        {
            if ( arg(0)->getType() == DataType::INT )
            {   return( arg(0)->eval<int32>( ctxt ));
            }
            else if ( arg(0)->getType() == DataType::LONG )
            {   return( static_cast<int32>( arg(0)->eval<int64>( ctxt ) ) );
            }
            else if ( arg(0)->getType() == DataType::SHORT )
            {   return( static_cast<int32>( arg(0)->eval<int16>( ctxt ) ) );
            }
            else if ( arg(0)->getType() == DataType::BYTE )
            {   return( static_cast<int32>( arg(0)->eval<int8>( ctxt ) ) );
            }
            else if ( arg(0)->getType() == DataType::SINGLE )
            {   return( static_cast<int32>( arg(0)->eval<single>( ctxt ) ) );
            }
            else // if ( arg(0)->getType() == DataType::DOUBLE )
            {   return( static_cast<int32>( arg(0)->eval<double>( ctxt ) ) );
            }
        }
    };
};



class long_function : public Function
{
    ptr<Expression> makeExpression( const NArgs  &args )
                             throw( ExprException )
    {
        args.requireNumArgs( 1 );

        args.require_arg_type( 0, DataType::INT,    DataType::LONG,
                                  DataType::SHORT,  DataType::BYTE,
                                  DataType::SINGLE, DataType::DOUBLE );

        return ptr<Expression>( new Impl( args ));
    }

public:
    class Impl : public TypedExpression<int64>, NArgs
    {
      public:
        Impl( const NArgs  &args ) : NArgs( args ) {}

        int64 evalImpl( EvalAs<int64>, const EvalContext  &ctxt )
                 throw( EvalException )
        {
            if ( arg(0)->getType() == DataType::INT )
            {   return( arg(0)->eval<int64>( ctxt ));
            }
            else if ( arg(0)->getType() == DataType::LONG )
            {   return( arg(0)->eval<int64>( ctxt ));
            }
            else if ( arg(0)->getType() == DataType::SHORT )
            {   return( arg(0)->eval<int64>( ctxt ));
            }
            else if ( arg(0)->getType() == DataType::BYTE )
            {   return( arg(0)->eval<int64>( ctxt ));
            }
            else if ( arg(0)->getType() == DataType::SINGLE )
            {   return( static_cast<int64>( arg(0)->eval<single>( ctxt ) ) );
            }
            else // if ( arg(0)->getType() == DataType::DOUBLE )
            {   return( static_cast<int64>( arg(0)->eval<double>( ctxt ) ) );
            }
        }
    };
};



class short_function : public Function
{
    ptr<Expression> makeExpression( const NArgs  &args )
                             throw( ExprException )
    {
        args.requireNumArgs( 1 );

        args.require_arg_type( 0, DataType::INT,    DataType::LONG,
                                  DataType::SHORT,  DataType::BYTE,
                                  DataType::SINGLE, DataType::DOUBLE );

        return ptr<Expression>( new Impl( args ));
    }

public:
    class Impl : public TypedExpression<int16>, NArgs
    {
      public:
        Impl( const NArgs  &args ) : NArgs( args ) {}

        int16 evalImpl( EvalAs<int16>, const EvalContext  &ctxt )
                 throw( EvalException )
        {
            if ( arg(0)->getType() == DataType::INT )
            {   return( arg(0)->eval<int16>( ctxt ));
            }
            else if ( arg(0)->getType() == DataType::LONG )
            {   return( arg(0)->eval<int16>( ctxt ));
            }
            else if ( arg(0)->getType() == DataType::SHORT )
            {   return( arg(0)->eval<int16>( ctxt ));
            }
            else if ( arg(0)->getType() == DataType::BYTE )
            {   return( arg(0)->eval<int16>( ctxt ));
            }
            else if ( arg(0)->getType() == DataType::SINGLE )
            {   return( static_cast<int16>( arg(0)->eval<single>( ctxt ) ) );
            }
            else // if ( arg(0)->getType() == DataType::DOUBLE )
            {   return( static_cast<int16>( arg(0)->eval<double>( ctxt ) ) );
            }
        }
    };
};



class byte_function : public Function
{
    ptr<Expression> makeExpression( const NArgs  &args )
                             throw( ExprException )
    {
        args.requireNumArgs( 1 );

        args.require_arg_type( 0, DataType::INT,    DataType::LONG,
                                  DataType::SHORT,  DataType::BYTE,
                                  DataType::SINGLE, DataType::DOUBLE );

        return ptr<Expression>( new Impl( args ));
    }

public:
    class Impl : public TypedExpression<int8>, NArgs
    {
      public:
        Impl( const NArgs  &args ) : NArgs( args ) {}

        int8 evalImpl( EvalAs<int8>, const EvalContext  &ctxt )
                throw( EvalException )
        {
            if ( arg(0)->getType() == DataType::INT )
            {   return( arg(0)->eval<int8>( ctxt ));
            }
            else if ( arg(0)->getType() == DataType::LONG )
            {   return( arg(0)->eval<int8>( ctxt ));
            }
            else if ( arg(0)->getType() == DataType::SHORT )
            {   return( arg(0)->eval<int8>( ctxt ));
            }
            else if ( arg(0)->getType() == DataType::BYTE )
            {   return( arg(0)->eval<int8>( ctxt ));
            }
            else if ( arg(0)->getType() == DataType::SINGLE )
            {   return( static_cast<int8>( arg(0)->eval<single>( ctxt ) ) );
            }
            else // if ( arg(0)->getType() == DataType::DOUBLE )
            {   return( static_cast<int8>( arg(0)->eval<double>( ctxt ) ) );
            }
        }
    };
};



class abs_function : public Function
{
    ptr<Expression> makeExpression( const NArgs  &args )
                             throw( ExprException )
    {
        args.requireNumArgs( 1 );

        args.require_arg_type( 0, DataType::INT,    DataType::LONG,
                                  DataType::SHORT,  DataType::BYTE,
                                  DataType::SINGLE, DataType::DOUBLE );

        if ( args[0]->getType() == DataType::INT )
        {    return( ptr<Expression>( new Impl<int32>( args )));
        }
        else if ( args[0]->getType() == DataType::LONG )
        {    return( ptr<Expression>( new Impl<int64>( args )));
        }
        else if ( args[0]->getType() == DataType::SHORT )
        {    return( ptr<Expression>( new Impl<int16>( args )));
        }
        else if ( args[0]->getType() == DataType::BYTE )
        {    return( ptr<Expression>( new Impl<int8>( args )));
        }
        else if ( args[0]->getType() == DataType::SINGLE )
        {    return( ptr<Expression>( new Impl<single>( args )));
        }
        else // if ( args[0]->getType() == DataType::DOUBLE )
        {    return( ptr<Expression>( new Impl<double>( args )));
        }
    }

public:
    template<class T>
    class Impl : public TypedExpression<T>, NArgs
    {
      public:
        Impl(const NArgs& args) : NArgs(args) {}

        T evalImpl(EvalAs<T>, const EvalContext& ctxt) throw (EvalException)
        {
            T val = arg(0)->template eval<T>(ctxt);
            return val >= T(0) ? val : -val;
        }
    };
};


class now_function : public Function
{
    ptr<Expression>  makeExpression( const NArgs  &args )
                              throw( ExprException )
    {

        if ( args.num_args() > 0 )
        {
            args.require_arg_type( 0 , DataType::SINGLE, DataType::DOUBLE );
        }

        // Construct a Timestamp expression by default.
        return( ptr<Expression>( new Impl( args )));
    }

  public:
    class Impl : public TypedExpression<double>, NArgs
    {
      public:
        Impl(const NArgs& args) : NArgs(args) {}

        double  evalImpl( EvalAs<double>, const EvalContext& ctxt )
                   throw( EvalException )
        {
            double r;
        
            if ( num_args() > 0 )
            {
                double n = (Timestamp::now().to_double());
                double a = (double) arg(0)->eval<double>(ctxt);
                r =  n + a;
                DEBUG << "NowFunction " << n << " + "
                      << a << " = " << r << " type " << arg(0)->getType();
            }
            else
            {   r = Timestamp::now().to_double();
                DEBUG << "NowFunction " << r;
            }

            return( r );
        }
    };
};



////////////////////////////////////////////////////////////////////////////////
//
Function  *Function::instantiate_standard(const string&  name)
{
//..............................................................................


    if (name == "pad")            return(new pad_function);
    else if (name == "min")       return(new min_or_max_function<MIN>);
    else if (name == "max")       return(new min_or_max_function<MAX>);
    else if (name == "sequence")  return(new sequence_function);
    else if (name == "strlen")    return(new str_len_function);
    else if (name == "int")       return(new int_function);
    else if (name == "long")      return(new long_function);
    else if (name == "short")     return(new short_function);
    else if (name == "byte")      return(new byte_function);
    else if (name == "abs")       return(new abs_function);
    else if (name == "now")       return(new now_function);

    return(NULL);
}


BOREALIS_NAMESPACE_END
