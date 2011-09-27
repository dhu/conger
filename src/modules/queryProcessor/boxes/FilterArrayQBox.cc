#include "FilterArrayQBox.h"
#include "DataPath.h"
#include "DynamicArray.h"


BOREALIS_NAMESPACE_BEGIN


void FilterArrayQBox::setup_impl() throw (AuroraException)
{
    if ( get_num_inputs() != 1 )
    {   Throw(aurora_typing_exception,
              "Filter requires exactly one input stream (not " +
              to_string(get_num_inputs()) + ")");
    }

    // Optional parameter: pass-on-false-port
    _pass_on_false_port = false;
    typed_param("pass-on-false-port",_pass_on_false_port);

    for ( uint32 i = 0; ; ++i )
    {   string expr = param( "expression." + to_string( i ));

        if ( expr.empty() )
        {   if ( i == 0 )
            {   Throw(aurora_typing_exception,
                      "Filter requires at least one expression parameter. (\"predicate\" is deprecated)");
            }
            else
            {   break;
            }
        }

        ExprContext  ctxt;

        //ctxt.setTupleDescription( TupleDescription( getInDescription( 0 )));
        ctxt.set_context_schema( *get_in_schema( 0 ));


        _expressions.push_back( Expression::parse( expr, ctxt ));
    }

    _expr_count = _expressions.size();

    // Set input and output descriptions
    // There are as many output streams as predicates, +1 if passing "false"
    // tuples too
    //
    for ( uint32  i = 0; i < _expr_count; ++i )
    {   set_out_description( i, get_in_stream( 0 )); // same output type as input everywhere
    }

    if ( _pass_on_false_port )
    {   set_out_description( _expr_count, get_in_stream( 0 ));
    }

    DEBUG << "FilterArrayQBox input schema " << get_in_schema(0)->as_string();
    DEBUG << "FilterArrayQBox output schema " << get_out_schema(0)->as_string();
}

void FilterArrayQBox::init_impl() throw (AuroraException)
{
  TupleDescription out_td(get_out_schema(0));
  _output_tuple_size = out_td.get_size();

}

void FilterArrayQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{
    // Track number of tuples that get output
    bool outputted[_expr_count + 1];
    for (unsigned int i = 0; i < _expr_count + 1; ++i)
        outputted[i] = false;

    DeqIterator my_deq_iterator = deq(0);

    TupleDescription in_td(get_in_schema(0));
    TupleDescription out_td(get_out_schema(0));

    // Do stuff...
    while (inv.continue_dequeue_on(my_deq_iterator, 0))
    {

        const void *data = my_deq_iterator.tuple();
        Tuple input_tuple = Tuple::factory(data);

        DEBUG << "FilterArrayQBox " << get_name() << " IN: " << in_td.tuple_to_string(data);
        DEBUG << "  BYTES " << to_hex_string(data, _output_tuple_size);

        // assuming a single-port filter is used in LS
        //
        if (input_tuple.get_fake_flag())             
        {
            memcpy(enq(0).tuple(), data, _output_tuple_size);

            DEBUG << "FilterArrayQBox OUT from expr: " << out_td.tuple_to_string(data);

            ++ (enq(0));
            outputted[0] = true;
            DEBUG << " -> through port " << 0;
            ++my_deq_iterator;
            continue;
        }

        bool passed = false;

        _ctxt.reset();
        _ctxt.set_tuple((const char *)data);
    
    // get our value field index for data
    int field_index = in_td.index_of_field(string("value"));
    if(field_index != -1)
      {
        // Acquire the offset of the field pointer in the tuple
        uint16 offset = in_td.get_offset(field_index);
        
        // the field gives a memory address to the actual array; 
        //point to the first value
        int32  *array_ptr= (int32 *) (*(int32 *) (((char *)data) + offset));

        // Add offset from DynamicArray header
        // was:  array_ptr = (array_ptr + sizeof(DataPath::DynamicArray));
        array_ptr += DynamicArray::header_size();

        uint8 *current_value;
        for(int x = 0; x < 320*240*3; ++x)
        {
        current_value = ((uint8 *)array_ptr) + x*sizeof(uint8);
        // apply your predicate here (idx >= 128)
        if((int) *current_value < 128)
          {
            // Null out all values that do not match the predicate
            *current_value = 0;
          }
        
          }
          
      }
    DEBUG << "Finished filtering array!";
    // end ad-hoc array handler 

        // check expressions
        for (unsigned int i = 0; i < _expressions.size(); ++i)
        {

            // If a tuple is of type BOUNDARY or RECONCILIATION_DONE, propagate as is on ALL output streams
            if ( (input_tuple.get_tuple_type() == TupleType::BOUNDARY) ||
                 (input_tuple.get_tuple_type() == TupleType::RECONCILIATION_DONE))
            {
                memcpy(enq(i).tuple(), data, _output_tuple_size);
                ++(enq(i));
                outputted[i] = true;
                passed = false; // Keep passed at false so we also output on false stream
            }

            else if ( _expressions[i]->eval<bool>(_ctxt) )
            {
                memcpy(enq(i).tuple(), data, _output_tuple_size);

                DEBUG << "FilterArrayQBox OUT from expr: " << out_td.tuple_to_string(data);

                ++(enq(i));
                passed = true;
                outputted[i] = true;
                DEBUG << " -> through port " << i;
                break; // Once it passed a predicate, move on
            }
            else if (input_tuple.get_win_spec() >= 0)
            {
                // win spec has to be preserved; will pass as a fake tuple
                // this case works for only in LS and there is single port
                input_tuple.set_fake_flag(true);
                memcpy(enq(i).tuple(), data, _output_tuple_size);

                DEBUG << "FilterArrayQBox OUT from expr: " << out_td.tuple_to_string(data);

                ++(enq(i));
                passed = true;
                outputted[i] = true;
                DEBUG << " -> through port " << i;
                break;
            }
        }

        // catch folks going to the false port
        if (!passed && _pass_on_false_port)
        {
            outputted[_expr_count] = true;

            DEBUG << " FilterArrayQBox OUT: " << out_td.tuple_to_string(data);

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

void FilterArrayQBox::set_pending_box_state(ptr<AbstractBoxState> packed_box)
{
    //FilterArrayBoxState* actual_packed_box = static_cast<FilterArrayBoxState*>(packed_box.get());
    //_pending_box_state.set(new FilterArrayBoxState(*actual_packed_box));
    _pending_box_state = packed_box.dynamic_cast_to<FilterArrayBoxState>();
}

ptr<AbstractBoxState> FilterArrayQBox::pack_state(bool delta_only)
{
  INFO << "Packing state for FilterArrayQBox named [" << get_name() << "]";
  ptr<FilterArrayBoxState> filter_box_state(new FilterArrayBoxState());
  return filter_box_state;
}


BOREALIS_NAMESPACE_END
