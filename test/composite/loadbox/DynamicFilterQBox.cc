#include "DynamicFilterQBox.h"

BOREALIS_NAMESPACE_BEGIN;

void DynamicFilterQBox::setup_impl() throw (AuroraException)
{
    if (get_num_inputs() != 1)
        Throw(aurora_typing_exception,
              "Filter requires exactly one input stream (not " +
              to_string(get_num_inputs()) + ")");

    // Optional parameter: pass-on-false-port
    _pass_on_false_port = false;
    typed_param("pass-on-false-port",_pass_on_false_port);

    for (unsigned int i = 0; ; ++i)
    {
        string expr = param("expression." + to_string(i));
        if (expr.empty())
        {
            if (i == 0)
                Throw(aurora_typing_exception,
                      "Filter requires at least one expression parameter. (\"predicate\" is deprecated)");
            else
                break;
        }

        ExprContext ctxt;
        ctxt.set_tuple_description(get_input_description(0));
        _expressions.push_back(Expression::parse(expr, ctxt));
    }

    _expr_count = _expressions.size();

    // Set input and output descriptions
    // There are as many output streams as predicates, +1 if passing "false" tuples too
    for (unsigned int i = 0; i < _expr_count; ++i)
        set_output_description(i, get_input_description(0)); // same output type as input everywhere

    if (_pass_on_false_port)
        set_output_description(_expr_count, get_input_description(0));
}

void DynamicFilterQBox::init_impl() throw (AuroraException)
{
  _output_tuple_size = get_output_description(0).get_size();
}

void DynamicFilterQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{
  //DEBUG << "In DynamicFilterQBox RunImpl() !!!!!!!!!!!!!!!!!!!!!!!!!!!!";
    // Track number of tuples that get output
    bool outputted[_expr_count + 1];
    for (unsigned int i = 0; i < _expr_count + 1; ++i)
        outputted[i] = false;

    DeqIterator my_deq_iterator = deq(0);

    // Do stuff...
    while (inv.continue_dequeue_on(my_deq_iterator, 0))
    {
        const void *data = my_deq_iterator.tuple();

        DEBUG << "DynamicFilterQBox " << get_name() << " IN: " << get_input_description(0).tuple_to_string(data);

        bool passed = false;

        _ctxt.reset();
        _ctxt.set_tuple((const char *)data);

        // check expressions
        for (unsigned int i = 0; i < _expressions.size(); ++i)
        {
            if (_expressions[i]->eval<bool>(_ctxt))
            {
                memcpy(enq(i).tuple(), data, _output_tuple_size);
                DEBUG << "DynamicFilterQBox OUT from expr: " << get_output_description(0).tuple_to_string(data);
                ++ (enq(i));
                passed = true;
                outputted[i] = true;
                DEBUG << " -> through port " << i;
                break; // Once it passed a predicate, move on
            }
        }

        // catch folks going to the false port
        if (!passed && _pass_on_false_port)
        {
            outputted[_expr_count] = true;
            DEBUG << " DynamicFilterQBox OUT: " << get_output_description(0).tuple_to_string(data);
            DEBUG << " -> through the false port";
            memcpy(enq(_expr_count).tuple(), data, _output_tuple_size);
            ++ (enq(_expr_count));
        } else if (!passed)
        {
            DEBUG << " -> tuple was dumped";
        }
        // neeeeeeeeeeext!
        ++my_deq_iterator;
    }

    // Done, notify everyone where appropriate (note, doing it at the end may introduce latency, so perhaps
    //  one could explore various timings at which to notify) -- eddie
    for (unsigned int i = 0; i < _expr_count; ++i)
        if (outputted[i])
            get_output(i).notify_enq();
    if (_pass_on_false_port && outputted[_expr_count])
        get_output(_expr_count).notify_enq();
}

void DynamicFilterQBox::set_pending_box_state(ptr<AbstractBoxState> packed_box)
{
    //FilterBoxState* actual_packed_box = static_cast<FilterBoxState*>(packed_box.get());
    //_pending_box_state.set(new FilterBoxState(*actual_packed_box));
    _pending_box_state = packed_box.dynamic_cast_to<FilterBoxState>();
}

ptr<AbstractBoxState> DynamicFilterQBox::pack_state()
{
  INFO << "Packing state for DynamicFilterQBox named [" << get_name() << "]";
  ptr<FilterBoxState> filter_box_state(new FilterBoxState());
  return filter_box_state;
}

AURORA_DEFINE_QBOX(DynamicFilterQBox, "dfilter");

BOREALIS_NAMESPACE_END;
