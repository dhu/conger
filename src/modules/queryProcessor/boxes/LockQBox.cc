#include "LockQBox.h"

BOREALIS_NAMESPACE_BEGIN

LockQBox::~LockQBox()
{
    if (_pending_tuples.size())
    {
        WARN << "Still " << _pending_tuples.size()
             << " pending in lock box " << get_name();
    }
    // TODO: Consider warning about locks which are still held.
}

void LockQBox::setup_impl() throw (AuroraException)
{
    // One input and output.
    if (get_num_inputs() != 1)
        Throw(aurora_typing_exception,
              "Lock requires exactly one input stream (not " +
              to_string(get_num_inputs()) + ")");

    _lock_set = get_lock_set(param("lockset", PARAM_REQUIRED));

    string expr = param("key", PARAM_REQUIRED);
    ExprContext ctxt;
    //ctxt.setTupleDescription(TupleDescription(getInDescription(0)));
    ctxt.set_context_schema( *get_in_schema( 0 ));
    _key_expr = Expression::parse(expr, ctxt);

    if (!_key_expr->is<int32>())
        Throw(aurora_typing_exception,
              "Lock key must be an int expression, not a " +
              to_string(_key_expr));

    // set the output stream's type, same as input
    set_out_description(0, get_in_stream(0));
}

void LockQBox::init_impl() throw (AuroraException)
{
    //_tuple_size = getOutDescription(0)->getSize();
    _tuple_size = get_out_schema(0)->get_size() + HEADER_SIZE;
}

void LockQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{
    bool outputted = false;

    EnqIterator ei = enq(0);
    DeqIterator di = deq(0);

    LockHolder hold(_lock_set->get_lock());
    // Check pending tuples.
    PendingMap::iterator pmi;
    DEBUG << "_pending_tuples=" << to_string(_pending_tuples);

    //TupleDescription input_desc(getInDescription(0));
    //TupleDescription output_desc(getOutDescription(0));
    TupleDescription input_desc(get_in_schema(0));
    TupleDescription output_desc(get_out_schema(0));

    vector<int> keys_to_delete;

    for (pmi = _pending_tuples.begin(); pmi != _pending_tuples.end(); ++pmi)
    {
        if (_lock_set->take_lock(pmi->first))
        {
            // Got the lock.
            DEBUG << "Got a lock for a stalled tuple (key=" << pmi->first << ").";

            outputted = true;
            memcpy(ei.tuple(), pmi->second.front().c_str(), _tuple_size);

            DEBUG << " LockQBox " << get_name() << " OUT: "
                << output_desc.tuple_to_string(pmi->second.front().c_str());

            ++ei;
            pmi->second.pop_front();

            if ( pmi->second.empty() )
                keys_to_delete.push_back(pmi->first);
        }
        else
        {   DEBUG << "Still can't grab lock for key " << pmi->first;
        }
    }
    
    vector<int>::iterator del_it = keys_to_delete.begin();
    vector<int>::iterator del_end = keys_to_delete.end();
    
    for (; del_it != del_end; ++del_it)
        _pending_tuples.erase(*del_it);


    while (inv.continue_dequeue_on(di,0))
    {
        DEBUG << " LockQBox " << get_name()
            << " IN: " << input_desc.tuple_to_string(di.tuple());

        _ctxt.reset();
        _ctxt.set_tuple(di.tuple());
        int key = _key_expr->eval<int32>(_ctxt);

        if (_lock_set->take_lock(key))
        {
            // Got the lock.
            outputted = true;
            memcpy(ei.tuple(), di.tuple(), _tuple_size);
            DEBUG << " LockQBox " << get_name()
                << " OUT: " << output_desc.tuple_to_string(ei.tuple());
            ++ei;
            ++di;
        }
        else
        {
            // No lock for me. Stop now, because we don't want to
            // reorder tuples.
            DEBUG << "Could not grab lock for key " << key << ", stalling "
                  << output_desc.tuple_to_string(di.tuple());
            _pending_tuples[key].push_back(string((const char*)di.tuple(), _tuple_size));
            
            ++di;
        }
    }

    // yna -- let's try to keep this scheduled.
    if ( !_pending_tuples.empty() )
        more_processing();

    if (outputted)
        get_output(0).notify_enq();
}


BOREALIS_NAMESPACE_END
