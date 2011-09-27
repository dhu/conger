#include "DelaySelQBox.h"

BOREALIS_NAMESPACE_BEGIN

void DelaySelQBox::setup_impl() throw (AuroraException)
{
    if (get_num_inputs() != 1)
    {
        Throw(aurora_typing_exception, "DelaySelQBox requires one input");
    }

    int delay_msecs;
    typed_param("delay-ms", delay_msecs, PARAM_REQUIRED);
    _delay = Time::msecs(delay_msecs);

    _churn = false;
    typed_param("churn", _churn, PARAM_NOT_REQUIRED);

    _selectivity = 1.0;
    typed_param("selectivity", _selectivity, PARAM_NOT_REQUIRED);

    set_out_description(0, get_in_stream(0));

    srand(time(NULL));
}

void DelaySelQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{
    EnqIterator my_enq_iterator = enq(0);
    DeqIterator my_deq_iterator = deq(0);

    bool output = false;
    bool drop   = true;

    TupleDescription in_td(get_in_schema(0));
    int tuple_size = in_td.get_size();

    // while I can still dequeue tuples from it...
    // while (my_deq_iterator.avail())
    while (inv.continue_dequeue_on(my_deq_iterator, 0)) 
    { 
        const void *data = my_deq_iterator.tuple();

        INFO << "DelaySelQBox IN/OUT: " << in_td.tuple_to_string(data);

        if (_churn)
        {
            Time start = Time::now();
            bool msg = false;

            while (Time::now() < start + _delay)
            {
                if (!msg)
                {
                    INFO << " - churning...";
                    msg = true;
                }
            }
            if (msg)
            {
                INFO << " - done churning.";
            }
        } 
        else
        {
            INFO << " - sleeping...";
            Thread::sleep(_delay);
            INFO << " - done sleeping.";
        }

        drop = true;

        // make a random decision
        //
        static unsigned int seed = 1;
        double rand_val;
        rand_val = (double)rand_r(&seed)/(double)(RAND_MAX);
        DEBUG << "rand_val = " << rand_val;

        if (rand_val <= _selectivity) 
        { 
            DEBUG << "keep it"; 
            drop = false; 
        }

        if (drop)
        {
            DEBUG << "dropping it";
        }
        else
        {
            DEBUG << "keeping it";
            memcpy(my_enq_iterator.tuple(), (char*)data, tuple_size);
            output = true;
            ++my_enq_iterator;
        }

        ++my_deq_iterator;
    }

    if (output)
    {
        get_output(0).notify_enq();
    }
}

BOREALIS_NAMESPACE_END
