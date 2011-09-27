#include "MergeSketchQBox.h"
#include "DataPath.h"
#include "DynamicArray.h"
#include <cmath>

BOREALIS_NAMESPACE_BEGIN


const int MergeQBox::LEFT = 0;
const int MergeQBox::RIGHT = 1;

bool MergeQBox::is_less_than(pair<single, single> a, pair<single, single> b)
{
  if(a.first < b.first)
    return true;
  if(a.first == b.first && a.second < b.second)
    return true;
  else
    return false;
}

void MergeQBox::setup_impl() throw( AuroraException )
{
    if ( get_num_inputs() != 2 )
    {   Throw( aurora_typing_exception,
               "Merge requires exactly 2 inputs (not " +
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
    


        DEBUG << "Input matches output for merge!";
    
        const vector<SchemaField> &left_fields =
            get_in_schema( LEFT )->get_schema_field();

     
        copy( left_fields.begin(), left_fields.end(),
              back_inserter( output_fields ));

    TupleDescription  td( output_fields.begin(), output_fields.end() );
    DEBUG << get_name() << " output description: " << td;
    set_out_description( 0, td );

    // inits for selectivity stat vectors... -- tatbul
    //
    _join_selectivity[LEFT].clear();
    _join_selectivity[RIGHT].clear();
    _join_selectivity_sum[LEFT] = 0;
    _join_selectivity_sum[RIGHT] = 0;
    
    typed_param("x_field", _x_field_name);
	typed_param("y_field", _y_field_name);
	typed_param("id_field", _id_field_name);
	typed_param("array_name", _array_field_name);
    CatalogSchema lhs_schema = *get_in_schema(0);
	CatalogSchema rhs_schema = *get_in_schema(1);
	const SchemaField *l_array = lhs_schema.get_schema_field(_array_field_name);
	_array_offset = l_array->get_offset();
	// TODO: make this an if (field or schema)
	_array_schema = l_array->get_array();
    
    DEBUG << "Element size: " << _array_schema->array_size();
	
	
	
	CatalogSchema *array_schema = _array_schema->get_tuple();
	
	const SchemaField *x_field = array_schema->get_schema_field(_x_field_name);
	_x_offset = x_field->get_offset() - get_tuple_header_size();
	const SchemaField *y_field = array_schema->get_schema_field(_y_field_name);
	_y_offset = y_field->get_offset() - get_tuple_header_size();
	const SchemaField *id_field = array_schema->get_schema_field(_id_field_name);
	_id_offset = id_field->get_offset() - get_tuple_header_size();
	DEBUG << "Position offsets: " << _x_offset << ", " << _y_offset;
	TupleDescription out_td(get_out_schema(0));
	_output_tuple_size = out_td.get_size();
	
	
}


// name will be "left" or "right";
ptr<MergeQBox::MergeBuffer> MergeQBox::create_buffer_by_parameter( int  stream_id )
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

    MergeBuffer  *ret;
    string  order_by_param = param( name + "-order-by", PARAM_NON_EMPTY );

    if ( order_by_param == "TUPLES" )
// yna
//    {   ret = new TupleCountMergeBuffer( stream_id, this, buffer_size );
// end yna
    {   ret = new TupleCountMergeBuffer( stream_id, this,
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
            ret = new FieldMergeBuffer<int32>( stream_id, this, field,
                static_cast<int32>(buffer_size) );

        else if ( field->getType() == DataType::LONG )
            ret = new FieldMergeBuffer<int64>( stream_id, this, field,
                static_cast<int64>(buffer_size) );

        else if ( field->getType() == DataType::SHORT )
            ret = new FieldMergeBuffer<int16>( stream_id, this, field,
                static_cast<int16>(buffer_size) );

        else if ( field->getType() == DataType::BYTE )
            ret = new FieldMergeBuffer<int8>( stream_id, this, field,
                static_cast<int8>(buffer_size) );

        else if ( field->getType() == DataType::SINGLE )
            ret = new FieldMergeBuffer<single>( stream_id, this, field,
                static_cast<single>(buffer_size) );

        else if ( field->getType() == DataType::DOUBLE )
            ret = new FieldMergeBuffer<double>( stream_id, this, field,
                buffer_size );

        else
            Throw(aurora_typing_exception,
                "Invalid order-on-field type, must be a numeric field.");
    }
    else
    {   Throw( aurora_typing_exception,
               "Merge only supports order-by TUPLES or VALUES, not `" +
               order_by_param + "'" );
    }

    return( ptr<MergeBuffer>( ret ));
}


void MergeQBox::init_impl() throw( AuroraException )
{
    _left_body_size = get_in_schema( LEFT )->get_size();
    _right_body_size = get_in_schema( RIGHT )->get_size();
}


void MergeQBox::run_impl( QBoxInvocation  &inv )
    throw( AuroraException )
{
    bool outputted = false;

    run_stream( inv, LEFT, outputted );
    run_stream( inv, RIGHT, outputted );

    // Done, notify if there was output
    if ( outputted )
    {   
    	DEBUG << "Notifying scheduler of enq";
    	get_output(0).notify_enq();
    }
}


void MergeQBox::run_stream( QBoxInvocation &inv, int stream_id,
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
        _buffer[other_stream_id]->join(data, _predicate.get(), outputted,
                                       &comparison_count, &match_count);
        _buffer[stream_id]->insert_tuple(data);
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


void MergeQBox::emit_tuple(EvalContext &ctxt)
{
    produce_dont_emit_tuple(ctxt);

    // And actually emit
    DEBUG << "Enqueueing and incrementing enq iterator";
    EnqIterator ei = enq(0);
    void *tuple_data = enq(0).tuple();
    DEBUG << "Output tuple contents: " << tuple_to_string(0, tuple_data);
	DynamicArray *out_array = (DynamicArray *) (*(uint32 *) ((((char *)tuple_data) + _array_offset)));
	uint8 *first_pos = out_array->first_position();
	DEBUG << "enq first coordinate: " << *(single *) first_pos << ", " << *(single *) (first_pos + 4);  
    ++ei;

}

void MergeQBox::produce_dont_emit_tuple(EvalContext &ctxt)
{
    EnqIterator ei = enq(0);
    const Tuple left = Tuple::factory(ctxt.getTuple(LEFT));
    const Tuple right = Tuple::factory(ctxt.getTuple(RIGHT));

	
    const void *left_tuple = left.get_data();
    const void *right_tuple = right.get_data();
    DEBUG << "merge left tuple: " << tuple_to_string(0, left.get_data());
    DEBUG << "merge right tuple: " << tuple_to_string(1, right.get_data());
    
	DynamicArray *lhs = (DynamicArray *) (*(uint32 *) ((((char *)left_tuple) + _array_offset)));
    DynamicArray *rhs = (DynamicArray *) (*(uint32 *) ((((char *)right_tuple) + _array_offset)));
    // elements need to be compatible: enforce this somehow in the next version
    // TODO: make the user specify which arrays are being merged
    uint8 *lhs_contents = ((uint8 *)lhs) + DynamicArray::header_size();
    uint8 *rhs_contents = ((uint8 *)rhs) + DynamicArray::header_size();
    DEBUG << "left bytes: " << to_hex_string(lhs_contents, 20);
    DEBUG << "right bytes: " << to_hex_string(rhs_contents, 20); 
    
    
    uint32 element_size = _array_schema->array_size();
    uint32 lhs_size = lhs->array_size(_array_schema);
    uint32 rhs_size = rhs->array_size(_array_schema);
    
    DEBUG << "Left has: " << lhs_size << " bytes; right has: " << rhs_size << " bytes.";
    uint32 total_size = lhs_size + rhs_size;
    // element sizes need to be equal
    
    uint32 total_elements = total_size / element_size;
    uint8 *r_pos, *l_pos, *out_pos;;
   
    DynamicArray *out_array = DynamicArray::allocate_array(total_size);
    out_array->set_count_size(1,total_elements); 
    
    out_pos = out_array->first_position();
    l_pos = lhs->first_position();
    r_pos = rhs->first_position();

    // conservatively allocate
	lhs->save_jump(_array_schema);
	rhs->save_jump(_array_schema);
	out_array->save_jump(_array_schema);
	
	DEBUG << "Left boundary: " << (uint32) lhs->get_boundary() << ", RHS boundary: " << (uint32) rhs->get_boundary(); 
	pair<single, single> l_lat_lon, r_lat_lon;
	uint32 l_id, r_id;
	uint32 ship_count = 0;
	while(l_pos != 0 || r_pos != 0)
       {	  	
	  	if(l_pos == 0)
	    	{
	      		// automatically grab the remaining rhs and break
	      		DEBUG << "Left side done; copying over rest of rhs";
	      		uint32 array_end_point = (uint32) rhs->get_boundary();
	      		uint32 current_pos = (uint32) r_pos;
	      		uint32 remaining_len = array_end_point - current_pos;
	      		DEBUG << "l pos = " << (uint32) l_pos << " r pos " << (uint32) r_pos;
	      		DEBUG << "Rhs size: " << rhs_size << " current pos: " << current_pos 
	      		  << " start point " << (uint32) rhs << " end point " << array_end_point
	      		  << " remaining length " << remaining_len;
	      		memcpy(out_pos, r_pos, remaining_len);  
	      		break; 
	    	}
	  		else if(r_pos == 0)
	    	{
	      	// grab lhs remainder
	      	DEBUG << "Right side done; copying over rest of lhs"; 
	      	uint32 array_end_point = (uint32) lhs->get_boundary();
	      	uint32 current_pos = (uint32) l_pos;
	      	uint32 remaining_len = array_end_point - current_pos;
	      	memcpy(out_pos, l_pos, remaining_len);
	      	ship_count += remaining_len / element_size; 
		    break;
		    }
	    	else 
	    	{
	    	
	    		l_lat_lon.first = *(single *) (l_pos + _x_offset);
    	   		l_lat_lon.second = *(single *) (l_pos + _y_offset);
    	   		l_id = *(uint32 *) (l_pos + _id_offset);
       			r_lat_lon.first = *(single *) (r_pos + _x_offset);
       			r_lat_lon.second = *(single *) (r_pos + _y_offset);
       			r_id = *(uint32 *) (r_pos + _id_offset);
       	
       		if(l_lat_lon.first == -1.0 || l_lat_lon.second == -1.0)
       		{
       			// this point is out of bounds; just increment the pointer and drop it
       			lhs->increment_iterator();
	  	   		l_pos = lhs->get_iterator_element();
       		}
       		else if(r_lat_lon.first == -1.0 || r_lat_lon.second == -1.0)
       		{
       			// this point is out of bounds; just increment the pointer and drop it
       			rhs->increment_iterator();
	  	   		r_pos = rhs->get_iterator_element();
       		}
       		else
       		{
		  	   	//if(is_less_than(l_lat_lon, r_lat_lon))
		  	   	if(l_id < r_id)
		  	   	{
	  		   		// grab left side, memcpy over the value to out array and 
	    			// increment out and lhs ptrs
		    			DEBUG << "Merging left value " << l_id << " over " << r_id;
	  		   			memcpy(out_pos, l_pos, element_size);
	  	   				lhs->increment_iterator();
	  	   				l_pos = lhs->get_iterator_element();
	  	   				out_array->increment_iterator();
	  	   				out_pos = out_array->get_iterator_element();
	  	   				//DEBUG << "Incremented left pointer to: " << (uint32) l_pos;
	  	   				++ship_count;	    		
	  			}
	     		else
	  			{
	  					DEBUG << "Merging right value " << r_lat_lon.first << ", " << r_lat_lon.second 
	    				  << " over " << l_lat_lon.first << ", " << l_lat_lon.second;
	    			// do inverse
	    				memcpy(out_pos, r_pos, element_size);
	  	   				rhs->increment_iterator();
	  	   				r_pos = rhs->get_iterator_element();
	  	   				out_array->increment_iterator();
	  	   				out_pos = out_array->get_iterator_element();
	  	   				//DEBUG << "Incremented right pointer to: " << (uint32) r_pos;
	  	   				++ship_count;
	  			}
       		}
	    } // end case of neither reaching array boundary
	} // end main iteration
		lhs->modify_array_reference(-1);
		rhs->modify_array_reference(-1);
        uint8  *pos = (uint8 *) enq(0).tuple();
        memcpy(enq(0).tuple(), left.get_data(), _output_tuple_size);
        uint32 *array_pos = (uint32 *) (pos + _array_offset);
        *array_pos = (uint32) out_array;
	    DEBUG << "Out array memory location: " << (uint32) out_array;
	    uint8 *first_pos = out_array->first_position();
  	    DEBUG << "First coordinate: " << *(single *) first_pos << ", " << *(single *) (first_pos + 4);
	    
    }


void MergeQBox::MergeBuffer::join(const void *other_tuple,
                                Expression *predicate,
                                bool &outputted,
                                int *comparison_count,
                                int *match_count)
{
    DEBUG << "Calling MergeBuffer::Merge on stream " << _stream_id;

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
void MergeQBox::MergeBuffer::join_no_emit(const void *other_tuple,
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


void MergeQBox::MergeBuffer::insert_tuple(const void *tuple_data)
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
                DEBUG << "Timing out tuple "
                      << _qbox->tuple_to_string(_stream_id, i->get_data())
                      << " its age is "
                      << (be.get_timestamp() - i->get_timestamp());
                // Record tuple's Merge selectivity before it disappears
                // from the Merge buffer. -- tatbul
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

void MergeQBox::MergeBuffer::clear()
{
    _buffer.clear();
}


void MergeQBox::TupleCountMergeBuffer::insert_tuple(const void *tuple_data)
{
    DEBUG << "Running TupleCountMergeBuffer::insertTuple("
          << _qbox->tuple_to_string(_stream_id, tuple_data)
          << ")";
    // Insert the tuple.
    MergeQBox::MergeBuffer::insert_tuple(tuple_data);

    // Check to make sure the buffer is not overfull.
    while (_buffer.size() > _buffer_size)
    {
        DEBUG << "Actual size " << _buffer.size()
              << " is greater than max size " << _buffer_size;

        DEBUG << "Pulling "
              << _qbox->tuple_to_string(_stream_id,
                                        _buffer.front().get_data())
              << " off the buffer.";

        // Record tuple's Merge selectivity before it disappears
        // from the Merge buffer. -- tatbul
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
T MergeQBox::FieldMergeBuffer<T>::field_value(const void *tuple_data)
{
    _ctxt.reset();
    _ctxt.set_tuple( tuple_data );
    return( _field->template eval<T>( _ctxt ));
}

template<typename T>
void MergeQBox::FieldMergeBuffer<T>::insert_tuple(const void *tuple_data)
{
    DEBUG << "Running FieldMergeBuffer::insertTuple("
          << _qbox->tuple_to_string(_stream_id, tuple_data)
          << ")";
    // Insert the tuple.
    MergeQBox::MergeBuffer::insert_tuple(tuple_data);

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

        // Record tuple's Merge selectivity before it disappears
        // from the Merge buffer. -- tatbul
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
template class MergeQBox::FieldMergeBuffer<int8>;
template class MergeQBox::FieldMergeBuffer<int16>;
template class MergeQBox::FieldMergeBuffer<int32>;
template class MergeQBox::FieldMergeBuffer<int64>;
template class MergeQBox::FieldMergeBuffer<single>;
template class MergeQBox::FieldMergeBuffer<double>;


void MergeQBox::set_pending_box_state(ptr<AbstractBoxState> packed_box)
{
    MergeBoxState* actual_packed_box = static_cast<MergeBoxState*>(packed_box.get());
    _pending_box_state.set(new MergeBoxState(*actual_packed_box));
}

ptr<AbstractBoxState> MergeQBox::pack_state(bool delta_only)
{

    INFO << "Packing state for MergeQBox named [" << get_name() << "]";

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
    ptr<MergeBoxState> Merge_box_state(new MergeBoxState(left_buffer,left_nb_tuples,right_buffer,right_nb_tuples));

    return Merge_box_state;

}


void MergeQBox::unpack_state(ptr<AbstractBoxState> box_state)
{

    ptr<MergeBoxState> my_state = box_state.dynamic_cast_to<MergeBoxState>();

    const dynbuf& left_buffer = my_state->get_left();
    int nb_left_tuples = my_state->get_nb_left_tuples();

    DEBUG << "Will try to unpack " << nb_left_tuples << " on left stream";
    _buffer[LEFT]->init(left_buffer,nb_left_tuples);

    const dynbuf& right_buffer = my_state->get_right();
    int nb_right_tuples = my_state->get_nb_right_tuples();

    DEBUG << "Will try to unpack " << nb_right_tuples << " on right stream";
    _buffer[RIGHT]->init(right_buffer,nb_right_tuples);

}

int MergeQBox::MergeBuffer::get_oldest()
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

int MergeQBox::MergeBuffer::dump(dynbuf& buf)
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

void MergeQBox::MergeBuffer::init(const dynbuf& buf, int nb_tuples)
{
    clear();
    int next_tuple_offset = 0;
    for (int i = 0; i < nb_tuples; i++)
    {
        insert_tuple(((dynbuf&)buf).data()+next_tuple_offset);
        next_tuple_offset += _tuple_size;
    }
}

NMSTL_SERIAL_DEFINE(MergeBoxState,     2000);

BOREALIS_NAMESPACE_END
