#include "RevisionFilterQBox.h"

BOREALIS_NAMESPACE_BEGIN

void RevisionFilterQBox::setup_impl() throw (AuroraException)
{
  if ( get_num_inputs() != 1 )
    {   Throw(aurora_typing_exception,
              "Filter requires exactly one input stream (not " +
              to_string(get_num_inputs()) + ")");
    }

  // Optional parameter: pass-on-false-port
  _pass_on_false_port = false;

  set_emit_hist( true );

  /** MODE OF REVISION PROCESSING **/
  string mode = param("processing-mode", PARAM_NON_EMPTY);
  if (mode == "UPSTREAM") 
    {
      DEBUG << get_name() << " map box in UPSTREAM mode";
      _revision_processing_mode = UPSTREAM;
    } 
  else if (mode == "DOWNSTREAM") 
    {
      DEBUG << get_name() << " map box in DOWNSTREAM mode";
      _revision_processing_mode = DOWNSTREAM;
    }
  else 
    {
      Throw(aurora_typing_exception, "Unsupported RevisionProcessingMode: " + mode);
    }
    

  typed_param("pass-on-false-port",_pass_on_false_port);
  if (_pass_on_false_port) {
    DEBUG << get_name() << " filter box is INVERTIBLE";
    _invertible = true;
  }

  for ( uint32 i = 0; ; ++i )
    {  
      string expr = param( "expression." + to_string( i ));
      
      if ( expr.empty() )
    {   if ( i == 0 )
      {  
        Throw(aurora_typing_exception,
          "Filter requires at least one expression parameter. (\"predicate\" is deprecated)");
      }
    else
      {   
        break;
      }
    }
      
      ExprContext  ctxt;
      ctxt.set_context_schema( *get_in_schema( 0 ));
      
      _expressions.push_back( Expression::parse( expr, ctxt ));
    }
  _expr_count = _expressions.size();
 
  // anchors for revision processing
  string anchor_name = param("anchor");
  split(anchor_name, ',', _anchor);


  // Set input and output descriptions
  // There are as many output streams as predicates, +1 if passing "false"
  // tuples too
  for ( uint32  i = 0; i < _expr_count; ++i )
    {   set_out_description( i, get_in_stream( 0 )); // same output type as input everywhere
    }

  if ( _pass_on_false_port )
    {   set_out_description( _expr_count, get_in_stream( 0 ));
    }

  DEBUG << "RevisionFilterQBox input schema " << get_in_schema(0)->as_string();
  DEBUG << "RevisionFilterQBox output schema " << get_out_schema(0)->as_string();
}

void RevisionFilterQBox::init_impl() throw (AuroraException)
{
  TupleDescription out_td(get_out_schema(0));
  _output_tuple_size = out_td.get_size();

}

void RevisionFilterQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
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
      void *out = NULL;

      bool stepping = // STEPPER: Change this to change paused-on tuples in DEBUG
    (input_tuple.get_tuple_type() == TupleType::DELETION ||
     input_tuple.get_tuple_type() == TupleType::REPLACEMENT ||
     (input_tuple.get_tuple_type() == TupleType::INSERTION &&
      input_tuple.get_revision_id() != -1));
      
      INFO << "RevisionFilterQBox " << get_name() << " IN: " << in_td.tuple_to_string(data);
      if(stepping)
    {
      ////    DEBUG << (char)getchar();
    }

      // assuming a single-port filter is used in LS
      //
      if (input_tuple.get_fake_flag())
        {
      memcpy(enq(0).tuple(), data, _output_tuple_size);

      INFO<< "RevisionFilterQBox " << get_name() << " OUT from expr: "
          << out_td.tuple_to_string(data);
      INFO << " -> through port " << 0;
      if(stepping)
        {
          ////        DEBUG << (char)getchar();
        }

      ++ (enq(0));
      outputted[0] = true;
      ++my_deq_iterator;
      continue;
        }

      bool passed = false;

      _ctxt.reset();
      _ctxt.set_tuple((const char *)data);

      TupleType input_tuple_type = input_tuple.get_tuple_type();

      if(     input_tuple_type == TupleType::INSERTION
          ||  input_tuple_type == TupleType::DELETION
          || (input_tuple_type == TupleType::HISTORICAL && get_emit_hist()) )
        {
      DEBUG << "NORMAL TUPLE:";
      // check expressions
      for (unsigned int i = 0; i < _expressions.size(); ++i)
            {
          if (_expressions[i]->eval<bool>(_ctxt))
                {
          out = enq(i).tuple();
          memset(out, '\0', _output_tuple_size);
          memcpy(out, data, _output_tuple_size);

          INFO << "RevisionFilterQBox " << get_name() << " OUT from expr: "
               << out_td.tuple_to_string(data);
          INFO << " -> through port " << i;
          if(stepping)
            {
              ////            DEBUG << (char)getchar();
            }

          ++(enq(i));
          passed = true;
          outputted[i] = true;
          break; // Once it passed a predicate, move on
                }
          else if (input_tuple.get_win_spec() >= 0)
                {
          // win spec has to be preserved; will pass as a fake tuple
          // this case works for only in LS and there is single port
          input_tuple.set_fake_flag(true);
          memcpy(enq(i).tuple(), data, _output_tuple_size);

          INFO << "RevisionFilterQBox " << get_name() << " OUT from expr: "
               << out_td.tuple_to_string(data);
          INFO << " -> through port " << i;
          if(stepping)
            {
              ////            DEBUG << (char)getchar();
            }

          ++(enq(i));
          passed = true;
          outputted[i] = true;
          break;
                }
            }

      // catch folks going to the false port
      if (!passed && _pass_on_false_port)
            {
          outputted[_expr_count] = true;

          INFO << "RevisionFilterQBox " << get_name() << " OUT: "
           << out_td.tuple_to_string(data);
          INFO << " -> through the false port";
          if(stepping)
        {
          ////        DEBUG << (char)getchar();
        }

          memcpy(enq(_expr_count).tuple(), data, _output_tuple_size);
          ++ (enq(_expr_count));
            } else if (!passed)
          {
                INFO << " -> tuple was dumped";
        if(stepping)
          {
            ////        DEBUG << (char)getchar();
          }
          }
        }
      else if( input_tuple_type == TupleType::DELIMITER && get_emit_hist() )
        {
      DEBUG << "DELIMITER TUPLE";
      // Send delimiter along to all predicate outputs
      for( unsigned int i = 0; i < _expressions.size(); i++ )
            {
          out = enq(i).tuple();
          memset( out, '\0', _output_tuple_size );
          memcpy( out, data, _output_tuple_size );
          INFO << "RevisionFilterQBox " << get_name() << " DELIMITER OUT ";
          ++(enq(i));
          outputted[i] = true;
          INFO << " -> through port " << i;
          if(stepping)
        {
          ////        DEBUG << (char)getchar();
        }
            }

      // And the false port, if appropriate
      if( _pass_on_false_port )
            {
          outputted[_expr_count] = true;
          INFO << " RevisionFilterQBox " << get_name() << " DELIMITER OUT "
           << " -> through the false port";
          if(stepping)
        {
          ////        DEBUG << (char)getchar();
        }
          memcpy( enq(_expr_count).tuple(), data, _output_tuple_size);
          ++(enq(_expr_count));
            }
        }
      else if( input_tuple_type == TupleType::REPLACEMENT )
        {
      bool old_passes;
      bool new_passes;
      bool new_passed = false;

      // data to hold input tuple as revised. Note that input and output
      // tuples are identical.
      char new_data[_output_tuple_size];
      memset( new_data, '\0', _output_tuple_size );
      memcpy( new_data, data, _output_tuple_size );
      Tuple new_tuple = Tuple::factory( new_data );

      string revised_val = get_revised_val( new_tuple, *get_in_schema(0) );
      const SchemaField *revised_field
        = in_td.get_schema_field(new_tuple.get_revised());
      revised_val.copy( new_data + revised_field->get_offset(),
                revised_field->get_size() );

      DEBUG << "REPLACEMENT PROCESSING:\n"
        <<   "Old tuple: " << in_td.tuple_to_string(data)
        << "\nNew Tuple: " << in_td.tuple_to_string(new_data);

      // -1 in bytes for setting purposes
      char negative_one[sizeof(int)];
      (*(int *) negative_one) = -1;

      for( unsigned int i = 0; i < _expressions.size(); ++i )
            {
          // Determine if old, new tuples pass through this output
          if( !passed )
                {
          _ctxt.reset();
          _ctxt.set_tuple((const char *)data);
          old_passes = _expressions[i]->eval<bool>(_ctxt);
                }
          else
                {
          old_passes = false;
                }

          if( !new_passed )
                {
          _ctxt.reset();
          _ctxt.set_tuple((const char *)new_data);
          new_passes = _expressions[i]->eval<bool>(_ctxt);
                }
          else
                {
          new_passes = false;
                }

          // Do as appropriate based on combination of passed/not
          // Both: replacement; old: deletion; new: insertion; none: none
          if( old_passes && new_passes )
                {
          DEBUG << "PASS REPLACEMENT";
          // Pass on the original input tuple
          out = enq(i).tuple();
          memset( out, '\0', _output_tuple_size );
          memcpy( out, data, _output_tuple_size );
          passed = true;
          new_passed = true;
          outputted[i] = true;
          ++(enq(i));

          INFO << "RevisionFilterQBox " << get_name() << " OUT from expr: "
               << out_td.tuple_to_string(data)
               << " -> through port " << i;
          if(stepping)
          {
              ////            DEBUG << (char)getchar();
          }
          }
          else if( old_passes )
          {
          DEBUG << "PASS DELETION";
          // Pass a deletion to this output
          input_tuple.set_tuple_type( TupleType::DELETION );
          // clear up rev_field and revised_value
          input_tuple.set_revised(-1);
          set_revised_val(input_tuple, *get_in_schema(0), to_string(negative_one));

          out = enq(i).tuple();
          memset( out, '\0', _output_tuple_size );
          memcpy( out, data, _output_tuple_size );
          passed = true;
          outputted[i] = true;
          ++(enq(i));
          INFO << "RevisionFilterQBox " << get_name() << " OUT from expr: "
               << out_td.tuple_to_string(data)
               << " -> through port " << i;
          if(stepping)
            {
              ////            DEBUG << (char)getchar();
            }
                }
          else if( new_passes )
                {
          DEBUG << "PASS INSERTION";
          // Pass an insertion to this output,
          // with the tuple as revised
          new_tuple.set_tuple_type( TupleType::INSERTION );
          // clear up rev_field and revised_value
          new_tuple.set_revised(-1);
          set_revised_val(new_tuple, *get_in_schema(0), to_string(negative_one));

          out = enq(i).tuple();
          memset( out, '\0', _output_tuple_size );
          memcpy( out, new_data, _output_tuple_size );
          new_passed = true;
          outputted[i] = true;
          ++(enq(i));
          INFO << "RevisionFilterQBox " << get_name() << " OUT from expr: "
               << out_td.tuple_to_string(new_data)
               << " -> through port " << i;
          if(stepping)
            {
              ////            DEBUG << (char)getchar();
            }
                }

          // Might as well break if everything has gone places
          if( passed && new_passed )
                {
          break;
                }
            }

      // False port, anyone?
      if( _pass_on_false_port && (!passed || !new_passed) )
            {
          if( !passed && !new_passed )
                {
          memset( new_data, '\0', _output_tuple_size );
          memcpy( new_data, data, _output_tuple_size );
                }
          else if( !passed )
                {
          memset( new_data, '\0', _output_tuple_size );
          memcpy( new_data, data, _output_tuple_size );
          new_tuple.set_tuple_type( TupleType::DELETION );
          // clear up rev_field and revised_value
          new_tuple.set_revised(-1);
          set_revised_val(new_tuple, *get_in_schema(0), to_string(negative_one));
          }
          else if( !new_passed )
          {
              new_tuple.set_tuple_type( TupleType::INSERTION );
            // clear up rev_field and revised_value
            new_tuple.set_revised(-1);
            set_revised_val(new_tuple, *get_in_schema(0), to_string(negative_one));
          }
          else
          {
              DEBUG << "RevisionFilterQBox: Computer can't do logic";
          }

          out = enq(_expr_count).tuple();
          memset( out, '\0', _output_tuple_size );

          memcpy(out,
                 new_data,
                 _output_tuple_size );

          outputted[_expr_count] = true;
          ++(enq(_expr_count));

          INFO << "RevisionFilterQBox " << get_name() << " OUT from expr: "
           << out_td.tuple_to_string(new_data)
           << " -> through the false port";

          if(stepping)
          {
            ////        DEBUG << (char)getchar();
          }
        }

        if( !_pass_on_false_port && !passed && !new_passed )
        {
          INFO << " -> tuple was dumped";
          if(stepping)
          {
            ////        DEBUG << getchar();
          }
        }
    }
    else if( input_tuple_type == TupleType::REQUEST )
    {
      // Send new requests to the sweeper and the outputs
            
      // Create the new request tuple:
      uint8  new_header[HEADER_SIZE];

      memcpy( new_header, data, HEADER_SIZE );
      Tuple new_tuple = Tuple::factory(new_header);
      new_tuple.set_revised(new_tuple.get_revision_id()); // old revision id in rev_field
      new_tuple.set_revision_id(_local_boxid);
      // no need to change win_spec and tuple_id

      // Send the new request tuple to all downstream boxes
      for( unsigned int i = 0; i <= _expr_count; i++ )
      {
          out = enq(i).tuple();
          memset( out, '\0', _output_tuple_size );
          memcpy( out, new_header, HEADER_SIZE );
          outputted[i] = true;
          ++(enq(i));

          INFO << "RevisionFilterQBox " << get_name() << " OUT from expr: "
           << out_td.tuple_to_string( new_header )
           << " -> through port " << to_string(i);

          if(stepping)
          {
             ////        DEBUG << (char)getchar();
          }
      }

      // And to the sweeper
      send_to_sweeper(new_header);

      INFO << "RevisionFilterQBox " << get_name() << " OUT: "
           << out_td.tuple_to_string( new_header )
           << " -> to the Sweeper";
      if(stepping)
        {
          ////        DEBUG << (char)getchar();
        }
    }
      else if( input_tuple_type == TupleType::NOTIFICATION )
    {
      // Grab the historical and drop it back on the sweeper
      // TODO: proper multi-sweeper behavior
      //uint8 *new_tuple = _sweeper_box->notify( data );
      uint8 *his_tuple = retrieve_historical( (uint8 *)data );

      if( his_tuple != NULL )
        {
          INFO << "RevisionFilterQBox retrieve: "
           << out_td.tuple_to_string( his_tuple )
           << " from Sweeper";
          send_to_sweeper(his_tuple);

          INFO << "RevisionFilterQBox " << get_name() << " OUT: "
           << out_td.tuple_to_string( his_tuple )
           << " -> to the Sweeper";
          if(stepping)
        {
          ////        DEBUG << (char)getchar();
        }
        }
    }
      else
        {
      INFO << "RevisionFilterQBox: Dropped tuple: "
           << out_td.tuple_to_string(data);
      if(stepping)
        {
          ////        DEBUG << (char)getchar();
        }
        }

      // neeeeeeeeeeext!
      ++my_deq_iterator;
    }

  // Done, notify everyone where appropriate (note, doing it at the end may
  //  introduce latency, so perhaps one could explore various timings at
  //  which to notify) -- eddie
  for (unsigned int i = 0; i < _expr_count; ++i)
    if (outputted[i])
      get_output(i).notify_enq();
  if (_pass_on_false_port && outputted[_expr_count])
    get_output(_expr_count).notify_enq();
}

void RevisionFilterQBox::set_pending_box_state(ptr<AbstractBoxState> packed_box)
{
  //FilterBoxState* actual_packed_box = static_cast<FilterBoxState*>(packed_box.get());
  //_pending_box_state.set(new FilterBoxState(*actual_packed_box));
  _pending_box_state = packed_box.dynamic_cast_to<RevisionFilterBoxState>();
}

ptr<AbstractBoxState> RevisionFilterQBox::pack_state(bool delta_only)
{
  INFO << "Packing state for RevisionFilterQBox named [" << get_name() << "]";
  ptr<RevisionFilterBoxState> filter_box_state(new RevisionFilterBoxState());
  return filter_box_state;
}


BOREALIS_NAMESPACE_END
