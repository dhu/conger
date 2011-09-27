#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <NMSTL/util>
#include <NMSTL/ptr>
#include <antlr/SemanticException.hpp>

#include "EString.h"
#include "TupleDescription.h"
#include "Exceptions.h"
#include "Timestamp.h"

// TRUE and FALSE need to be undefined so they do not conflict with the
// enumeration in ExprParserTokenTypes.hpp; which is generated from the
// expr.g grammar file.  This file is included by expr.g to achieve this.
//
#ifdef  TRUE
#undef  TRUE
#endif

#ifdef  FALSE
#undef  FALSE
#endif

//using namespace std;
//using namespace NMSTL;

BOREALIS_NAMESPACE_BEGIN

AURORA_EXCEPTION_TYPE( ExprException );
AURORA_EXCEPTION_TYPE( EvalException );



/// Context for use in generating an expression from an AST.  Before
/// parsing an expression, create an ExprContext to describe the
/// context in which the expression will be evaluated.
///
/// Currently, the only property of an ExprContext is a single
/// TupleDescription describing a tuple that will be available to an
/// expression when it is evaluated.
///
class ExprContext
{
  public:
    // Move to the DataType class???
    static const string to_string( DataType t );

    /// Get the description of the input tuple to the expression.
    ///
    /// @return an empty TupleDescription if no description was ever
    ///     provided
    const CatalogSchema  *get_context_schema() const
    {   return( &_desc );
    }

    /// Set the description of the input tuple to the expression.
    void set_context_schema( CatalogSchema  desc )
    {   _desc = desc;
    }

    /// Get the description of a named input tuple, and the index of that
    /// tuple in the input.
    void get_named_schema_info( string   name,
               const CatalogSchema  *&desc,
                       unsigned int  &index
                          ) const
    {
        NamedDesc::const_iterator sip = _named_desc.find(name);
        if (sip == _named_desc.end())
            return;
    
        else
        {
            desc  = &(sip->second.first);
            index = sip->second.second;
            
            DEBUG << "get_named_schema_info set index for " << name
                << " to " << index << " / " << sip->second.second;
        }
    }


    /// Set the description and index of a named input tuple.
    void set_named_schema_info( string   name,
                      CatalogSchema  *desc,
                       unsigned int   index )
    {
        _named_desc[name] = make_pair(*desc, index);
        DEBUG << "ExprContext set named schema info for " << name
              << " to " << _named_desc[name].first;
    }

    /************************************* work in progress
    /// Get the description of the input tuple to the expression.
    ///
    /// @return an empty TupleDescription if no description was ever
    ///     provided
    TupleDescription getTupleDescription() const { return _desc; }

    /// Set the description of the input tuple to the expression.
    void setTupleDescription(TupleDescription desc) { _desc = desc; }

    /// Get the description of a named input tuple, and the index of that
    /// tuple in the input.
    void getNamedTupleInfo( string   name,
                  TupleDescription  &desc,
                      unsigned int  &index
                          ) const
    {
        pair<TupleDescription, unsigned int> info = lookup(_named_desc, name);
        desc  = info.first;
        index = info.second;
    }


    /// Set the description and index of a named input tuple.
    void setNamedTupleInfo(string name,
                           TupleDescription desc,
                           unsigned int index)
    {
        _named_desc[name] = make_pair(desc, index);
    }
    **************************************/

    /// Find a field with a particular name.
    ///
    /// @throw ExprException if it doesn't find exactally one field.
    void find_field_name( string   field_name,
                        string  &tuple_name,
                  unsigned int  &tuple_index,
            //TupleDescription  &tuple_desc,
          const CatalogSchema  *&tuple_desc,
                  unsigned int  &field_index
                      ) const
                 throw( ExprException )
    {
        bool found = false;
	    	
        for ( NamedDesc::const_iterator i = _named_desc.begin();
              i != _named_desc.end(); ++i)
        {
        DEBUG << "Checking schema " << i->second.first;

            int off = i->second.first.index_of_field( field_name );

            if ( off >= 0 )
            {   if ( found )
                {    Throw(ExprException, "multiple fields " + field_name + " in input tuple(s)");
                }

                found = true;
                // whoa... lispy...
                tuple_name = i->first;
                tuple_index = i->second.second;
                tuple_desc = &i->second.first;
                field_index = off;
                DEBUG << "Found field " << field_name << " in  "
                      << tuple_name << " " << tuple_desc->as_string();
            }
        }

        if ( !found )
        {   Throw(ExprException, "no field " + field_name + " in any input tuple");
        }
    }
    
    string get_tuple_name(unsigned int idx = 0)
        throw( ExprException )
    {
        string r;
        NamedDesc::const_iterator nd_it = _named_desc.begin();
        NamedDesc::const_iterator nd_end = _named_desc.end();
        
        for (; nd_it != nd_end; ++nd_it) {
            if ( nd_it->second.second == idx ) {
                r = nd_it->first;
                break;
            }
        }
        
        if ( nd_it == nd_end )
            Throw(ExprException,
                  "Tuple index " + ::NMSTL::to_string(idx) +
                  " not found in expression context.");
                  
        return r;
    }

    string as_string() const
    {
        string r = "Input {" + _desc.as_string() + "} ";
        
        NamedDesc::const_iterator nd_it = _named_desc.begin();
        NamedDesc::const_iterator nd_end = _named_desc.end();
        
        for (; nd_it != nd_end; ++nd_it)
        {
            r += nd_it->first + "(" + NMSTL::to_string(nd_it->second.second) + ")" +
                nd_it->second.first.as_string() + " ";
        }
        
        return r;
    }

  private:
    CatalogSchema    _desc;

    typedef map<string, pair<CatalogSchema, unsigned int> > NamedDesc;
    NamedDesc        _named_desc;

    // Eventually - multiple descriptions (LEFT/RIGHT for join, etc.)
};



/// Context for use in evaluating an expression.  Before evaluating
/// an expression, create an EvalContext to describe the actual evaluation
/// environment.
///
/// Currently, the only property of an EvalContext is a single input
/// tuple (which must match the TupleDescription in the ExprContext
/// specified when the expression was originally parsed).
///
/// Note that creating an EvalContext is (by design) expensive since
/// an EvalContext contains an EStringPool; create only one
/// EvalContext for many evaluation and use reset() to clear the
/// context each time.
class EvalContext
{
  public:
    /// Constructor.
    ///
    /// @param num_tuples the number of tuples contained in the EvalContext
    /// @param pool_size the initial size of the memory pool to use for
    ///     string data
    EvalContext(unsigned int num_tuples = 1,
                unsigned int pool_size = EStringPool::DEFAULT_INITIAL_SIZE) :
        _tuple(num_tuples), _pool(pool_size)
    {}

    /// Resets the evaluation context.  Should be called after every
    /// evaluation.  This resets the memory pool and hence invalidates
    /// any memory allocated with alloc.
    void reset()
    {
        _pool.reset();
    }

    /// Sets the input tuple.
    void set_tuple(const void *tuple) { _tuple[0] = tuple; }

    /// Sets an input tuple.
    void set_tuple(unsigned int index, const void *tuple)
    {
        ASSERT(index < _tuple.size());
        _tuple[index] = tuple;
    }

    /// Returns the input tuple.
    const char *getTuple(unsigned int index = 0) const
    {
        ASSERT(index < _tuple.size());
        return static_cast<const char*>(_tuple[index]);
    }

    /// Allocates memory from the string pool.  This memory remains
    /// usable only until the context is reset.
    char *alloc(unsigned int size) const
    {
        return _pool.alloc(size);
    }

    /// Returns the string pool contained within this context.
    EStringPool& getPool() const
    {
        return _pool;
    }

  private:
    vector<const void*> _tuple;
    mutable EStringPool _pool;

    EvalContext(const EvalContext&);
    EvalContext& operator = (const EvalContext&);

    // Eventually - multiple tuples.
};




template<class T> class EvalAs;

/// Used as an argument to an Expression::evalImpl() method to
/// indicate that the expression should be evaluated as an int8.
template<> class EvalAs<int8> {};

/// Used as an argument to an Expression::evalImpl() method to
/// indicate that the expression should be evaluated as an int16.
template<> class EvalAs<int16> {};

/// Used as an argument to an Expression::evalImpl() method to
/// indicate that the expression should be evaluated as an int32.
template<> class EvalAs<int32> {};

/// Used as an argument to an Expression::evalImpl() method to
/// indicate that the expression should be evaluated as an int64.
template<> class EvalAs<int64> {};

/// Used as an argument to an Expression::evalImpl() method to
/// indicate that the expression should be evaluated as a single.
template<> class EvalAs<single> {};

/// Used as an argument to an Expression::evalImpl() method to
/// indicate that the expression should be evaluated as a double.
template<> class EvalAs<double> {};

/// Used as an argument to an Expression::evalImpl() method to
/// indicate that the expression should be evaluated as a bool.
template<> class EvalAs<bool> {};

/// Used as an argument to an Expression::evalImpl() method to
/// indicate that the expression should be evaluated as an EString.
template<> class EvalAs<EString> {};

/// Used as an argument to an Expression::evalImpl() method to
/// indicate that the expression should be evaluated as a string
/// object.
template<> class EvalAs<string> {};

/// Used as an argument to an Expression::evalImpl() method to
/// indicate that the expression should be evaluated as a Timestamp
/// object.
template<> class EvalAs<Timestamp> {};

/// The base class for expressions.
///
/// An expression has a static type: a bool, int8, int16, int32, int64,
/// single, double or EString.  It can be evaluated either as that type
/// or as a type which can be implicitly coerced to it; for example, if
/// an expression has integer type, eval<int32>(), eval<int32>(),
/// eval<int32>() or eval<int64>() will yield its integer value and
/// eval<single>() or eval<double>() will yield the same value but as a float.
/// (However, eval<bool>() will throw an exception since an int cannot
/// be coerced to an bool.)
///
/// When evaluating an expression, provide an EvalContext which
/// described the environment of the expression (e.g., the tuples to
/// which the tuple may refer).
///
/// Subclasses may override evalImpl() and isImpl() methods to specify
/// the behavior when an expression is evaluated as a particular type
/// (specified using an EvalAs<T> as the first argument).  Custom
/// expression types (e.g., user-defined functions) should generally
/// be derived from TypedExpression<> rather than Expression.
/// TypedExpression<> implements the default coercion behavior.
///
/// The default eval<xxx>() methods all throw exceptions, and the
/// default is<xxx>() functions all return false (unless otherwise
/// noted).  is<xxx>() should always return true iff eval<xxx>() is
/// typesafe.
///
class Expression
{
  public:
    Expression() {}

    /// Destructor.
    virtual ~Expression() {}

    /// Evaluates this expression as a T.  (Invokes the appropriate
    /// evalImpl<>() method.)
    template<typename T>
    T eval( const EvalContext  &ctxt ) throw( EvalException )
    {
        return( evalImpl( EvalAs<T>(), ctxt ));
    }

    /// Returns true iff this expression can be evaluated as a T.
    /// (Invokes the appropriate isImpl<>() method.)
    template<typename T> bool is() const
    {
        return( is_impl( EvalAs<T>() ));
    }

    /// Returns the size of the string returned by the expression, or
    /// -1 if not known.  By default returns -1.
    virtual int32 getStringLength() const { return -1; }

    /// Returns the size returned by the expression, or -1 if not known.
    int32 getLength() const
    {
        if      ( _type == DataType::BYTE   ) return sizeof( int8 );
        else if ( _type == DataType::SHORT  ) return sizeof( int16 );
        else if ( _type == DataType::INT    ) return sizeof( int32 );
        else if ( _type == DataType::LONG   ) return sizeof( int64 );
        else if ( _type == DataType::SINGLE ) return sizeof( single );
        else if ( _type == DataType::DOUBLE ) return sizeof( double );
        else if ( _type == DataType::BOOL   ) return sizeof( bool );
        else if ( _type == DataType::TIMESTAMP  ) return sizeof( Timestamp );
        else
            return( getStringLength() );
    }

    /// Evaluates into a buffer.  The buffer length must be getLength()
    /// (and getLength() must not be -1).
    void eval_into(char *buf, const EvalContext& ctxt) throw (EvalException)
    {
        ASSERT( getLength() != -1 );

        if      ( _type == DataType::BYTE   ) *(int8*)buf   = eval<int8>(ctxt);
        else if ( _type == DataType::SHORT  ) *(int16*)buf  = eval<int16>(ctxt);
        else if ( _type == DataType::INT    ) *(int32*)buf  = eval<int32>(ctxt);
        else if ( _type == DataType::LONG   ) *(int64*)buf  = eval<int64>(ctxt);
        else if ( _type == DataType::SINGLE ) *(single*)buf = eval<single>(ctxt);
        else if ( _type == DataType::DOUBLE ) *(double*)buf = eval<double>(ctxt);
        else if ( _type == DataType::BOOL   ) *(bool*)buf   = eval<bool>(ctxt);
        else if ( _type == DataType::TIMESTAMP ) *( Timestamp* )buf = eval<Timestamp>(ctxt);
        else
        {
            EString str = eval<EString>(ctxt);
            __builtin_memcpy( buf, str.data(), str.length() );

            int len = getStringLength();
            ASSERT( len >= 0 );
            ASSERT( str.length() <= (unsigned int)len );
            __builtin_memset( buf + str.length(), 0, len - str.length() );
        }
    }

    /// Returns true iff the expression can be evaluated as a numeric.
    /// By default returns is<double>().
    virtual bool is_numeric() const { return( is<double>() ); }

    /// Returns the actual type to which the expression will evaluate.
    DataType getType() const { return _type; }
    
    // returns whether or not the expression uses array references
    bool array_use() const { return _array_flag; }

    /// Returns a TupleDescription::Field describing this value.
    /// Throws an exception if this is a variable-length string field.
    //???TupleDescription::Field getField(string name = string()) const throw (ExprException);
    SchemaField get_schema_field(string name = string()) const throw (ExprException);

    /// Dumps a human-readable representation of the expression.
    virtual void dump( string& s, unsigned int indent = 0 ) const
    {
        for ( unsigned int i = 0; i < indent; ++i )
        {   s += "    ";
        }

        s << typeid( *this );

        string val = value();
        if ( !val.empty() )
        {   s << " - " << val;
        }

        s << "\n";

        dump_children( s, indent + 1 );
    }

    /// Returns a dump of the expression.
    string as_string() const
    {
        return( NMSTL::to_string( typeid( *this )));
    }

    /// Parses and returns an expression given a context.
    static ptr<Expression> parse( string  expr, const ExprContext  &ctxt )
                           throw( ExprException );

    /// Evaluate each of the expressions in exprs, concatenating the
    /// results, and place them in the buffer. The buffer must be as
    /// large as them sum of the getLength()s of the expressions, and
    /// none can be -1.
    static void eval_vector_into( const vector<ptr<Expression> >  &exprs,
                                char  *buf, const EvalContext  &ctxt )
        throw( EvalException );

    
    static ptr<Expression> makeArrayReference( string   tuple_name,
                                                 string   field_name,
                                      const ExprContext  &ctxt
                                               )
                                          throw( ExprException );
    
    
    /// Create a field expression from the given tuple and field
    /// names. If tuple_name is "", then there must be exactly one
    /// tuple in the context with a field named field_name or an
    /// exception is thrown.
    static ptr<Expression> makeFieldExpression( string   tuple_name,
                                                 string   field_name,
                                      const ExprContext  &ctxt
                                               )
                                          throw( ExprException );

    /// Return a field suitable for holding this expression.
    //TupleDescription::Field toField( string  name ) const
    //                          throw( AuroraException );
    SchemaField  to_field( string  name ) const
                   throw( AuroraException );

  protected:

    Expression( DataType  type ) : _type(type) {}

    /// Evaluate as a bool.
    virtual bool evalImpl( EvalAs<bool>, const EvalContext& )
                    throw( EvalException )
    {
        throw EvalException( "Cannot coerce result of "
                           + NMSTL::to_string( typeid( *this )) + " to bool" );
    }

    /// Returns true iff the expression can be evaluated as a bool.
    virtual bool is_impl( EvalAs<bool> ) const { return( false ); }


    /// Evaluate as an int8.
    virtual int8 evalImpl( EvalAs<int8>, const EvalContext& )
                    throw( EvalException )
    {
        throw EvalException( "Cannot coerce result of "
                           + NMSTL::to_string( typeid( *this )) + " to byte" );
    }

    /// Returns true iff the expression can be evaluated as an int8.
    virtual bool is_impl( EvalAs<int8> ) const { return( false ); }



    /// Evaluate as an int16.
    virtual int16 evalImpl( EvalAs<int16>, const EvalContext& )
                     throw( EvalException )
    {
        throw EvalException( "Cannot coerce result of "
                           + NMSTL::to_string( typeid( *this )) + " to short" );
    }

    /// Returns true iff the expression can be evaluated as an int16.
    virtual bool is_impl( EvalAs<int16> ) const { return( false ); }



    /// Evaluate as an int32.
    virtual int32 evalImpl( EvalAs<int32>, const EvalContext& )
                     throw( EvalException )
    {
        throw EvalException( "Cannot coerce result of "
                           + NMSTL::to_string( typeid( *this )) + " to int" );
    }

    /// Returns true iff the expression can be evaluated as an int32.
    virtual bool is_impl( EvalAs<int32> ) const { return( false ); }



    /// Evaluate as an int64.
    virtual int64 evalImpl( EvalAs<int64>, const EvalContext& )
                     throw( EvalException )
    {
        throw EvalException( "Cannot coerce result of "
                           + NMSTL::to_string( typeid( *this )) + " to long" );
    }

    /// Returns true iff the expression can be evaluated as an int64.
    virtual bool is_impl( EvalAs<int64> ) const { return( false ); }



    /// Evaluate as a single.
    virtual single evalImpl( EvalAs<single>, const EvalContext& )
                      throw( EvalException )
    {
        throw EvalException( "Cannot coerce result of "
                           + NMSTL::to_string( typeid( *this )) + " to single" );
    }

    /// Returns true iff the expression can be evaluated as a single.
    virtual bool is_impl( EvalAs<single> ) const { return( false ); }



    /// Evaluate as a double.
    virtual double evalImpl( EvalAs<double>, const EvalContext& )
                      throw( EvalException )
    {
        throw EvalException( "Cannot coerce result of "
                           + NMSTL::to_string( typeid( *this )) + " to double" );
    }

    /// Returns true iff the expression can be evaluated as a double.
    virtual bool is_impl( EvalAs<double> ) const { return( false ); }



    /// Evaluate as a string.
    virtual EString evalImpl( EvalAs<EString>, const EvalContext& )
                       throw( EvalException )
    {
        throw EvalException( "Cannot coerce result of "
                           + NMSTL::to_string( typeid( *this )) + " to EString" );
    }

    /// Returns true iff the expression can be evaluated as an EString.
    virtual bool is_impl( EvalAs<EString> ) const { return false; }

    /// Evaluate as a string.  By default calls eval<EString>; in
    /// general subclasses should override evalImpl(EvalAs<EString>)
    /// rather than this.
    virtual string evalImpl( EvalAs<string>, const EvalContext& ctxt )
                      throw( EvalException )
    {
        return( NMSTL::to_string( eval<EString>( ctxt )));
    }

    /// Returns true iff the expression can be evaluated as a string.  By
    /// default calls is_EString().
    virtual bool is_impl( EvalAs<string> ) const { return is<EString>(); }


    /// Evaluate as a Timestamp
    virtual Timestamp evalImpl( EvalAs<Timestamp>, const EvalContext& )
                      throw( EvalException )
    {
        throw EvalException( "Cannot coerce result of "
                           + NMSTL::to_string( typeid( *this )) + " to Timestamp" );
    }

    /// Returns true iff the expression can be evaluated as a double.
    virtual bool is_impl( EvalAs<Timestamp> ) const { return( false ); }



    virtual void dump_children( string& s, unsigned int indent ) const {}
    virtual string value() const { return string(); }

    NMSTL_SERIAL_BASE(Expression, long, << _type);

  private:
    DataType  _type;
    bool  _array_flag;
};



template<typename T> class TypedExpression;

/// An expression that evaluates to a boolean value.
///
/// Expressions returning a bool should derive from this class and
/// implement the evalImpl(EvalAs<bool>) method.
template<>
class TypedExpression<bool> : public Expression
{
  public:
    TypedExpression() : Expression( DataType::BOOL ) {}

    virtual bool evalImpl( EvalAs<bool>, const EvalContext  &ctxt )
                    throw( EvalException ) = 0;

    bool is_impl( EvalAs<bool> ) const { return true; }



    int8 evalImpl( EvalAs<int8>, const EvalContext  &ctxt)
             throw( EvalException ) { return eval<bool>(ctxt); }

    bool is_impl( EvalAs<int8> ) const { return true; }



    int16 evalImpl( EvalAs<int16>, const EvalContext  &ctxt)
             throw( EvalException ) { return eval<bool>(ctxt); }

    bool is_impl( EvalAs<int16> ) const { return true; }



    int32 evalImpl( EvalAs<int32>, const EvalContext  &ctxt)
             throw( EvalException ) { return eval<bool>(ctxt); }

    bool is_impl( EvalAs<int32> ) const { return true; }


    int64 evalImpl( EvalAs<int64>, const EvalContext  &ctxt)
             throw( EvalException ) { return eval<bool>(ctxt); }

    bool is_impl( EvalAs<int64> ) const { return true; }


    single evalImpl( EvalAs<single>, const EvalContext &ctxt )
              throw( EvalException ) { return eval<bool>(ctxt); }

    bool is_impl( EvalAs<single> ) const { return true; }


    double evalImpl( EvalAs<double>, const EvalContext &ctxt )
              throw( EvalException ) { return eval<bool>(ctxt); }

    bool is_impl( EvalAs<double> ) const { return true; }


    EString evalImpl( EvalAs<EString>, const EvalContext  &ctxt )
               throw( EvalException )
    {
        return( EString::coerce( eval<bool>(ctxt), ctxt.getPool() ));
    }

    bool is_impl( EvalAs<EString> ) const { return true; }

    // Do we want to convert timestamps into bool? Maybe?
    //Timestamp evalImpl( EvalAs<Timestamp>, const EvalContext  &ctxt)
    //  throw( EvalException ) { return eval<bool>(ctxt); }
    //bool isImpl( EvalAs<Timestamp> ) const { return true; }

};



/// An expression that evaluates to an 8 bit integer value.
///
/// Expressions returning int8 should derive from this class and
/// implement the evalImpl(EvalAs<int8>) method.
template<>
class TypedExpression<int8> : public Expression
{
  public:
    TypedExpression() : Expression( DataType::BYTE ) {}

    virtual int8 evalImpl( EvalAs<int8>, const EvalContext  &ctxt )
                     throw( EvalException ) = 0;

    bool is_impl( EvalAs<int8> ) const { return( true ); }


    int32 evalImpl( EvalAs<int32>, const EvalContext  &ctxt )
             throw( EvalException ) { return eval<int8>(ctxt); }

    bool is_impl( EvalAs<int32> ) const { return( true ); }


    int16 evalImpl( EvalAs<int16>, const EvalContext  &ctxt )
             throw( EvalException ) { return eval<int8>(ctxt); }

    bool is_impl( EvalAs<int16> ) const { return( true ); }


    int64 evalImpl( EvalAs<int64>, const EvalContext  &ctxt )
             throw( EvalException ) { return eval<int8>(ctxt); }

    bool is_impl( EvalAs<int64> ) const { return( true ); }


    single evalImpl( EvalAs<single>, const EvalContext  &ctxt )
             throw( EvalException ) { return eval<int8>(ctxt); }

    bool is_impl( EvalAs<single> ) const { return( true ); }


    double evalImpl( EvalAs<double>, const EvalContext  &ctxt )
             throw( EvalException ) { return eval<int8>(ctxt); }

    bool is_impl( EvalAs<double> ) const { return( true ); }


    EString evalImpl( EvalAs<EString>, const EvalContext  &ctxt )
               throw( EvalException )
    {
        return( EString::coerce( eval<int64>(ctxt), ctxt.getPool() ));
    }

    bool is_impl(EvalAs<EString>) const { return( true ); }

    //Timestamp evalImpl( EvalAs<Timestamp>, const EvalContext  &ctxt)
    //  throw( EvalException ) { return eval<int8>(ctxt); }
    //bool isImpl( EvalAs<Timestamp> ) const { return true; }
};



/// An expression that evaluates to a 16 bit integer value.
///
/// Expressions returning int16 should derive from this class and
/// implement the evalImpl(EvalAs<int16>) method.
template<>
class TypedExpression<int16> : public Expression
{
  public:
    TypedExpression() : Expression( DataType::SHORT ) {}

    virtual int16 evalImpl( EvalAs<int16>, const EvalContext  &ctxt )
                     throw( EvalException ) = 0;

    bool is_impl( EvalAs<int16> ) const { return( true ); }


    int32 evalImpl( EvalAs<int32>, const EvalContext  &ctxt )
             throw( EvalException ) { return eval<int16>(ctxt); }

    bool is_impl( EvalAs<int32> ) const { return( true ); }


    int64 evalImpl( EvalAs<int64>, const EvalContext  &ctxt )
             throw( EvalException ) { return eval<int16>(ctxt); }

    bool is_impl( EvalAs<int64> ) const { return( true ); }


    int8 evalImpl( EvalAs<int8>, const EvalContext  &ctxt )
             throw( EvalException ) { return eval<int16>(ctxt); }

    bool is_impl( EvalAs<int8> ) const { return( true ); }


    single evalImpl( EvalAs<single>, const EvalContext  &ctxt )
             throw( EvalException ) { return eval<int16>(ctxt); }

    bool is_impl( EvalAs<single> ) const { return( true ); }


    double evalImpl( EvalAs<double>, const EvalContext  &ctxt )
             throw( EvalException ) { return eval<int16>(ctxt); }

    bool is_impl( EvalAs<double> ) const { return( true ); }


    EString evalImpl( EvalAs<EString>, const EvalContext  &ctxt )
               throw( EvalException )
    {
        return( EString::coerce( eval<int16>(ctxt), ctxt.getPool() ));
    }

    bool is_impl(EvalAs<EString>) const { return( true ); }

    //Timestamp evalImpl( EvalAs<Timestamp>, const EvalContext  &ctxt)
    //  throw( EvalException ) { return eval<int16>(ctxt); }
    //bool isImpl( EvalAs<Timestamp> ) const { return true; }
};



/// An expression that evaluates to a 32 bit integer value.
///
/// Expressions returning int32 should derive from this class and
/// implement the evalImpl(EvalAs<int32>) method.
template<>
class TypedExpression<int32> : public Expression
{
  public:
    TypedExpression() : Expression( DataType::INT ) {}

    virtual int32 evalImpl( EvalAs<int32>, const EvalContext  &ctxt )
                     throw( EvalException ) = 0;

    bool is_impl( EvalAs<int32> ) const { return( true ); }


    int8 evalImpl( EvalAs<int8>, const EvalContext  &ctxt )
             throw( EvalException ) { return eval<int32>(ctxt); }

    bool is_impl( EvalAs<int8> ) const { return( true ); }


    int16 evalImpl( EvalAs<int16>, const EvalContext  &ctxt )
             throw( EvalException ) { return eval<int32>(ctxt); }

    bool is_impl( EvalAs<int16> ) const { return( true ); }


    int64 evalImpl( EvalAs<int64>, const EvalContext  &ctxt )
             throw( EvalException ) { return eval<int32>(ctxt); }

    bool is_impl( EvalAs<int64> ) const { return( true ); }


    single evalImpl( EvalAs<single>, const EvalContext  &ctxt )
             throw( EvalException ) { return eval<int32>(ctxt); }

    bool is_impl( EvalAs<single> ) const { return( true ); }


    double evalImpl( EvalAs<double>, const EvalContext  &ctxt )
             throw( EvalException ) { return eval<int32>(ctxt); }

    bool is_impl( EvalAs<double> ) const { return( true ); }


    EString evalImpl( EvalAs<EString>, const EvalContext  &ctxt )
               throw( EvalException )
    {
        return( EString::coerce( eval<int32>(ctxt), ctxt.getPool() ));
    }

    bool is_impl(EvalAs<EString>) const { return( true ); }

    //Timestamp evalImpl( EvalAs<Timestamp>, const EvalContext  &ctxt)
    //  throw( EvalException ) { return eval<int32>(ctxt); }
    //bool isImpl( EvalAs<Timestamp> ) const { return true; }

};


/// An expression that evaluates to a 64 bit integer value.
///
/// Expressions returning int64 should derive from this class and
/// implement the evalImpl(EvalAs<int64>) method.
template<>
class TypedExpression<int64> : public Expression
{
  public:
    TypedExpression() : Expression( DataType::LONG ) {}

    virtual int64 evalImpl( EvalAs<int64>, const EvalContext  &ctxt )
                     throw( EvalException ) = 0;

    bool is_impl( EvalAs<int64> ) const { return( true ); }


    int32 evalImpl( EvalAs<int32>, const EvalContext  &ctxt )
             throw( EvalException ) { return eval<int64>(ctxt); }

    bool is_impl( EvalAs<int32> ) const { return( true ); }


    int16 evalImpl( EvalAs<int16>, const EvalContext  &ctxt )
             throw( EvalException ) { return eval<int64>(ctxt); }

    bool is_impl( EvalAs<int16> ) const { return( true ); }


    int8 evalImpl( EvalAs<int8>, const EvalContext  &ctxt )
             throw( EvalException ) { return eval<int64>(ctxt); }

    bool is_impl( EvalAs<int8> ) const { return( true ); }


    single evalImpl( EvalAs<single>, const EvalContext  &ctxt )
             throw( EvalException ) { return eval<int64>(ctxt); }

    bool is_impl( EvalAs<single> ) const { return( true ); }


    double evalImpl( EvalAs<double>, const EvalContext  &ctxt )
             throw( EvalException ) { return eval<int64>(ctxt); }

    bool is_impl( EvalAs<double> ) const { return( true ); }


    EString evalImpl( EvalAs<EString>, const EvalContext  &ctxt )
               throw( EvalException )
    {
        return( EString::coerce( eval<int64>(ctxt), ctxt.getPool() ));
    }

    bool is_impl(EvalAs<EString>) const { return( true ); }

    //Timestamp evalImpl( EvalAs<Timestamp>, const EvalContext  &ctxt)
    //  throw( EvalException ) { return eval<int64>(ctxt); }
    //bool isImpl( EvalAs<Timestamp> ) const { return true; }
};



/// An expression that evaluates to a 32 bit floating-point value.
///
/// Expressions returning a single should derive from this class and
/// implement the evalImpl(EvalAs<single>) method.
template<>
class TypedExpression<single> : public Expression
{
  public:
    TypedExpression() : Expression( DataType::SINGLE ) {}

    virtual single evalImpl( EvalAs<single>, const EvalContext  &ctxt )
                      throw( EvalException ) = 0;

    bool is_impl( EvalAs<single> ) const { return( true ); }

    double evalImpl( EvalAs<double>, const EvalContext  &ctxt )
             throw( EvalException ) { return eval<single>(ctxt); }

    bool is_impl( EvalAs<double> ) const { return( true ); }

    EString evalImpl( EvalAs<EString>, const EvalContext  &ctxt )
               throw( EvalException )
    {
        return( EString::coerce( eval<single>(ctxt), ctxt.getPool() ));
    }

    bool is_impl(EvalAs<EString>) const { return( true ); }

    //Timestamp evalImpl( EvalAs<Timestamp>, const EvalContext  &ctxt)
    //  throw( EvalException ) { return eval<single>(ctxt); }
    //bool isImpl( EvalAs<Timestamp> ) const { return true; }
};


/// An expression that evaluates to a 64 bit floating-point value.
///
/// Expressions returning a double should derive from this class and
/// implement the evalImpl(EvalAs<double>) method.
template<>
class TypedExpression<double> : public Expression
{
  public:
    TypedExpression() : Expression( DataType::DOUBLE ) {}

    virtual double evalImpl( EvalAs<double>, const EvalContext  &ctxt )
                      throw( EvalException ) = 0;

    bool is_impl( EvalAs<double> ) const { return( true ); }

    EString evalImpl( EvalAs<EString>, const EvalContext  &ctxt )
               throw( EvalException )
    {
        return( EString::coerce(eval<double>(ctxt), ctxt.getPool() ));
    }

    bool is_impl(EvalAs<EString>) const { return( true ); }

    //Timestamp evalImpl( EvalAs<Timestamp>, const EvalContext  &ctxt)
    //  throw( EvalException ) { return eval<double>(ctxt); }
    //bool isImpl( EvalAs<Timestamp> ) const { return true; }
};



/// An expression that evaluates to a string value.
///
/// Expressions returning a string should derive from this class and
/// implement the evalImpl(EvalAs<EString>) method.
template<>
class TypedExpression<EString> : public Expression
{
  public:
    virtual EString evalImpl( EvalAs<EString>, const EvalContext  &ctxt )
                       throw( EvalException ) = 0;

    bool is_impl( EvalAs<EString> ) const { return( true ); }

    TypedExpression() : Expression( DataType::STRING ) {}
};

/// An expression that evaluates to a Timestamp value
///
/// Expressions returning a Timestamp should derive from this class and
/// implement the evalImpl(EvalAs<Timestamp>) method.
template<>
class TypedExpression<Timestamp> : public Expression
{
 public:
    TypedExpression() : Expression ( DataType::TIMESTAMP ) {}

    virtual Timestamp evalImpl( EvalAs<Timestamp>, const EvalContext  &ctxt)
        throw( EvalException ) = 0;

    bool is_impl( EvalAs<Timestamp> ) const { return true; }
};

BOREALIS_NAMESPACE_END

#endif                       // EXPRESSION_H
