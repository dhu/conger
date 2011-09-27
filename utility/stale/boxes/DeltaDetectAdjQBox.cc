#include "DeltaDetectAdjQBox.h"
#include <math.h>
#include "timer.h"

BOREALIS_NAMESPACE_BEGIN

    
void DeltaDetectAdjQBox::setup_impl() throw (AuroraException)
{
  int dim_size;
  SchemaArray *the_array_info;

  vector<SchemaArray::DimensionInformation> theDimInfo;
    if ( get_num_inputs() != 1 )
    {   Throw(aurora_typing_exception,
              "DeltaDetectAdj requires exactly one input stream (not " +
              to_string(get_num_inputs()) + ")");
    }

    set_out_description( 0, get_in_stream( 0 ));

    DEBUG << "DeltaDetectAdjQBox input schema " << get_in_schema(0)->as_string();
    DEBUG << "DeltaDetectAdjQBox output schema " << get_out_schema(0)->as_string();

    typed_param("delta_threshold", _delta_threshold);
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
    _total_time = 0.0;
}

DeltaDetectAdjQBox::~DeltaDetectAdjQBox()
{
  _cached_array->modify_array_reference(-1);
}

void DeltaDetectAdjQBox::init_impl() throw (AuroraException)
{
  TupleDescription out_td(get_out_schema(0));
  _output_tuple_size = out_td.get_size();

}

void DeltaDetectAdjQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{
    uint32 *element;
    DeqIterator my_deq_iterator = deq(0);
    DynamicArray *new_chunk  = 0;
    TupleDescription in_td(get_in_schema(0));
    TupleDescription out_td(get_out_schema(0));
    vector<int> chunks_changed;
    vector<DynamicArray *> the_out_chunks;
    timer the_time;
    //-----------------------------------------------------------------
    // Do stuff...
    DEBUG << "Starting next frame!";
    the_time.start();
    while (inv.continue_dequeue_on(my_deq_iterator, 0))
    {
      
     
        const void *data = my_deq_iterator.tuple();
    
    _current_array = (DynamicArray *) (*(uint32 *) ((((char *)data) + _frame_offset)));  
    new_chunk = 0;
    chunks_changed.clear();

    for(unsigned int x = 0; x < _num_chunks; ++x)
      {
        float theP = calculate_percent_changed(x);
    
        if(theP >= _delta_threshold)
          { 
               map_chunk(x);
               chunks_changed.push_back(x);
           } 
      } 
    DEBUG << "Creating batches!";
    the_out_chunks = create_batches(chunks_changed);
    DEBUG << "Breaks into: " << the_out_chunks.size() << " batches";
    if(the_out_chunks.size() > 0)
    {
        for(unsigned int x = 0; x < the_out_chunks.size()-1; ++x)
        {
            new_chunk = the_out_chunks[x];
            memcpy(enq(0).tuple(), data, _output_tuple_size);
            element = (uint32 *)((uint8 *)enq(0).tuple() + _frame_offset); 
            *element  = (uint32) new_chunk;
            ++(enq(0));
            get_output(0).notify_enq();
        }
        // set frame bit to 1 on the last one
        new_chunk = the_out_chunks[the_out_chunks.size()-1];
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

void DeltaDetectAdjQBox::set_pending_box_state(ptr<AbstractBoxState> packed_box)
{
    //DeltaDetectAdjBoxState* actual_packed_box = static_cast<DeltaDetectAdjBoxState*>(packed_box.get());
    //_pending_box_state.set(new DeltaDetectAdjBoxState(*actual_packed_box));
    _pending_box_state = packed_box.dynamic_cast_to<DeltaDetectAdjBoxState>();
}

ptr<AbstractBoxState> DeltaDetectAdjQBox::pack_state(bool delta_only)
{
  INFO << "Packing state for DeltaDetectAdjQBox named [" << get_name() << "]";
  ptr<DeltaDetectAdjBoxState> filter_box_state(new DeltaDetectAdjBoxState());
  return filter_box_state;
}

vector<DynamicArray *> DeltaDetectAdjQBox::create_batches(vector<int> the_chunks)
{
    vector<DynamicArray *> the_batches;
    int chunk_id;
    unsigned int x;
    list<bounding_box > the_chunk_clusters;
    list<bounding_box>::iterator clusters_list_pos;
    bounding_box current_cluster;
    bool found;
    
    //------------------------------------------------
    vector< vector<int> > batches;
    vector<int> current_start_pos;
    
    if(_first_flag)
    {
        DynamicArray *the_whole = create_new_chunk(0, 0, _array_dimensions[0], _array_dimensions[1]);
        the_batches.push_back(the_whole);
        return the_batches;
    }
    
    for(x = 0; x < the_chunks.size(); ++x)
    {
        chunk_id = the_chunks[x];
        clusters_list_pos = the_chunk_clusters.begin();
        found = false;
        current_start_pos = upper_left_of_chunk(chunk_id);
        // iterate through cluster list
        while(clusters_list_pos != the_chunk_clusters.end() && found == false)
        {
            current_cluster = *clusters_list_pos;
            if(is_adjacent(current_cluster, current_start_pos[0], current_start_pos[1]))
                {
                    *clusters_list_pos = current_cluster; // update value
                    found = true;
                }           
            ++clusters_list_pos;    
        }// end cluster list iter

        if(found == false) // create a new cluster
        {
            bounding_box a_cluster;
            a_cluster.x_start = current_start_pos[0];
            a_cluster.y_start = current_start_pos[1];
            a_cluster.x_length = _chunk_x;
            a_cluster.y_length = _chunk_y;
            the_chunk_clusters.push_back(a_cluster);
        }   
    }
    DEBUG << "Creating dyn array sized chunks.";
    clusters_list_pos = the_chunk_clusters.begin();
    while(clusters_list_pos != the_chunk_clusters.end())
    {
        // TODO: fix this up!
        // create and push back chunk
        current_cluster = *clusters_list_pos;
        DynamicArray *new_chunk = create_new_chunk(current_cluster.x_start, current_cluster.y_start, current_cluster.x_start + current_cluster.x_length, current_cluster.y_start + current_cluster.y_length);
        the_batches.push_back(new_chunk);       
        ++clusters_list_pos;    
    }
    return the_batches;
          
}

vector<int> DeltaDetectAdjQBox::upper_left_of_chunk(int chunk_number)
{
    
    vector<int> the_pos;
    
    uint32 g = _num_chunks;
    theCoords.clear();
    addCoord(2, chunk_number, g);
    DEBUG << "Mapped " << chunk_number << " to coord: " << theCoords[0] << ", " << theCoords[1];
    the_pos.push_back(theCoords[0] * _chunk_x);
    the_pos.push_back(theCoords[1] * _chunk_y);
    the_pos[2] = theCoords[2] * _chunk_z;
    return the_pos;
}

// arguments: cluster being compared to, upper left corner of the chunk
bool DeltaDetectAdjQBox::is_adjacent(bounding_box &cluster, uint32 x, uint32 y)
{
    bounding_box grown_box;
    
    // base case first
    grown_box.x_start = cluster.x_start - _chunk_x;
    grown_box.y_start = cluster.y_start - _chunk_y;
    grown_box.x_length = cluster.x_length + 2*_chunk_x;
    grown_box.y_length = cluster.y_length + 2*_chunk_y;
    
    // fix the corner cases
    if(cluster.x_start == 0)
    {
        grown_box.x_start = 0;
        grown_box.x_length -= _chunk_x;
    }
    
    if(cluster.y_start == 0)
    {
        grown_box.y_start = 0;
        grown_box.y_length -= _chunk_y;
    }
    
    if(cluster.x_start + cluster.x_length == _array_dimensions[0])
    {
        grown_box.x_length -= _chunk_x;
    }
    
    if(cluster.y_start + cluster.y_length == _array_dimensions[1])
    {
        grown_box.y_length -= _chunk_y;
    }
    
    if(x >= grown_box.x_start && x <= (grown_box.x_start + grown_box.x_length) &&
       y >= grown_box.y_start && y <= (grown_box.y_start + grown_box.y_length))
       {
        // fix the bounds to accomodate the new chunk
        if(x < cluster.x_start)
            cluster.x_start = x;
        if(x + _chunk_x > cluster.x_start + cluster.x_length)
            cluster.x_length += _chunk_x;
        if(y  < cluster.y_start)
            cluster.y_start = y;
        if(y + _chunk_y > cluster.y_start + cluster.y_length)
            cluster.y_length += _chunk_y;

        return true;
       }
       return false;
    return false;
}

void DeltaDetectAdjQBox::addCoord(uint32 dim, uint32 n, uint32 &g)
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


float DeltaDetectAdjQBox::calculate_percent_changed(uint32 chunk_number)
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



  // 100% difference for first one
  if(_first_flag)
    return 1;

   set_length = _chunk_size;

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

  percent_changed /= set_length;
  percent_changed /= 256.0;
  //DEBUG << "Percent changed for chunk " << chunk_number << ": " << percent_changed;
  return percent_changed;

}

void DeltaDetectAdjQBox::map_chunk(int chunk_number)
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

uint8 DeltaDetectAdjQBox::getValue( unsigned int x, unsigned int y, unsigned int z, char *array)
{
  int offset = (_array_dimensions[0]*y+x)*_array_dimensions[2]+z;
  return (uint8) *(array+offset);

}

void DeltaDetectAdjQBox::setValue( unsigned int x, unsigned int y, unsigned int z, char *array, uint8 theValue)
{
  int offset = (_array_dimensions[0]*y+x)*_array_dimensions[2]+z;
  *(array+offset) = theValue;

}


DynamicArray *DeltaDetectAdjQBox::create_new_chunk(int a_min_x, int a_min_y, int a_max_x, int a_max_y)
{
  DynamicArray *the_chunk;
  char *the_chunk_vals;
  // using current array gives it slightly better vals
  char *the_array_vals = (char *) (((uint8 *) _current_array) + _current_array->header_size());

  uint32 chunk_x, chunk_y, chunk_z;
  uint32 x_dim, y_dim;
  char *src, *dst;
  uint32 chunk_row_jump, array_row_jump, initial_offset;

  if(a_max_x < a_min_x || a_max_y < a_min_y)
    FATAL << "Improper bounding box of (" << a_min_x << ", " << a_min_y << ") (" << a_max_x << ", " << a_max_y << ")"; 
  x_dim = a_max_x - a_min_x;
  y_dim = a_max_y - a_min_y;

  DEBUG << "Requesting an alloc of: " << DynamicArray::header_size() + x_dim*y_dim*3
        << " for bb of (" << a_min_x << ", " << a_min_y << ") (" << a_max_x << ", " << a_max_y << ")"; 
    
  the_chunk = DynamicArray::allocate_array(DynamicArray::header_size() + x_dim*y_dim*3);
  the_chunk_vals = (char *) (((uint8 *) the_chunk) + the_chunk->header_size());
 
  //Maintain reference counters
  the_chunk->set_count_size(1,1);
  the_chunk->_start_coord[0] = a_min_x;
  the_chunk->_start_coord[1] = a_min_y;
  the_chunk->_start_coord[2] = 0;
  the_chunk->_dim_length[0] = a_max_x - a_min_x;
  the_chunk->_dim_length[1] = a_max_y - a_min_y;
  the_chunk->_dim_length[2] = 3;
  the_chunk->_frame_bit = 0;  
  
  
  chunk_x = x_dim;
  chunk_y = y_dim;
  chunk_z = 3;
  
  initial_offset = (_array_dimensions[0]*a_min_y+a_min_x)*_array_dimensions[2];
  src = the_array_vals + initial_offset;
  dst = the_chunk_vals;
  chunk_row_jump = x_dim*chunk_z;
  array_row_jump = _array_dimensions[0]*_array_dimensions[2]; 

  for(unsigned int y = 0; y < y_dim; ++y)
  {
    memcpy(dst, src, chunk_row_jump);
    src = src + array_row_jump;
    dst = dst + chunk_row_jump;
  }

    return the_chunk;
}

BOREALIS_NAMESPACE_END


