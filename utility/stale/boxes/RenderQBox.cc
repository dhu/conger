#include "RenderQBox.h"
#include <Magick++.h>
#include <math.h>

BOREALIS_NAMESPACE_BEGIN



void RenderQBox::load_map()
{
   MAP_WIDTH = 640;
  MAP_HEIGHT = 480;
  MAP_DEPTH = 3;

  MAP_SIZE = MAP_HEIGHT*MAP_WIDTH*MAP_DEPTH;
  _filename = "/pro/borealis/data/demo/array/us_map.raw";

    ifstream fin(_filename.c_str(), ios::in | ios::binary);
    fin.read((char *)_original_map, MAP_SIZE);
    if (fin.fail())
    {
        cout << "Read on " << _filename << " failed.\n";
        return;
    }
    else 
      fin.close();
}

void RenderQBox::setup_impl() throw (AuroraException)
{
    const SchemaField* array_field;
    SchemaField curr_field;
    CatalogSchema schema;
    SchemaArray *array_info;
    
    if ( get_num_inputs() != 1 && get_num_outputs() != 1 )
    {   Throw(aurora_typing_exception,
              "Render requires exactly one input stream (not " +
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
    int a_field;
    for(unsigned int x = 0; x < schema._field.size(); ++x)
      {
        curr_field = schema._field[x];
        if(curr_field.get_type() == DataType::ARRAY && array_rewrite == true)
            FATAL << "Render currently supports only one array per schema.";
        else if(curr_field.get_type() == DataType::ARRAY)
        {
            _array_name = curr_field.get_name();
            array_rewrite = true;
            a_field = x;
            DEBUG << "Array found: "  << _array_name;
        }
      }
      if(!array_rewrite) // no array found
        FATAL << "Render requires at least one array.";
    // Set input and output descriptions
    //set_out_description( 0, get_in_stream( 0 )); // same output type as input everywhere
    
    schema = *get_in_schema(0);
    array_field = schema.get_schema_field(_array_name);
    _array_offset = array_field->get_offset();
    array_info = array_field->get_array();
    
    CatalogSchema *out_schema = new CatalogSchema;
    out_schema->clone(*get_in_schema(0));
    out_schema->set_schema_name("rendered_map");
    SchemaField map_field("map", DataType::ARRAY, 4, get_tuple_header_size());
    SchemaArray *map_info = new SchemaArray(DataType::BYTE, 1, NULL);
    map_info->add_dimension(0, 639);
    map_info->add_dimension(0, 479);
    map_info->add_dimension(0, 2);
    map_field.set_array(map_info);
    out_schema->_field[0] = map_field;
    
    
    TupleDescription out_td(out_schema);
    // Field must match position of array in incoming stream
    
     
    set_out_description(0, out_td);

    load_map();    
    string out_array_name;
    typed_param("out_array", out_array_name);
    // out array = dense array + timestamp
    array_field = out_schema->get_schema_field(out_array_name);
    _out_array_offset = array_field->get_offset();
    schema = *get_in_schema(0);
    array_field = schema.get_schema_field(_array_name);
    _array_offset = array_field->get_offset();
    _tuple_schema = array_field->get_array();
    
    
    
    // get offsets for x, y fields
    /*array_schema = array_info->get_tuple();
    x_field = array_schema->get_schema_field(_x_field_name);
    _x_offset = x_field->get_offset() - get_tuple_header_size();
    y_field = array_schema->get_schema_field(_y_field_name);
    _y_offset = y_field->get_offset() - get_tuple_header_size();
    */
        
    DEBUG << "RenderQBox input schema " << get_in_schema(0)->as_string();
    DEBUG << "RenderQBox output schema " << get_out_schema(0)->as_string();
}

void RenderQBox::init_impl() throw (AuroraException)
{
  TupleDescription out_td(get_out_schema(0));
  _output_tuple_size = out_td.get_size();

}

void RenderQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{

    DeqIterator my_deq_iterator = deq(0);
    uint32 *element;
    DynamicArray *positions;
    // Do stuff...
    while (inv.continue_dequeue_on(my_deq_iterator, 0))
    {

        const void *data = my_deq_iterator.tuple();
    
        positions = (DynamicArray *) (*(uint32 *) ((((char *)data) + _array_offset)));
        vector<uint32> points_in_bb;
        DynamicArray *out_array = DynamicArray::allocate_array(MAP_SIZE);
        out_array->set_count_size(1,1);
        uint8 *first_element = ((uint8 *) out_array) + DynamicArray::header_size();
        
        memcpy(first_element, _original_map, MAP_SIZE);
        
        DEBUG << "array starting with: " << to_hex_string(((uint8 *) positions) + DynamicArray::header_size(), 20);
        update_cache(positions);
        render_points(out_array);
        memcpy(enq(0).tuple(), data, _output_tuple_size);
        // set element val here
        element = (uint32 *)((uint8 *)enq(0).tuple() + _out_array_offset);
        *element  = (uint32) out_array;
        ++(enq(0));
        get_output(0).notify_enq();
        ++(deq(0));
    }
}

void RenderQBox::update_cache(DynamicArray *updates)
{
    ship_tuple *current_rec = (ship_tuple *) updates->first_position();
    updates->save_jump(_tuple_schema);
    
    while(current_rec != 0  ) 
    {
        ship_position sp;
        sp.x = (uint32) current_rec->x;
        sp.y = (uint32) current_rec->y;
        if(0 <= sp.x && sp.x < MAP_WIDTH && 0 <= sp.y && sp.y < MAP_HEIGHT)
            _cached_ships[current_rec->id] = sp;
        updates->increment_iterator();
        current_rec = (ship_tuple *) updates->get_iterator_element();
    }
    DEBUG << "Exited build chunks";
}


void RenderQBox::render_points(DynamicArray *out_array)
{
    ship_cache::iterator cache_pos = _cached_ships.begin();
    ship_position coordinate;
    uint32 start_x, start_y;
    
    while(cache_pos != _cached_ships.end())
    {
        coordinate = cache_pos->second;
        ostringstream the_id_str;
        the_id_str << cache_pos->first;
        start_x = (uint32) coordinate.x + 2;
        start_y = (uint32) coordinate.y - 5; 
        
        fill_point(out_array, coordinate.x, coordinate.y);
        
        draw_id(out_array, the_id_str.str(), start_x, start_y); 
        ++cache_pos;    
    }
}

// upper left corner of the id
void RenderQBox::draw_id(DynamicArray *out_array, string id, uint32 start_x, uint32 start_y)
{
    try
    {
        uint32 effective_width;
        uint32 img_width = id.length()* 7;
        uint32 img_height = 12;
        if(start_x + img_width > MAP_WIDTH)
            effective_width = MAP_WIDTH - start_x;
        else
            effective_width = img_width;
        Magick::Image render(Magick::Geometry(img_width, img_height), Magick::Color("white"));
        render.strokeColor("red");
        render.fillColor(Magick::ColorRGB(11, 87, 228));
        //render.font("Sans");
        render.fontPointsize(11);
        Magick::ColorRGB pix_color;
        double r, g, b;
            uint32 offset = (MAP_WIDTH*start_y + start_x)*MAP_DEPTH;
        uint8 *curr_pos = ((uint8 *) out_array) + DynamicArray::header_size() + offset;
        uint32 jump_size = (MAP_WIDTH - effective_width)*MAP_DEPTH;
        char single_pix;  
        // first copy out background of ROI
        for(uint32 y=0; y < img_height; ++y)
        {
            for(uint32 x=0; x < effective_width; ++x)
            {
               // "safe" casting is time consuming and expensive
               // TODO: look for a better way to do this! 
               single_pix = *curr_pos;
               // range of pixel = 0.0...1.0
               r = static_cast<double>(single_pix) / 255.0;
               ++curr_pos;
               
               single_pix = *curr_pos;
               g = static_cast<double>(single_pix) / 255.0;;
               ++curr_pos;
               
               single_pix = *curr_pos;
               b = static_cast<double>(single_pix) / 255.0;
               ++curr_pos;
               
               pix_color = Magick::ColorRGB(r, g, b);
               render.pixelColor(x, y, pix_color);     
            }
            curr_pos += jump_size;
        }
        
        render.draw(Magick::DrawableText(0,10, id));
        Magick::Pixels view(render);
        Magick::PixelPacket *pixels = view.get(0,0, effective_width, img_height);
        curr_pos = ((uint8 *) out_array) + DynamicArray::header_size() + offset;
        double pix_sample;
        //render.display();
        //sleep(2);
        // copy it back to the map
        for(uint32 y=0; y < img_height; ++y)
        {
            for(uint32 x=0; x < effective_width; ++x)
            {
                pix_color = *pixels;
                pix_sample = pix_color.red()*255.0;
                *curr_pos = static_cast<uint8>(pix_sample);     
                ++curr_pos;
                
                pix_sample = pix_color.green()*255.0;
                *curr_pos = static_cast<uint8>(pix_sample);     
                ++curr_pos;
                
                pix_sample = pix_color.blue()*255.0;
                *curr_pos = static_cast<uint8>(pix_sample);     
                ++curr_pos;
                
                ++pixels;
            }
            curr_pos += jump_size;
        }
    }
    catch( exception &error_ )
    {
        string my_error = error_.what();
        DEBUG << "Caught exception: " <<  my_error;
    
    }
    
     
}


void RenderQBox::fill_point(DynamicArray *array, uint32 x, uint32 y)
{
    //DEBUG << "Filling point: " << x << ", " << y;
    set_pixel_red(x, y, array);
    set_pixel_red(x+1, y, array);
    set_pixel_red(x-1, y-1, array);
    set_pixel_red(x, y+1, array);
    set_pixel_red(x-1, y-1, array);
    set_pixel_red(x+1, y-1, array);
    set_pixel_red(x-1, y+1, array);
    set_pixel_red(x+1, y+1, array);
}

// this can be better optimized with adjacent offsets later
void RenderQBox::set_pixel_red(uint32 x, uint32 y, DynamicArray *the_out)
{
    uint32 offset = (MAP_WIDTH*y + x)*MAP_DEPTH;
    uint8 *out_values = (uint8 *) (((uint8 *)the_out) + DynamicArray::header_size());

    *(out_values + offset) = 255;
    *(out_values + offset+1) = 0;
    *(out_values + offset+2) = 0;
}


void RenderQBox::set_pending_box_state(ptr<AbstractBoxState> packed_box)
{
    //RenderBoxState* actual_packed_box = static_cast<RenderBoxState*>(packed_box.get());
    //_pending_box_state.set(new RenderBoxState(*actual_packed_box));
    _pending_box_state = packed_box.dynamic_cast_to<RenderBoxState>();
}

ptr<AbstractBoxState> RenderQBox::pack_state(bool delta_only)
{
  INFO << "Packing state for RenderQBox named [" << get_name() << "]";
  ptr<RenderBoxState> filter_box_state(new RenderBoxState());
  return filter_box_state;
}



BOREALIS_NAMESPACE_END
