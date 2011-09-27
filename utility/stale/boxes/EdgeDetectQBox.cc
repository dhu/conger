#include "EdgeDetectQBox.h"
#include "DataPath.h"
#include "DynamicArray.h"
#include <list>
#include <math.h>

BOREALIS_NAMESPACE_BEGIN

void EdgeDetectQBox::setup_impl() throw (AuroraException)
{
    if ( get_num_inputs() != 1 )
    {   Throw(aurora_typing_exception,
              "Filter requires exactly one input stream (not " +
              to_string(get_num_inputs()) + ")");
    }

    // Optional parameter: pass-on-false-port
    _pass_on_false_port = false;
    typed_param("pass-on-false-port",_pass_on_false_port);
    typed_param("threshold", _ed_threshold);
    DEBUG << "Threshold: " << _ed_threshold;
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

    DEBUG << "EdgeDetectQBox input schema " << get_in_schema(0)->as_string();
    DEBUG << "EdgeDetectQBox output schema " << get_out_schema(0)->as_string();

    /* 3x3 GX Sobel mask.  Ref: www.cee.hw.ac.uk/hipr/html/sobel.html */
    GX[0][0] = -1; GX[0][1] = 0; GX[0][2] = 1;
    GX[1][0] = -2; GX[1][1] = 0; GX[1][2] = 2;
    GX[2][0] = -1; GX[2][1] = 0; GX[2][2] = 1;

    /* 3x3 GY Sobel mask.  Ref: www.cee.hw.ac.uk/hipr/html/sobel.html */
    GY[0][0] =  1; GY[0][1] =  2; GY[0][2] =  1;
    GY[1][0] =  0; GY[1][1] =  0; GY[1][2] =  0;
    GY[2][0] = -1; GY[2][1] = -2; GY[2][2] = -1;
}

void EdgeDetectQBox::init_impl() throw (AuroraException)
{
  TupleDescription out_td(get_out_schema(0));
  _output_tuple_size = out_td.get_size();

}

void EdgeDetectQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
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

        DEBUG << "EdgeDetectQBox " << get_name() << " IN: " << in_td.tuple_to_string(data);
        DEBUG << "  BYTES " << to_hex_string(data, _output_tuple_size);

        // assuming a single-port filter is used in LS
        //
        if (input_tuple.get_fake_flag())             
        {
            memcpy(enq(0).tuple(), data, _output_tuple_size);

            DEBUG << "EdgeDetectQBox OUT from expr: " << out_td.tuple_to_string(data);

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
    int field_index = in_td.index_of_field(string("selected_frame"));
    if(field_index != -1)
          {
        // Acquire the offset of the field pointer in the tuple
        uint16 offset = in_td.get_offset(field_index);
          
        // the field gives a memory address to the actual array; 
        //point to the first value - needs to stay int32 b/c that is how it is handed to us?
        DynamicArray *theArray = (DynamicArray *)((*(int32 *) (((char *)data) + offset))); 
        // was: uint8 *array_ptr = (uint8 *) (theArray + sizeof(DynamicArray));
        uint8 *array_ptr = (uint8 *) (theArray + DynamicArray::header_size());

        //DEBUG << "First vals: " << (int) *(array_ptr) << ", " << (int) *(array_ptr+1) << ", " << (int) *(array_ptr+2);


        int32 X, Y, SUM, I, J, sumX, sumY;
        const int numRows = 288;
        const int numCols = 352;
        /*  Sobel algorithm from http://www.pages.drexel.edu/~weg22/edge.html */
        for(Y=0; Y<=(numRows-1); ++Y) {
          for(X=0; X<=(numCols-1); ++X) {

        SUM = sumX = sumY = 0;

        /* image boundaries */
        if(Y==0 || Y==numRows-1)
          SUM = 0;
        else if(X==0 || X==numCols-1)
          SUM = 0;

        /* Convolution starts here */
        else {

          /*-------X GRADIENT APPROXIMATION------*/
          for(I=-1; I<=1; ++I) {
            for(J=-1; J<=1; ++J) {
              int pixelR =  (int8) *(array_ptr + (X + I + (Y+J)*numCols)*3);
              //DEBUG << "R-val: " << pixelR;
              sumX += pixelR*GX[I+1][J+1];
              //              sumX = sumX + (signed int32) ((*(array_ptr + (X + I + (Y+J)*numCols)*3))*GX[I+1][J+1]);
            }
          }

          //-------Y GRADIENT APPROXIMATION-------*
          for(I=-1; I<=1; ++I) {
            for(J=-1; J<=1; ++J) {
              int pixelR =  (int8) *(array_ptr + (X + I + (Y+J)*numCols)*3);
              //DEBUG << "R-val: " << pixelR;
              sumY += pixelR*GY[I+1][J+1];
              //sumY = sumY + (signed int32)((*(array_ptr + (X + I + (Y+J)*numCols)*3))*GY[I+1][J+1]);
            }
          }
          
          SUM = static_cast<int>(sqrt(sumX*sumX + sumY*sumY));
          
        }
          
          /*---GRADIENT MAGNITUDE APPROXIMATION (Myler p.218)----*/
          
        //DEBUG << "Sum: " << SUM;

        
        
        if(SUM > _ed_threshold)
          { //set to white
            *(array_ptr + X*3 + Y*numCols*3) = 255;
            *(array_ptr + X*3 + Y*numCols*3+1) = 255;
            *(array_ptr + X*3 + Y*numCols*3+2) = 255;
          }
            else
          { // non-edges = black
            *(array_ptr + X*3 + Y*numCols*3) = 0;
            *(array_ptr + X*3 + Y*numCols*3+1) = 0;
            *(array_ptr + X*3 + Y*numCols*3+2) = 0;
          }
        
          
          }

        }

          
        // end Sobel Edge Detect Algorithm
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
          
          DEBUG << "EdgeDetectQBox OUT from expr: " << out_td.tuple_to_string(data);

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

                DEBUG << "EdgeDetectQBox OUT from expr: " << out_td.tuple_to_string(data);

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

            DEBUG << " EdgeDetectQBox OUT: " << out_td.tuple_to_string(data);

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

void EdgeDetectQBox::set_pending_box_state(ptr<AbstractBoxState> packed_box)
{
    //EdgeDetectBoxState* actual_packed_box = static_cast<EdgeDetectBoxState*>(packed_box.get());
    //_pending_box_state.set(new EdgeDetectBoxState(*actual_packed_box));
    _pending_box_state = packed_box.dynamic_cast_to<EdgeDetectBoxState>();
}

ptr<AbstractBoxState> EdgeDetectQBox::pack_state(bool delta_only)
{
  INFO << "Packing state for EdgeDetectQBox named [" << get_name() << "]";
  ptr<EdgeDetectBoxState> filter_box_state(new EdgeDetectBoxState());
  return filter_box_state;
}


BOREALIS_NAMESPACE_END
