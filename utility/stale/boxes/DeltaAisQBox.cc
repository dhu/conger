#include "DeltaAisQBox.h"
#include <math.h>

BOREALIS_NAMESPACE_BEGIN

// earth's radius in miles
single a_earth_radius = 3958.75;
single a_deg_to_rads = 3.141592653589793238462643383279 / 180.0;


// points arrive in degrees; convert to radians
//single DeltaAisQBox::distance_between_two_points(single start_lat, single start_lon, single finish_lat, single finish_lon)
single DeltaAisQBox::distance_between_two_points(ship_tuple first, ship_tuple second)
{
	single start_lat = first.x;
	single start_lon = first.y;
	single finish_lat = second.x;
	single finish_lon = second.y;
	
	
    start_lat *= a_deg_to_rads;
    start_lon *= a_deg_to_rads;
    finish_lat *= a_deg_to_rads;
    finish_lon *= a_deg_to_rads;
    
    single delta_lat = fabs(start_lat - finish_lat);;
	single delta_lon = fabs(start_lon - finish_lon);
	
	single a = sin(delta_lat/2.0) * sin(delta_lat/2.0) +
	  cos(start_lat) * cos(finish_lat) * 
	  sin(delta_lon/2.0) * sin(delta_lon/2.0); 
	single c = 2.0 * atan2(sqrt(a), sqrt(1.0-a)); 
	single  d = a_earth_radius * c;
	return d;

}

void DeltaAisQBox::build_new_updates(DynamicArray *positions)
{
	uint8 *current_pos = positions->first_position();
	ship_tuple *current_tuple = (ship_tuple *) current_pos;
	ship_tuple cached_val;
	_new_values.clear();
	single distance;
	uint32 point_count = 0;
	
	while(current_pos != 0)
	{
		//DEBUG << "Examining point: " << current_tuple->id;
		if(_ship_cache.find(current_tuple->id) != _ship_cache.end())
		{
			cached_val = _ship_cache[current_tuple->id];
			distance = distance_between_two_points(cached_val, *current_tuple);
			// if the point has moved far enough then save it off
			if(distance > _update_threshold)
			{
				_new_values.push_back(*current_tuple);
				_ship_cache[current_tuple->id] = *current_tuple;
				//DEBUG << "            Updating position.";
			}
			//else
				//DEBUG << "            Discarding point";
		}
		else
		{
			//DEBUG << "           Adding point: " << current_tuple->id << " to cache.";
			_new_values.push_back(*current_tuple);
			_ship_cache[current_tuple->id] = *current_tuple;
		}
		positions->increment_iterator();
	    current_pos = positions->get_iterator_element();
	    current_tuple = (ship_tuple *) current_pos;
	    ++point_count;
	}
	DEBUG << "Delta started with: " << point_count;
	DEBUG << "Temporally compressed to " << _new_values.size();
}

void DeltaAisQBox::cluster_points()
{
	_spatially_compressed.clear();
	vector<ship_tuple>::iterator the_pos = _new_values.begin();
	vector<ship_tuple>::iterator the_clusters_pos;
	ship_tuple cluster, ship_record;
	single distance;
	bool found;
	
	while(the_pos != _new_values.end())
	{
		ship_record = *the_pos;
		the_clusters_pos = _spatially_compressed.begin();
		found = false;
		while(the_clusters_pos != _spatially_compressed.end())
		{
			cluster = *the_clusters_pos;
			distance = distance_between_two_points(cluster, ship_record);
			if(distance < _compression_threshold)
			{
				DEBUG << "Clustering: " << cluster.x << ", " << cluster.y << " and " << ship_record.x << ", " << ship_record.y;
				// discard point
				found = true;
				break;
			}
			++the_clusters_pos;
		} 
		if(found == false)
		{
			DEBUG << "Adding point: " << ship_record.x << ", " << ship_record.y;
			_spatially_compressed.push_back(ship_record);
		}
		++the_pos;	
	}
	DEBUG << "Delta finished with: " << _spatially_compressed.size();
	DEBUG << "First points: " << _spatially_compressed[0].x << ", " << _spatially_compressed[0].y << " and " << _spatially_compressed[1].x << ", " << _spatially_compressed[1].y;
	
}

void DeltaAisQBox::flatten_points(DynamicArray *out_array)
{
	uint8 *current_pos = out_array->first_position();
	vector<ship_tuple>::iterator points = _spatially_compressed.begin();
	ship_tuple current_record;
	uint32 record_size = sizeof(ship_tuple);
	
	while(points != _spatially_compressed.end())
	{
		current_record = *points;
		memcpy(current_pos, (uint8 *) &current_record, record_size);
		current_pos += record_size; 		
		++points;
	}
}

void DeltaAisQBox::setup_impl() throw (AuroraException)
{
	const SchemaField* array_field;
	SchemaField curr_field;
	CatalogSchema schema, *array_schema;
	
	const SchemaField* x_field, *y_field;
	
    if ( get_num_inputs() != 1 && get_num_outputs() != 1 )
    {   Throw(aurora_typing_exception,
              "DeltaAis requires exactly one input stream (not " +
              to_string(get_num_inputs()) + ")");
    }



        ExprContext  ctxt;
	
        ctxt.set_context_schema( *get_in_schema( 0 ));
        
        // set expression context to tuple plus flattened version of arrays 
        // (1 tuple per array)
        
	    	
	// for each expression make sure it only has one array
	// fatal out otherwise
    schema = *get_in_schema(0);
    
    
    bool array_rewrite = false;
    for(unsigned int x = 0; x < schema._field.size(); ++x)
      {
		curr_field = schema._field[x];
		if(curr_field.get_type() == DataType::ARRAY && array_rewrite == true)
	  		FATAL << "DeltaAis currently supports only one array per schema.";
		else if(curr_field.get_type() == DataType::ARRAY)
	  	{
	    	_array_name = curr_field.get_name();
	    	array_rewrite = true;
	    	DEBUG << "Array found: "  << _array_name;
	  	}
      }
      if(!array_rewrite) // no array found
      	FATAL << "DeltaAis requires at least one array.";
    // Set input and output descriptions
   	set_out_description( 0, get_in_stream( 0 )); // same output type as input everywhere
    
    
	typed_param("x_field", _x_field_name);
	typed_param("y_field", _y_field_name);
	// distance in miles between two points to warrent each type of (spatial, temporal) compression
	typed_param("distance_to_update", _update_threshold);
	typed_param("distance_to_compress", _compression_threshold);
	
	array_field = schema.get_schema_field(_array_name);
	_array_offset = array_field->get_offset();
	_array_info = array_field->get_array();
	// get offsets for x, y fields
	array_schema = _array_info->get_tuple();
	x_field = array_schema->get_schema_field(_x_field_name);
	_x_offset = x_field->get_offset() - get_tuple_header_size();
	y_field = array_schema->get_schema_field(_y_field_name);
	_y_offset = y_field->get_offset() - get_tuple_header_size();
	
		
    DEBUG << "DeltaAisQBox input schema " << get_in_schema(0)->as_string();
    DEBUG << "DeltaAisQBox output schema " << get_out_schema(0)->as_string();
}

void DeltaAisQBox::init_impl() throw (AuroraException)
{
  TupleDescription out_td(get_out_schema(0));
  _output_tuple_size = out_td.get_size();

}

void DeltaAisQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{
    
    DeqIterator my_deq_iterator = deq(0);

    DynamicArray *positions;
    // Do stuff...
    while (inv.continue_dequeue_on(my_deq_iterator, 0))
    {

        const void *data = my_deq_iterator.tuple();
		DEBUG << "Tuple: " << to_hex_string(data, _output_tuple_size);
		positions = (DynamicArray *) (*(uint32 *) ((((char *)data) + _array_offset)));
	    positions->save_jump(_array_info);
	    build_new_updates(positions);
	    cluster_points();
	    // build new output tuple
	    uint32 out_size = _spatially_compressed.size()*sizeof(ship_tuple);
	    if(out_size > 0)
	    {
		    //DEBUG << "Allocating " << out_size << " for " << _spatially_compressed.size() << " elements.";
		    DynamicArray *out_array = DynamicArray::allocate_array(out_size);
		    out_array->set_count_size(1,_spatially_compressed.size());
		    //DEBUG << "Out array start " << (uint32) out_array << " boundary " << (uint32) out_array->get_boundary();
	    	flatten_points(out_array);
	    	//DEBUG << "Fluffy out array bytes: " << to_hex_string(out_array->first_position(), 20);
	    	uint8  *pos = (uint8 *) enq(0).tuple();
	    	memcpy(enq(0).tuple(), data, _output_tuple_size);
	    	uint32 *array_pos = (uint32 *) (pos + _array_offset);
        	*array_pos = (uint32) out_array;
	    	++(enq(0));
	    	get_output(0).notify_enq();
	    	++(deq(0));
	    }
    }
}



void DeltaAisQBox::set_pending_box_state(ptr<AbstractBoxState> packed_box)
{
    //DeltaAisBoxState* actual_packed_box = static_cast<DeltaAisBoxState*>(packed_box.get());
    //_pending_box_state.set(new DeltaAisBoxState(*actual_packed_box));
    _pending_box_state = packed_box.dynamic_cast_to<DeltaAisBoxState>();
}

ptr<AbstractBoxState> DeltaAisQBox::pack_state(bool delta_only)
{
  INFO << "Packing state for DeltaAisQBox named [" << get_name() << "]";
  ptr<DeltaAisBoxState> filter_box_state(new DeltaAisBoxState());
  return filter_box_state;
}


BOREALIS_NAMESPACE_END
