#include "ConcatenateQBox.h"
#include "tupleGenerator.h"

BOREALIS_NAMESPACE_BEGIN

void ConcatenateQBox::setup_impl() throw (AuroraException)
{
    if (get_num_inputs() != 1)
    {
        Throw(aurora_typing_exception,
              "Concatenate requires exactly one input stream (not " +
              to_string(get_num_inputs()) + ")");
    }
    
        _max_z = 3;
        typed_param("real_x", _max_x);
        typed_param("real_y", _max_y);
        typed_param("window_size", _window_size);
        typed_param("slide", _slide);
        typed_param("overlap", _overlap);
        if(_window_size < _slide)
            FATAL << "Window size " << _window_size << " must be greater than slide " << _slide;
        _out_y = _max_y*_window_size - _overlap*(_window_size - 1);
        ostringstream out_y_str;
        out_y_str << _out_y; 
        vector<SchemaField> output_fields;
        

        const vector<SchemaField> &fields =
            get_in_schema( 0 )->get_schema_field();

     
        copy( fields.begin(), fields.end(),
              back_inserter( output_fields ));
   
       // TODO: check to see if this will give us the correct offset
        SchemaField img_field("scroll_frame", DataType::ARRAY, 4, get_tuple_header_size());
        SchemaArray *img_info = new SchemaArray(DataType::BYTE, 1, NULL);
        img_info->add_dimension(0, 1023);
        img_info->add_dimension(0, _out_y);  // out_y_str.str());
        img_info->add_dimension(0, 2);
        img_field.set_array(img_info);
        output_fields[0] = img_field;
    
    TupleDescription  td( output_fields.begin(), output_fields.end() );
    DEBUG << get_name() << " output description: " << td;
    set_out_description( 0, td );
    _slide_count = 0;
    _first_flag = 1;
    _tuple_count = 0;
    _cache_size = _max_x*_out_y*_max_z;
    DEBUG << "Cache size: " << _cache_size << " height: " << _out_y;
    _cached_view = DynamicArray::allocate_array(_cache_size);
    _cached_view->set_count_size(1, 1);
    _cached_view->set_boundary(_cache_size);
    _cached_view_values = ((uint8 *) _cached_view) + DynamicArray::header_size();
    _cached_view_pos = _cached_view_values;
    
    CatalogSchema in_schema = *get_in_schema(0);
    const SchemaField *array_field = in_schema.get_schema_field("tri_frame");
    _in_array_offset = array_field->get_offset();
    DEBUG << "In array offset: " << _in_array_offset; 
    
    CatalogSchema out_schema = *get_out_schema(0);
    const SchemaField *out_field = out_schema.get_schema_field("scroll_frame");
    _out_array_offset = out_field->get_offset();
    DEBUG << "Out array offset: " << _out_array_offset;
    
    _full_frame_size = _max_x*_max_y*_max_z;
    _partial_frame_size = _max_x*(_max_y - _overlap)*_max_z;
    _tuple_count = 0;
    
    TupleDescription out_td(get_out_schema(0));
    _output_tuple_size = out_td.get_size();
}

void ConcatenateQBox::init_impl() throw (AuroraException)
{
}

void ConcatenateQBox::print_cvv()
{
        uint32 throwaway_size = (_slide - 1)*_full_frame_size + _partial_frame_size;
    uint32 buffer_size = _cache_size - throwaway_size;
    uint8 *start_cache_pos = _cached_view_values + throwaway_size;
    DEBUG << "cvv at start: " << (int) start_cache_pos[0] << " end " << (int) start_cache_pos[buffer_size-1];
}
void ConcatenateQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{

    DeqIterator di = deq(0);
    EnqIterator ei = enq(0);
    DynamicArray *in_array;
    uint8 *in_values, *in_pos;
    TupleDescription in_desc(  get_in_schema( 0 ));
    TupleDescription out_desc( get_out_schema( 0 ));

    while (inv.continue_dequeue_on(di, 0))
    {
        const void *data = di.tuple();
        Tuple input_tuple = Tuple::factory(data);
        
        DEBUG << "ConcatenateQBox " << get_name()
            << " IN: " << in_desc.tuple_to_string(data);
        in_array = (DynamicArray *) (*(uint32 *) ((((char *)data) + _in_array_offset)));
        in_values = ((uint8 *) in_array) + _in_array_offset;
    DEBUG << "Adding frame!";
        if(_tuple_count < (_window_size - _slide))
        {
            // unconditionally fill it
            if(_first_flag)
            {
                memcpy(_cached_view_pos, in_values, _full_frame_size);
                _first_flag = false;
                _cached_view_pos += _full_frame_size;
                ++_tuple_count;
            } 
            else
            {
                in_pos = in_values + _max_x * _overlap * _max_z;
                memcpy(_cached_view_pos, in_pos, _partial_frame_size);
                _cached_view_pos += _partial_frame_size;
                ++_tuple_count;
            }
        DEBUG << "Did simple fill.";
        }
        else  // have first "starter" values before slider interval
        {
            in_pos = in_values + _max_x * _overlap * _max_z;
                memcpy(_cached_view_pos, in_pos, _partial_frame_size);
            ++_slide_count;
            _cached_view_pos += _partial_frame_size;
            // we are full enough and ready to output
            DEBUG << "Standard slide!";
        
        if(_slide_count == _slide) 
            {
                DynamicArray *out_array = DynamicArray::allocate_array(_cache_size);
                out_array->set_count_size(1, 1);
                out_array->set_boundary(_cache_size);
                uint8 *out_array_values = ((uint8 *) out_array) + DynamicArray::header_size();
                memcpy(out_array_values, _cached_view_values, _cache_size); 
                
                // output the new one
                uint8  *pos = (uint8 *) enq(0).tuple();
                memcpy(enq(0).tuple(), di.tuple(), _output_tuple_size);
                uint32 *array_pos = (uint32 *) (pos + _out_array_offset);
                *array_pos = (uint32) out_array;
                DEBUG << "ConcatenateQBox " << get_name()
                      << " OUT: " << out_desc.tuple_to_string(ei.tuple());
                ++ei;
                _cached_view->modify_array_reference(1);
                get_output(0).notify_enq();         
                // move the previous contents up by the slide amount and move pointers accordingly
                slide_up_and_reset();           
            }
        }

        
        
        ++di;
    }

    
}

void ConcatenateQBox::slide_up_and_reset()
{
    uint32 throwaway_size = (_slide - 1)*_full_frame_size + _partial_frame_size;
    uint32 buffer_size = _cache_size - throwaway_size;
    
    uint8 *buffer = new uint8[buffer_size];
    
    
    DEBUG << "Memcpying " << buffer_size << " bytes.";
    
    
    memcpy(buffer, _cached_view_values + throwaway_size, buffer_size);
    memcpy(_cached_view_values, buffer, buffer_size);
    uint32 new_offset = _cache_size - _partial_frame_size*_slide;
    _cached_view_pos = _cached_view_values + new_offset;
    _slide_count = 0;

    delete [] buffer;
    DEBUG << "Reset completed!";
}

BOREALIS_NAMESPACE_END
