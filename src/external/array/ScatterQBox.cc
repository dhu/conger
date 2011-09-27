#include "ScatterQBox.h"

BOREALIS_NAMESPACE_BEGIN


#define   CLONE_SIZE   160


////////////////////////////////////////////////////////////////////////////////
//
void ScatterQBox::setup_impl() throw (AuroraException)
{
    CatalogSchema   my_schema;
    CatalogSchema  *out_schema;
    CatalogStream  *out_stream;
    SchemaArray    *array_schema;

    SchemaField     array_field("array", DataType::ARRAY, 4, 53);
    SchemaField     slice("slice", DataType::INT, 4, 57);
    SchemaField     id("id", DataType::INT, 4, 61);
    SchemaField     chunk("chunk", DataType::INT, 4, 65);

    Name    out_name("tile");
    int32   the_field_index;
//..............................................................................


    if (get_num_inputs() != 1)
    {   Throw(aurora_typing_exception,
              "Scatter requires exactly one input stream (not " +
              to_string(get_num_inputs()) + ")");
    }

    _worker_count = get_num_outputs();
    DEBUG  << "worker=" << _worker_count;

    // for each expression make sure it only has one array
    // fatal out otherwise
    //
    my_schema = *get_in_schema(0);
      
    // Set input and output descriptions
    // There are as many output streams as predicates, +1 if passing "false"
    // tuples too
    //
    out_stream = new CatalogStream();
    out_stream->shallow_copy(*get_in_stream(0));

    out_schema = new CatalogSchema();
    out_schema->set_schema_name(out_name);

    array_schema = new SchemaArray(DataType::BYTE, CLONE_SIZE);
    array_schema->add_dimension(0, CLONE_SIZE - 1);
    array_field.set_array(array_schema);

    out_schema->_field.push_back(array_field);
    out_schema->_field.push_back(slice);
    _slice_offset = 57;
    
    out_schema->_field.push_back(id);
    _id_offset = 61; 
    
    out_schema->_field.push_back(chunk);
    _chunk_offset = 65;
    _seq_no = 0;

    out_stream->set_stream_schema(out_schema);
    TupleDescription  out_td(out_schema);
    set_out_description(0, out_td);
    
    DEBUG << "out stream " << *out_stream << " in stream " << get_in_stream(0);

    for (uint32  i = 0; i < _worker_count; ++i)
    {   set_out_description(i, out_stream); 
    } 


    TupleDescription  in_td(get_in_schema(0));

    the_field_index = in_td.index_of_field(string("array"));
    _array_offset   = in_td.get_offset(the_field_index);

    // TODO: fix this - the name "tile" is stuck to the input schema
    DEBUG << "ScatterQBox input schema "  << get_in_schema(0)->as_string();
    DEBUG << "ScatterQBox output schema " << get_out_schema(0)->as_string();
}



////////////////////////////////////////////////////////////////////////////////
//
void ScatterQBox::init_impl() throw (AuroraException)
{
    TupleDescription  out_td(get_out_schema(0));
    TupleDescription  in_td(get_in_schema(0));
//..............................................................................


    _output_tuple_size = out_td.get_size();
    _input_tuple_size  = in_td.get_size();

}



////////////////////////////////////////////////////////////////////////////////
//
void ScatterQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{
    void  *out_data;     
//..............................................................................


    // Track number of tuples that get output
    //
    DeqIterator  my_deq_iterator = deq(0);

    TupleDescription  in_td(get_in_schema(0));
    TupleDescription  out_td(get_out_schema(0));

    // Do stuff...
    //
    while (inv.continue_dequeue_on(my_deq_iterator, 0))
    {
        const  void  *data = my_deq_iterator.tuple();
        Tuple  input_tuple = Tuple::factory(data);

        DEBUG << " ScatterQBox " << get_name() << " IN: " << in_td.tuple_to_string(data);
        DEBUG << "  BYTES " << to_hex_string(data, _output_tuple_size);

        // assuming a single-port Scatter is used in LS
        //
        if (input_tuple.get_fake_flag())             
        {
            memcpy(enq(0).tuple(), data, _output_tuple_size);

            DEBUG << " ScatterQBox " << get_name()                // run
                  << " OUT(0): " << out_td.tuple_to_string(data);

            ++ (enq(0));
            //outputted[0] = true;
            DEBUG << " -> through port " << 0;    // Run
            ++my_deq_iterator;
            continue;
        }

        for (unsigned int i = 0; i < _worker_count; ++i)
        {
            out_data = create_output_tuple(data, i);

            DEBUG << " ScatterQBox " << get_name()
                  << " OUT(" << i << "): " << out_td.tuple_to_string(data);

            DEBUG << "BYTES: " << to_hex_string(out_data, _output_tuple_size);
            memcpy(enq(i).tuple(), out_data, _output_tuple_size);
                    
            ++(enq(i));
            //passed = true;
            //outputted[i] = true;

            DEBUG << " -> through port " << i;
        }

        ++my_deq_iterator;
    }

    // Done, notify everyone where appropriate (note, doing it at the end may introduce latency, so perhaps
    //  one could explore various timings at which to notify) -- eddie
    //
    for (unsigned int i = 0; i < _worker_count; ++i)
    {   {    get_output(i).notify_enq();
        }
    }

    ++_seq_no;
}



////////////////////////////////////////////////////////////////////////////////
//
void ScatterQBox::set_pending_box_state(ptr<AbstractBoxState> packed_box)
{
//..............................................................................


    //ScatterBoxState* actual_packed_box = static_cast<ScatterBoxState*>(packed_box.get());
    //_pending_box_state.set(new ScatterBoxState(*actual_packed_box));
    //
    _pending_box_state = packed_box.dynamic_cast_to<ScatterBoxState>();
}



////////////////////////////////////////////////////////////////////////////////
//
ptr<AbstractBoxState> ScatterQBox::pack_state(bool delta_only)
{
//..............................................................................


  INFO << "Packing state for ScatterQBox named [" << get_name() << "]";
  ptr<ScatterBoxState> Scatter_box_state(new ScatterBoxState());

  return  Scatter_box_state;
}



////////////////////////////////////////////////////////////////////////////////
//
void *ScatterQBox::create_output_tuple(const void *data, uint32 i)
{
    uint8   *new_data = new uint8[_output_tuple_size];
    uint8   *addr;
    uint8   *source;

    uint32  *current_position;
    uint32  *element;
    uint32   chunk, id, slice;

    DynamicArray  *new_array;
    DynamicArray  *in_array;
//..............................................................................

    
    // TODO: make this a list, iterate over elements, save myself the constant checks
    //
    in_array = (DynamicArray *) (*(uint32 *) ((uint8 *)data) + _array_offset);
            
    addr = new uint8[4];
    source = ((uint8 *)data) + _array_offset; 
    memcpy(addr, source, 4);
            
    in_array = (DynamicArray*) (*(uint32 *) addr);
    delete [] addr;

    // deep copy the array because we are splitting streams
    //
    new_array = halve_array(in_array, i);
    memcpy(new_data, data, _input_tuple_size);

    element  = (uint32 *)((uint8 *)new_data + _array_offset);                 
    *element = (uint32) new_array;

    current_position = (uint32 *)(new_data + _slice_offset);

    *current_position = i;
    current_position++;

    *current_position = _seq_no;
    current_position++;

    *current_position = 2; // the number of slices
    
    slice = *(uint32 *)(new_data + 57);
    id    = *(uint32 *)(new_data + 61);
    chunk = *(uint32 *)(new_data + 65);

    DEBUG << "Outgoing tuple fields: slice: " << slice << " id " << id << " chunk " << chunk;
    return  (void *)new_data;
}



////////////////////////////////////////////////////////////////////////////////
//
DynamicArray *ScatterQBox::clone_array(DynamicArray *orig)
{
    SchemaArray   *array_info;
    int32          array_size;
    DynamicArray  *clone;
//..............................................................................

  
    const SchemaField *_array_found = (*get_in_schema(0)).get_schema_field(_array_field);

    array_info = _array_found->get_array();
    array_size = array_info->array_size();

    clone = DynamicArray::allocate_array(array_size);
    clone->set_count_size(1);

    //uint8 *old_first_idx = (uint8 *) orig + DynamicArray::header_size();
    //uint8 *new_first_idx = (uint8 *) clone + DynamicArray::header_size();

    memcpy((uint8 *) clone, (uint8 *) orig, array_size + DynamicArray::header_size());

    return  clone;
}



////////////////////////////////////////////////////////////////////////////////
//
DynamicArray *ScatterQBox::halve_array(DynamicArray *orig, int out_stream)
{
    uint32   header_size;
    uint8   *target;
    uint8   *source;

    DynamicArray *clone;
//..............................................................................


    clone = DynamicArray::allocate_array(CLONE_SIZE);

    clone->set_count_size(1);
    header_size = DynamicArray::header_size();

    if (out_stream == 0)
    {
        target = ((uint8 *)clone) + header_size;
        source = ((uint8*)orig) + header_size;
        memcpy(target, source, CLONE_SIZE); 
    }
    else
    {
        target = ((uint8 *)clone) + header_size;
        source = ((uint8*)orig) + header_size + CLONE_SIZE;
        memcpy(target, source, CLONE_SIZE);
    }

    return  clone;
}



////////////////////////////////////////////////////////////////////////////////
//
// takes in current tuple and expression number in set
// TODO: set a flag to tell whether to drop the tuple entirely b/c every time failed
// Does this conform with the semantics?  Is it better to pass on a nulled out array?
//
void *ScatterQBox::iterate_over_and_null_vals(const void  *data,
                                              int  i)
{
    DataType        element_type;
    CatalogSchema   my_schema;
    CatalogSchema  *element_schema;
    SchemaArray    *array_info;
    DynamicArray   *current_array;

    uint32   element_size;

    uint8    null_filler[element_size];
    uint8   *array_ptr;
    uint8   *current_element;

    void    *new_tuple;
//..............................................................................

       
    my_schema = *get_in_schema(0);
    const SchemaField *array_found = my_schema.get_schema_field(_array_field);

    array_info = array_found->get_array();
    _array_offset = array_found->get_offset();

    new_tuple = create_output_tuple(data, i);
    current_array = (DynamicArray *) *(uint32 *) (((char *)new_tuple) + _array_offset);

    array_ptr = (uint8 *) current_array;
    DEBUG << "Array @ " << array_ptr;

    current_array->save_jump(array_info);
    current_element = current_array->first_position();

    if (array_info->get_element_type() == DataType::NONE)
    {
        element_schema = array_info->get_tuple();
        element_size   = element_schema->get_size();
    }
    else
    {   element_type = array_info->get_element_type(); 
        element_size = element_type.get_size();
    }
    
    for (unsigned int x = 0; x < element_size; ++x)
    {   null_filler[x] = '\0';
    }

    /************** What is this doing???
    while (current_element != 0)
    {
        if (!_expressions[i]->eval<bool>(_ctxt))
        {
            memcpy(current_element, null_filler, element_size);
        }

        current_array->increment_iterator();
        current_element = current_array->get_iterator_element();
        ++counter;
    }
    *********************/

    return  new_tuple;
}


BOREALIS_NAMESPACE_END
