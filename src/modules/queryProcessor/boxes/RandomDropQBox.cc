
#include "RandomDropQBox.h"

BOREALIS_NAMESPACE_BEGIN

void RandomDropQBox::setup_impl() throw (AuroraException)
{
    if (get_num_inputs() != 1)
    {
        Throw(aurora_typing_exception,
              "RandomDrop requires exactly one input stream (not " +
              to_string(get_num_inputs()) + ")");
    }

    // set the parameters
    typed_param("drop_rate", _drop_rate, PARAM_NON_EMPTY);
    typed_param("max_batch_size", _max_batch_size, PARAM_NON_EMPTY);
    NOTICE << get_name() << ": drop_rate = " << _drop_rate;

    // set the output stream's type, same as input
    set_out_description(0, get_in_stream(0));

    // reset the private variables
    _drop_counter = 0;
    _extra_tuples = 0;

    srand(time(NULL));
}

void RandomDropQBox::update_params()
{
    typed_param("drop_rate", _drop_rate, PARAM_NON_EMPTY);
    typed_param("max_batch_size", _max_batch_size, PARAM_NON_EMPTY);
    NOTICE << get_name() << ": drop_rate = " << _drop_rate;

    srand(time(NULL));
}

void RandomDropQBox::init_impl() throw (AuroraException)
{
    //_output_tuple_size = getOutDescription(0)->getSize();
    _output_tuple_size = get_out_schema(0)->get_size() + HEADER_SIZE;
}

void RandomDropQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{
    DeqIterator di = deq(0);
    EnqIterator ei = enq(0);
    bool outputted = false;
    bool drop;

    //TupleDescription in_desc(getInDescription(0));
    //TupleDescription out_desc(getOutDescription(0));
    TupleDescription in_desc(get_in_schema(0));
    TupleDescription out_desc(get_out_schema(0));

    DEBUG << "Running the drop box " << get_name();

    int tuple_count = 0; 

    //while (inv.continue_dequeue_on(di, 0))
    //while (di.avail())
    while ((di.avail()) && (tuple_count < 100))
    {
        ++tuple_count;

        const void *data = di.tuple();
        Tuple input_tuple = Tuple::factory(data);

        DEBUG << "RandomDropQBox " << get_name() << " IN: "
              << in_desc.tuple_to_string(data);

        // if fake tuple, just pass it
        //
        if (input_tuple.get_fake_flag())
        {
            memcpy(ei.tuple(), data, _output_tuple_size);
            DEBUG << " -> fake flag is on";
            DEBUG << "RandomDropQBox OUT: " << out_desc.tuple_to_string(data);
            outputted = true;
            ++ei;
            ++di;
            continue;
        }

        drop = false;

        // make a random decision
        //
        //srand(time(NULL));  // should we do this once in the setup???
        static unsigned int seed = 1;
        double rand_val;
        rand_val = (double)rand_r(&seed)/(double)(RAND_MAX);
        DEBUG << "rand_val = " << rand_val;
        DEBUG << get_name() << ": drop_rate = " << _drop_rate;
        if (rand_val <= _drop_rate)
        {
            DEBUG << " -> dice says drop it!";
            drop = true;
        }

        // For now, ignore the max_batch requirement.
        //
        /*
        if ((!drop) && (_extra_tuples) && (_drop_counter < _max_batch_size))
        {
            // could keep it but must compensate for extra tuples
            //
            DEBUG << " -> must drop it to compensate";
            --_extra_tuples;
            ++_drop_counter;
        }
        else if (!drop)
        {
            // keep it
            //
            DEBUG << " -> keeping it";
            memcpy(ei.tuple(), data, _output_tuple_size);
            DEBUG << "RandomDropQBox OUT: "  << out_desc.tuple_to_string(data);
            outputted = true;
            _drop_counter = 0;
            ++ei;
        }
        else if (_drop_counter == _max_batch_size)
        {
            // could drop it but must keep as extra tuple
            //
            DEBUG << " -> must keep it as extra";
            memcpy(ei.tuple(), data, _output_tuple_size);
            DEBUG << "RandomDropQBox OUT: " << out_desc.tuple_to_string(data);
            outputted = true;
            _drop_counter = 0;
            ++_extra_tuples;
            ++ei;
        }
        else
        {
            // must drop it
            //
            DEBUG << " -> dropping it";
            ++_drop_counter;
        }
        */
    
        if (drop)
        {
            // must drop it
            //
            DEBUG << " -> dropping it";
        }
        else
        {
            // keep it
            //
            DEBUG << " -> keeping it";
            memcpy(ei.tuple(), data, _output_tuple_size);
            DEBUG << "RandomDropQBox OUT: "  << out_desc.tuple_to_string(data);
            outputted = true;
            ++ei;
        }

        ++di;
    }

    if (outputted)
    {
        get_output(0).notify_enq();
    }
}

double RandomDropQBox::get_drop_rate()
{
    return _drop_rate;
}

void RandomDropQBox::set_drop_rate(double rate)
{
    _drop_rate = rate;
}

int RandomDropQBox::get_max_batch_size()
{
    return _max_batch_size;
}

void RandomDropQBox::set_max_batch_size(int batch_size)
{
    _max_batch_size = batch_size;
}


BOREALIS_NAMESPACE_END
