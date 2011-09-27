#ifndef STANDARD_AGGREGATES_H
#define STANDARD_AGGREGATES_H

#include "Aggregate.h"

BOREALIS_NAMESPACE_BEGIN

enum standard_aggregate_type { MIN, MAX, SUM, AVG, FIRSTVAL, LASTVAL };

class standard_aggregate : public Aggregate
{
  private:
    int agg_type;

  public:
    standard_aggregate(int a_type): agg_type(a_type) {};
    string name()
    {
        return
            agg_type == MIN ? "min" :
            agg_type == MAX ? "max" :
            agg_type == SUM ? "sum" :
            agg_type == AVG ? "avg" :
            agg_type == FIRSTVAL ? "firstval" :
            agg_type == LASTVAL ? "lastval" :
            "?";
    }
    virtual AggregateWindow* open_window() = 0;
    void setup( const ExprContext  &ctxt ) throw( AuroraException )
    {
        if ( num_args() != 1 )
        {
            Throw( ExprException, "requires one argument" );
        }
        else if ( arg(0)->getType() == DataType::BYTE )
        {   add_int_8output_field( name() );
        }
        else if ( arg(0)->getType() == DataType::SHORT )
        {   add_int_16output_field( name() );
        }
        else if ( arg(0)->getType() == DataType::INT )
        {   add_int_32output_field( name() );
        }
        else if ( arg(0)->getType() == DataType::LONG )
        {   add_int_64output_field( name() );
        }
        else if ( arg(0)->getType() == DataType::SINGLE )
        {   add_single_output_field( name() );
        }
        else if ( arg(0)->getType() == DataType::DOUBLE )
        {   add_double_output_field( name() );
        }
        else
        {   Throw(ExprException,
                  "Only defined for byte, short, int, long, single and double.");
        }
    }
};


template <typename type>
class standard_aggregate_window : public AggregateWindow
{
  protected:

    int   agg_type;
    type  val;
    int   count;

  public:

    standard_aggregate_window(int a_type): agg_type(a_type) {};


    void init()
    {
        val =
            agg_type == SUM || agg_type == AVG ? type(0) :
            agg_type == MIN ? numeric_limits<type>::max() :
            agg_type == MAX ? numeric_limits<type>::min() :
            type(0);

        count = 0;
    }


    #define NEWVAL (arg(0).template eval<type>(ctxt))
    void incr(const EvalContext& ctxt) throw (EvalException)
    {
        val =
            agg_type == MIN ? min(val, NEWVAL) :
            agg_type == MAX ? max(val, NEWVAL) :
            agg_type == SUM ? val + NEWVAL :
            agg_type == AVG ? val + NEWVAL :
            agg_type == FIRSTVAL ? (count ? val : NEWVAL) :
            agg_type == LASTVAL ? NEWVAL :
            val;

        ++count;
    }


    void remove(const EvalContext& ctxt) throw (EvalException)
    {
        val =
            agg_type == SUM ? val - NEWVAL :
            agg_type == AVG ? val - NEWVAL :
            val;

        --count;
    }


    void final( char *data )
    {
        if (count || agg_type == SUM)
        {
            type final_val = agg_type == AVG ? val / count : val;
            *(type*)data = final_val;

            DEBUG << "Aggregate " << agg_type
                  << " final: "   << cast_into_string<type>::into_string(final_val);
        }
        else
        {   *(type*)data = numeric_limits<type>::signaling_NaN();
        }
    }


    virtual const string as_string() const
    {
        string s;

        s += "StandardAggregateWindow[";
        s += AggregateWindow::as_string();
        s += ", val = ";
        s += cast_into_string<type>::into_string(val);
        s += ", count = ";
        s += count;
        s += "]";

        return s;
    }
};


////////////////////////////////////////////////////////////////////////////////
/////////////////////////  Aggregate functions  ////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


class count_window : public AggregateWindow
{
  protected:

    int32 count;

  public:

    virtual void init() { count = 0; }
    virtual void incr( const EvalContext& ) throw( EvalException ) { ++count; }
    virtual void remove( const EvalContext& ) throw( EvalException ) { --count; }
    virtual void final( char *data ) {*(int*)data = count;}
    virtual void update(OSerial& o) { o << count;};
    virtual void update(ISerial& i) { i >> count;};

    virtual const string as_string() const
    {
        string s;
        s += "CountWindow[";
        s += AggregateWindow::as_string();
        s += ", count = ";
        s += count;
        s += "]";

        return s;
    }

    NMSTL_SERIAL_SUBCLASS(count_window, AggregateWindow, << count);
};


class CountAggregate : public Aggregate
{
  public:
    virtual AggregateWindow* open_window() { return( new count_window ); }

    void setup( const ExprContext  &ctxt ) throw( AuroraException )
    {
        if ( num_args() != 0 )
        {   Throw( ExprException, "requires no arguments" );
        }

        add_int_32output_field( "count" );
    }

    NMSTL_SERIAL_SUBCLASS(CountAggregate, Aggregate, );
};


template <typename type>
class MinWindow: public standard_aggregate_window<type>
{
  public:
    MinWindow<type>(): standard_aggregate_window<type>(MIN) {};
};

class ByteMinWindow : public MinWindow<int8>
{
    const char *hit() const { return to_string((int)val).data(); }
    NMSTL_SERIAL_SUBCLASS(ByteMinWindow, AggregateWindow, << hit() << count);
    //NMSTL_SERIAL_SUBCLASS(ByteMinWindow, AggregateWindow, << val << count);
};

class ShortMinWindow : public MinWindow<int16>
{
    NMSTL_SERIAL_SUBCLASS(ShortMinWindow, AggregateWindow, << val << count);
};

class IntMinWindow : public MinWindow<int32>
{
    NMSTL_SERIAL_SUBCLASS(IntMinWindow, AggregateWindow, << val << count);
};

class LongMinWindow : public MinWindow<int64>
{
    NMSTL_SERIAL_SUBCLASS(LongMinWindow, AggregateWindow, << val << count);
};

class SingleMinWindow : public MinWindow<single>
{
    NMSTL_SERIAL_SUBCLASS(SingleMinWindow, AggregateWindow, << val << count);
};

class DoubleMinWindow : public MinWindow<double>
{
    NMSTL_SERIAL_SUBCLASS(DoubleMinWindow, AggregateWindow, << val << count);
};

class MinAggregate : public standard_aggregate
{

  public:

    MinAggregate():standard_aggregate(MIN) {};

    virtual AggregateWindow* open_window()
    {
        if ( arg(0)->getType() == DataType::INT )
        {   return( new IntMinWindow);
        }
        else if ( arg(0)->getType() == DataType::LONG )
        {   return( new LongMinWindow);
        }
        else if ( arg(0)->getType() == DataType::BYTE )
        {   return( new ByteMinWindow);
        }
        else if ( arg(0)->getType() == DataType::SHORT )
        {   return( new ShortMinWindow);
        }
        else if ( arg(0)->getType() == DataType::SINGLE )
        {   return( new SingleMinWindow);
        }
        else // if ( arg(0)->getType() == DataType::DOUBLE )
        {   return( new DoubleMinWindow);
        }
    }

    NMSTL_SERIAL_SUBCLASS(MinAggregate, Aggregate, );
};


template <typename type>
class max_window: public standard_aggregate_window<type>
{
  public:
    max_window<type>(): standard_aggregate_window<type>(MAX) {};
};

class ByteMaxWindow : public max_window<int8>
{
    const char *hit() const { return to_string((int)val).data(); }
    NMSTL_SERIAL_SUBCLASS(ByteMaxWindow, AggregateWindow, << hit() << count);
    //NMSTL_SERIAL_SUBCLASS(ByteMaxWindow, AggregateWindow, << val << count);
};

class ShortMaxWindow : public max_window<int16>
{
    NMSTL_SERIAL_SUBCLASS(ShortMaxWindow, AggregateWindow, << val << count);
};

class IntMaxWindow : public max_window<int32>
{
    NMSTL_SERIAL_SUBCLASS(IntMaxWindow, AggregateWindow, << val << count);
};

class LongMaxWindow : public max_window<int64>
{
    NMSTL_SERIAL_SUBCLASS(LongMaxWindow, AggregateWindow, << val << count);
};

class SingleMaxWindow : public max_window<single>
{
    NMSTL_SERIAL_SUBCLASS(SingleMaxWindow, AggregateWindow, << val << count);
};

class DoubleMaxWindow : public max_window<double>
{
    NMSTL_SERIAL_SUBCLASS(DoubleMaxWindow, AggregateWindow, << val << count);
};

class max_aggregate : public standard_aggregate
{

  public:

    max_aggregate():standard_aggregate(MAX) {};

    virtual AggregateWindow* open_window()
    {
        if ( arg(0)->getType() == DataType::INT )
        {   return( new IntMaxWindow);
        }
        else if ( arg(0)->getType() == DataType::LONG )
        {   return( new LongMaxWindow);
        }
        else if ( arg(0)->getType() == DataType::SHORT )
        {   return( new ShortMaxWindow);
        }
        else if ( arg(0)->getType() == DataType::BYTE )
        {   return( new ByteMaxWindow);
        }
        else if ( arg(0)->getType() == DataType::SINGLE )
        {   return( new SingleMaxWindow);
        }
        else // if ( arg(0)->getType() == DataType::DOUBLE )
        {   return( new DoubleMaxWindow);
        }
    }

    NMSTL_SERIAL_SUBCLASS(max_aggregate, Aggregate, );
};


////////////////////////////

template <typename type>
class sum_window: public standard_aggregate_window<type>
{
  public:
    sum_window<type>(): standard_aggregate_window<type>(SUM) {};
};

class ByteSumWindow : public sum_window<int8>
{
    const char *hit() const { return to_string((int)val).data(); }
    NMSTL_SERIAL_SUBCLASS(ByteSumWindow, AggregateWindow, << hit() << count);
    //NMSTL_SERIAL_SUBCLASS(ByteSumWindow, AggregateWindow, << val << count);
};

class ShortSumWindow : public sum_window<int16>
{
    NMSTL_SERIAL_SUBCLASS(ShortSumWindow, AggregateWindow, << val << count);
};

class IntSumWindow : public sum_window<int32>
{
    NMSTL_SERIAL_SUBCLASS(IntSumWindow, AggregateWindow, << val << count);
};

class LongSumWindow : public sum_window<int64>
{
    NMSTL_SERIAL_SUBCLASS(LongSumWindow, AggregateWindow, << val << count);
};

class SingleSumWindow : public sum_window<single>
{
    NMSTL_SERIAL_SUBCLASS(SingleSumWindow, AggregateWindow, << val << count);
};

class DoubleSumWindow : public sum_window<double>
{
    NMSTL_SERIAL_SUBCLASS(DoubleSumWindow, AggregateWindow, << val << count);
};

class sum_aggregate : public standard_aggregate
{

  public:

    sum_aggregate():standard_aggregate(SUM) {};

    virtual AggregateWindow* open_window()
    {
        if ( arg(0)->getType() == DataType::INT )
        {   return( new IntSumWindow);
        }
        else if ( arg(0)->getType() == DataType::LONG )
        {   return( new LongSumWindow);
        }
        else if ( arg(0)->getType() == DataType::BYTE )
        {   return( new ByteSumWindow);
        }
        else if ( arg(0)->getType() == DataType::SHORT )
        {   return( new ShortSumWindow);
        }
        else if ( arg(0)->getType() == DataType::SINGLE )
        {   return( new SingleSumWindow);
        }
        else // if ( arg(0)->getType() == DataType::DOUBLE )
        {   return( new DoubleSumWindow);
        }
    }

    NMSTL_SERIAL_SUBCLASS(sum_aggregate, Aggregate, );
};


////////////////////////////

template <typename type>
class avg_window: public standard_aggregate_window<type>
{
  public:
    avg_window<type>(): standard_aggregate_window<type>(AVG) {};
};

class ByteAvgWindow : public avg_window<int8>
{
    const char *hit() const { return to_string((int)val).data(); }
    NMSTL_SERIAL_SUBCLASS(ByteAvgWindow, AggregateWindow, << hit() << count);
    //NMSTL_SERIAL_SUBCLASS(ByteAvgWindow, AggregateWindow, << val << count);
};

class ShortAvgWindow : public avg_window<int16>
{
    NMSTL_SERIAL_SUBCLASS(ShortAvgWindow, AggregateWindow, << val << count);
};

class IntAvgWindow : public avg_window<int32>
{
    NMSTL_SERIAL_SUBCLASS(IntAvgWindow, AggregateWindow, << val << count);
};

class LongAvgWindow : public avg_window<int64>
{
    NMSTL_SERIAL_SUBCLASS(LongAvgWindow, AggregateWindow, << val << count);
};

class SingleAvgWindow : public avg_window<single>
{
    NMSTL_SERIAL_SUBCLASS(SingleAvgWindow, AggregateWindow, << val << count);
};

class DoubleAvgWindow : public avg_window<double>
{
    NMSTL_SERIAL_SUBCLASS(DoubleAvgWindow, AggregateWindow, << val << count);
};

class avg_aggregate : public standard_aggregate
{

  public:

    avg_aggregate():standard_aggregate(AVG) {};

    virtual AggregateWindow* open_window()
    {
        if ( arg(0)->getType() == DataType::INT )
        {   return( new IntAvgWindow);
        }
        else if ( arg(0)->getType() == DataType::LONG )
        {   return( new LongAvgWindow);
        }
        else if ( arg(0)->getType() == DataType::BYTE )
        {   return( new ByteAvgWindow);
        }
        else if ( arg(0)->getType() == DataType::SHORT )
        {   return( new ShortAvgWindow);
        }
        else if ( arg(0)->getType() == DataType::SINGLE )
        {   return( new SingleAvgWindow);
        }
        else // if ( arg(0)->getType() == DataType::DOUBLE )
        {   return( new DoubleAvgWindow);
        }
    }

    NMSTL_SERIAL_SUBCLASS(avg_aggregate, Aggregate, );
};


////////////////////////////

template <typename type>
class firstval_window: public standard_aggregate_window<type>
{
  public:
    firstval_window<type>(): standard_aggregate_window<type>(FIRSTVAL) {};
};

class ByteFirstvalWindow : public firstval_window<int8>
{
    const char *hit() const { return to_string((int)val).data(); }
    NMSTL_SERIAL_SUBCLASS(ByteFirstvalWindow, AggregateWindow, << hit() << count);
    //NMSTL_SERIAL_SUBCLASS(ByteFirstvalWindow, AggregateWindow, << val << count);
};

class ShortFirstvalWindow : public firstval_window<int16>
{
    NMSTL_SERIAL_SUBCLASS(ShortFirstvalWindow, AggregateWindow, << val << count);
};

class IntFirstvalWindow : public firstval_window<int32>
{
    NMSTL_SERIAL_SUBCLASS(IntFirstvalWindow, AggregateWindow, << val << count);
};

class LongFirstvalWindow : public firstval_window<int64>
{
    NMSTL_SERIAL_SUBCLASS(LongFirstvalWindow, AggregateWindow, << val << count);
};

class SingleFirstvalWindow : public firstval_window<single>
{
    NMSTL_SERIAL_SUBCLASS(SingleFirstvalWindow, AggregateWindow, << val << count);
};

class DoubleFirstvalWindow : public firstval_window<double>
{
    NMSTL_SERIAL_SUBCLASS(DoubleFirstvalWindow, AggregateWindow, << val << count);
};

class FirstvalAggregate : public standard_aggregate
{

  public:

    FirstvalAggregate():standard_aggregate(FIRSTVAL) {};

    virtual AggregateWindow* open_window()
    {
        if ( arg(0)->getType() == DataType::INT )
        {   return( new IntFirstvalWindow);
        }
        else if ( arg(0)->getType() == DataType::LONG )
        {   return( new LongFirstvalWindow);
        }
        else if ( arg(0)->getType() == DataType::BYTE )
        {   return( new ByteFirstvalWindow);
        }
        else if ( arg(0)->getType() == DataType::SHORT )
        {   return( new ShortFirstvalWindow);
        }
        else if ( arg(0)->getType() == DataType::SINGLE )
        {   return( new SingleFirstvalWindow);
        }
        else // if ( arg(0)->getType() == DataType::DOUBLE )
        {   return( new DoubleFirstvalWindow);
        }
    }

    NMSTL_SERIAL_SUBCLASS(FirstvalAggregate, Aggregate, );
};

////////////////////////////

template <typename type>
class lastval_window: public standard_aggregate_window<type>
{
  public:
    lastval_window<type>(): standard_aggregate_window<type>(LASTVAL) {};
};

class ByteLastvalWindow : public lastval_window<int8>
{
    const char *hit() const { return to_string((int)val).data(); }
    NMSTL_SERIAL_SUBCLASS(ByteLastvalWindow, AggregateWindow, << hit() << count);
    //NMSTL_SERIAL_SUBCLASS(ByteLastvalWindow, AggregateWindow, << val << count);
};

class ShortLastvalWindow : public lastval_window<int16>
{
    NMSTL_SERIAL_SUBCLASS(ShortLastvalWindow, AggregateWindow, << val << count);
};

class IntLastvalWindow : public lastval_window<int32>
{
    NMSTL_SERIAL_SUBCLASS(IntLastvalWindow, AggregateWindow, << val << count);
};

class LongLastvalWindow : public lastval_window<int64>
{
    NMSTL_SERIAL_SUBCLASS(LongLastvalWindow, AggregateWindow, << val << count);
};

class SingleLastvalWindow : public lastval_window<single>
{
    NMSTL_SERIAL_SUBCLASS(SingleLastvalWindow, AggregateWindow, << val << count);
};

class DoubleLastvalWindow : public lastval_window<double>
{
    NMSTL_SERIAL_SUBCLASS(DoubleLastvalWindow, AggregateWindow, << val << count);
};

class LastvalAggregate : public standard_aggregate
{

  public:

    LastvalAggregate():standard_aggregate(LASTVAL) {};

    virtual AggregateWindow* open_window()
    {
        if ( arg(0)->getType() == DataType::INT )
        {   return( new IntLastvalWindow);
        }
        else if ( arg(0)->getType() == DataType::LONG )
        {   return( new LongLastvalWindow);
        }
        else if ( arg(0)->getType() == DataType::BYTE )
        {   return( new ByteLastvalWindow);
        }
        else if ( arg(0)->getType() == DataType::SHORT )
        {   return( new ShortLastvalWindow);
        }
        else if ( arg(0)->getType() == DataType::SINGLE )
        {   return( new SingleLastvalWindow);
        }
        else // if ( arg(0)->getType() == DataType::DOUBLE )
        {   return( new DoubleLastvalWindow);
        }
    }

    NMSTL_SERIAL_SUBCLASS(LastvalAggregate, Aggregate, );
};


BOREALIS_NAMESPACE_END

#endif
