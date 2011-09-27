#include "WaitForQBox.h"
#include <cmath>

BOREALIS_NAMESPACE_BEGIN

void WaitForQBox::setup_impl() throw (AuroraException)
{
    if (get_num_inputs() != 2)
        Throw(aurora_typing_exception, "WaitFor requires exactly two input streams (not "+to_string(get_num_inputs()) + ")");

    // The timeout
    double timeout;
    typed_param("timeout",timeout,PARAM_REQUIRED);
    int32  sec = static_cast<int32>(floor(timeout));
    int32  usec = static_cast<int32>(floor((timeout - static_cast<double>(sec)) * 1000000));
    _timeout = Timestamp(sec, usec);

    // The predicate
    string pred = param("predicate");
    if (pred.empty()) Throw(aurora_typing_exception, "WaitFor requires a predicate");
    ExprContext ctxt;
    //ctxt.setNamedTupleInfo("buffered", TupleDescription(getInDescription(0)), 0);
    //ctxt.setNamedTupleInfo("enabler", TupleDescription(getInDescription(1)), 1);
    ctxt.set_named_schema_info("buffered", get_in_schema(0), 0);
    ctxt.set_named_schema_info("enabler",  get_in_schema(1), 1);

    _predicate = Expression::parse(pred, ctxt);

    _ctxt.set(new EvalContext(2)); // Create the EvalContext

    // The output is simple the tuples from the first stream
    set_out_description(0, get_in_stream(0));
}

void WaitForQBox::init_impl() throw (AuroraException)
{
    TupleDescription out_td(get_out_schema(0));
    _output_tuple_size = out_td.get_size();
}

void WaitForQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{
    /**
     * Tuples from stream 0 are simply buffered
     * Then, for every tuple on stream 1, _predicate is applied to it and each of
     *  the tuples in the buffer. Emit the buffered tuple if _predicate is true
     */
    struct timeval now;
    gettimeofday(&now,NULL);
    Timestamp now_ts(now);

    INFO << "Buffer size at start of run: " << _buffer.size();

    //TupleDescription input_desc(getInDescription(0));
    TupleDescription input_desc( get_in_schema(0) );

    // First check if any tuples timed out, and throw them away
    list<ptr<TimestampTuple> >::iterator i = _buffer.begin();
    while (i != _buffer.end())
    {
        if (now_ts - _timeout > (*i)->get_timestamp())
        {
            DEBUG << "WaitForQBox DISCARDING: " << input_desc.tuple_to_string((*i)->getTuple());
            i = _buffer.erase(i);
        } else
        {
            ++i;
        }
    }

    // Store every tuple from the Buffering Input stream
    unsigned int max_tuples = inv.get_max_tuples_to_dequeue(0);
    DeqIterator di = deq(0);

    while (max_tuples-- && di.avail())
    {
        char *tuple = (char *)di.tuple();
        DEBUG << "WaitForQBox BUFFERING IN: " << input_desc.tuple_to_string(tuple);
        ptr<TimestampTuple> t(new TimestampTuple(now,
                                                 tuple,
                                                 input_desc.get_size()));
        _buffer.push_back(t);
        // Next!
        ++di;
    }


    // Now retrieve (if any) enabling tuples
    unsigned int max_tuples_e = inv.get_max_tuples_to_dequeue(1);
    DeqIterator di_e = deq(1);

    //  ... and emit if the predicate matched
    EnqIterator ei = enq(0);
    bool emitted = false;

    //TupleDescription input2_desc(getInDescription(1));
    TupleDescription input2_desc( get_in_schema( 1 ));

    while (max_tuples_e-- && di_e.avail())
    {
        char *tuple = (char *)di_e.tuple();
        DEBUG << "WaitForQBox ENABLER IN: " << input2_desc.tuple_to_string(tuple);

        // Run this tuple through the buffer
        list<ptr<TimestampTuple> >::iterator i = _buffer.begin();
        while(i != _buffer.end())
        {
            const char *wait_tuple = (*i)->getTuple();
            _ctxt->reset();
            _ctxt->set_tuple(0, wait_tuple);
            _ctxt->set_tuple(1, tuple);
            if (_predicate->eval<bool>(*_ctxt))
            {
                emitted = true;
                DEBUG << "WaitForQBox OUT: " << input_desc.tuple_to_string(wait_tuple);
                memcpy(ei.tuple(), wait_tuple, _output_tuple_size);
                ++ei;
                i = _buffer.erase(i);
            } else
            {
                ++i;
            }
        }
        // Next!
        ++di_e;
    }

    INFO << "Buffer size at end of run: " << _buffer.size();

    if (emitted) get_output(0).notify_enq();
}


BOREALIS_NAMESPACE_END
