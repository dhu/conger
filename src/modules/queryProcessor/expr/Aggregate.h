#ifndef AGGREGATE_H
#define AGGREGATE_H

#include "TupleDescription.h"
#include "Expression.h"

//using namespace Borealis;
BOREALIS_NAMESPACE_BEGIN

class AggregateWindow;

class Aggregate
{

  private:

    vector<SchemaField>       _out_fields;
    unsigned int              _out_size;
    vector<ptr<Expression> >  _args;


  public:

    Aggregate() : _out_size(0) {}

    virtual ~Aggregate() {}

    /// Get an instnace of an aggregate function.
    static Aggregate  *instantiate(const string  &name)
                             throw(AuroraBadEntityException);


    /// Get an instnace of a user defined aggregate function.
    static Aggregate  *instantiate_udb(const string  &name);
    

    // Create a new AggregateWindow
    virtual AggregateWindow  *open_window();

    // Figure out the output TupleDescription based on args; call
    // setTupleDescription
    virtual void setup(const ExprContext& ctxt)
        throw (AuroraException) {};

    void setArgs(const vector<ptr<Expression> >& args) { _args = args; }

    int numOutputFields() const { return _out_fields.size(); }

    SchemaField get_output_field(unsigned int i) const
    {
        ASSERT(i < _out_fields.size());
        return _out_fields[i];
    }

    unsigned int get_output_size() const { return _out_size; }

    virtual const string as_string() const;

    ptr<Expression>  arg( unsigned int  index ) const
    {
        ASSERT( index < _args.size() );
        return( _args[ index ]);
    }

    unsigned int num_args() const
    {
        return( _args.size() );
    }

    static ptr<Aggregate> parse(string expr, const ExprContext& ctxt)
        throw (ExprException);

  protected:
    // Description of aggregate outputs; to be called by subclass
    // constructor
    void add_output_field( SchemaField  field )
    {
        _out_fields.push_back(field);
        _out_size += field.get_size();
    }

    void add_int_8output_field( string  name = string() )
    {   add_output_field( Field<int8>( name )); }

    void add_int_16output_field( string  name = string() )
    {   add_output_field( Field<int16>( name )); }

    void add_int_32output_field( string  name = string() )
    {   add_output_field( Field<int32>( name )); }

    void add_int_64output_field( string  name = string() )
    {   add_output_field( Field<int64>( name )); }

    void add_single_output_field( string  name = string() )
    {   add_output_field( Field<single>( name )); }

    void add_double_output_field( string  name = string() )
    {   add_output_field( Field<double>( name )); }

    void add_string_output_field( unsigned int  length )
    {   add_output_field( Field<string>( length )); }

    void add_string_output_field( string  name, unsigned int  length )
    {   add_output_field( Field<string>( name, length )); }


 public:
    NMSTL_SERIAL_BASE(Aggregate, long, << _out_fields << _out_size << _args);
};


class AggregateWindow
{

  public:

    AggregateWindow() {}
    virtual ~AggregateWindow() {}

    virtual void init() {};
    virtual void incr(const EvalContext& ctxt) throw (EvalException) {};
    virtual void final(char *out_data) {};
    virtual void remove(const EvalContext& ctxt) throw (EvalException) {};
    virtual void update(OSerial& o) {};
    virtual void update(ISerial& i) {};
    virtual const string as_string() const;

    const ptr<Aggregate> get_aggregate() { return _agg; }

    virtual void set_aggregate(ptr<Aggregate> agg)
    {
        _agg = agg;
    }

//    NMSTL_SERIAL_BASE(AggregateWindow, long, << _agg); 
//    because serialization/deserilzatin doesn't handle shared pointers correctly, 
//    we set _agg explicitly after deserialization: Jeong-Hyon
    NMSTL_SERIAL_BASE(AggregateWindow, long, );

  protected:

    unsigned int num_args() { return _agg->num_args(); }
    Expression& arg(unsigned int index) { return *(_agg->arg(index)); }

  private:
    ptr<Aggregate> _agg;
};

BOREALIS_NAMESPACE_END
#endif                   // AGGREGATE_H
