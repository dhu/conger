#include "AuroraJoinQBox.h"
#include <cmath>

BOREALIS_NAMESPACE_BEGIN


const int AuroraJoinQBox::LEFT = 0;
const int AuroraJoinQBox::RIGHT = 1;

void AuroraJoinQBox::setup_impl() throw( AuroraException )
{
  if ( get_num_inputs() != 2 )
    {   
      Throw( aurora_typing_exception,
         "Join requires exactly 2 inputs (not " +
         to_string( get_num_inputs() ) + ")" );
    }

  // The predicate.
  ExprContext  ctxt;

  ctxt.set_named_schema_info( "left",  get_in_schema(LEFT),  LEFT );
  ctxt.set_named_schema_info( "right", get_in_schema(RIGHT), RIGHT );

  string  pred = param( "predicate", PARAM_NON_EMPTY );
  _predicate = Expression::parse( pred, ctxt );

  // Create the buffers based on parameters.
  _buffer[ LEFT ]  = create_buffer_by_parameter( LEFT );
  _buffer[ RIGHT ] = create_buffer_by_parameter( RIGHT );

  // Timeout.
  double  timeout;
  _timing_out = typed_param( "timeout", timeout, PARAM_NOT_REQUIRED );

  if ( _timing_out )
    {
      if ( timeout < 0 )
        {
      // Negative timeout means not timing out.
      // TODO: What does a timeout of 0 mean? Maybe its like
      // window_size by tuples = 1.
      _timing_out = false;
        }
      else
        {
      // Convert to timeval.
      int32 sec  = static_cast<int32>( floor( timeout ));
      int32 usec = static_cast<int32>( floor(( timeout - static_cast<double>( sec )) * 1000000 ));

      _timeout = Timestamp(sec, usec);

      DEBUG << get_name() << ": Got timeout of " << _timeout
        << " from parameter \"" << param("timeout", "") << "\"";
        }
    }

  // output fields
  //vector<TupleDescription::Field> output_fields;
  vector<SchemaField> output_fields;
  unsigned int field_num = 0;

  while ( true )
    {
      string  field_str = param( "out-field." + to_string( field_num ));

      if ( field_str.empty() )
        {    
      break;
        }

      ptr<Expression> field_exp = Expression::parse( field_str, ctxt );

      _just_concat = false;
      _output_expressions.push_back(field_exp);
      // Use the string as the name, which is fine if it is a field
      // and icky if it is not, but good for now.

      // allowing to name output fields
      string name = param("out-field-name." + to_string(field_num),
              PARAM_NON_EMPTY);

      output_fields.push_back(field_exp->get_schema_field(name));

      //output_fields.push_back(field_exp->getField(field_str));
      ++field_num;
    }

  if ( output_fields.empty() )
    {
      _just_concat = true;

      const vector<SchemaField> &left_fields =
    get_in_schema( LEFT )->get_schema_field();

      const vector<SchemaField> &right_fields =
    get_in_schema( RIGHT )->get_schema_field();

      copy( left_fields.begin(), left_fields.end(),
        back_inserter( output_fields ));

      copy( right_fields.begin(), right_fields.end(),
        back_inserter( output_fields ));
    }

  TupleDescription  td( output_fields.begin(), output_fields.end() );
  DEBUG << get_name() << " output description: " << td;
  set_out_description( 0, td );

  // inits for selectivity stat vectors... -- tatbul
  //
  _join_selectivity[LEFT].clear();
  _join_selectivity[RIGHT].clear();
  _join_selectivity_sum[LEFT] = 0;
  _join_selectivity_sum[RIGHT] = 0;
}


// name will be "left" or "right";
ptr<AuroraJoinQBox::JoinBuffer> AuroraJoinQBox::create_buffer_by_parameter( int  stream_id )
{
  string name = (stream_id == LEFT) ? "left" : "right";

  // yna
  //    size_t  buffer_size;
  // end yna

  // Use the most general numeric type, and cast later -- yna
  double  buffer_size;
  // "left" stream saves "down-size" while "right" stream saves "up-size"
  if (stream_id == LEFT) 
    typed_param( "down-size", buffer_size, PARAM_NON_EMPTY );
  else
    typed_param( "up-size", buffer_size, PARAM_NON_EMPTY );
    
  JoinBuffer  *ret;
  string  order_by_param = param( name + "-order-by", PARAM_NON_EMPTY );

  if ( order_by_param == "TUPLES" )
    // yna
    //    {   ret = new TupleCountJoinBuffer( stream_id, this, buffer_size );
    // end yna
    {  
      ret = new TupleCountJoinBuffer( stream_id, this,
                      static_cast<size_t>(buffer_size) );
    }
  else if ( order_by_param == "VALUES" )
    {
      ExprContext  ctxt;

      //ctxt.setTupleDescription( TupleDescription(getInDescription( stream_id )) );
      ctxt.set_context_schema( *get_in_schema( stream_id ));

      string  field_str = param( name + "-order-on-field", PARAM_NON_EMPTY );

      ptr<Expression>  field = Expression::parse( field_str, ctxt );
        
      // Use field type to fill in template argument -- yna
      if ( field->getType() == DataType::INT )
    ret = new FieldJoinBuffer<int32>( stream_id, this, field,
                      static_cast<int32>(buffer_size) );

      else if ( field->getType() == DataType::LONG )
    ret = new FieldJoinBuffer<int64>( stream_id, this, field,
                      static_cast<int64>(buffer_size) );

      else if ( field->getType() == DataType::SHORT )
    ret = new FieldJoinBuffer<int16>( stream_id, this, field,
                      static_cast<int16>(buffer_size) );

      else if ( field->getType() == DataType::BYTE )
    ret = new FieldJoinBuffer<int8>( stream_id, this, field,
                      static_cast<int8>(buffer_size) );

      else if ( field->getType() == DataType::SINGLE )
    ret = new FieldJoinBuffer<single>( stream_id, this, field,
                       static_cast<single>(buffer_size) );

      else if ( field->getType() == DataType::DOUBLE )
    ret = new FieldJoinBuffer<double>( stream_id, this, field,
                       buffer_size );

      else
    Throw(aurora_typing_exception,
          "Invalid order-on-field type, must be a numeric field.");
    }
  else
    {  
      Throw( aurora_typing_exception,
         "Join only supports order-by TUPLES or VALUES, not `" +
         order_by_param + "'" );
    }

  return( ptr<JoinBuffer>( ret ));
}


void AuroraJoinQBox::init_impl() throw( AuroraException )
{
  _left_body_size = get_in_schema( LEFT )->get_size();
  _right_body_size = get_in_schema( RIGHT )->get_size();
}


void AuroraJoinQBox::run_impl( QBoxInvocation  &inv )
  throw( AuroraException )
{
  bool outputted = false;

  run_stream( inv, LEFT, outputted );
  run_stream( inv, RIGHT, outputted );

  // Done, notify if there was output
  if ( outputted )
    {
      get_output(0).notify_enq();
    }
}


void AuroraJoinQBox::run_stream( QBoxInvocation &inv, int stream_id,
                 bool &outputted )
{
  int comparison_count, match_count; // -- tatbul

  DEBUG << "Running stream " << stream_id;
  DeqIterator di = deq(stream_id);
  int other_stream_id = (stream_id == LEFT) ? RIGHT : LEFT;

  while (inv.continue_dequeue_on(di, stream_id))
    {
      const void *data = di.tuple();
      DEBUG << get_name() << " IN(" << stream_id << "): "
        << tuple_to_string(stream_id, data);

      comparison_count = match_count = 0;

      // add the tuple to buffer
      _buffer[stream_id]->insert_tuple(data);
      // trim the other buffer
      _buffer[other_stream_id]->trim_buffer(data);
      // join with tuples from the other buffer
      _buffer[other_stream_id]->join(data, _predicate.get(), outputted,
                     &comparison_count, &match_count);

//       _buffer[other_stream_id]->join(data, _predicate.get(), outputted,
//                   &comparison_count, &match_count);
//       _buffer[stream_id]->insert_tuple(data);
      _buffer[stream_id]->back()->add_comparison_count(comparison_count);
      _buffer[stream_id]->back()->add_match_count(match_count);
      ++di;
    }

  //    DEBUG << "selectivity vectors collected:";
  //    DEBUG << "LEFT (SUM=" << _join_selectivity_sum[LEFT] << "):";
  //    for (unsigned int i = 0; i < _join_selectivity[LEFT].size(); i++)
  //    {
  //        DEBUG << _join_selectivity[LEFT][i];
  //    }
  //    DEBUG << "RIGHT (SUM=" << _join_selectivity_sum[RIGHT] << "):";;
  //    for (unsigned int i = 0; i < _join_selectivity[RIGHT].size(); i++)
  //    {
  //        DEBUG << _join_selectivity[RIGHT][i];
  //    }

  // We must avoid the selectivity vector from growing indefinitely.
  // This may happen when scheduler is not collecting the accumulated stats.
  // For now, let's simply clear the vector if it grows beyond a limit.
  // -- tatbul
  //
  if (_join_selectivity[LEFT].size() > SELECTIVITY_VECTOR_LIMIT)
    {
      _join_selectivity[LEFT].clear();
      _join_selectivity_sum[LEFT] = 0;
    }
  if (_join_selectivity[RIGHT].size() > SELECTIVITY_VECTOR_LIMIT)
    {
      _join_selectivity[RIGHT].clear();
      _join_selectivity_sum[RIGHT] = 0;
    }
}


void AuroraJoinQBox::emit_tuple(EvalContext &ctxt)
{
  produce_dont_emit_tuple(ctxt);

  // And actually emit
  EnqIterator ei = enq(0);
  ++ei;

}

void AuroraJoinQBox::produce_dont_emit_tuple(EvalContext &ctxt)
{
  EnqIterator ei = enq(0);
  const Tuple left = Tuple::factory(ctxt.getTuple(LEFT));
  const Tuple right = Tuple::factory(ctxt.getTuple(RIGHT));
  Tuple output = Tuple::factory(ei.tuple());

  // Fill-in the header
  //
  output.set_timestamp(max(left.get_timestamp(),
               right.get_timestamp()));
  output.set_local_timestamp(max(left.get_local_timestamp(),
                 right.get_local_timestamp()));
  // Revision Processor may want to fix the following three lines!
  //
  output.set_tuple_id(left.get_tuple_id());
  output.set_tuple_type(left.get_tuple_type());
  output.set_revision_id(left.get_revision_id());
  output.set_quality(max(left.get_quality(), right.get_quality()));
  output.set_win_spec(-1);
  output.set_fake_flag(false);

  // Normally, tuple_stime should be set in the same way as the order_on
  // output attribute to preserve order. In join, we have 2 order on, one for each 
  // stream. The output tuple is simply a concatenation of the joined tuples
  // so we are going to use the max value of their tuple_stime to set the output tuple_stime
  // Note: for processing boundaries, the above approach means that the output
  // boundary is the minimum of the latest input boundaries.
  output.set_tuple_stime(max(left.get_tuple_stime(),
                 right.get_tuple_stime()));

  if (_just_concat)
    {
      uint8  *pos = output.get_body();
      memcpy(pos, left.get_body(), _left_body_size);
      pos += _left_body_size;
      memcpy(pos, right.get_body(), _right_body_size);
    }
  else
    {
      Expression::eval_vector_into(_output_expressions,
                                   (char *)output.get_body(), ctxt);
    }
}


void AuroraJoinQBox::JoinBuffer::join(const void *other_tuple,
                      Expression *predicate,
                      bool &outputted,
                      int *comparison_count,
                      int *match_count)
{
  DEBUG << "Calling JoinBuffer::join on stream " << _stream_id;

  int other_stream_id = (_stream_id == LEFT) ? RIGHT : LEFT;
  _ctxt.set_tuple(other_stream_id, other_tuple);

  DEBUG << "setTuple(" << other_stream_id << ", "
    << _qbox->tuple_to_string(other_stream_id, other_tuple)
    << ")";

  BufferList::iterator i;

  DEBUG << "buffer size = " << _buffer.size();

  for (i = _buffer.begin(); i != _buffer.end(); i++)
    {
      const void *our_tuple = i->get_data();
      _ctxt.set_tuple(_stream_id, our_tuple);

      DEBUG << "setTuple(" << _stream_id << ", "
        << _qbox->tuple_to_string(_stream_id, our_tuple)
        << ")";

      _ctxt.reset();

      // increment comparison count for both tuples -- tatbul
      //
      i->incr_comparison_count();
      ++comparison_count;

      if (predicate->eval<bool>(_ctxt))
        {
      DEBUG << "Matched predicate for tuple "
        << _qbox->tuple_to_string(_stream_id, our_tuple);

      outputted = true;
      _qbox->emit_tuple(_ctxt);

      // increment match count for both tuples -- tatbul
      //
      i->incr_match_count();
      ++match_count;
        }
      else
        {
      DEBUG << "Failed to match predicate for tuple "
        << _qbox->tuple_to_string(_stream_id, our_tuple);
        }
    }
}

// Duplicates above except for the _qbox->emitTuple(_ctxt) call but I don't see a better way
void AuroraJoinQBox::JoinBuffer::join_no_emit(const void *other_tuple,
                          Expression *predicate,
                          bool &outputted)
{
  int other_stream_id = (_stream_id == LEFT) ? RIGHT : LEFT;
  _ctxt.set_tuple(other_stream_id, other_tuple);

  BufferList::const_iterator i;

  for (i = _buffer.begin(); i != _buffer.end(); ++i)
    {
      const void *our_tuple = i->get_data();
      _ctxt.set_tuple(_stream_id, our_tuple);
      _ctxt.reset();

      if (predicate->eval<bool>(_ctxt))
        {
      outputted = true;
      _qbox->produce_dont_emit_tuple(_ctxt);
        }
    }
}


void AuroraJoinQBox::JoinBuffer::insert_tuple(const void *tuple_data)
{
  DEBUG << "Running JoinBuffer::insert_tuple("
    << _qbox->tuple_to_string(_stream_id, tuple_data)
    << ")";

  BufferElement be(tuple_data, _tuple_size);

  if (_qbox->_timing_out)
    {
      // Use the timestamp off the current tuple.
      Timestamp threshold = be.get_timestamp() - _qbox->_timeout;
      BufferList::iterator i = _buffer.begin();

      while (i != _buffer.end())
        {
      if (i->get_timestamp() < threshold)
            {
          DEBUG << "Timing out tuple "
            << _qbox->tuple_to_string(_stream_id, i->get_data())
            << " its age is "
            << (be.get_timestamp() - i->get_timestamp());
          // Record tuple's join selectivity before it disappears
          // from the join buffer. -- tatbul
          //
          if (i->get_comparison_count() > 0)
                {
          (_qbox->_join_selectivity[_stream_id]).push_back(
                                   double(i->get_match_count())/
                                   double(i->get_comparison_count()));
          _qbox->_join_selectivity_sum[_stream_id] +=
            double(i->get_match_count())/
            double(i->get_comparison_count());
                }
          else
                {
          (_qbox->_join_selectivity[_stream_id]).push_back(0);
                }

          i = _buffer.erase(i);
            }
      else
            {
          DEBUG << "Found a young enough tuple, no need to test the rest";
          break;
            }
        }
    }

  // Put the tuple on the buffer.
  _buffer.push_back(be);

  DEBUG << "Putting " << _qbox->tuple_to_string(_stream_id, tuple_data)
    << " on the buffer.";
}

void AuroraJoinQBox::JoinBuffer::clear()
{
  _buffer.clear();
}


void AuroraJoinQBox::TupleCountJoinBuffer::trim_buffer(const void *tuple_data)
{
  // Check to make sure the buffer is not overfull.
  while (_buffer.size() > _buffer_size)
    {
      DEBUG << "Actual size " << _buffer.size()
        << " is greater than max size " << _buffer_size;

      DEBUG << "Pulling "
        << _qbox->tuple_to_string(_stream_id,
                      _buffer.front().get_data())
        << " off the buffer.";

      // Record tuple's join selectivity before it disappears
      // from the join buffer. -- tatbul
      //
      if (_buffer.front().get_comparison_count() > 0)
        {
      (_qbox->_join_selectivity[_stream_id]).push_back(
                               double(_buffer.front().get_match_count())/
                               double(_buffer.front().get_comparison_count()));
      _qbox->_join_selectivity_sum[_stream_id] +=
        double(_buffer.front().get_match_count())/
        double(_buffer.front().get_comparison_count());
        }
      else
        {
      (_qbox->_join_selectivity[_stream_id]).push_back(0);
        }

      _buffer.pop_front();
    }
}

template<typename T>
T AuroraJoinQBox::FieldJoinBuffer<T>::field_value(const void *tuple_data)
{
  _ctxt.reset();
  _ctxt.set_tuple( tuple_data );
  return( _field->template eval<T>( _ctxt ));
}

template<typename T>
void AuroraJoinQBox::FieldJoinBuffer<T>::trim_buffer(const void *tuple_data)
{
  if (_buffer.empty())
    {
      DEBUG << "Nothing in buffer to trim";
      return;
    }

  // Check to make sure the buffer is not overfull.
  T current_value = field_value(tuple_data);
  T oldest_value = field_value(_buffer.front().get_data());

  // keep current_value - oldest_values <= _buffer_size
  while ((current_value - oldest_value) > _buffer_size)
    {
      DEBUG << "Oldest value " << cast_into_string<T>::into_string(oldest_value)
        << " is too much older than new value "
        << cast_into_string<T>::into_string(current_value);

      DEBUG << "Pulling "
        << _qbox->tuple_to_string(_stream_id,
                      _buffer.front().get_data())
        << " off the buffer.";

      // Record tuple's join selectivity before it disappears
      // from the join buffer. -- tatbul
      //
      if (_buffer.front().get_comparison_count() > 0)
        {
      (_qbox->_join_selectivity[_stream_id]).push_back(
                               double(_buffer.front().get_match_count()) /
                               double(_buffer.front().get_comparison_count()));
      _qbox->_join_selectivity_sum[_stream_id] =
        double(_buffer.front().get_match_count()) /
        double(_buffer.front().get_comparison_count());
        }
      else
        {
      (_qbox->_join_selectivity[_stream_id]).push_back(0);
        }

      _buffer.pop_front();
      if (_buffer.empty()) 
    {
      DEBUG << "Buffer emptied";
      return;
    }
      oldest_value = field_value(_buffer.front().get_data());
    }

  DEBUG << "Oldest value " << cast_into_string<T>::into_string(oldest_value)
    << " is young enough compared to new value "
    << cast_into_string<T>::into_string(current_value);
}

// Tell compiler which template types to instantiate -- yna
template class AuroraJoinQBox::FieldJoinBuffer<int8>;
template class AuroraJoinQBox::FieldJoinBuffer<int16>;
template class AuroraJoinQBox::FieldJoinBuffer<int32>;
template class AuroraJoinQBox::FieldJoinBuffer<int64>;
template class AuroraJoinQBox::FieldJoinBuffer<single>;
template class AuroraJoinQBox::FieldJoinBuffer<double>;


void AuroraJoinQBox::set_pending_box_state(ptr<AbstractBoxState> packed_box)
{
  AuroraJoinBoxState* actual_packed_box = static_cast<AuroraJoinBoxState*>(packed_box.get());
  _pending_box_state.set(new AuroraJoinBoxState(*actual_packed_box));
}

ptr<AbstractBoxState> AuroraJoinQBox::pack_state(bool delta_only)
{

  INFO << "Packing state for AuroraJoinQBox named [" << get_name() << "]";

  int left_nb_tuples = _buffer[LEFT]->size();
  // dynbufs are allocated on the heap because constructor uses malloc
  //dynbuf left_buffer(left_nb_tuples*getInDescription(LEFT)->getSize());
  dynbuf left_buffer(left_nb_tuples*get_in_schema(LEFT)->get_size()
             + HEADER_SIZE);
  left_nb_tuples = _buffer[LEFT]->dump(left_buffer);

  int right_nb_tuples = _buffer[RIGHT]->size();
  // dynbufs are allocated on the heap because constructor uses malloc
  //dynbuf right_buffer(right_nb_tuples*getInDescription(RIGHT)->getSize());
  dynbuf right_buffer(right_nb_tuples*get_in_schema(RIGHT)->get_size()
              + HEADER_SIZE);
  right_nb_tuples = _buffer[RIGHT]->dump(right_buffer);

  INFO << "Packed " << left_nb_tuples << " for left buffer"
       << "and " << right_nb_tuples << " for right buffer";
  ptr<AuroraJoinBoxState> join_box_state(new AuroraJoinBoxState(left_buffer,left_nb_tuples,right_buffer,right_nb_tuples));

  return join_box_state;

}


void AuroraJoinQBox::unpack_state(ptr<AbstractBoxState> box_state)
{

  ptr<AuroraJoinBoxState> my_state = box_state.dynamic_cast_to<AuroraJoinBoxState>();

  const dynbuf& left_buffer = my_state->get_left();
  int nb_left_tuples = my_state->get_nb_left_tuples();

  DEBUG << "Will try to unpack " << nb_left_tuples << " on left stream";
  _buffer[LEFT]->init(left_buffer,nb_left_tuples);

  const dynbuf& right_buffer = my_state->get_right();
  int nb_right_tuples = my_state->get_nb_right_tuples();

  DEBUG << "Will try to unpack " << nb_right_tuples << " on right stream";
  _buffer[RIGHT]->init(right_buffer,nb_right_tuples);

}

int AuroraJoinQBox::JoinBuffer::get_oldest()
{
  int next_tuple_offset = 0;
  int lowest_id = -1;
  int nb_tuples = 0;

  // Making the cost be equivalent to a linear scan of the buffer
  for ( BufferList::iterator i = _buffer.begin(); i != _buffer.end(); ++i)
    {
      // Dump as many tuples as possible without exceeding the size of the buffer
      next_tuple_offset += _tuple_size;
      int current_id =  Tuple::factory(i->get_data()).get_tuple_id();
      if ( (current_id < lowest_id) || (lowest_id == -1))
        {
      lowest_id = current_id;
        }
      nb_tuples++;
    }
  if ( nb_tuples == 0)
    { 
      return -1;
    }
  return Tuple::factory(_buffer.begin()->get_data()).get_tuple_id();
}

int AuroraJoinQBox::JoinBuffer::dump(dynbuf& buf)
{

  int next_tuple_offset = 0;
  int nb_tuples = 0;
  for ( BufferList::iterator i = _buffer.begin(); i != _buffer.end(); ++i)
    {
      // Dump as many tuples as possible without exceeding the size of the buffer
      if ( ( next_tuple_offset + _tuple_size) <= buf.max_length() )
        {
      memcpy(buf.data()+next_tuple_offset,i->get_data(),_tuple_size);
      next_tuple_offset += _tuple_size;
      nb_tuples++;
        }
    }
  return nb_tuples;
}

void AuroraJoinQBox::JoinBuffer::init(const dynbuf& buf, int nb_tuples)
{
  clear();
  int next_tuple_offset = 0;
  for (int i = 0; i < nb_tuples; i++)
    {
      insert_tuple(((dynbuf&)buf).data()+next_tuple_offset);
      next_tuple_offset += _tuple_size;
    }
}

NMSTL_SERIAL_DEFINE(AuroraJoinBoxState, 2000);

BOREALIS_NAMESPACE_END
