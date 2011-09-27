#include "JoinFitsQBox.h"
#include "DataPath.h"
#include <cmath>

BOREALIS_NAMESPACE_BEGIN


const int JoinFitsQBox::LEFT = 0;
const int JoinFitsQBox::RIGHT = 1;



void JoinFitsQBox::setup_impl() throw( AuroraException )
{
    if ( get_num_inputs() != 2 )
    {   Throw( aurora_typing_exception,
               "JoinFits requires exactly 2 inputs (not " +
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

        }
    }

    // output fields
    //vector<TupleDescription::Field> output_fields;
    vector<SchemaField> output_fields;
    

        const vector<SchemaField> &left_fields =
            get_in_schema( LEFT )->get_schema_field();

    typed_param("first_cascade", _first_pass);
    typed_param("motion_vector", _motion_vector_flag);
    typed_param("overlap", _overlap);
    typed_param("null_suppress", _ns_flag);
    typed_param("max_x", _max_x);
    typed_param("max_y", _max_y);
    typed_param("run_length_encode", _rle_flag);
    _bytes_per_idx = 1;
    
    if(_first_pass)
      {
        _concatenate_length = 2;
        _lhs_cell_length = _rhs_cell_length = 1;
        if(_ns_flag)
          {
        _lhs_null = new uint8[1];
        _lhs_null[0] = 0;

        _rhs_null = new uint8[1];
        _rhs_null[0] = 0;
          }

      }
    else
      {
        _concatenate_length = 3;
        _lhs_cell_length = 2;
        _rhs_cell_length = 1;
        if(_ns_flag)
          {
        _lhs_null = new uint8[2];
        _lhs_null[0] = 0;
        _lhs_null[1] = 0;

        _rhs_null = new uint8[1];
        _rhs_null[0] = 0;
          }

      }
    _concatenated_cell = new uint8[_concatenate_length];

    if(_motion_vector_flag)
      {
        
        DEBUG << "Applying motion vector processing!";
        _rows_motion_vector = _max_y - _overlap;
        _mv_length = _overlap*_max_x*3;
        _mv_cache = new uint8[_mv_length];
      }
    if(_rle_flag)
      {
        _join_result = new uint8[_max_x*_max_y*10];  // TODO: how do I dealloc this?
        _join_result_pos = _join_result;
        _join_result_length = 0;
        
        _lhs_value = new uint8[_lhs_cell_length];
        _rhs_value = new uint8[_rhs_cell_length];
         
      }
        copy( left_fields.begin(), left_fields.end(),
              back_inserter( output_fields ));
    if(_first_pass && !_ns_flag && !_rle_flag)
      {
           // add a dimension
            
            
            SchemaField out_array = output_fields[0];  // hack, should not do this - JMR
            out_array.set_name("tri_frame");
                        SchemaArray *img_info = new SchemaArray(DataType::BYTE, 1, NULL);
            // TODO: make it copy the first two dimensions from its predecessor
            img_info->add_dimension(0, 1023);
              img_info->add_dimension(0, 744);  
              /*img_info->add_dimension(0, 511);
            img_info->add_dimension(0, 371);  */
             img_info->add_dimension(0, 2);
            out_array.set_array(img_info);
            output_fields[0] = out_array;

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
    


    _overlap_jump_g = _overlap * _max_x;
    _overlap_jump_c = _overlap * _max_x*3; 
    CatalogSchema lhs_schema = *get_in_schema(0);
    CatalogSchema rhs_schema = *get_in_schema(1);
    if(_ns_flag)
      {
        _ns_cache = new uint8[5600000];  // estimate on the basis of file size

        _max_run_size = 1000000;
        _ns_current_run = new uint8[_max_run_size];

        
      }

    if(_first_pass || _ns_flag || _rle_flag)
          {
        
        const SchemaField *l_array = lhs_schema.get_schema_field("frame");
        _array_offset = l_array->get_offset();
          }
        else 
          {
        const SchemaField *l_array = lhs_schema.get_schema_field("tri_frame");
        _array_offset = l_array->get_offset();
        
          }    
    


    const SchemaField *l_seq_no = lhs_schema.get_schema_field("seq_no");
    _seq_no_offset = l_seq_no->get_offset();

    CatalogSchema out_schema = *get_out_schema(0);
    DEBUG << "schema out: " << out_schema.as_string();
    // TODO: fix me to accomodate the field name "tri_frame"
    SchemaField *out_field;
    out_field = const_cast<SchemaField *>(out_schema.get_schema_field("frame"));
    // try, try again
    if(!out_field)
      out_field = const_cast<SchemaField *>(out_schema.get_schema_field("tri_frame"));

    if(out_field == 0)
        FATAL << "Could not find out field!";
    _out_array_offset = out_field->get_offset();
    
    
    TupleDescription out_td(get_out_schema(0));
    _output_tuple_size = out_td.get_size();
    
    
}


// name will be "left" or "right";
ptr<JoinFitsQBox::JoinFitsBuffer> JoinFitsQBox::create_buffer_by_parameter( int  stream_id )
{
    string name;

    if ( stream_id == LEFT )
    {   name = "left";
    }
    else if ( stream_id == RIGHT )
    {   name = "right";
    }

// yna
//    size_t  buffer_size;
// end yna

    // Use the most general numeric type, and cast later -- yna
    double  buffer_size;
    typed_param( name + "-buffer-size", buffer_size, PARAM_NON_EMPTY );

    JoinFitsBuffer  *ret;
    string  order_by_param = param( name + "-order-by", PARAM_NON_EMPTY );

    if ( order_by_param == "TUPLES" )
// yna
//    {   ret = new TupleCountJoinFitsBuffer( stream_id, this, buffer_size );
// end yna
    {   ret = new TupleCountJoinFitsBuffer( stream_id, this,
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
            ret = new FieldJoinFitsBuffer<int32>( stream_id, this, field,
                static_cast<int32>(buffer_size) );

        else if ( field->getType() == DataType::LONG )
            ret = new FieldJoinFitsBuffer<int64>( stream_id, this, field,
                static_cast<int64>(buffer_size) );

        else if ( field->getType() == DataType::SHORT )
            ret = new FieldJoinFitsBuffer<int16>( stream_id, this, field,
                static_cast<int16>(buffer_size) );

        else if ( field->getType() == DataType::BYTE )
            ret = new FieldJoinFitsBuffer<int8>( stream_id, this, field,
                static_cast<int8>(buffer_size) );

        else if ( field->getType() == DataType::SINGLE )
            ret = new FieldJoinFitsBuffer<single>( stream_id, this, field,
                static_cast<single>(buffer_size) );

        else if ( field->getType() == DataType::DOUBLE )
            ret = new FieldJoinFitsBuffer<double>( stream_id, this, field,
                buffer_size );

        else
            Throw(aurora_typing_exception,
                "Invalid order-on-field type, must be a numeric field.");
    }
    else
    {   Throw( aurora_typing_exception,
               "JoinFits only supports order-by TUPLES or VALUES, not `" +
               order_by_param + "'" );
    }

    return( ptr<JoinFitsBuffer>( ret ));
}


void JoinFitsQBox::init_impl() throw( AuroraException )
{
    _left_body_size = get_in_schema( LEFT )->get_size();
    _right_body_size = get_in_schema( RIGHT )->get_size();
}


void JoinFitsQBox::run_impl( QBoxInvocation  &inv )
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


void JoinFitsQBox::run_stream( QBoxInvocation &inv, int stream_id,
                           bool &outputted )
{
    int comparison_count, match_count; // -- tatbul

    DeqIterator di = deq(stream_id);
    int other_stream_id = (stream_id == LEFT) ? RIGHT : LEFT;

    while (inv.continue_dequeue_on(di, stream_id))
    {
        const void *data = di.tuple();

        comparison_count = match_count = 0;
        _buffer[other_stream_id]->join(data, _predicate.get(), outputted,
                                       &comparison_count, &match_count);
        _buffer[stream_id]->insert_tuple(data);
        _buffer[stream_id]->back()->add_comparison_count(comparison_count);
        _buffer[stream_id]->back()->add_match_count(match_count);
        ++di;
    }

//    DEBUG << "selectivity vectors collected:";
//    DEBUG << "LEFT (SUM=" << _join_selectivity_sum[LEFT] << "):";
//    for (uint32 i = 0; i < _join_selectivity[LEFT].size(); i++)
//    {
//        DEBUG << _join_selectivity[LEFT][i];
//    }
//    DEBUG << "RIGHT (SUM=" << _join_selectivity_sum[RIGHT] << "):";;
//    for (uint32 i = 0; i < _join_selectivity[RIGHT].size(); i++)
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


void JoinFitsQBox::emit_tuple(EvalContext &ctxt)
{
    produce_dont_emit_tuple(ctxt);

    // And actually emit
    EnqIterator ei = enq(0);
    ++ei;

}

void JoinFitsQBox::produce_dont_emit_tuple(EvalContext &ctxt)
{
  Time start_time, finish_time;
    EnqIterator ei = enq(0);
    const Tuple left = Tuple::factory(ctxt.getTuple(LEFT));
    const Tuple right = Tuple::factory(ctxt.getTuple(RIGHT));

    start_time = Time::now();
    const void *left_tuple = left.get_data();
    const void *right_tuple = right.get_data();
    // JMR stuff here
    DynamicArray *lhs = (DynamicArray *) (*(uint32 *) ((((uint8 *)left_tuple) + _array_offset)));
    DynamicArray *rhs = (DynamicArray *) (*(uint32 *) ((((uint8 *)right_tuple) + _array_offset)));
    uint32 lhs_length = lhs->variable_size();
    uint32 rhs_length = rhs->variable_size();
    uint8 *lhs_vals = ((uint8 *) lhs) + DynamicArray::header_size();
    uint8 *rhs_vals = ((uint8 *) rhs) + DynamicArray::header_size();
    uint32 seq_no = *(uint32 *) ((((uint8 *)right_tuple) + _seq_no_offset));
    DEBUG << "Got seq no: " << seq_no;
    // first pass
    // lhs = i, getting mapped to r
    // rhs = r, getting mapped to g
    
    // second pass
    // lhs = full output, ir filled
    // rhs = g, mapped to b
    DynamicArray *output_array;
    if(_ns_flag)
      {
       fill_channel_ns(lhs_vals, rhs_vals, lhs_length, rhs_length);
    
    output_array = DynamicArray::allocate_array(_out_ns_length);
    output_array->set_count_size(1, _out_ns_length);
    
    uint8 *cache_values = ((uint8 *) _ns_cache) + DynamicArray::header_size();
    memcpy(cache_values, _ns_cache, _out_ns_length);
      }
    else if(_rle_flag)
      {

        join_rle(lhs_vals, rhs_vals, lhs_length, rhs_length);
        lhs->modify_array_reference(-1);
        rhs->modify_array_reference(-1);
        DEBUG << "Allocating RLE of length: " << _join_result_length;
        output_array = DynamicArray::allocate_array(_join_result_length);
        output_array->set_count_size(1, _join_result_length);
        uint8 *output_array_values = ((uint8 *) output_array) + DynamicArray::header_size();
        memcpy(output_array_values, _join_result, _join_result_length);
       }
    else
      {
    if(_first_pass)
    {
        output_array = first_join(lhs, rhs, seq_no);
        lhs->modify_array_reference(-1);
        rhs->modify_array_reference(-1);
        // ship it from here
    }
    else 
    {
        second_join(lhs, rhs, seq_no);
        rhs->modify_array_reference(-1);
        // output LHS
        output_array = lhs; 
    }
    }
    uint8 *output_values = ((uint8 *) output_array) + DynamicArray::header_size();
        DEBUG << "First output vals: " << (int) output_values[0] << " " << (int) output_values[1] << " "  << (int) output_values[2]; 
        uint8  *pos = (uint8 *) enq(0).tuple();
        memcpy(enq(0).tuple(), left.get_data(), _output_tuple_size);
        uint32 *array_pos = (uint32 *) (pos + _out_array_offset);
        *array_pos = (uint32) output_array;
    finish_time = Time::now();
    if(seq_no == 0)
      _total_time = finish_time - start_time;
    else
      _total_time = _total_time + finish_time - start_time;
    if(seq_no == 40)
      NOTICE << "Total time taken: " << _total_time;
    }


void JoinFitsQBox::JoinFitsBuffer::join(const void *other_tuple,
                                Expression *predicate,
                                bool &outputted,
                                int *comparison_count,
                                int *match_count)
{

    int other_stream_id = (_stream_id == LEFT) ? RIGHT : LEFT;
    _ctxt.set_tuple(other_stream_id, other_tuple);

 
    BufferList::iterator i;

 
    for (i = _buffer.begin(); i != _buffer.end(); i++)
    {
        const void *our_tuple = i->get_data();
        _ctxt.set_tuple(_stream_id, our_tuple);

 
        _ctxt.reset();

        // increment comparison count for both tuples -- tatbul
        //
        i->incr_comparison_count();
        ++comparison_count;

        
        if (predicate->eval<bool>(_ctxt))
        {
 
            outputted = true;
            _qbox->emit_tuple(_ctxt);

            // increment match count for both tuples -- tatbul
            //
            i->incr_match_count();
            ++match_count;
        }
        /*else
        {
            DEBUG << "Failed to match predicate for tuple "
                  << _qbox->tuple_to_string(_stream_id, our_tuple);
        }*/
    }
}

// Duplicates above except for the _qbox->emitTuple(_ctxt) call but I don't see a better way
void JoinFitsQBox::JoinFitsBuffer::join_no_emit(const void *other_tuple,
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


void JoinFitsQBox::JoinFitsBuffer::insert_tuple(const void *tuple_data)
{
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
                // Record tuple's JoinFits selectivity before it disappears
                // from the JoinFits buffer. -- tatbul
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
                break;
            }
        }
    }

    // Put the tuple on the buffer.
    _buffer.push_back(be);

}

void JoinFitsQBox::JoinFitsBuffer::clear()
{
    _buffer.clear();
}


void JoinFitsQBox::TupleCountJoinFitsBuffer::insert_tuple(const void *tuple_data)
{
    // Insert the tuple.
    JoinFitsQBox::JoinFitsBuffer::insert_tuple(tuple_data);

    // Check to make sure the buffer is not overfull.
    while (_buffer.size() > _buffer_size)
    {
        /*DEBUG << "Actual size " << _buffer.size()
              << " is greater than max size " << _buffer_size;

        DEBUG << "Pulling "
              << _qbox->tuple_to_string(_stream_id,
                                        _buffer.front().get_data())
              << " off the buffer.";
*/
        // Record tuple's JoinFits selectivity before it disappears
        // from the JoinFits buffer. -- tatbul
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
T JoinFitsQBox::FieldJoinFitsBuffer<T>::field_value(const void *tuple_data)
{
    _ctxt.reset();
    _ctxt.set_tuple( tuple_data );
    return( _field->template eval<T>( _ctxt ));
}

template<typename T>
void JoinFitsQBox::FieldJoinFitsBuffer<T>::insert_tuple(const void *tuple_data)
{
    // Insert the tuple.
    JoinFitsQBox::JoinFitsBuffer::insert_tuple(tuple_data);

    // Check to make sure the buffer is not overfull.
    T new_value = field_value(_buffer.back().get_data());
    T oldest_value = field_value(_buffer.front().get_data());

    while ((new_value - oldest_value) >= _buffer_size)
    {
        DEBUG << "Oldest value " << cast_into_string<T>::into_string(oldest_value)
              << " is too much older than new value " << cast_into_string<T>::into_string(new_value);

        DEBUG << "Pulling "
              << _qbox->tuple_to_string(_stream_id,
                                        _buffer.front().get_data())
              << " off the buffer.";

        // Record tuple's JoinFits selectivity before it disappears
        // from the JoinFits buffer. -- tatbul
        //
        if (_buffer.front().get_comparison_count() > 0)
        {
            (_qbox->_join_selectivity[_stream_id]).push_back(
                                double(_buffer.front().get_match_count())/
                                double(_buffer.front().get_comparison_count()));
            _qbox->_join_selectivity_sum[_stream_id] =
                double(_buffer.front().get_match_count())/
                double(_buffer.front().get_comparison_count());
        }
        else
        {
            (_qbox->_join_selectivity[_stream_id]).push_back(0);
        }

        _buffer.pop_front();
        oldest_value = field_value(_buffer.front().get_data());
    }

    DEBUG << "Oldest value " << cast_into_string<T>::into_string(oldest_value)
          << " is young enough compared to new value "
          << cast_into_string<T>::into_string(new_value);
}

// Tell compiler which template types to instantiate -- yna
template class JoinFitsQBox::FieldJoinFitsBuffer<int8>;
template class JoinFitsQBox::FieldJoinFitsBuffer<int16>;
template class JoinFitsQBox::FieldJoinFitsBuffer<int32>;
template class JoinFitsQBox::FieldJoinFitsBuffer<int64>;
template class JoinFitsQBox::FieldJoinFitsBuffer<single>;
template class JoinFitsQBox::FieldJoinFitsBuffer<double>;


void JoinFitsQBox::set_pending_box_state(ptr<AbstractBoxState> packed_box)
{
    JoinFitsBoxState* actual_packed_box = static_cast<JoinFitsBoxState*>(packed_box.get());
    _pending_box_state.set(new JoinFitsBoxState(*actual_packed_box));
}

ptr<AbstractBoxState> JoinFitsQBox::pack_state(bool delta_only)
{

    INFO << "Packing state for JoinFitsQBox named [" << get_name() << "]";

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
    ptr<JoinFitsBoxState> JoinFits_box_state(new JoinFitsBoxState(left_buffer,left_nb_tuples,right_buffer,right_nb_tuples));

    return JoinFits_box_state;

}


void JoinFitsQBox::unpack_state(ptr<AbstractBoxState> box_state)
{

    ptr<JoinFitsBoxState> my_state = box_state.dynamic_cast_to<JoinFitsBoxState>();

    const dynbuf& left_buffer = my_state->get_left();
    int nb_left_tuples = my_state->get_nb_left_tuples();

    _buffer[LEFT]->init(left_buffer,nb_left_tuples);

    const dynbuf& right_buffer = my_state->get_right();
    int nb_right_tuples = my_state->get_nb_right_tuples();

    _buffer[RIGHT]->init(right_buffer,nb_right_tuples);

}

int JoinFitsQBox::JoinFitsBuffer::get_oldest()
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
    { return -1;
    }
    return Tuple::factory(_buffer.begin()->get_data()).get_tuple_id();
}

int JoinFitsQBox::JoinFitsBuffer::dump(dynbuf& buf)
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

void JoinFitsQBox::JoinFitsBuffer::init(const dynbuf& buf, int nb_tuples)
{
    clear();
    int next_tuple_offset = 0;
    for (int i = 0; i < nb_tuples; i++)
    {
        insert_tuple(((dynbuf&)buf).data()+next_tuple_offset);
        next_tuple_offset += _tuple_size;
    }
}
// TODO: Make sure _i, _g, _r are initialized before calling this
// meant to be called from setup_impl


DynamicArray *JoinFitsQBox::first_join(DynamicArray *lhs, DynamicArray *rhs, uint32 seq_no)
{
    DynamicArray *output;
    DEBUG << "Execing first join";
    uint32 bytes_requested = _max_x*_max_y*_bytes_per_idx*3;
    DEBUG << "Asked for " << bytes_requested << " bytes. max_x=" << _max_x << " max_y=" << _max_y << " bpi=" << _bytes_per_idx;
    // should be intersection size, but the schema is not flexible enough for this, so we are defaulting to allocating for complete overlap.
    
    output = DynamicArray::allocate_array(bytes_requested);
    // TODO: make this dynamic
    output->set_count_size(1, 1);

    fill_channel(lhs, output, 0, 0, 3*_bytes_per_idx, seq_no);
    
    fill_channel(rhs, output, 0, _bytes_per_idx, 3*_bytes_per_idx, seq_no);
    DEBUG << "Done first join!";
    return output;
}

void JoinFitsQBox::second_join(DynamicArray *output, DynamicArray *rhs, uint32 seq_no)
{
    DEBUG << "Execing second join!";
    
        fill_channel(rhs, output, 0, 2*_bytes_per_idx, 3*_bytes_per_idx, seq_no);
    DEBUG << "Done second join!";
}
    
int32 JoinFitsQBox::get_first_idx(coordinate a_coord)
{
    
    int num_rows = _intersect.min_y - a_coord.y;
    int num_cols = _intersect.min_x - a_coord.x;
    return (num_rows * _max_x + num_cols)*_bytes_per_idx;   
}

/*****************************************Null Suppression******************************************/
void JoinFitsQBox::close_out_current_run_ns(uint32 cell_length)
{
  _current_run_size -= _concatenate_length;
  uint32 run_length_in_idxs = _current_run_size / _concatenate_length ;
  memcpy(_ns_cache_pos, (uint8 *) &_run_start_idx, 4);
  _ns_cache_pos += 4;
  memcpy(_ns_cache_pos, (uint8 *) &run_length_in_idxs, 4);
  _ns_cache_pos += 4;
  memcpy(_ns_cache_pos, _ns_current_run, _current_run_size);
  //DEBUG<< "OUTPUT run starting at " << _run_start_idx << " of length " << _current_run_size << " idx size " << run_length_in_idxs ; 
  _ns_cache_pos += _current_run_size;
  _out_ns_length += _current_run_size + 8;
  _in_run = false;
}

void JoinFitsQBox::add_to_cache(uint8 *value, uint32 idx, uint8 concat_cell_length)
{
  //DEBUG << "Adding idx " << idx << " to cache." ;

  if(_current_run_size >= _max_run_size)
  {
    FATAL << "Create a larger max run size! cr=" << _current_run_size << " max=" << _max_run_size ;
  }
  if(_in_run)
    {

      if(idx == _last_idx+1)
    {
      //continuing run
      memcpy(_run_pos, value, concat_cell_length);
      _run_pos += concat_cell_length;
      _current_run_size += concat_cell_length;
      ++_last_idx;
      //DEBUG<< "Contiguous run size = " << _current_run_size ;

    }
      else
    {
      //close out the noncontiguous run and write it to the main cache
      
      close_out_current_run_ns(concat_cell_length);
        
         // start a new one
         _in_run = true;
        _last_idx = idx;
     _run_start_idx = idx;
        _run_pos = _ns_current_run;
        memcpy(_run_pos, value, concat_cell_length);
        _run_pos += concat_cell_length;
        _current_run_size = concat_cell_length;
        //DEBUG<< "Noncontiguous: set run size to " << _current_run_size ;
    }
    }
  else
    {
      // start a run
      _last_idx = idx;
      _run_start_idx = idx;
      //DEBUG<< "starting a run at idx: " << _run_start_idx ; 
      _run_pos = _ns_current_run;
      memcpy(_run_pos, value, concat_cell_length);
      _run_pos += concat_cell_length;
      _current_run_size = concat_cell_length;
      _in_run = true;
      //DEBUG << "Starting new run, run size = " << _current_run_size ;
    }

}

void JoinFitsQBox::setup_read_run_lhs()
{
  uint32 first_idx, run_length;
  memcpy((uint8 *) &first_idx, _lhs_pos, 4);
  _lhs_pos += 4;
  memcpy((uint8 *) &run_length, _lhs_pos, 4);
  _lhs_pos += 4;
  _lhs_last_run_idx = first_idx + run_length; // -1 ?
  _lhs_current_idx = first_idx;
  //DEBUG << "LHS run at: " << first_idx << " of length " << run_length ;
  
}

void JoinFitsQBox::setup_read_run_rhs()
{
  
  //DEBUG<< "Hex of next vals: "  << to_hex_string(_lhs_pos, 10);
  uint32 first_idx, run_length;
  memcpy((uint8 *) &first_idx, _rhs_pos, 4);
  _rhs_pos += 4;
  memcpy((uint8 *) &run_length, _rhs_pos, 4);
  _rhs_pos += 4;
  _rhs_current_idx = first_idx;
  _rhs_last_run_idx = first_idx + run_length;
  //DEBUG<< "RHS run that starts at " << first_idx << " with a run length of " << run_length ;
}


void JoinFitsQBox::advance_lhs( uint32 cell_size)
{
    //DEBUG<< "Advancing lhs! current idx " << _lhs_current_idx << " last idx " << _lhs_last_run_idx;
  if(_lhs_last_run_idx == _lhs_current_idx)
    {
      setup_read_run_lhs();
      
    }
  else
  {
    _lhs_pos += cell_size;
    ++_lhs_current_idx;
  }
}

void JoinFitsQBox::advance_rhs( uint32 cell_size)
{
  //DEBUG<< "Advancing rhs! current idx " << _rhs_current_idx << " last idx " << _rhs_last_run_idx;
  if(_rhs_last_run_idx ==  _rhs_current_idx)
   {
     setup_read_run_rhs();
   }
  else
  {
    _rhs_pos += cell_size;
    ++_rhs_current_idx;
  }
}

void JoinFitsQBox::print_tuple(uint8 *data, uint32 tuple_length)
{
  if(tuple_length == 2)
    DEBUG<< "         Values: " << (int) data[0] << " " << (int) data[1] ;
  else
    DEBUG<< "         Values: " << (int) data[0] << " " << (int) data[1] << " " << (int) data[2] ;
}

void JoinFitsQBox::join_lhs()
{
  memcpy(_concatenated_cell, _lhs_pos, _lhs_cell_length);
  memcpy(_concatenated_cell + _lhs_cell_length, _rhs_null, _rhs_cell_length);
  add_to_cache(_concatenated_cell, _lhs_current_idx, _concatenate_length);
  advance_lhs(_lhs_cell_length);
}
void JoinFitsQBox::join_rhs()
{
  memcpy(_concatenated_cell, _lhs_null, _lhs_cell_length);
  memcpy(_concatenated_cell + _lhs_cell_length, _rhs_pos, _rhs_cell_length);
  add_to_cache(_concatenated_cell, _rhs_current_idx, _concatenate_length);
  advance_rhs(_rhs_cell_length);
}

void JoinFitsQBox::join_both()
{
  memcpy(_concatenated_cell, _lhs_pos, _lhs_cell_length);
  memcpy(_concatenated_cell + _lhs_cell_length, _rhs_pos, _rhs_cell_length);
  add_to_cache(_concatenated_cell, _rhs_current_idx, _concatenate_length);
  advance_rhs(_rhs_cell_length);
  advance_lhs(_lhs_cell_length);
}
void JoinFitsQBox::fill_channel_ns(uint8 *lhs, uint8 *rhs, uint32 lhs_length, uint32 rhs_length)
{
  uint8 *lhs_end, *rhs_end;
    
  _in_run = false;
  _out_ns_length = 0;
  _current_run_size = 0;
  _ns_cache_pos =  _ns_cache;
  _run_pos = _ns_current_run;
  
  _lhs_pos = lhs;
  _rhs_pos = rhs;

  //DEBUG<< "lhs var size: " << lhs->variable_size() << " rhs var size: " << rhs->variable_size();
  lhs_end = _lhs_pos + lhs_length;
  rhs_end = _rhs_pos + rhs_length;


  
  setup_read_run_lhs();
  setup_read_run_rhs();
  
  // manually put state into the output coordinate system
  //DEBUG << "LHS current index: " << _lhs_current_idx << " RHS: " << _rhs_current_idx ;
  //iterate through until we are at the end of both sets
  while(_lhs_pos < lhs_end || _rhs_pos < rhs_end)
    {
      //DEBUG << "Current lhs pos: " << (int) _lhs_pos << " lhs end " << (int) lhs_end 
      //<< " current rhs pos: " << (int) _rhs_pos << " rhs end " << (int) rhs_end ;
    //DEBUG << "LHS current index: " << _lhs_current_idx << " RHS: " << _rhs_current_idx ;
      if(_lhs_pos >= lhs_end)
    {
      //DEBUG << "lhs done, joining rhs" ;
      join_rhs();
    }
      else if (_rhs_pos >= rhs_end)
    {
      //DEBUG << "rhs done, joining lhs" ;
      join_lhs();
    }
      else if(_lhs_current_idx < _rhs_current_idx)
    {
      //DEBUG << "lhs less than rhs, joining lhs" ;
      join_lhs();
    }
      else if(_lhs_current_idx > _rhs_current_idx)
    {
      //DEBUG << "rhs less than lhs, joining rhs" ;
    join_rhs();
    }
      else
    {
      //DEBUG<< "Both at the same point! joining together." ;
      join_both();
    }
      
    }
  ////DEBUG<< "Done with frame!" ;
  // finally make sure we have the last run
  close_out_current_run_ns(_concatenate_length);
}

/*****************************************End Null Suppression******************************************/
/*****************************************RLE Encoding*************************************************/
void JoinFitsQBox::load_next_lhs_rle()
{
  uint32 start_idx;
  uint32 run_length;

  memcpy(&start_idx, _lhs_pos, 4);
  _lhs_pos += 4;
  memcpy(&run_length, _lhs_pos, 4);
  _lhs_pos += 4;
  _lhs_start_idx = start_idx;
  _lhs_end_idx = start_idx + run_length-1;
  memcpy(_lhs_value, _lhs_pos, _lhs_cell_length);
  _lhs_pos += _lhs_cell_length;
  //if(start_idx < 10000)
    //DEBUG << "LHS: start idx: " << start_idx << " rl: " << run_length << " value: " << (int) _lhs_value[0] << " end idx: " << _lhs_end_idx ;
}

void JoinFitsQBox::load_next_rhs_rle()
{
  uint32 start_idx;
  uint32 run_length;

  memcpy(&start_idx, _rhs_pos, 4);
  _rhs_pos += 4;
  memcpy(&run_length, _rhs_pos, 4);
  _rhs_pos += 4;
  _rhs_start_idx = start_idx;
  _rhs_end_idx = start_idx + run_length-1;
  memcpy(_rhs_value, _rhs_pos, _rhs_cell_length);
  _rhs_pos += _rhs_cell_length;
  //if(start_idx < 10000)
    //DEBUG << "RHS: start idx: " << start_idx << " rl: " << run_length << " value: " << (int) _rhs_value[0] << " end idx: " << _rhs_end_idx ;
}

void JoinFitsQBox::add_run_rle(uint32 start_idx, uint32 run_length, uint8 *value)
{
  
  if(start_idx < 10000)
    //DEBUG << "OUTPUT: start idx: " << start_idx << " rl: " << run_length ;
 
  
  memcpy(_join_result_pos, (uint8 *) &start_idx, 4);
  _join_result_pos += 4;
  memcpy(_join_result_pos, (uint8 *) &run_length, 4);
  _join_result_pos += 4;
  memcpy(_join_result_pos, value, _concatenate_length);
  _join_result_pos += _concatenate_length;
  _join_result_length = _join_result_length + 8 + _concatenate_length;
  
}

void JoinFitsQBox::join_rle(uint8 *lhs, uint8 *rhs, uint32 lhs_size, uint32 rhs_size)
{

  uint8 *lhs_end, *rhs_end;
  uint32 run_length;

  _lhs_pos = lhs;
  _rhs_pos = rhs;
 
  
  
  lhs_end = _lhs_pos + lhs_size + 1;
  rhs_end = _rhs_pos + rhs_size + 1;
  _current_idx = 0;
  
  load_next_lhs_rle();
  load_next_rhs_rle();

  _join_result_pos = _join_result;
  _join_result_length = 0;

  //DEBUG << "Reinitialized it all!";
  while(_lhs_pos < lhs_end || _rhs_pos < rhs_end)
    {
      if(_lhs_end_idx < _rhs_end_idx)
    {
      memcpy(_concatenated_cell, _lhs_value, _lhs_cell_length);
      memcpy(_concatenated_cell+_lhs_cell_length, _rhs_value, _rhs_cell_length);
      run_length = _lhs_end_idx -  _current_idx+2; // for '0' idx
      add_run_rle(_current_idx, run_length, _concatenated_cell);
      
      _current_idx = _lhs_end_idx + 1;
      load_next_lhs_rle();
    }
      else if(_lhs_end_idx >  _rhs_end_idx)
    {
      memcpy(_concatenated_cell, _lhs_value, _lhs_cell_length);
      memcpy(_concatenated_cell+_lhs_cell_length, _rhs_value, _rhs_cell_length);
      run_length = _rhs_end_idx - _current_idx;
      add_run_rle(_current_idx, run_length, _concatenated_cell);
      
      _current_idx = _rhs_end_idx + 1;
      load_next_rhs_rle();
    }
      else 
    {
      // ending equally
          memcpy(_concatenated_cell, _lhs_value, _lhs_cell_length);
          memcpy(_concatenated_cell+_lhs_cell_length, _rhs_value, _rhs_cell_length);
          run_length = _lhs_end_idx - _current_idx;
          add_run_rle(_current_idx, run_length, _concatenated_cell);
      
          _current_idx = _lhs_end_idx + 1;
          load_next_lhs_rle();
          load_next_rhs_rle();
    }
    }
  //DEBUG << "Done joining RLE!";
}

/*************************************end RLE Encoding*************************************************/
void JoinFitsQBox::fill_channel(DynamicArray *channel, DynamicArray *output, uint32 channel_offset, uint32 output_offset, uint32 output_jump, uint32 seq_no)
{
        
    if(_motion_vector_flag)
    {
      if(seq_no == 0)
        {
          uint8 *output_pos = ((uint8 *) output) + DynamicArray::header_size() + output_offset;
        uint8 *channel_pos = ((uint8 *) channel) + DynamicArray::header_size() + channel_offset;
        
        //DEBUG << "Channel fill: channel offset: " << channel_offset << " output offset " << output_offset;
        for(uint32 y = 0; y < _max_y; ++y)
          {
            for(uint32 x = 0; x < _max_x; ++x)
                {
                    memcpy(output_pos, channel_pos, _bytes_per_idx);
                
                    output_pos +=   output_jump;
                    channel_pos += _bytes_per_idx;
                }
            
          }
        // copy last n pixels into cache
        memcpy(_mv_cache, channel_pos - _mv_length, _mv_length);
        }
      else
        {
          uint8 *output_pos = ((uint8 *) output) + DynamicArray::header_size() + output_offset + _overlap_jump_c;
          uint8 *channel_pos = ((uint8 *) channel) + DynamicArray::header_size() + channel_offset + _overlap_jump_g;
          uint8 *channel_vals = ((uint8 *) channel) + DynamicArray::header_size();
          memcpy(channel_vals, _mv_cache, _mv_length);
        for(uint32 y = 0; y < _rows_motion_vector; ++y)
                 {
             for(uint32 x = 0; x < _max_x; ++x)
                {
                    memcpy(output_pos, channel_pos, _bytes_per_idx);
                
                    output_pos +=   output_jump;
                    channel_pos += _bytes_per_idx;
                }
         }
        // copy last n pixels into cache
        memcpy(_mv_cache, channel_pos - _mv_length, _mv_length);
        }

    }
    else
    {
        uint8 *output_pos = ((uint8 *) output) + DynamicArray::header_size() + output_offset;
        uint8 *channel_pos = ((uint8 *) channel) + DynamicArray::header_size() + channel_offset;
        
        //DEBUG << "Channel fill: channel offset: " << channel_offset << " output offset " << output_offset;
        for(uint32 y = 0; y < _max_y; ++y)
        {
            for(uint32 x = 0; x < _max_x; ++x)
                {
                    memcpy(output_pos, channel_pos, _bytes_per_idx);
                
                    output_pos +=   output_jump;
                    channel_pos += _bytes_per_idx;
                }
            
        }
    }
}

NMSTL_SERIAL_DEFINE(JoinFitsBoxState,     2000);

BOREALIS_NAMESPACE_END
