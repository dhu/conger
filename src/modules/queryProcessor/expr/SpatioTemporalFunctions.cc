#include "Expression.h"
#include "ExprUtil.h"

BOREALIS_NAMESPACE_BEGIN;


class overlap1_function : public Function
{
    ptr<Expression> makeExpression( const NArgs& args )
        throw( ExprException )
    {
        args.requireNumArgs(4);

        args.require_arg_type( 0, DataType::INT, DataType::LONG,
            DataType::SINGLE, DataType::DOUBLE );
        args.require_arg_type( 1, DataType::INT, DataType::LONG,
            DataType::SINGLE, DataType::DOUBLE );
        args.require_arg_type( 2, DataType::INT, DataType::LONG,
            DataType::SINGLE, DataType::DOUBLE );
        args.require_arg_type( 3, DataType::INT, DataType::LONG,
            DataType::SINGLE, DataType::DOUBLE );

        return ptr<Expression>(new Impl(args));
    }
    
public:
    class Impl : public TypedExpression<bool>, NArgs
    {
    public:
        Impl(const NArgs& args) : NArgs(args) {}

        bool evalImpl(EvalAs<bool>, const EvalContext& ctxt)
            throw( EvalException )
        {
            double xl = arg(0)->eval<double>(ctxt);
            double xu = arg(1)->eval<double>(ctxt);
            double yl = arg(2)->eval<double>(ctxt);
            double yu = arg(3)->eval<double>(ctxt);
            
            if ( xl > xu || yl > yu )
                Throw(EvalException, string("Erroneous ranges ") +
                    "[" + to_string(xl) + ", " + to_string(xu) + "], " +
                    "[" + to_string(yl) + ", " + to_string(yu) + "]");

            DEBUG << "overlap1 [" << xl << ", " << xu
                << "], [" << yl << "," << yu << "] "
                << ( !(xu <= yl || yu <= xl) );

            return ( !(xu <= yl || yu <= xl) );
        }
    };
};

class contained1_function : public Function
{
    ptr<Expression> makeExpression( const NArgs& args )
        throw( ExprException )
    {
        args.requireNumArgs(4);

        args.require_arg_type( 0, DataType::INT, DataType::LONG,
            DataType::SINGLE, DataType::DOUBLE );
        args.require_arg_type( 1, DataType::INT, DataType::LONG,
            DataType::SINGLE, DataType::DOUBLE );
        args.require_arg_type( 2, DataType::INT, DataType::LONG,
            DataType::SINGLE, DataType::DOUBLE );
        args.require_arg_type( 3, DataType::INT, DataType::LONG,
            DataType::SINGLE, DataType::DOUBLE );

        return ptr<Expression>(new Impl(args));
    }
    
public:
    class Impl : public TypedExpression<bool>, NArgs
    {
    public:
        Impl(const NArgs& args) : NArgs(args) {}

        bool evalImpl(EvalAs<bool>, const EvalContext& ctxt)
            throw( EvalException )
        {
            double xl = arg(0)->eval<double>(ctxt);
            double xu = arg(1)->eval<double>(ctxt);
            double yl = arg(2)->eval<double>(ctxt);
            double yu = arg(3)->eval<double>(ctxt);
            
            if ( xl > xu || yl > yu )
                Throw(EvalException, string("Erroneous ranges ") +
                    "[" + to_string(xl) + ", " + to_string(xu) + "], " +
                    "[" + to_string(yl) + ", " + to_string(yu) + "]");

            return ( xl <= yl && yu <= xu );
        }
    };
};


class overlap2_function : public Function
{
    ptr<Expression> makeExpression( const NArgs& args )
        throw( ExprException )
    {
        args.requireNumArgs(8);

        args.require_arg_type( 0, DataType::INT, DataType::LONG,
            DataType::SINGLE, DataType::DOUBLE );
        args.require_arg_type( 1, DataType::INT, DataType::LONG,
            DataType::SINGLE, DataType::DOUBLE );
        args.require_arg_type( 2, DataType::INT, DataType::LONG,
            DataType::SINGLE, DataType::DOUBLE );
        args.require_arg_type( 3, DataType::INT, DataType::LONG,
            DataType::SINGLE, DataType::DOUBLE );

        args.require_arg_type( 4, DataType::INT, DataType::LONG,
            DataType::SINGLE, DataType::DOUBLE );
        args.require_arg_type( 5, DataType::INT, DataType::LONG,
            DataType::SINGLE, DataType::DOUBLE );
        args.require_arg_type( 6, DataType::INT, DataType::LONG,
            DataType::SINGLE, DataType::DOUBLE );
        args.require_arg_type( 7, DataType::INT, DataType::LONG,
            DataType::SINGLE, DataType::DOUBLE );

        return ptr<Expression>(new Impl(args));
    }
    
public:
    class Impl : public TypedExpression<bool>, NArgs
    {
    public:
        Impl(const NArgs& args) : NArgs(args) {}

        bool evalImpl(EvalAs<bool>, const EvalContext& ctxt)
            throw( EvalException )
        {
            double x1l = arg(0)->eval<double>(ctxt);
            double x1u = arg(1)->eval<double>(ctxt);
            double y1l = arg(2)->eval<double>(ctxt);
            double y1u = arg(3)->eval<double>(ctxt);

            double x2l = arg(4)->eval<double>(ctxt);
            double x2u = arg(5)->eval<double>(ctxt);
            double y2l = arg(6)->eval<double>(ctxt);
            double y2u = arg(7)->eval<double>(ctxt);
            
            if ( x1l > x1u || y1l > y1u || x2l > x2u || y2l > y2u )
                Throw(EvalException, string("Erroneous ranges ") +
                    "[" + to_string(x1l) + ", " + to_string(x1u) + "], " +
                    "[" + to_string(y1l) + ", " + to_string(y1u) + "], " +
                    "[" + to_string(x2l) + ", " + to_string(x2u) + "], " +
                    "[" + to_string(y2l) + ", " + to_string(y2u) + "]");

            return ( !( (x1u < x2l && y1u < y2l) || (x2u < x1l && y2u < y1l) ||
                        (x2u < x1l && y1u < y2l) || (x1u < x2l && y2u < y1l) ) );
        }
    };
};


class contained2_function : public Function
{
    ptr<Expression> makeExpression( const NArgs& args )
        throw( ExprException )
    {
        args.requireNumArgs(8);

        args.require_arg_type( 0, DataType::INT, DataType::LONG,
            DataType::SINGLE, DataType::DOUBLE );
        args.require_arg_type( 1, DataType::INT, DataType::LONG,
            DataType::SINGLE, DataType::DOUBLE );
        args.require_arg_type( 2, DataType::INT, DataType::LONG,
            DataType::SINGLE, DataType::DOUBLE );
        args.require_arg_type( 3, DataType::INT, DataType::LONG,
            DataType::SINGLE, DataType::DOUBLE );

        args.require_arg_type( 4, DataType::INT, DataType::LONG,
            DataType::SINGLE, DataType::DOUBLE );
        args.require_arg_type( 5, DataType::INT, DataType::LONG,
            DataType::SINGLE, DataType::DOUBLE );
        args.require_arg_type( 6, DataType::INT, DataType::LONG,
            DataType::SINGLE, DataType::DOUBLE );
        args.require_arg_type( 7, DataType::INT, DataType::LONG,
            DataType::SINGLE, DataType::DOUBLE );

        return ptr<Expression>(new Impl(args));
    }
    
public:
    class Impl : public TypedExpression<bool>, NArgs
    {
    public:
        Impl(const NArgs& args) : NArgs(args) {}

        bool evalImpl(EvalAs<bool>, const EvalContext& ctxt)
            throw( EvalException )
        {
            double x1l = arg(0)->eval<double>(ctxt);
            double x1u = arg(1)->eval<double>(ctxt);
            double y1l = arg(2)->eval<double>(ctxt);
            double y1u = arg(3)->eval<double>(ctxt);

            double x2l = arg(4)->eval<double>(ctxt);
            double x2u = arg(5)->eval<double>(ctxt);
            double y2l = arg(6)->eval<double>(ctxt);
            double y2u = arg(7)->eval<double>(ctxt);
            
            if ( x1l > x1u || y1l > y1u || x2l > x2u || y2l > y2u )
                Throw(EvalException, string("Erroneous ranges ") +
                    "[" + to_string(x1l) + ", " + to_string(x1u) + "], " +
                    "[" + to_string(y1l) + ", " + to_string(y1u) + "], " +
                    "[" + to_string(x2l) + ", " + to_string(x2u) + "], " +
                    "[" + to_string(y2l) + ", " + to_string(y2u) + "]");

            return ( x1l <= x2l && y1l <= y2l && x2u <= x1u && y2u <= y1u );
        }
    };
};

Function* Function::instantiate_spatio_temporal(const string& name)
{
    if ( name == "overlap1" )           return(new overlap1_function);
    else if ( name == "overlap2" )      return(new overlap2_function);
    else if ( name == "contained1" )    return(new contained1_function);
    else if ( name == "contained2" )    return(new contained2_function);
    
    return NULL;
}

BOREALIS_NAMESPACE_END;
