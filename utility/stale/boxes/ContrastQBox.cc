#include "ContrastQBox.h"
#include "DataPath.h"
#include "DynamicArray.h"
#include <list>

BOREALIS_NAMESPACE_BEGIN

void ContrastQBox::setup_impl() throw (AuroraException)
{
    if ( get_num_inputs() != 1 )
    {   Throw(aurora_typing_exception,
              "Contrast requires exactly one input stream (not " +
              to_string(get_num_inputs()) + ")");
    }

    // Optional parameter: pass-on-false-port
    _pass_on_false_port = false;
    typed_param("pass-on-false-port",_pass_on_false_port);
    typed_param("type", _box_type);
    DEBUG << "Box type: " << _box_type;
    for ( uint32 i = 0; ; ++i )
    {   string expr = param( "expression." + to_string( i ));

        if ( expr.empty() )
        {   if ( i == 0 )
            {   Throw(aurora_typing_exception,
                      "Contrast requires at least one expression parameter. (\"predicate\" is deprecated)");
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

//    setDataPath("/u/backman/borealis/sandbox/borealis/demo/array_Contrastion/data/data");

    DEBUG << "ContrastQBox input schema " << get_in_schema(0)->as_string();
    DEBUG << "ContrastQBox output schema " << get_out_schema(0)->as_string();
}

void ContrastQBox::init_impl() throw (AuroraException)
{
  TupleDescription out_td(get_out_schema(0));
  _output_tuple_size = out_td.get_size();

}

void ContrastQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
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

        DEBUG << "ContrastQBox " << get_name() << " IN: " << in_td.tuple_to_string(data);
        DEBUG << "  BYTES " << to_hex_string(data, _output_tuple_size);

        // assuming a single-port Contrast is used in LS
        //
        if (input_tuple.get_fake_flag())             
        {
            memcpy(enq(0).tuple(), data, _output_tuple_size);

            DEBUG << "ContrastQBox OUT from expr: " << out_td.tuple_to_string(data);

            ++ (enq(0));
            outputted[0] = true;
            DEBUG << " -> through port " << 0;
            ++my_deq_iterator;
            continue;
        }

        bool passed = false;

        _ctxt.reset();
        _ctxt.set_tuple((const char *)data);


        int contrast_index = in_td.index_of_field(string("contrast"));
        int *contrast_Ptr;
        if (contrast_index != -1)
        {
            uint16 contrast_offset = in_td.get_offset(contrast_index);
            contrast_Ptr = (int*)((unsigned char *)data + contrast_offset);

            // get our value field index for data
            int field_index = in_td.index_of_field(string("ccd_frame"));
            if(field_index != -1)
            {
                // Acquire the offset of the field pointer in the tuple
                uint16 offset = in_td.get_offset(field_index);

        DEBUG << "Contrast index: " << in_td.index_of_field(string("contrast")) << "   offset: " << in_td.get_offset(contrast_index);
        DEBUG << "Value index: " << in_td.index_of_field(string("ccd_array")) << "   offset: " << in_td.get_offset(field_index);


                // the field gives a memory address to the actual array; 
                //point to the first value
                unsigned char  *array_ptr = reinterpret_cast<unsigned char *>((*(int32 *) (((char *)data) + offset)));
                // Add offset from DynamicArray header
                array_ptr = array_ptr + ((DynamicArray::header_size()) / sizeof(unsigned char));

                //Contrast Binning
                int histogram[256];
                for(int i=0; i < 256; i++)
                    histogram[i] = 0;

                for(int i=0; i < 288; i++)
                for(int j=0; j < 352*3; j+=3)
                {
                    double grayValue = 0.3*((double)array_ptr[i*352*3 + j]);
                    grayValue += 0.59*((double)array_ptr[i*352*3 + j+1]);
                    grayValue += 0.11*((double)array_ptr[i*352*3 + j+2]);
                    histogram[ (int)grayValue ]++;
                }

                int low=0, high=255;
                for(int lowCounter=0;  lowCounter  < 10000;)
                    lowCounter  += histogram[ low++ ];
                for(int highCounter=0; highCounter < 10000;)
                    highCounter += histogram[ high--];
                *contrast_Ptr = high-low;
        NOTICE << "Contrast: calculated!";
            }
    }

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

            DEBUG << " ContrastQBox OUT: " << out_td.tuple_to_string(data);

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
      {
            get_output(i).notify_enq();
        DEBUG << "Notified of enqueue";
      }
    if (_pass_on_false_port && outputted[_expr_count])
        get_output(_expr_count).notify_enq();
}

void ContrastQBox::set_pending_box_state(ptr<AbstractBoxState> packed_box)
{
    //ContrastBoxState* actual_packed_box = static_cast<ContrastBoxState*>(packed_box.get());
    //_pending_box_state.set(new ContrastBoxState(*actual_packed_box));
    _pending_box_state = packed_box.dynamic_cast_to<ContrastBoxState>();
}

ptr<AbstractBoxState> ContrastQBox::pack_state(bool delta_only)
{
  INFO << "Packing state for ContrastQBox named [" << get_name() << "]";
  ptr<ContrastBoxState> Contrast_box_state(new ContrastBoxState());
  return Contrast_box_state;
}


BOREALIS_NAMESPACE_END
