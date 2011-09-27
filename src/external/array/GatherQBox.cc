#include "GatherQBox.h"

BOREALIS_NAMESPACE_BEGIN

#define   CLONE_SIZE   160


////////////////////////////////////////////////////////////////////////////////
//
void GatherQBox::setup_impl() throw (AuroraException)
{
    int32  input_size;

    SchemaField array_field("array", DataType::ARRAY, 4,
                            get_tuple_header_size());

    SchemaArray    *array_info;
    SchemaField    *out_field;
    CatalogSchema  *out_schema;
    CatalogSchema   out_schema_check;
    CatalogSchema   first_in_schema;
//..............................................................................


    // Gather is the magic can-take-any-number-of-inputs box!
    //
    _worker_count = get_num_inputs();
    DEBUG  << "worker=" << _worker_count;

    //size_t input_size = getInDescription(0)->getSize();
    input_size = get_in_schema(0)->get_size() + HEADER_SIZE;

    // FOR FUTURE NOTE - The check should also check types, and also check field *order*
    //
    for (uint32  i = 1; i < _worker_count; ++i)
    {
        //if (input_size != getInDescription(i)->getSize())
        if (input_size != (get_in_schema(i)->get_size() + HEADER_SIZE))
        {   Throw(aurora_typing_exception,
                  "Gather inputs must all have the same schema");
        }
    }

    // set the output stream's type, same as input
    //    set_out_description(0, get_in_stream(0));
    //
    array_info = new SchemaArray(DataType::BYTE, 1, NULL);
    array_info->add_dimension(0, CLONE_SIZE * _worker_count - 1);
    array_field.set_array(array_info);

    out_schema = new CatalogSchema;
    out_schema->set_schema_name("frame");
    out_schema->_field.push_back(array_field);
     
    TupleDescription  out_td(out_schema);
    set_out_description(0, out_td);
  
    
    // checking that it all connected ok
    //
    out_schema_check = *get_out_schema(0);
    out_field = const_cast<SchemaField *>(out_schema_check.get_schema_field("array"));
  
    if (out_field == NULL)
    {   FATAL << "Could not find out field!"; 
    }

    _out_array_offset = out_field->get_offset();
    first_in_schema = *get_in_schema(0);

    const SchemaField  *in_array = first_in_schema.get_schema_field("array");
    _in_array_offset = in_array->get_offset();
    
    DEBUG << "GatherQBox input schema "  << get_in_schema(0)->as_string();
    DEBUG << "GatherQBox output schema " << get_out_schema(0)->as_string();
}



////////////////////////////////////////////////////////////////////////////////
//
void GatherQBox::init_impl() throw (AuroraException)
{
//..............................................................................


    // all streams must have same schema, so no problem doing this
    TupleDescription out_td(get_out_schema(0));
    _output_tuple_size = out_td.get_size();
}



////////////////////////////////////////////////////////////////////////////////
//
void GatherQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{
    bool      outputted = false;
    uint32    chunk, id, slice;

    uint8    *pos;
    uint8    *output_values;
    uint8    *data;
    uint8    *source;
    uint8    *addr;
    uint32   *array_pos;

    DynamicArray     *incoming_array;
    DynamicArray     *output_array;

    EnqIterator       my_enq_iterator = enq(0);
    DeqIterator       my_deq_iterator;

    TupleDescription  t(get_in_schema(0));

//...............................................................................


    // Do stuff, the greedy way (suck all tuples from each stream in succession)
    //
    for (unsigned int i = 0; i < _worker_count; ++i)  // For each stream to dequeue from...
    {   
        my_deq_iterator = deq(i);

        while (inv.continue_dequeue_on(my_deq_iterator, i))  // while I can still dequeue tuples from it...
        {   data = (uint8 *)const_cast<void *>(my_deq_iterator.tuple());

            DEBUG << " GatherQBox " << get_name()
                  << " IN: " << t.tuple_to_string(data);

            source = ((uint8 *)data) + _in_array_offset;
            addr = new uint8[4];
            memcpy(addr, source, 4);
            
            incoming_array = (DynamicArray*) (*(uint32 *) addr);
            delete [] addr;

            slice = *(uint32 *)(data + 57);
            id    = *(uint32 *)(data + 61);
            chunk = *(uint32 *)(data + 65);

            DEBUG << "Incoming tuple fields: slice: " << slice << " id " << id << " chunk " << chunk; 

            if (process_input_tuple(incoming_array, i))
            {
                outputted = true;
                output_array = DynamicArray::allocate_array(320);
                output_array->set_count_size(1);
                output_values = ((uint8 *) output_array) + DynamicArray::header_size();
                memcpy(output_values, _output_array, 320);
            
                pos = (uint8 *) enq(0).tuple();
                memcpy(pos, data, _output_tuple_size);
                array_pos = (uint32 *)(pos + _out_array_offset);
                
                *array_pos = (uint32) output_array;
                
                ++my_enq_iterator;
            }

            ++my_deq_iterator;
        }
    }

    // Done, notify everyone (note this later should be done at some other moment than just at the end
    //
    if (outputted)  get_output(0).notify_enq();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// takes an input and returns whether or not it has enqueued anything
//
bool  GatherQBox::process_input_tuple(DynamicArray  *array,
                                      uint32         input)
{
    uint8   lhs[CLONE_SIZE], rhs[CLONE_SIZE];

    uint8  *queue_contents;
    uint8  *data;
    uint8  *insert;
//..............................................................................


    data = ((uint8 *) array) + DynamicArray::header_size();

    if (input)
    {
        insert = new uint8[CLONE_SIZE];
        memcpy(insert, data, CLONE_SIZE);
        _rhs_arrays.push(insert);
    }  
    else
    {   insert = new uint8[CLONE_SIZE];
        memcpy(insert, data, CLONE_SIZE);
        _lhs_arrays.push(insert);
    }

    if (!_lhs_arrays.empty()  &&  !_rhs_arrays.empty())
    {
        queue_contents = _rhs_arrays.front();
        memcpy(rhs, queue_contents, CLONE_SIZE);
        _rhs_arrays.pop();
        delete [] queue_contents;

        queue_contents = _lhs_arrays.front();
        memcpy(lhs, queue_contents, CLONE_SIZE);
        _lhs_arrays.pop();
        delete [] queue_contents;
      
        memcpy(_output_array, lhs, CLONE_SIZE);
        memcpy(_output_array + CLONE_SIZE, rhs, CLONE_SIZE);
      
        return  true;
    }
  
    return  false;
}


BOREALIS_NAMESPACE_END
