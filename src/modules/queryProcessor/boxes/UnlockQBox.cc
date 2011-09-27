#include "UnlockQBox.h"

BOREALIS_NAMESPACE_BEGIN

void UnlockQBox::setup_impl() throw (AuroraException)
{
    // One input and output.
    if (get_num_inputs() != 1)
        Throw(aurora_typing_exception,
              "Unlock requires exactly one input stream (not " +
              to_string(get_num_inputs()) + ")");

    _lock_set = get_lock_set(param("lockset", PARAM_REQUIRED));

    string expr = param("key", PARAM_REQUIRED);
    ExprContext ctxt;
    //ctxt.setTupleDescription(TupleDescription(getInDescription(0)));
    ctxt.set_context_schema( *get_in_schema( 0 ));
    _key_expr = Expression::parse(expr, ctxt);

    if (!_key_expr->is<int32>())
        Throw(aurora_typing_exception,
              "Unlock key must be an int expression, not a " +
              to_string(_key_expr));

    // set the output stream's type, same as input
    set_out_description(0, get_in_stream(0));
}

void UnlockQBox::init_impl() throw (AuroraException)
{
    _tuple_size = get_out_schema(0)->get_size() + HEADER_SIZE;
    //_tuple_size = getOutDescription(0)->getSize();
}

void UnlockQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{
    bool outputted = false;

    EnqIterator ei = enq(0);
    DeqIterator di = deq(0);

    LockHolder hold(_lock_set->get_lock());

    //TupleDescription in_desc(getInDescription(0));
    //TupleDescription out_desc(getOutDescription(0));
    TupleDescription in_desc(  get_in_schema( 0 ));
    TupleDescription out_desc( get_out_schema( 0 ));

    while ( inv.continue_dequeue_on( di,0 ))
    {
        INFO << "UnlockQBox IN: " << in_desc.tuple_to_string(di.tuple());

        _ctxt.reset();
        _ctxt.set_tuple( di.tuple() );

        int32  key = _key_expr->eval<int32>(_ctxt);
        _lock_set->release_lock( key );

        outputted = true;

        memcpy( ei.tuple(), di.tuple(), _tuple_size );

        INFO << "UnlockQBox OUT: "
         << out_desc.tuple_to_string(ei.tuple());

        ++ei;
        ++di;
    }

    if (outputted)
        get_output(0).notify_enq();
}

BOREALIS_NAMESPACE_END
