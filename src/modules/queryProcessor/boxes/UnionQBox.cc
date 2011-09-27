#include "UnionQBox.h"

BOREALIS_NAMESPACE_BEGIN

void UnionQBox::setup_impl() throw (AuroraException)
{
    // Union is the magic can-take-any-number-of-inputs box!
    _number_of_inputs = get_num_inputs();

    //size_t input_size = getInDescription(0)->getSize();
    int input_size = get_in_schema(0)->get_size() + HEADER_SIZE;

    // FOR FUTURE NOTE - The check should also check types, and also check field *order*
    for (unsigned int i = 1; i < get_num_inputs(); ++i)
    {
        //if (input_size != getInDescription(i)->getSize())
        if (input_size != (get_in_schema(i)->get_size() + HEADER_SIZE))
        {  Throw( aurora_typing_exception,
                  "Union inputs must all have the same schema");
        }
    }

    // set the output stream's type, same as input
    set_out_description(0, get_in_stream(0));

}

void UnionQBox::init_impl() throw (AuroraException)
{
    // all streams must have same schema, so no problem doing this
    TupleDescription out_td(get_out_schema(0));
    _output_tuple_size = out_td.get_size();
}

void UnionQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{
    bool outputted = false;

    EnqIterator my_enq_iterator = enq(0);

    TupleDescription t( get_in_schema( 0 ) );

    // Do stuff, the greedy way (suck all tuples from each stream in succession)
    for (unsigned int i = 0; i < _number_of_inputs; ++i)  // For each stream to dequeue from...
    {   DeqIterator my_deq_iterator = deq(i);

        while (inv.continue_dequeue_on(my_deq_iterator, i))  // while I can still dequeue tuples from it...
        {   const void *data = my_deq_iterator.tuple();

            DEBUG << " UnionQBox " << get_name()
                << " IN/OUT: " << t.tuple_to_string(data);

            // Union - da one-liner
            outputted = true;
            memcpy(my_enq_iterator.tuple(),(char*) data,_output_tuple_size);
            ++my_enq_iterator;

            // thanks
            ++my_deq_iterator;

        }
    }

    // Done, notify everyone (note this later should be done at some other moment than just at the end
    if (outputted) get_output(0).notify_enq();

}

BOREALIS_NAMESPACE_END
