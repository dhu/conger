#ifndef  NARGS_H
#define  NARGS_H

#include  "Expression.h"

using namespace Borealis;

/// A sequence of arguments, each of type ptr<Expression>.
class NArgs
{
  public:
    /// Indicates that an arbitrary number of arguments are allowed.
    static const int VARARGS = -1;


    /// Constructor.  Each non-null argument is added to the argument list.
    ///
    NArgs( ptr<Expression> a = ptr<Expression>(),
           ptr<Expression> b = ptr<Expression>(),
           ptr<Expression> c = ptr<Expression>() );


    /// Constructs an NArgs, setting the argument list to the contents
    /// of the vector provided.
    NArgs( const vector<ptr<Expression> >& args );


    /// Throws an ExprException if the number of arguments is not equal
    /// to args_required.
    void requireNumArgs(int args_required) const throw (ExprException);


    /// Throws an ExprException if the number of arguments is not between
    /// minargs and maxargs, inclusive.  (If maxargs is VARARGS, then there
    /// is no upper limit to the number of arguments allowed.)
    void requireNumArgs(int minargs, int maxargs) const throw (ExprException);


    /// Requires that the index'th argument (zero-relative) is of one
    /// of the provided types.
    void require_arg_type( unsigned int  index,
                             DataType  type1,
                             DataType  type2 = DataType(),
                             DataType  type3 = DataType(),
                             DataType  type4 = DataType(),
                             DataType  type5 = DataType(),
                             DataType  type6 = DataType()
                       ) const
                  throw( ExprException );


    /// Returns the idx'th argument (zero-relative).  Undefined
    /// behavior if the argument list is too short.
    const ptr<Expression>& arg( unsigned int idx ) const;


    /// Returns the number of arguments.
    unsigned int num_args() const;


    /// Returns the idx'th argument (zero-relative).  Undefined
    /// behavior if the argument list is too short.
    const ptr<Expression>& operator[] (unsigned int idx) const;


    /// Returns the number of arguments.
    unsigned int size() const;


    /// Returns true iff there are zero arguments.
    bool empty() const;

    /// Sets the ith argument.
    void set_arg( unsigned int idx, ptr<Expression> arg );

  protected:
    vector<ptr<Expression> > _args;
};


BOREALIS_NAMESPACE_BEGIN

/// A function factory, capable of returning a function implementation
/// given a statically typed set of arguments.
///
/// To create a custom function, create a subclass of Function that
/// implements makeExpression().  Extend StandardFunction() in
/// StandardFunctions.cc or MathFunction() in MathFunctions.cc
/// with:  register_function("<function name>",   new <function class>);
///
/// See plugin/src/RandFunction.C for an example.???
//
class Function
{
  public:
    /// Generates a function implementation.  This method should
    /// examine the given set of arguments, throwing an
    /// AuroraException if they are not valid (e.g., there are too
    /// many or too few, or the types are not correct).  It should
    /// then return an Expression that can be evaluated to determine
    /// the result of the function.  (Generally, the returned
    /// Expression will be a TypedExpression<T>, where T is the
    /// return type of the function.)
    ///
    virtual ~Function() {};

    virtual ptr<Expression> makeExpression( const NArgs  &args )
                                     throw( ExprException ) = 0;

    /// Get an instance of a function.
    static Function  *instantiate(const string  &name)
                             throw(AuroraBadEntityException);

    /// Get an instance of a math function.
    static Function  *instantiate_math(const string  &name);

    /// Get an instance of a standard function.
    static Function  *instantiate_standard(const string  &name);

    /// Get an instance of a spatio-temporal function.
    static Function  *instantiate_spatio_temporal(const string& name);
    
    /// Get an instance of a user defined function.
    static Function  *instantiate_udb(const string  &name);
};

BOREALIS_NAMESPACE_END
#endif                       // NARGS_H
