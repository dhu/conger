#include "DeltaDetectRecurseQBox.h"
#include "timer.h"
#include <math.h>

BOREALIS_NAMESPACE_BEGIN


void DeltaDetectRecurseQBox::setup_impl() throw (AuroraException)
{
  int dim_size;
  SchemaArray *the_array_info;

  vector<SchemaArray::DimensionInformation> theDimInfo;
    if ( get_num_inputs() != 1 )
    {   Throw(aurora_typing_exception,
              "DeltaDetect requires exactly one input stream (not " +
              to_string(get_num_inputs()) + ")");
    }


    set_out_description( 0, get_in_stream( 0 ));
 


    DEBUG << "DeltaDetectRecurseQBox input schema " << get_in_schema(0)->as_string();
    DEBUG << "DeltaDetectRecurseQBox output schema " << get_out_schema(0)->as_string();

    typed_param("delta_threshold", _delta_threshold);
    typed_param("continue_threshold", _continue_threshold);
    typed_param("max_depth", _max_depth);
    
    DEBUG << "Threshold: " << _delta_threshold;
    TupleDescription in_td(get_in_schema(0));
    int the_field_index = in_td.index_of_field(string("frame"));
    const SchemaField *the_array = in_td.get_schema_field(the_field_index);
    the_array_info  = the_array->get_array();
    _array_size = the_array_info->array_size(); 
    
    _cached_array = DynamicArray::allocate_array(DynamicArray::header_size()+_array_size);
    _cached_array->set_count_size();
    theDimInfo = the_array_info->get_all_dimensions();    
    _frame_offset = in_td.get_offset(the_field_index);


    for(unsigned int x = 0; x < theDimInfo.size(); ++x)
      {
    dim_size = (uint32)(theDimInfo[x]._upper - theDimInfo[x]._lower) + 1;
    _array_dimensions.push_back(dim_size);
      }
    
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
    _first_flag = 1;
}

DeltaDetectRecurseQBox::~DeltaDetectRecurseQBox()
{
  _cached_array->modify_array_reference(-1);
}

void DeltaDetectRecurseQBox::init_impl() throw (AuroraException)
{
  TupleDescription out_td(get_out_schema(0));
  _output_tuple_size = out_td.get_size();

}

void DeltaDetectRecurseQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{
    uint32 *element;
    DeqIterator my_deq_iterator = deq(0);
    TupleDescription in_td(get_in_schema(0));
    TupleDescription out_td(get_out_schema(0));
    timer the_time;
    bounding_box_rec the_whole_box;
    DynamicArray *frame_flag;
    //--------------------------------------------------------------
    the_whole_box._min_x = 0;
    the_whole_box._min_y = 0;
    the_whole_box._len_x = _array_dimensions[0];
    the_whole_box._len_y = _array_dimensions[1];
    
    frame_flag = DynamicArray::allocate_array(DynamicArray::header_size());
    frame_flag->set_count_size();
    frame_flag->_frame_bit = 1;
    
    the_time.start();
    // Do stuff...
    while (inv.continue_dequeue_on(my_deq_iterator, 0))
    {
      
      
      data = const_cast<void *>(my_deq_iterator.tuple());
      ++my_deq_iterator;
      
      _current_array = (DynamicArray *) (*(uint32 *) ((((char *)data) + _frame_offset)));
      
        
      break_up_array(the_whole_box, 0);
      frame_flag->modify_array_reference(1);
      // send over frame bit flag to tell FD to process it
      memcpy(enq(0).tuple(), data, _output_tuple_size);
      element = (uint32 *)((uint8 *)enq(0).tuple() + _frame_offset);
      *element  = (uint32) frame_flag;
      DEBUG << "Enqueuing frame flag!";
      ++(enq(0));
      get_output(0).notify_enq();
      
      _first_flag = 0;
     
     
    }
    the_time.stop();
    _total_time += the_time.elapsed_time();
    NOTICE << "Time elapsed: " << the_time.elapsed_time() << " total time: " << _total_time;

 }


void DeltaDetectRecurseQBox::set_pending_box_state(ptr<AbstractBoxState> packed_box)
{
    //DeltaDetectBoxState* actual_packed_box = static_cast<DeltaDetectBoxState*>(packed_box.get());
    //_pending_box_state.set(new DeltaDetectBoxState(*actual_packed_box));
    _pending_box_state = packed_box.dynamic_cast_to<DeltaDetectRecurseBoxState>();
}

ptr<AbstractBoxState> DeltaDetectRecurseQBox::pack_state(bool delta_only)
{
  INFO << "Packing state for DeltaDetectRecurseQBox named [" << get_name() << "]";
  ptr<DeltaDetectRecurseBoxState> filter_box_state(new DeltaDetectRecurseBoxState());
  return filter_box_state;
}


void DeltaDetectRecurseQBox::break_up_array(bounding_box_rec a_box, uint32 depth)
{
    float delta;
    bounding_box_rec new_box;
    DynamicArray *new_chunk;
    uint32 *element;
    //DEBUG << "Looking at box: " << a_box._min_x << ", " << a_box._min_y << ") (" << a_box._min_x + a_box._len_x << ", " << a_box._len_y << ")" << " depth " << depth;
    if(depth > _max_depth)
        return;
    delta = calculate_percent_changed(a_box);
    //DEBUG << "First flag: " << (int) _first_flag;
    if(delta > _delta_threshold || _first_flag == 1)
    { 
        DEBUG << "Creating and sending chunk on BB of (" << a_box._min_x << ", " << a_box._min_y << ") (" << a_box._min_x + a_box._len_x << ", " << a_box._min_y +  a_box._len_y << ")";  
        map_chunk(a_box);
        new_chunk = create_new_chunk(a_box);
        memcpy(enq(0).tuple(), data, _output_tuple_size);
        element = (uint32 *)((uint8 *)enq(0).tuple() + _frame_offset);
        *element  = (uint32) new_chunk;
        ++(enq(0));
        get_output(0).notify_enq();
        return;
    }
    if(delta > _continue_threshold)
    {
        // break into four pieces
        // upper left
        new_box = a_box;
        new_box._len_x = a_box._len_x / 2;
        new_box._len_y = a_box._len_y / 2;
        break_up_array(new_box, depth+1);
        
        // upper right  
        new_box._min_x += new_box._len_x;
        break_up_array(new_box, depth+1);
        
        // lower left
        new_box._min_y += new_box._len_y;
        new_box._min_x -= new_box._len_x;
        break_up_array(new_box, depth+1);
        
        // lower right
        new_box._min_x += new_box._len_x;
        break_up_array(new_box, depth+1);
        return;
    }
    // else if insufficient change 
    return;
}

float DeltaDetectRecurseQBox::calculate_percent_changed(bounding_box_rec a_box)
{
  uint32 offset, row_jump, cutoff_idx;
  float set_length;
  float percent_changed = 0.0;
  uint8 *cache_pos, *in_pos;
  uint8 old_val, new_val;
  uint32 row_count, row_pos;


  uint8 *old_array_start_val = (((uint8 *)_cached_array) + DynamicArray::header_size());
  uint8 *new_array_start_val = (((uint8 *)_current_array) + DynamicArray::header_size());
  
    
   set_length = a_box._len_x*a_box._len_y*_array_dimensions[2];
   
   offset = (_array_dimensions[0]*a_box._min_y + a_box._min_x)*_array_dimensions[2];
   cache_pos = old_array_start_val + offset;
   in_pos = new_array_start_val + offset;
   row_jump = (_array_dimensions[0] - a_box._len_x)*_array_dimensions[2];
   row_count = row_pos = 0;
   cutoff_idx = a_box._len_x*_array_dimensions[2];
 
  while(row_count != a_box._len_y)
  {
        old_val = *cache_pos;
        new_val = *in_pos;
        //DEBUG << "Adding " <<  fabs(old_val - new_val);
        percent_changed += fabs(old_val - new_val);
        ++cache_pos;
        ++in_pos;
        ++row_pos;
    
    if(row_pos == cutoff_idx)
        {
                // on to the next row
                row_pos = 0;
                ++row_count;
                cache_pos = cache_pos + row_jump;
                in_pos = in_pos + row_jump;
        }

  }
    //DEBUG << "Total: " << percent_changed;
  percent_changed /= set_length;
  percent_changed /= 256.0;
  //DEBUG << "Percent changed for box: " << a_box._min_x << ", " << a_box._min_y << ", lengths: " << a_box._len_x << ", " << a_box._len_y << " is: " << percent_changed;
  return percent_changed;
}

void DeltaDetectRecurseQBox::map_chunk(bounding_box_rec a_box)
{
  char *cache_start_val = (char *)(((uint8 *) _cached_array) + DynamicArray::header_size()); 
  char *new_array_start_val = (char *)(((uint8 *) _current_array) + DynamicArray::header_size()); 
  char *src, *dst;
  int row_size, array_row_size, offset;


    
  row_size = a_box._len_x*_array_dimensions[2];
  // calculate initial offset
  offset = (_array_dimensions[0]*a_box._min_y+a_box._min_x)*_array_dimensions[2];
  
  array_row_size = _array_dimensions[0]*_array_dimensions[2]; 
  src = new_array_start_val + offset;
  dst = cache_start_val + offset;
  for(unsigned int y = 0; y < a_box._len_y; ++y)
    {
        memcpy(dst, src, row_size);
        src = src + array_row_size; 
        dst = dst + array_row_size;
    }
}

void DeltaDetectRecurseQBox::fix_bounds(uint chunkNum)
{
  uint32 start_x, start_y, last_x, last_y;
  uint32 chunk_x, chunk_y;
  
  chunk_x = _array_dimensions[0]/_chunks_in_each_dimension[0];
  chunk_y = _array_dimensions[1]/_chunks_in_each_dimension[1];
  

  start_x = chunk_x*theCoords[0];
  start_y = chunk_y*theCoords[1];
  last_x = start_x + chunk_x;
  last_y = start_y + chunk_y;

  if(start_x < _min_x)
    _min_x = start_x;
  if(start_y < _min_y)
    _min_y = start_y;
  if(last_x > _max_x)
    _max_x = last_x;
  if(last_y > _max_y)
    _max_y = last_y;
  

}

uint8 DeltaDetectRecurseQBox::getValue( unsigned int x, unsigned int y, unsigned int z, char *array)
{
  int offset = (_array_dimensions[0]*y+x)*_array_dimensions[2]+z;
  return (uint8) *(array+offset);

}

void DeltaDetectRecurseQBox::setValue( unsigned int x, unsigned int y, unsigned int z, char *array, uint8 theValue)
{
  int offset = (_array_dimensions[0]*y+x)*_array_dimensions[2]+z;
  *(array+offset) = theValue;

}


DynamicArray *DeltaDetectRecurseQBox::create_new_chunk(bounding_box_rec a_box)
{
  DynamicArray *the_chunk;
  char *the_chunk_vals;
  // using current array gives it slightly better vals
  char *the_array_vals = (char *) (((uint8 *) _current_array) + _current_array->header_size());

  
  char *src, *dst;
  uint32 chunk_row_jump, array_row_jump, initial_offset;

  DEBUG << "Requesting an alloc of: " << DynamicArray::header_size() + a_box._len_x*a_box._len_y*3
        << " for bb of (" << a_box._min_x << ", " << a_box._min_y << ")"; 
    
  the_chunk = DynamicArray::allocate_array(DynamicArray::header_size() + a_box._len_x*a_box._len_y*_array_dimensions[2]);
  the_chunk_vals = (char *) (((uint8 *) the_chunk) + the_chunk->header_size());
 
  //Maintain reference counters
  the_chunk->set_count_size(1,1);
  the_chunk->_start_coord[0] = a_box._min_x;
  the_chunk->_start_coord[1] = a_box._min_y;
  the_chunk->_start_coord[2] = 0;
  the_chunk->_dim_length[0] = a_box._len_x;
  the_chunk->_dim_length[1] = a_box._len_y;
  the_chunk->_dim_length[2] = 3;
  the_chunk->_frame_bit = 0;
  
  
    
  initial_offset = (_array_dimensions[0]*a_box._min_y+a_box._min_x)*_array_dimensions[2];
  src = the_array_vals + initial_offset;
  dst = the_chunk_vals;
  chunk_row_jump = a_box._len_x*_array_dimensions[2];
  array_row_jump = _array_dimensions[0]*_array_dimensions[2]; 

  for(unsigned int y = 0; y < a_box._len_y; ++y)
  {
    memcpy(dst, src, chunk_row_jump);
    src = src + array_row_jump;
    dst = dst + chunk_row_jump;
  }

    return the_chunk;
}

BOREALIS_NAMESPACE_END


