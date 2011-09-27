#include "DelayQBox.h"

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
void  DelayQBox::setup_impl()
                      throw(AuroraException)
{
    int _delay_msecs;
//..............................................................................


    if (get_num_inputs() != 1)
    {
        Throw(aurora_typing_exception, "DelayQBox requires one input");
    }

    typed_param("delay-ms", _delay_msecs, PARAM_REQUIRED);
    _delay = Time::msecs(_delay_msecs);

    _churn = false;
    typed_param("churn", _churn, PARAM_NOT_REQUIRED);

    set_out_description(0, get_in_stream(0));

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  DelayQBox::run_impl(QBoxInvocation& inv)
                    throw(AuroraException)
{
    EnqIterator my_enq_iterator = enq(0);
    DeqIterator my_deq_iterator = deq(0);

    bool output = false;
    TupleDescription in_td(get_in_schema(0));
    int tuple_size = in_td.get_size();
//..............................................................................


    while (inv.continue_dequeue_on(my_deq_iterator, 0))
    {   // while I can still dequeue tuples from it...
        output = true;
        const void *data = my_deq_iterator.tuple();

        INFO << "DelayQBox IN/OUT: " << in_td.tuple_to_string(data);

        if (_churn)
        {
            Time start = Time::now();
            bool msg = false;

            while (Time::now() < start + _delay)
            {
                if (!msg)
                {   INFO << " - churning...";
                    msg = true;
                }
            }

            if (msg)
            {   INFO << " - done churning.";
            }
        }
        else
        {   INFO << " - sleeping...";
            Thread::sleep(_delay);
            INFO << " - done sleeping.";
        }

        memcpy(my_enq_iterator.tuple(), (char*)data, tuple_size);
        ++my_enq_iterator;
        ++my_deq_iterator;
    }

    if (output)
    {   get_output(0).notify_enq();
    }

    return;
}

BOREALIS_NAMESPACE_END
