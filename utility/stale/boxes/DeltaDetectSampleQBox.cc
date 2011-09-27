#include "DeltaDetectSampleQBox.h"
#include <math.h>
#include "timer.h"

BOREALIS_NAMESPACE_BEGIN

void DeltaDetectSampleQBox::setup_impl() throw (AuroraException)
{
  int dim_size;
  SchemaArray *the_array_info;

  vector<SchemaArray::DimensionInformation> theDimInfo;
    if ( get_num_inputs() != 1 )
    {   Throw(aurora_typing_exception,
              "DeltaDetectSample requires exactly one input stream (not " +
              to_string(get_num_inputs()) + ")");
    }

    set_out_description( 0, get_in_stream( 0 ));

    DEBUG << "DeltaDetectSampleQBox input schema " << get_in_schema(0)->as_string();
    DEBUG << "DeltaDetectSampleQBox output schema " << get_out_schema(0)->as_string();

    typed_param("delta_threshold", _delta_threshold);
    typed_param("sample_jump", _jump_size);
    
    DEBUG << "Threshold: " << _delta_threshold;
    TupleDescription in_td(get_in_schema(0));
    int the_field_index = in_td.index_of_field(string("frame"));
    const SchemaField *the_array = in_td.get_schema_field(the_field_index);
    the_array_info  = the_array->get_array();
    _array_size = the_array_info->array_size(); 
    uint32 header_size = DynamicArray::header_size();
    _cached_array = DynamicArray::allocate_array(header_size+_array_size);
    _cached_array->set_count_size();
    theDimInfo = the_array_info->get_all_dimensions();    
    _frame_offset = in_td.get_offset(the_field_index);

    for(unsigned int x = 0; x < theDimInfo.size(); ++x)
      {
    dim_size = (uint32)(theDimInfo[x]._upper - theDimInfo[x]._lower) + 1;
    _array_dimensions.push_back(dim_size);
      }
    
    // Push in chunk dimensions
        int cid;
    _num_chunks = 1;    
    for(int x = 0; x < 3; ++x)
    {
        ostringstream the_param;
        
        the_param << "chunks_in_dimension." << x;
        typed_param(the_param.str().c_str(),cid);
        _chunks_in_each_dimension.push_back(cid);
        _num_chunks *= cid;
    }

    _chunk_x = _array_dimensions[0] / _chunks_in_each_dimension[0];  
    _chunk_y = _array_dimensions[1] / _chunks_in_each_dimension[1];
    _chunk_z = _array_dimensions[2] / _chunks_in_each_dimension[2];
    
    if(_chunks_in_each_dimension.size() != _array_dimensions.size())
      {
    NOTICE << "Require the same number of dimensions and chunks specified in each dimension.";
    exit(-1);
      }
    _chunk_size = _array_dimensions[0] / _chunks_in_each_dimension[0];

    for(unsigned int x = 1; x < _array_dimensions.size(); ++x)
      _chunk_size *= _array_dimensions[x] / _chunks_in_each_dimension[x];
    DEBUG <<  "chunk size " << _chunk_size;
    _first_flag = 1;
    _total_time = 0;
}

DeltaDetectSampleQBox::~DeltaDetectSampleQBox()
{
  _cached_array->modify_array_reference(-1);
}

void DeltaDetectSampleQBox::init_impl() throw (AuroraException)
{
  TupleDescription out_td(get_out_schema(0));
  _output_tuple_size = out_td.get_size();

}

void DeltaDetectSampleQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{
    uint32 *element;
    timer the_time;
    DeqIterator my_deq_iterator = deq(0);
    DynamicArray *new_chunk  = 0;
    TupleDescription in_td(get_in_schema(0));
    TupleDescription out_td(get_out_schema(0));

    // Do stuff...
    the_time.start();
    while (inv.continue_dequeue_on(my_deq_iterator, 0))
    {

        const void *data = my_deq_iterator.tuple();
    
    _current_array = (DynamicArray *) (*(uint32 *) ((((char *)data) + _frame_offset)));  
    new_chunk = 0;

    for(unsigned int x = 0; x < _num_chunks; ++x)
      {
        float theP = calculate_percent_changed(x);
    
        if(theP >= _delta_threshold)
          { 
            DEBUG << "Exceeded threshold, sending chunk!";
               map_chunk(x);
           // if we have a pre-existing value, send it
               if(new_chunk != 0)
               {
                    memcpy(enq(0).tuple(), data, _output_tuple_size);
                    element = (uint32 *)((uint8 *)enq(0).tuple() + _frame_offset);
                    
                    *element  = (uint32) new_chunk;
                    ++(enq(0));
                    get_output(0).notify_enq(); 
               }
               new_chunk = create_new_chunk(x); 
        
           } 
      } 

    if(new_chunk != 0)
       {
            // last one sent gets the frame bit
            new_chunk->_frame_bit = 1; 
            memcpy(enq(0).tuple(), data, _output_tuple_size);
            element = (uint32 *)((uint8 *)enq(0).tuple() + _frame_offset); 
                        
        *element  = (uint32) new_chunk;
            ++(enq(0));
            get_output(0).notify_enq();
     }
     ++my_deq_iterator;
     _first_flag = 0;
    }
    the_time.stop();
    _total_time += the_time.elapsed_time();
    NOTICE << "Total preprocessing time: " << _total_time;
 }

void DeltaDetectSampleQBox::set_pending_box_state(ptr<AbstractBoxState> packed_box)
{
    //DeltaDetectSampleBoxState* actual_packed_box = static_cast<DeltaDetectSampleBoxState*>(packed_box.get());
    //_pending_box_state.set(new DeltaDetectSampleBoxState(*actual_packed_box));
    _pending_box_state = packed_box.dynamic_cast_to<DeltaDetectSampleBoxState>();
}

ptr<AbstractBoxState> DeltaDetectSampleQBox::pack_state(bool delta_only)
{
  INFO << "Packing state for DeltaDetectSampleQBox named [" << get_name() << "]";
  ptr<DeltaDetectSampleBoxState> filter_box_state(new DeltaDetectSampleBoxState());
  return filter_box_state;
}


void DeltaDetectSampleQBox::addCoord(uint32 dim, uint32 n, uint32 &g)
{

  int theCoordinate;

  if(n == 0 && dim == 0)
    {
      theCoords.insert(theCoords.begin(), 0);
      return;
    }

  if(n == 0)
    {
      theCoords.insert(theCoords.begin(), 0);
      addCoord(dim-1, n, g);
      return;
    }

  if(dim == 0)
    {

      theCoordinate = n;
      theCoords.insert(theCoords.begin(), theCoordinate);
      return;
    }
  else
    {
      g = (int) floor(g / _chunks_in_each_dimension[dim]);
      theCoordinate = (int) floor(n / g);
      n = n - theCoordinate*g;
      theCoords.insert(theCoords.begin(),theCoordinate);
      addCoord(dim-1, n, g);
    }
}


float DeltaDetectSampleQBox::calculate_percent_changed(uint32 chunk_number)
{
  uint32 start_x, start_y, end_x, end_y, offset, row_jump, cutoff_idx;
  float set_length;
  float percent_changed = 0.0;
  uint8 *cache_pos, *in_pos;
  uint8 old_val, new_val;
  uint32 row_count, row_pos;


  uint8 *old_array_start_val = (((uint8 *)_cached_array) + DynamicArray::header_size());
  uint8 *new_array_start_val = (((uint8 *)_current_array) + DynamicArray::header_size());

  uint32 g = _num_chunks;
  theCoords.clear();
  addCoord(2, chunk_number, g);


    DEBUG << "Entering DD % changed";
  // 100% difference for first one
  if(_first_flag)
    return 1;

   set_length = _chunk_size / _jump_size;

  start_x = _chunk_x*theCoords[0];
  start_y = _chunk_y*theCoords[1];
  end_x = start_x + _chunk_x;
  end_y = start_y + _chunk_y;

  offset = (_array_dimensions[0]*start_y + start_x)*_array_dimensions[2];
  cache_pos = old_array_start_val + offset;
  in_pos = new_array_start_val + offset;
  row_jump = (_array_dimensions[0] - _chunk_x)*_array_dimensions[2];
  row_count = row_pos = 0;
  cutoff_idx = _chunk_x*_chunk_z;
 
 
  while(row_count != _chunk_y)
  {
        old_val = *cache_pos;
        new_val = *in_pos;
        percent_changed += fabs(old_val - new_val);
        cache_pos += _jump_size;
        in_pos  += _jump_size;
        row_pos  += _jump_size;

    
    if(row_pos >= cutoff_idx)
        {
                // on to the next row
                row_pos = 0;
                ++row_count;
                cache_pos = cache_pos + row_jump;
                in_pos = in_pos + row_jump;
        }

  }

  percent_changed /= set_length;
  percent_changed /= 256.0;
  DEBUG << "Percent changed for chunk " << chunk_number << ": " << percent_changed;
  return percent_changed;

}

void DeltaDetectSampleQBox::map_chunk(int chunk_number)
{
  DEBUG << "Mapping chunk " << chunk_number;
  char *cache_start_val = (char *)(((uint8 *)_cached_array) + DynamicArray::header_size()); 
  char *new_array_start_val = (char *)(((uint8 *) _current_array) + DynamicArray::header_size()); 

  uint32 start_x, start_y, start_z;
  //uint8 array_value;
  char *src, *dst;
  int row_size, array_row_size, offset;

  //DEBUG << "Mapping chunk!";
  //DEBUG << "Chunk number: " << chunk_number << " corresponds to coords: " << theCoords[0] << ", " << theCoords[1] << ", "
  //  << theCoords[2];
  
  _chunk_x = _array_dimensions[0]/_chunks_in_each_dimension[0];
  _chunk_y = _array_dimensions[1]/_chunks_in_each_dimension[1];
  _chunk_z = _array_dimensions[2]/_chunks_in_each_dimension[2];

  start_x = _chunk_x*theCoords[0];
  start_y = _chunk_y*theCoords[1];
  start_z = _chunk_z*theCoords[2];
    
  
  row_size = _chunk_x*_chunk_z;
  // calculate initial offset
  
  offset = (_array_dimensions[0]*start_y+start_x)*_array_dimensions[2];
  
  array_row_size = _array_dimensions[0]*_array_dimensions[2]; 
  src = new_array_start_val + offset;
  dst = cache_start_val + offset;
  for(unsigned int y = 0; y < _chunk_y; ++y)
    {
        memcpy(dst, src, row_size);
        src = src + array_row_size; 
        dst = dst + array_row_size;
    }
    
}

uint8 DeltaDetectSampleQBox::getValue( unsigned int x, unsigned int y, unsigned int z, char *array)
{
  int offset = (_array_dimensions[0]*y+x)*_array_dimensions[2]+z;
  return (uint8) *(array+offset);

}

void DeltaDetectSampleQBox::setValue( unsigned int x, unsigned int y, unsigned int z, char *array, uint8 theValue)
{
  int offset = (_array_dimensions[0]*y+x)*_array_dimensions[2]+z;
  *(array+offset) = theValue;

}


DynamicArray *DeltaDetectSampleQBox::create_new_chunk(int chunk_number)
{
  DynamicArray *the_chunk;
  uint8 *the_chunk_vals;
  uint8 *the_array_vals = (uint8 *) (((uint8 *) _cached_array) + _current_array->header_size());
  
  uint32 start_x, start_y, start_z;
  uint32 array_offset;
  uint32 chunk_row_jump, array_row_jump;
  uint32 row_size;
  uint8 *src, *dst;
  //-------------------------------------------------
  
  _chunk_x = _array_dimensions[0]/_chunks_in_each_dimension[0];
  _chunk_y = _array_dimensions[1]/_chunks_in_each_dimension[1];
  _chunk_z = _array_dimensions[2]/_chunks_in_each_dimension[2];

  start_x = _chunk_x*theCoords[0];
  start_y = _chunk_y*theCoords[1];
  start_z = _chunk_z*theCoords[2];

  the_chunk = DynamicArray::allocate_array(_current_array->header_size() + _chunk_x*_chunk_y*_chunk_z);
  
  //Maintain reference counters and metadata
  the_chunk->set_count_size(1,1);
  the_chunk->_start_coord[0] = start_x;
  the_chunk->_start_coord[1] = start_y;
  the_chunk->_start_coord[2] = start_z;
  the_chunk->_dim_length[0] = _chunk_x;
  the_chunk->_dim_length[1] = _chunk_y;
  the_chunk->_dim_length[2] = _chunk_z;
  
  
  the_chunk->_frame_bit = 0;
  the_chunk_vals = (uint8 *) (((uint8 *)the_chunk) + the_chunk->header_size());


  array_offset = (_array_dimensions[0]*start_y+start_x)*_array_dimensions[2]; 
  array_row_jump = _array_dimensions[0]*_array_dimensions[2]; 
  chunk_row_jump = _chunk_x*_chunk_z;
  row_size = chunk_row_jump;
  
  src = the_array_vals + array_offset;
  dst = the_chunk_vals;
  
  for(unsigned int y = 0; y < _chunk_y; ++y)
    {    
      memcpy(dst, src, row_size);
      src = src + array_row_jump;
      dst = dst + chunk_row_jump;
    }

    return the_chunk;
}

BOREALIS_NAMESPACE_END


