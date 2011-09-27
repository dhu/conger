#include "ContrastCompositeQBox.h"
#include <list>

BOREALIS_NAMESPACE_BEGIN
#define FRAME_WIDTH 352
#define FRAME_HEIGHT 288
#define CHUNK_X 44
#define CHUNK_Y 44


uint8 contrast_chunk(uint8 *array, int startX, int startY)
{
  uint8 *startPos = array + (FRAME_WIDTH*startY + startX)*3;
  int x, y;
  //Contrast Binning
  int histogram[256];
  double grayValue;
  uint8 *currentPos;
  uint8 contrast;
//--------------------------------
  for(int i=0; i < 256; i++)
    histogram[i] = 0;


  for(y = 0; y < CHUNK_Y; ++y)
    {
      for(x = 0; x < CHUNK_X; ++x)
    {
      currentPos = startPos + (y*FRAME_WIDTH+x)*3;
      grayValue = 0.3*((double)*currentPos);
      grayValue += 0.59*((double)*(currentPos+1));
      grayValue += 0.11*((double)*(currentPos+2));
      histogram[ (int)grayValue ]++;
    }
    }
  uint8 low=0, high=255;
  for(int lowCounter=0;  lowCounter  < 100;)
    lowCounter  += histogram[ low++ ];
  for(int highCounter=0; highCounter < 100;)
    highCounter += histogram[ high--];
  //DEBUG << "High: " << (uint) high << " Low: " << (uint) low;
  contrast =  high-low;
  //NOTICE << "Contrast: " << (uint) contrast;
  return contrast;
    
}

void ContrastCompositeQBox::setup_impl() throw (AuroraException)
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

    DEBUG << "ContrastCompositeQBox input schema " << get_in_schema(0)->as_string();
    DEBUG << "ContrastCompositeQBox output schema " << get_out_schema(0)->as_string();
}

void ContrastCompositeQBox::init_impl() throw (AuroraException)
{
  TupleDescription out_td(get_out_schema(0));
  _output_tuple_size = out_td.get_size();

}

void ContrastCompositeQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{

  int x, y;
  //Contrast Binning
  int histogram[256];
  double grayValue;
  uint8 *currentPos;


  //----------------------------------
  
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

        DEBUG << "ContrastCompositeQBox " << get_name() << " IN: " << in_td.tuple_to_string(data);
        DEBUG << "  BYTES " << to_hex_string(data, _output_tuple_size);

        // assuming a single-port Contrast is used in LS
        //
        if (input_tuple.get_fake_flag())             
        {
            memcpy(enq(0).tuple(), data, _output_tuple_size);

            DEBUG << "ContrastCompositeQBox OUT from expr: " << out_td.tuple_to_string(data);

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
    int field_index = in_td.index_of_field(string("ccd_frame"));
        uint8 *ccd_ptr;
    char *contrast_vals;
    int i, j;

        if (contrast_index != -1 && field_index != -1)
      {
        DEBUG << "Doing contrast composite!";
            uint16 contrast_offset = in_td.get_offset(contrast_index);
        uint16 ccd_offset  = in_td.get_offset(field_index);

          // the field gives a memory address to the actual array; 
          //point to the first value
          ccd_ptr = reinterpret_cast<unsigned char *>((*(int32 *) (((char *)data) + ccd_offset)));
          contrast_vals = ((char *)data) + contrast_offset;
          // Add offset from DynamicArray header
          ccd_ptr = ccd_ptr + 64;
          for(i = 0; i < 6; ++i)
        {
          for(j = 0; j < 8; ++j)
            {

              int startX = CHUNK_X*j;
              int startY = CHUNK_Y*i;
            
            uint8 *startPos = ccd_ptr + (FRAME_WIDTH*startY + startX)*3;
              

              for(int k=0; k < 256; k++)
            histogram[k] = 0;


              for(y = 0; y < CHUNK_Y; ++y)
            {
              for(x = 0; x < CHUNK_X; ++x)
                {
                  currentPos = startPos + (y*FRAME_WIDTH+x)*3;
                  grayValue = 0.3*((double)*currentPos);
                  grayValue += 0.59*((double)*(currentPos+1));
                  grayValue += 0.11*((double)*(currentPos+2));
                  histogram[ (int)grayValue ]++;
                }
            }
              uint8 low=0, high=255;
              for(int lowCounter=0;  lowCounter  < 100;)
            lowCounter  += histogram[ low++ ];
              for(int highCounter=0; highCounter < 100;)
            highCounter += histogram[ high--];
              char contrast = high-low;
              memcpy((void *)(contrast_vals + i*8+j), &contrast, sizeof(char));
              //char c = contrast_vals[i*8+j];
              //NOTICE << "Contrast: " << (uint) c << ", " << high-low;

              //DEBUG << "Calling contrast_chunk";
              //contrast_vals[i*8+j] = contrast_chunk(ccd_ptr, CHUNK_X*j, CHUNK_Y*i);
              // JMR may want to investigate this later; although it is printing out funny I may be debugging only a debug statement
              //DEBUG << "Returned from contrast_chunk, with value " << (uint) contrast_vals[i*8+j];
            }
        }

      }
    else
      DEBUG << "Not doing composite!";

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

            DEBUG << " ContrastCompositeQBox OUT: " << out_td.tuple_to_string(data);

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

void ContrastCompositeQBox::set_pending_box_state(ptr<AbstractBoxState> packed_box)
{
    //ContrastCompositeBoxState* actual_packed_box = static_cast<ContrastCompositeBoxState*>(packed_box.get());
    //_pending_box_state.set(new ContrastCompositeBoxState(*actual_packed_box));
    _pending_box_state = packed_box.dynamic_cast_to<ContrastCompositeBoxState>();
}

ptr<AbstractBoxState> ContrastCompositeQBox::pack_state(bool delta_only)
{
  INFO << "Packing state for ContrastCompositeQBox named [" << get_name() << "]";
  ptr<ContrastCompositeBoxState> Contrast_box_state(new ContrastCompositeBoxState());
  return Contrast_box_state;
}


BOREALIS_NAMESPACE_END
