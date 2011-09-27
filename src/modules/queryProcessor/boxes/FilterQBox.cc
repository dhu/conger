#include "FilterQBox.h"

BOREALIS_NAMESPACE_BEGIN

void FilterQBox::setup_impl() throw (AuroraException)
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
	
        ctxt.set_context_schema( *get_in_schema( 0 ));
        
        // set expression context to tuple plus flattened version of arrays 
        // (1 tuple per array)
        
	    	
        _expressions.push_back( Expression::parse( expr, ctxt ));
    }

    _expr_count = _expressions.size();
	// for each expression make sure it only has one array
	// fatal out otherwise
    CatalogSchema my_schema = *get_in_schema(0);
    _array_rewrite = false;
    SchemaField curr_field;
    for(unsigned int x = 0; x < my_schema._field.size(); ++x)
      {
	curr_field = my_schema._field[x];
	if(curr_field.get_type() == DataType::ARRAY && _array_rewrite == true)
	  FATAL << "Filter currently supports only one array per schema.";
	else if(curr_field.get_type() == DataType::ARRAY)
	  {
	    //_array_rewrite = true;
	    _array_field = curr_field.get_name();
	    DEBUG << "Array found: "  << _array_field;
	  }
      }
      
    for(uint32 i = 0; i < _expressions.size(); ++i)
    {
    	if(_expressions[i]->array_use())
    		_array_rewrite = true;
    }
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
	
    DEBUG << "FilterQBox input schema " << get_in_schema(0)->as_string();
    DEBUG << "FilterQBox output schema " << get_out_schema(0)->as_string();
}

void FilterQBox::init_impl() throw (AuroraException)
{
  TupleDescription out_td(get_out_schema(0));
  _output_tuple_size = out_td.get_size();

}

void FilterQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
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
          void *out_data;	        
        Tuple input_tuple = Tuple::factory(data);

        DEBUG << " FilterQBox " << get_name() << " IN: " << in_td.tuple_to_string(data);
        DEBUG << "  BYTES " << to_hex_string(data, _output_tuple_size);

        // assuming a single-port filter is used in LS
        //
        if (input_tuple.get_fake_flag())             
        {
            memcpy(enq(0).tuple(), data, _output_tuple_size);

            DEBUG << " FilterQBox " << get_name()
                << " OUT(0): " << out_td.tuple_to_string(data);

            ++ (enq(0));
            outputted[0] = true;
            DEBUG << " -> through port " << 0;
            ++my_deq_iterator;
            continue;
        }

        bool passed = false;

        _ctxt.reset();
        // Do a normal tuple build
        if(!_array_rewrite)
        {
        	DEBUG << "Not in array  mode.";
	        _ctxt.set_tuple((const char *)data);
	        // check expressions
		for (unsigned int i = 0; i < _expressions.size(); ++i)
        	{

            	// If a tuple is of type BOUNDARY or RECONCILIATION_DONE, propagate as is on ALL output streams
            	if ( (input_tuple.get_tuple_type() == TupleType::BOUNDARY) ||
                	 (input_tuple.get_tuple_type() == TupleType::RECONCILIATION_DONE))
            	{
                	out_data = create_output_tuple(data, i);
                	memcpy(enq(i).tuple(), out_data, _output_tuple_size);
			++(enq(i));
                	outputted[i] = true;
                	passed = false; // Keep passed at false so we also output on false stream
            	}

            	else if ( _expressions[i]->eval<bool>(_ctxt) )
            	{
                	out_data = create_output_tuple(data, i);
				    memcpy(enq(i).tuple(), out_data, _output_tuple_size);
	                DEBUG << " FilterQBox " << get_name()
    	                << " OUT(" << i << "): " << out_td.tuple_to_string(data);

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
                out_data = create_output_tuple(data, i);
		memcpy(enq(i).tuple(), out_data, _output_tuple_size);
                DEBUG << " FilterQBox " << get_name()
                    << " OUT(" << i << "): " << out_td.tuple_to_string(data);

                ++(enq(i));
                passed = true;
                outputted[i] = true;
                DEBUG << " -> through port " << i;
                break;
            }
        }
	}
        else
        {
        	// construct tuple by extracting fields and iterating over values
        	// TODO: make sure the array is actually in the expression
        	// Need to add code to the expr library to walk it and just enumerate
        	// what arrays exist in the set
	        // Need to work through lexer tokens
        	        	
        	// for each expression where we need to copy - preserve orig until last step
	  DEBUG << "In array mode!";
	  for (uint32 i = _expr_count - 1; i >  0; --i)
        	{
        	  DEBUG << "Evaluating expression: " << i;
			  out_data = iterate_over_and_null_vals(data, i);
			  memcpy(enq(i).tuple(), out_data, _output_tuple_size);
			  ++(enq(i));
			  outputted[i] = true;
			  passed = true;
        	}
        	DEBUG << "Evaluating expression: " << 0;
        	  out_data = iterate_over_and_null_vals(data, 0);
			
  	
  			  memcpy(enq(0).tuple(), out_data, _output_tuple_size);
			  ++(enq(0));
			  outputted[0] = true;
			  passed = true;
        }
		
        // catch folks going to the false port
        if (!passed && _pass_on_false_port)
        {
            outputted[_expr_count] = true;

            DEBUG << " FilterQBox " << get_name()
                << " OUT(" << _expr_count << "): " << out_td.tuple_to_string(data);

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

void FilterQBox::set_pending_box_state(ptr<AbstractBoxState> packed_box)
{
    //FilterBoxState* actual_packed_box = static_cast<FilterBoxState*>(packed_box.get());
    //_pending_box_state.set(new FilterBoxState(*actual_packed_box));
    _pending_box_state = packed_box.dynamic_cast_to<FilterBoxState>();
}

ptr<AbstractBoxState> FilterQBox::pack_state(bool delta_only)
{
  INFO << "Packing state for FilterQBox named [" << get_name() << "]";
  ptr<FilterBoxState> filter_box_state(new FilterBoxState());
  return filter_box_state;
}

void *FilterQBox::create_output_tuple(const void *data, int i)
{
	void *new_data;
	
	
	DEBUG << "Stream " << i;
	if(i == 0 || !_array_rewrite)
	{
		// recycle the array for the first out stream
		DEBUG << "Copying out original array!";
	        new_data = (void *) new char[_output_tuple_size];
			memcpy(new_data, data, _output_tuple_size);
		
	}
	else
	{
		// TODO: make this a list, iterate over elements, save myself the constant checks
	        TupleDescription in_td(get_in_schema(0));
	        uint32 field_idx = in_td.index_of_field(_array_field);
	        _array_offset = in_td.get_offset(field_idx);
	        uint32 *element;
		
		DynamicArray *in_array = (DynamicArray *) (*(uint32 *) ((char *)data) + _array_offset); 
		// deep copy the array because we are splitting streams
		DynamicArray *new_array = clone_array(in_array);
		
		memcpy(new_data, data, _output_tuple_size);
		element = (uint32 *)((uint8 *)new_data + _array_offset);                 
                *element  = (uint32) new_array;
	}
	return new_data;
}

DynamicArray *FilterQBox::clone_array(DynamicArray *orig)
{
  
		const SchemaField *_array_found = (*get_in_schema(0)).get_schema_field(_array_field);
		SchemaArray *array_info = _array_found->get_array();
		int32 array_size = array_info->array_size();
		DynamicArray *clone = DynamicArray::allocate_array(array_size);
		//clone->set_count_size(1);
		//uint8 *old_first_idx = (uint8 *) orig + DynamicArray::header_size();
		//uint8 *new_first_idx = (uint8 *) clone + DynamicArray::header_size();

		memcpy((uint8 *) clone, (uint8 *) orig, array_size + DynamicArray::header_size());
		return clone;
}




// takes in current tuple and expression number in set
// TODO: set a flag to tell whether to drop the tuple entirely b/c every time failed
// Does this conform with the semantics?  Is it better to pass on a nulled out array?
void *FilterQBox::iterate_over_and_null_vals(const void *data, int i)
{
       
  CatalogSchema my_schema = *get_in_schema(0);
  const SchemaField *array_found = my_schema.get_schema_field(_array_field);
  SchemaArray *array_info = array_found->get_array();
  _array_offset = array_found->get_offset();
  void * new_tuple = create_output_tuple(data, i);
  DynamicArray *current_array = (DynamicArray *) *(uint32 *) (((char *)new_tuple) + _array_offset);
	uint8 *array_ptr = (uint8 *) current_array;
	DEBUG << "Array @ " << array_ptr;
	current_array->save_jump(array_info);
			
	uint8 *current_element = current_array->first_position();
	/*CatalogSchema *tuple = array_info->get_tuple();
	const SchemaField *id_field = tuple->get_schema_field("id");
	int offset = id_field->get_offset();
	single id = *((uint8 *)current_element + offset); 
	DEBUG << "Got first element with id: " <<  id;   
	DEBUG << "First elements " << to_hex_string((void *) current_element, 40); */
	unsigned int element_size;
	if(array_info->get_element_type() == DataType::NONE)
		{
			CatalogSchema *element_schema = array_info->get_tuple();
			element_size = element_schema->get_size();
		}
	else
	{
		DataType element_type = array_info->get_element_type(); 
		element_size = element_type.get_size();
	}
	
	uint8 null_filler[element_size];
	for(unsigned int x = 0; x < element_size; ++x)
		null_filler[x] = '\0';

	_ctxt.reset();
	_ctxt.set_tuple((const char *) new_tuple);
	int counter = 0;
	
	while(current_element != 0)
		{			
	
			if(!_expressions[i]->eval<bool>(_ctxt) )
			{
			
				
				memcpy(current_element, null_filler, element_size);
			    
			}
			current_array->increment_iterator();
			current_element = current_array->get_iterator_element();
			++counter;
		}
		return new_tuple;
			  
}
BOREALIS_NAMESPACE_END
