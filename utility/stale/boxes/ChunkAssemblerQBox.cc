#include "ChunkAssemblerQBox.h"
#include <math.h>


BOREALIS_NAMESPACE_BEGIN

void ChunkAssemblerQBox::setup_impl() throw (AuroraException)
{
    int dim_size;
    SchemaArray *the_array_info;

    vector<SchemaArray::DimensionInformation> theDimInfo;
    
    if ( get_num_inputs() != 1 )
    {   Throw(aurora_typing_exception,
              "ChunkAssembler requires exactly one input stream (not " +
              to_string(get_num_inputs()) + ")");
    }


    set_out_description( 0, get_in_stream( 0 ));
 


    DEBUG << "ChunkAssemblerQBox input schema " << get_in_schema(0)->as_string();
    DEBUG << "ChunkAssemblerQBox output schema " << get_out_schema(0)->as_string();

    TupleDescription in_td(get_in_schema(0));
    int the_field_index = in_td.index_of_field(string("frame"));
    SchemaField *the_array = const_cast<SchemaField *>(in_td.get_schema_field(the_field_index));
    the_array_info  = the_array->get_array();
    _array_size = the_array_info->array_size(); 
    theDimInfo = the_array_info->get_all_dimensions();
    
    _frame_offset = in_td.get_offset(the_field_index);
    
    for(unsigned int x = 0; x < theDimInfo.size(); ++x)
      {
        dim_size = (uint32)(theDimInfo[x]._upper - theDimInfo[x]._lower) + 1;
        _array_dimensions.push_back(dim_size);
      }

    // the cache holds values and we ship out a copy of it when the frame bit arrives
    // will need to include a bounding box for region of change in schema
    _cached_array = (DynamicArray *) malloc(DynamicArray::header_size() + _array_size);
    _cached_array->set_count_size();
    _chunks_in_each_dimension.push_back(8);
    _chunks_in_each_dimension.push_back(6);
    _chunks_in_each_dimension.push_back(1);
    _num_chunks = 48;
}


ChunkAssemblerQBox::~ChunkAssemblerQBox()
{
    _cached_array->modify_array_reference(-1);
}

void ChunkAssemblerQBox::init_impl() throw (AuroraException)
{
  TupleDescription out_td(get_out_schema(0));
  _output_tuple_size = out_td.get_size();

}

void ChunkAssemblerQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{
    DEBUG << "Running ChunkAssembler!";

  uint32 *element;

    DeqIterator my_deq_iterator = deq(0);

    TupleDescription in_td(get_in_schema(0));
    TupleDescription out_td(get_out_schema(0));

    // Do stuff...
    while (inv.continue_dequeue_on(my_deq_iterator, 0))
    {

        const void *data = my_deq_iterator.tuple();
        //Tuple input_tuple = Tuple::factory(data);

       // unconditionally pass through
       memcpy(enq(0).tuple(), data, _output_tuple_size);
      DynamicArray *the_chunk = (DynamicArray *) (*(uint32 *) ((((char *)data) + _frame_offset)));
      DynamicArray *the_out_array = (DynamicArray *)malloc(DynamicArray::header_size() + _array_size);
        
      DEBUG << "Received chunk #: " << the_chunk->_chunk_number;
      /*uint8 * the_chunk_vals = (uint8 *) (the_chunk + DynamicArray::header_size());
      uint8 *_cached_array_vals = (uint8 *) (_cached_array + DynamicArray::header_size());
      for(int x = 0; x < 44; ++x)
        {
            for(int y = 0; y < 48; ++y)
            {
                *(_cached_array_vals + x*3+y*352*3) = *(the_chunk_vals +x*3+y*44*3); 
                *(_cached_array_vals + x*3+y*352*3+1) = *(the_chunk_vals +x*3+y*44*3+1);
                *(_cached_array_vals + x*3+y*352*3+2) = *(the_chunk_vals +x*3+y*44*3+2);
            }
        }           
      */
      map_chunk(the_chunk);
      if(the_chunk->_frame_bit)
      {
        memcpy(the_out_array, _cached_array, DynamicArray::header_size() + _array_size);
        element = (uint32 *)((uint8 *)enq(0).tuple() + _frame_offset);
        *element  = (uint32) the_out_array;
        ++(enq(0));
        get_output(0).notify_enq();
      }   
      ++my_deq_iterator;
      // sleep(1);
    }
    
 }

void ChunkAssemblerQBox::add_coord(uint32 dim, uint32 n, uint32 &g)
{

  int theCoordinate;

  if(n == 0 && dim == 0)
    {
      _the_coords.insert(_the_coords.begin(), 0);
      return;
    }

  if(n == 0)
    {
      _the_coords.insert(_the_coords.begin(), 0);
      add_coord(dim-1, n, g);
      return;
    }

  if(dim == 0)
    {

      theCoordinate = n;
      _the_coords.insert(_the_coords.begin(), theCoordinate);
      return;
    }
  else
    {
      g = (int) floor(g / _chunks_in_each_dimension[dim]);
      theCoordinate = (int) floor(n / g);
      n = n - theCoordinate*g;
      _the_coords.insert(_the_coords.begin(),theCoordinate);
      add_coord(dim-1, n, g);
    }
}


void ChunkAssemblerQBox::map_chunk(DynamicArray *the_chunk)
{
  char *cache_vals = (char *)(_cached_array + DynamicArray::header_size()); 
  char *chunk_vals = (char *)(the_chunk + DynamicArray::header_size()); 

  uint32 start_x, start_y, start_z;
  uint32 chunk_x, chunk_y, chunk_z;
  
  uint32 chunk_offset, cache_offset;

  DEBUG << "Mapping chunk!";
  uint32 g = _num_chunks;
  _the_coords.clear();
  add_coord(2, the_chunk->_chunk_number, g);
  DEBUG << "Chunk number: " << the_chunk->_chunk_number << " corresponds to coords: " << _the_coords[0] << ", " << _the_coords[1] << ", "
    << _the_coords[2];
  chunk_x = _array_dimensions[0]/_chunks_in_each_dimension[0];
  chunk_y = _array_dimensions[1]/_chunks_in_each_dimension[1];
  chunk_z = _array_dimensions[2]/_chunks_in_each_dimension[2];

  start_x = chunk_x*_the_coords[0];
  start_y = chunk_y*_the_coords[1];
  start_z = chunk_z*_the_coords[2];
  DEBUG << "Start coords: " << start_x << ", " << start_y << ", " << start_z;
   for(unsigned int y = 0; y < chunk_y; ++y)
    {
      for(unsigned int x = 0; x < chunk_x; ++x)
        {
          for(unsigned int z = 0; z < chunk_z; ++z)
           {
            cache_offset = (_array_dimensions[0]*(start_y+y)+(start_x+x))*_array_dimensions[2] + start_z+z;
            chunk_offset = chunk_z*x + chunk_x*chunk_z*y+z;
            *(cache_vals + cache_offset) = *(chunk_vals + chunk_offset);   
            }
      }
    }
  
}



void ChunkAssemblerQBox::set_pending_box_state(ptr<AbstractBoxState> packed_box)
{
    //ChunkAssemblerBoxState* actual_packed_box = static_cast<ChunkAssemblerBoxState*>(packed_box.get());
    //_pending_box_state.set(new ChunkAssemblerBoxState(*actual_packed_box));
    _pending_box_state = packed_box.dynamic_cast_to<ChunkAssemblerBoxState>();
}

ptr<AbstractBoxState> ChunkAssemblerQBox::pack_state(bool delta_only)
{
  INFO << "Packing state for ChunkAssemblerQBox named [" << get_name() << "]";
  ptr<ChunkAssemblerBoxState> filter_box_state(new ChunkAssemblerBoxState());
  return filter_box_state;
}


BOREALIS_NAMESPACE_END
