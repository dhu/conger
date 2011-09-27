#include "RegridQBox.h"
#include <math.h>

BOREALIS_NAMESPACE_BEGIN

// earth's radius in miles
single earth_radius = 3958.75;
single deg_to_rads = 3.141592653589793238462643383279 / 180.0;


// points must be in radians
single RegridQBox::distance_between_two_points(single start_lat, single start_lon, single finish_lat, single finish_lon)
{
        single delta_lat = fabs(start_lat - finish_lat);;
	single delta_lon = fabs(start_lon - finish_lon);
	
	single a = sin(delta_lat/2.0) * sin(delta_lat/2.0) +
	  cos(start_lat) * cos(finish_lat) * 
	  sin(delta_lon/2.0) * sin(delta_lon/2.0); 
	single c = 2.0 * atan2(sqrt(a), sqrt(1.0-a)); 
	single  d = earth_radius * c;
	return d;

}

void RegridQBox::setup_impl() throw (AuroraException)
{
	const SchemaField* array_field;
	SchemaField curr_field;
	CatalogSchema schema, *array_schema;
	
	const SchemaField* x_field, *y_field;
	
    if ( get_num_inputs() != 1 && get_num_outputs() != 1 )
    {   Throw(aurora_typing_exception,
              "Regrid requires exactly one input stream (not " +
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
	  		FATAL << "Regrid currently supports only one array per schema.";
		else if(curr_field.get_type() == DataType::ARRAY)
	  	{
	    	_array_name = curr_field.get_name();
	    	array_rewrite = true;
	    	DEBUG << "Array found: "  << _array_name;
	  	}
      }
      if(!array_rewrite) // no array found
      	FATAL << "Regrid requires at least one array.";
    // Set input and output descriptions
   	set_out_description( 0, get_in_stream( 0 )); // same output type as input everywhere
    
    
	typed_param("start_lat", _start_lat);
	typed_param("start_lon", _start_lon);
	typed_param("x_field", _x_field_name);
	typed_param("y_field", _y_field_name);
	typed_param("rhs", _rhs);
	typed_param("miles_per_pixel", _miles_per_pixel);
	DEBUG << "Start lat: " << _start_lat << " start lon " << _start_lon;
	DEBUG << "Right flag: " << _rhs;
	_start_lat_deg = _start_lat;
	_start_lon_deg = _start_lon;
	_start_lon *= deg_to_rads;
	_start_lat *= deg_to_rads;
	array_field = schema.get_schema_field(_array_name);
	_array_offset = array_field->get_offset();
	_array_info = array_field->get_array();
	// get offsets for x, y fields
	array_schema = _array_info->get_tuple();
	x_field = array_schema->get_schema_field(_x_field_name);
	_x_offset = x_field->get_offset() - get_tuple_header_size();
	y_field = array_schema->get_schema_field(_y_field_name);
	_y_offset = y_field->get_offset() - get_tuple_header_size();
	
		
    DEBUG << "RegridQBox input schema " << get_in_schema(0)->as_string();
    DEBUG << "RegridQBox output schema " << get_out_schema(0)->as_string();
}

void RegridQBox::init_impl() throw (AuroraException)
{
  TupleDescription out_td(get_out_schema(0));
  _output_tuple_size = out_td.get_size();

}

void RegridQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{
    
    single x_degrees, y_degrees, cart_x, cart_y;
    DeqIterator my_deq_iterator = deq(0);

    DynamicArray *positions;
    // Do stuff...
    while (inv.continue_dequeue_on(my_deq_iterator, 0))
    {

        const void *data = my_deq_iterator.tuple();
		DEBUG << "Tuple: " << to_hex_string(data, _output_tuple_size);
		positions = (DynamicArray *) (*(uint32 *) ((((char *)data) + _array_offset)));
		DEBUG << "Got array at: " << (uint32) positions << " boundary " << (uint32) positions->get_boundary();
	    uint8 *current_pos = positions->first_position();
	    DEBUG << "Fluffy out array bytes: " << to_hex_string(current_pos, 20);
	    positions->save_jump(_array_info);
	    bool pass_flag = true;
	    bool first_flag = true;
	    while(current_pos != 0)
	    {
	    	// get lat/lon positions
	    	x_degrees = *(single *) (current_pos + _x_offset);
	    	y_degrees = *(single *) (current_pos + _y_offset);
	    	if(first_flag)
	    	{
	    		DEBUG << "first pos: " << x_degrees << ", " << y_degrees;
	    		first_flag = false;
	    	}
	    	// need to account for space above start point
	    	pass_flag = true;
		    if(!_rhs)
		    {
		    	if(x_degrees > _start_lat_deg || y_degrees < _start_lon_deg)
		    	{
		    		pass_flag = false;
		    		//DEBUG << "LHS Disqualifying point " << x_degrees << ", " << y_degrees << " in comparison to " << _start_lat_deg << ", " << _start_lon_deg;
		    	}
		    }
		    else
		    {
		    	if(x_degrees > _start_lat_deg || y_degrees > _start_lon_deg)
				{
					pass_flag = false;
		    		//DEBUG << "RHS Disqualifying point " << x_degrees << ", " << y_degrees << " in comparison to " << _start_lat_deg << ", " << _start_lon_deg;
				}
		    }
		    if(pass_flag)
		    {
			    cart_y = distance_between_two_points(_start_lat, _start_lon, x_degrees*deg_to_rads, _start_lon);
		    	cart_x = distance_between_two_points(_start_lat, _start_lon, _start_lat, y_degrees*deg_to_rads);
	    		//DEBUG << "This is " << cart_x << ", " << cart_y << " miles from the upper corner. ";
	    	
	    		cart_x /= _miles_per_pixel;
	    		cart_y /= _miles_per_pixel;
	    		//DEBUG << "Produces pixel lengths of " << cart_x << ", " << cart_y;
	    		if(_rhs)
	    		{
		    		cart_x = 640 - cart_x;
		    		DEBUG << "RHS (" << x_degrees << ", " << y_degrees << ")  produces (" << cart_x << ", " << cart_y << ")";
	    		}
	    		else
	    			DEBUG << "LHS (" << x_degrees << ", " << y_degrees << ")  produces (" << cart_x << ", " << cart_y << ")";
	    		cart_x = floor(cart_x);
	    		cart_y = floor(cart_y);
	    		
	    		*(single *) (current_pos + _x_offset) = cart_x;
	    		*(single *) (current_pos + _y_offset) = cart_y;
				
				//DEBUG << "Array thinks it has: " << *(single *) (current_pos + _x_offset)  << ", " << *(single *) (current_pos + _y_offset);
		    }
		    // pos = -1
		    else
		    {
		    	*(single *) (current_pos + _x_offset) = -1.0;
		    	*(single *) (current_pos + _y_offset) = -1.0;
		    }	
			
		    positions->increment_iterator();
	    	current_pos = positions->get_iterator_element();
	    	
	    }
    
	    DEBUG << "array starting with: " << to_hex_string(((uint8 *) positions) + DynamicArray::header_size(), 20);
	    memcpy(enq(0).tuple(), data, _output_tuple_size);
	    ++(enq(0));
	    get_output(0).notify_enq();
	    ++(deq(0));
    }
}

void RegridQBox::set_pending_box_state(ptr<AbstractBoxState> packed_box)
{
    //FilterBoxState* actual_packed_box = static_cast<FilterBoxState*>(packed_box.get());
    //_pending_box_state.set(new FilterBoxState(*actual_packed_box));
    _pending_box_state = packed_box.dynamic_cast_to<FilterBoxState>();
}

ptr<AbstractBoxState> RegridQBox::pack_state(bool delta_only)
{
  INFO << "Packing state for RegridQBox named [" << get_name() << "]";
  ptr<FilterBoxState> filter_box_state(new FilterBoxState());
  return filter_box_state;
}


BOREALIS_NAMESPACE_END
