#ifndef EXPRUTIL_H
#define EXPRUTIL_H

#include <cmath>

#include "NArgs.h"
#include "DynamicArray.h"

using namespace Borealis;

inline vector<ptr<Expression> > makeArgs(ptr<Expression> a = ptr<Expression>(),
                                         ptr<Expression> b = ptr<Expression>(),
                                         ptr<Expression> c = ptr<Expression>())
{
    vector<ptr<Expression> > out;

    if (a) { out.push_back(a); }
    if (b) { out.push_back(b); }
    if (c) { out.push_back(c); }

    return out;
}


// A Constant expression of type T.
template<typename T>
class ConstantExpression : public TypedExpression<T>
{
  public:
    ConstantExpression(T val) : _val(val) {}
    T evalImpl(EvalAs<T>, const EvalContext& ctxt) throw (EvalException) { return _val; }

    T getValue() const { return _val; }
    string value() const { return to_string(_val); }

  private:
    T _val;
};

template<>
class ConstantExpression<EString> : public TypedExpression<EString>
{
  public:
    ConstantExpression(string val) : _val(val) {}
    EString evalImpl(EvalAs<EString>, const EvalContext& ctxt) throw (EvalException) { return EString(_val); }

    string value() const { return "\"" + to_escaped_string(to_string(_val)) + "\""; }

  private:
    string _val;
};


// An expression returning the value, of type T, at a given offset in
// the input tuple.
template<typename T>
class FieldExpression : public TypedExpression<T>
{
  protected:
    unsigned int _which_tuple;
    unsigned int _offset;
    unsigned int _array_offset;
    string _field_name;
    string _array_field_name;

  public:
    FieldExpression(unsigned int which_tuple, unsigned int offset, string field_name, unsigned int array_offset, string array_field_name) :
        _which_tuple(which_tuple), _offset(offset), _array_offset(array_offset), _field_name(field_name), _array_field_name(array_field_name) {}

    T evalImpl(EvalAs<T>, const EvalContext& ctxt) throw (EvalException)
    {
        const char *contents = ctxt.getTuple(_which_tuple) + _offset;
    if(_array_field_name != "")
     {
        
       DynamicArray *current_array = (DynamicArray *) (*(uint32 *) contents);
       uint8 *current_element = current_array->get_iterator_element();
       
       return *(T*)(current_element + _array_offset);
     }
    else
      {
        return *(T*) contents;
      }
    }

    void set_field_offset(unsigned int offset)
    {
        _offset = offset;
    }

    void set_field_name(string field_name)
    {
        _field_name = field_name;
    }
    
    string get_field_name() { return _field_name; }

    string get_tuple_name(ExprContext& ctxt) throw( ExprException )
    {
        string r = ctxt.get_tuple_name(_which_tuple);
        DEBUG << "get_tuple_name " << _which_tuple << " " << r;
        return r;
    }
    
    string get_full_name(ExprContext& ctxt)
    {
        string r;
        DEBUG << "get_full_name ctxt " << ctxt << " tuple " << _which_tuple;
        try {
            string tuple_name = get_tuple_name(ctxt);
            r = tuple_name + "." + _field_name;
        } catch (ExprException e) {
            // yna -- added for the context's unnamed schema.
            r = _field_name;            
        }
        
        return r;
    }
};

class ByteFieldExpression: public FieldExpression<int8>
{
  public:
    ByteFieldExpression():FieldExpression<int8>(0,0,"",0,"") {};
    ByteFieldExpression(unsigned int which_tuple, unsigned int offset, string field_name, 
            unsigned int array_offset, string array_field_name):FieldExpression<int8>(which_tuple, offset, field_name, array_offset, array_field_name) {};
////???    NMSTL_SERIAL_SUBCLASS(ByteFieldExpression, Expression, << _which_tuple << _offset << _field_name);
};

class ShortFieldExpression: public FieldExpression<int16>
{
  public:
    ShortFieldExpression():FieldExpression<int16>(0,0,"",0,"") {};
    ShortFieldExpression(unsigned int which_tuple, unsigned int offset,
        string field_name, unsigned int array_offset, string array_field_name):FieldExpression<int16>(which_tuple, offset, field_name, array_offset, array_field_name) {};
////???    NMSTL_SERIAL_SUBCLASS(ShortFieldExpression, Expression, << _which_tuple << _offset << _field_name);
};

class IntFieldExpression: public FieldExpression<int32>
{
  public:
    IntFieldExpression():FieldExpression<int32>(0,0,"",0,"") {};
    IntFieldExpression(unsigned int which_tuple, unsigned int offset,
        string field_name, unsigned int array_offset, string array_field_name):FieldExpression<int32>(which_tuple, offset, field_name, array_offset, array_field_name) {};
////???    NMSTL_SERIAL_SUBCLASS(IntFieldExpression, Expression, << _which_tuple << _offset << _field_name);
};


class LongFieldExpression: public FieldExpression<int64>
{
  public:
    LongFieldExpression():FieldExpression<int64>(0,0,"",0,"") {};
    LongFieldExpression(unsigned int which_tuple, unsigned int offset,
        string field_name, unsigned int array_offset, string array_field_name):FieldExpression<int64>(which_tuple, offset, field_name, array_offset, array_field_name) {};
////???    NMSTL_SERIAL_SUBCLASS(IntFieldExpression, Expression, << _which_tuple << _offset << _field_name);
};


class SingleFieldExpression: public FieldExpression<single>
{
  public:
    SingleFieldExpression(unsigned int which_tuple, unsigned int offset,
        string field_name, unsigned int array_offset, string array_field_name):FieldExpression<single>(which_tuple, offset, field_name, array_offset, array_field_name) {};
//    NMSTL_SERIAL_SUBCLASS(SingleFieldExpression, Expression, << _which_tuple << _offset << _field_name);
};

class DoubleFieldExpression: public FieldExpression<double>
{
  public:
    DoubleFieldExpression(unsigned int which_tuple, unsigned int offset,
        string field_name, unsigned int array_offset = 0, string array_field_name = ""):FieldExpression<double>(which_tuple, offset, field_name, array_offset, array_field_name) {};
  //  NMSTL_SERIAL_SUBCLASS(DoubleFieldExpression, Expression, << _which_tuple << _offset << _field_name);
};

class StringFieldExpression : public TypedExpression<EString>
{
  private:
    unsigned int _which_tuple;
    unsigned int _offset;
    string _field_name;
    unsigned int _length;
    unsigned int _array_offset;
    string _array_field_name;
  public:
    StringFieldExpression() {StringFieldExpression(0,0,"",0,0,"");};
    StringFieldExpression(unsigned int which_tuple, unsigned int offset, string field_name, unsigned int length, unsigned int array_offset, string array_field_name) :
        _which_tuple(which_tuple), _offset(offset), _field_name(field_name), _length(length), _array_offset(array_offset), _array_field_name(array_field_name) {}

    EString evalImpl(EvalAs<EString>, const EvalContext& ctxt) throw (EvalException)
    {
        const char *field = ctxt.getTuple(_which_tuple) + _offset;
        const char *end = field + _length;

        // ignore null padding at end
        while (end != field)
        {
            if (*(end - 1) != 0) break;
            --end;
        }

        return EString(field, end - field);
    }

    string get_field_name() { return _field_name; }

    int32 getStringLength() const { return _length; }

////???    NMSTL_SERIAL_SUBCLASS(StringFieldExpression, Expression, << _which_tuple << _offset << _field_name << _length);

};


class TimestampFieldExpression: public FieldExpression<Timestamp>
{
  public:
    TimestampFieldExpression(unsigned int which_tuple, unsigned int offset,
        string field_name, unsigned int array_offset, string array_field_name):FieldExpression<Timestamp>(which_tuple, offset, field_name, array_offset, array_field_name) {};
//    NMSTL_SERIAL_SUBCLASS(TimestampFieldExpression, Expression, << _which_tuple << _offset << _field_name);
};


#define OP(N, Name, Out, Expr)                                               \
template<typename T>                                                         \
class Name##Expression : public TypedExpression<Out>, public NArgs           \
{ public:                                                                    \
    Name##Expression(const NArgs& args) throw (ExprException) : NArgs(args)  \
    {   requireNumArgs(N);                                                   \
    }                                                                        \
    Out evalImpl(EvalAs<Out>, const EvalContext& ctxt) throw (EvalException) \
    {   return Expr;                                                         \
    }                                                                        \
};                                                                           \
class Name##Expressions                                                      \
{ public:                                                                    \
    template <class T>                                                       \
    class forType                                                            \
    { public:                                                                \
        typedef Name##Expression<T> ExprType;                                \
    };                                                                       \
};


#define EPSILON(T) std::numeric_limits<T>::epsilon()

template <class T>
inline bool equal(T a, T b) { return a == b; }

template <>
inline bool equal(single a, single b) { return( fabs(a - b) < EPSILON(single) ); }

template <>
inline bool equal(double a, double b) { return( fabs(a - b) < EPSILON(double) ); }

#undef EPSILON

template <class T>
inline T exprMod(T a, T b) { return a % b; }

template <>
inline single exprMod( single a, single b) { return( fmodf( a, b )); }

template <>
inline double exprMod( double a, double b) { return( fmod( a, b )); }


template <class T>
inline T divide(T a, T b)
{
    if (b == 0)
    {   WARN << "Divided by zero.";
        return numeric_limits<T>::infinity();
    }
    else
    {   return a / b;
    }
}

// Actual operator declarations

#define A (_args[0]->template eval<T>(ctxt))
#define B (_args[1]->template eval<T>(ctxt))
#define C (_args[2]->template eval<T>(ctxt))

#define A_as(type) (_args[0]->template eval<type>(ctxt))

OP(2, Plus,   T,    A + B);
OP(2, Minus,  T,    A - B);
OP(2, Times,  T,    A * B);
OP(2, Div,    T,    divide(A,B));
OP(2, Mod,    T,    exprMod(A, B));
OP(2, Concat, T,    A.concat(B, ctxt.getPool()));

//OP(2, EQ,     bool, A == B);
//OP(2, NE,     bool, A != B);
//OP(2, LT,     bool, A < B);
//OP(2, GT,     bool, A > B);
//OP(2, LE,     bool, A <= B);
//OP(2, GE,     bool, A >= B);

OP(2, EQ,     bool, (DEBUG << A << " ==? " << B,  equal(A, B)));
OP(2, NE,     bool, (DEBUG << A << " !=? " << B,  A != B));
OP(2, LT,     bool, (DEBUG << A << " <?  " << B,  A < B));
OP(2, GT,     bool, (DEBUG << A << " >?  " << B,  A > B));
OP(2, LE,     bool, (DEBUG << A << " <=? " << B,  A <= B));
OP(2, GE,     bool, (DEBUG << A << " >=? " << B,  A >= B));

OP(2, And,    bool, A && B);
OP(2, Or,     bool, A || B);

OP(1, UMinus, T,    -A);
OP(1, Not,    T,    !A);

OP(3, IfThenElse, T, A_as(bool) ? B : C);

#undef A
#undef B
#undef C



template <typename Types>
ptr<Expression> arithPromote( ptr<Expression>  l, ptr<Expression>  r )
                       throw( ExprException )
{
  // DEBUG << "arithPromote l:" << l->getType() << "/" << l->is_numeric()
  //    << " r:" << r->getType() << "/" << r->is_numeric();

    if ( l->is_numeric() && r->is_numeric() )
    {   if (( l->getType() == DataType::DOUBLE )  ||
            ( r->getType() == DataType::DOUBLE ))
        {
                return( ptr<Expression>(
                        new typename Types::template forType<double>::ExprType(
                                         NArgs( l, r ))));
        }
        else if (( l->getType() == DataType::SINGLE )  ||
                 ( r->getType() == DataType::SINGLE ))
        {
              return( ptr<Expression>(
                        new typename Types::template forType<single>::ExprType(
                                         NArgs( l, r ))));
        }
        else if (( l->getType() == DataType::LONG )  ||
                 ( r->getType() == DataType::LONG ))
        {
            return( ptr<Expression>(
                        new typename Types::template forType<int64>::ExprType(
                                         NArgs( l, r ))));
        }
        else if (( l->getType() == DataType::INT )  ||
                 ( r->getType() == DataType::INT ))
        {
            return( ptr<Expression>(
                        new typename Types::template forType<int32>::ExprType(
                                         NArgs( l, r ))));
        }
        else if (( l->getType() == DataType::SHORT )  ||
                 ( r->getType() == DataType::SHORT ))
        {
            return( ptr<Expression>(
                        new typename Types::template forType<int16>::ExprType(
                                         NArgs( l, r ))));
        }
        else
        {   return( ptr<Expression>(
                        new typename Types::template forType<int8>::ExprType(
                                         NArgs( l, r ))));
        }
    }

    Throw( ExprException, "invalid types" );
}

template <typename Types>
ptr<Expression> relPromote( ptr<Expression>  l, ptr<Expression>  r )
                     throw( ExprException )
{
    if (( l->getType() == DataType::BOOL )  &&
        ( r->getType() == DataType::BOOL ))
    {
        return( ptr<Expression>(
                    new typename Types::template forType<bool>::ExprType(
                                     NArgs( l, r ))));
    }
    else if ( l->is_numeric()  &&  r->is_numeric())
    {
        if (( l->getType() == DataType::DOUBLE )  ||
            ( r->getType() == DataType::DOUBLE ))
        {
            return( ptr<Expression>(
                        new typename Types::template forType<double>::ExprType(
                                         NArgs( l, r ))));
        }
        else if (( l->getType() == DataType::SINGLE )  ||
                 ( r->getType() == DataType::SINGLE ))
        {
            return( ptr<Expression>(
                        new typename Types::template forType<single>::ExprType(
                                         NArgs( l, r ))));
        }
        else if (( l->getType() == DataType::LONG )  ||
                 ( r->getType() == DataType::LONG ))
        {
           return( ptr<Expression>(
                        new typename Types::template forType<int64>::ExprType(
                                         NArgs( l, r ))));
        }
        else
        {   return( ptr<Expression>(
                        new typename Types::template forType<int32>::ExprType(
                                         NArgs( l, r ))));
        }
    }
    else if (( l->getType() == DataType::STRING )  &&
             ( r->getType() == DataType::STRING ))
    {
        return( ptr<Expression>(
                    new typename Types::template forType<EString>::ExprType(
                                     NArgs( l, r ))));
    }
    else
    {   Throw(ExprException, "invalid types");
    }
}

#endif                           // EXPRUTIL_H
