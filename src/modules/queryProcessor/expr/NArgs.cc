#include  "NArgs.h"


////////////////////////////////////////////////////////////////////////////////
//
Function  *Function::instantiate(const string  &name)
                           throw(AuroraBadEntityException)
{
    Function  *function;
//..............................................................................


    function = instantiate_math(name);

    if (function == NULL)
    {   function = instantiate_spatio_temporal(name);
    }
    
    if (function == NULL)
    {   function = instantiate_standard(name);
    }

    if (function == NULL)
    {   function = instantiate_udb(name);
    }
    
    if (function == NULL)
    {   Throw(AuroraBadEntityException, "Unknown function:  " + name);
    }

    return(function);
}


// This is in the standard namespace so it will work with expr.g.
//using namespace std;
//using namespace Borealis;


/// Constructor.  Each non-null argument is added to the argument
/// list.
NArgs::NArgs( ptr<Expression> a,
              ptr<Expression> b,
              ptr<Expression> c )
{
    if ( a ) _args.push_back( a );
    if ( b ) _args.push_back( b );
    if ( c ) _args.push_back( c );
}

/// Constructs an NArgs, setting the argument list to the contents
/// of the vector provided.
NArgs::NArgs(const vector<ptr<Expression> >& args) : _args(args) {}

/// Throws an ExprException if the number of arguments is not equal
/// to args_required.
///
void NArgs::requireNumArgs( int  args_required ) const
                     throw( ExprException )
{
    if ( args_required != VARARGS && (int)_args.size() != args_required )
    {   Throw( ExprException,
               "requires " + ( args_required == 1 ?
                   to_string( "one argument" ) :
                   to_string( args_required ) + to_string( " arguments" )));
    }
}


/// Throws an ExprException if the number of arguments is not between
/// minargs and maxargs, inclusive.  (If maxargs is VARARGS, then there
/// is no upper limit to the number of arguments allowed.)
///
void NArgs::requireNumArgs( int   minargs,
                            int   maxargs
                          ) const
                     throw( ExprException )
{
    if (( int( _args.size() ) < minargs ) ||
        ( maxargs != VARARGS && int( _args.size() ) > maxargs ))
    {
        string err;

        if ( maxargs == VARARGS )
        {   err << "requires " << minargs << " or more arguments";
        }

        if ( minargs + 1 == maxargs )
        {   if ( maxargs == 1 )
            {   err << "requires zero or one argument";
            }
            else
            {   err << "requires " << minargs << " or "
                                   << maxargs << " arguments";
            }
        }
        else
        {   err << "requires between " << minargs << " and "
                                       << maxargs << " arguments";
        }

        Throw( ExprException, err );
    }
}


/// Requires that the index'th argument (zero-relative) is of one
/// of the provided types.
///
void NArgs::require_arg_type( unsigned int  index,
                                DataType  type1,
                                DataType  type2,
                                DataType  type3,
                                DataType  type4,
                                DataType  type5,
                                DataType  type6
                          ) const
                     throw( ExprException )
{
    if ( index < _args.size() )
    {   DataType type =_args[ index ]->getType();

        if (type != type1 && type != type2  &&
            type != type3 && type != type4  &&
            type != type5 && type != type6)
        {
            Throw( ExprException,
                   "invalid type for argument #" + to_string( index + 1 ));
        }
    }
}


/// Returns the idx'th argument (zero-relative).  Undefined
/// behavior if the argument list is too short.
///
const ptr<Expression>& NArgs::arg( unsigned int  idx ) const
{
    ASSERT(idx < _args.size());
    return( _args[ idx ]);
}


/// Returns the number of arguments.
///
unsigned int NArgs::num_args() const
{
    return( _args.size() );
}


/// Returns the idx'th argument (zero-relative).  Undefined
/// behavior if the argument list is too short.
///
const ptr<Expression>& NArgs::operator[]( unsigned int  idx ) const
{
    return( arg( idx ));
}


/// Returns the number of arguments.
unsigned int NArgs::size() const { return( _args.size() ); }


/// Returns true iff there are zero arguments.
bool NArgs::empty() const { return( _args.empty() ); }


/// Sets the i'th argument, if there exists one already.
void NArgs::set_arg( unsigned int idx, ptr<Expression> expr )
{
    assert(_args.size() > idx);
    _args[idx] = expr;
}
