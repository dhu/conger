#ifndef DELTA_RECURSE_QBOX_H
#define DELTA_RECURSE_QBOX_H

#include "QBox.h"
#include "Expression.h"
#include "DataPath.h"
#include "DynamicArray.h"

BOREALIS_NAMESPACE_BEGIN

typedef struct bb_rec
{
    uint32 _min_x;
    uint32 _min_y;
    uint32 _len_x;
    uint32 _len_y;
} bounding_box_rec;

class DeltaDetectRecurseQBox : public QBox
{
  friend class DeltaDetectRecurseBoxState;

private:
    unsigned int _output_tuple_size;

    // add-ins
    uint32 _frame_offset;
    vector<uint32> _array_dimensions;
    vector<uint32> _chunks_in_each_dimension;
    uint32 _num_chunks;
    float _delta_threshold, _continue_threshold;
    uint32 _max_depth;
    uint32 _min_x, _min_y, _max_x, _max_y;
    void *data; // current tuple contents

    
    float calculate_percent_changed(bounding_box_rec);
    void break_up_array(bounding_box_rec a_box, uint32 depth);
    uint8 getValue(unsigned int, unsigned int, unsigned int, char *);
    void setValue( unsigned int, unsigned int, unsigned int, char *, uint8);
    void map_chunk(bounding_box_rec);
    void fix_bounds(uint);
    void set_pixel_red(uint32 x, uint32 y, DynamicArray *the_out);
    DynamicArray *create_new_chunk(bounding_box_rec);

    vector<uint32> theCoords;
    uint8 _first_flag;
    int32 _chunk_count;
    DynamicArray *_current_array;
    DynamicArray *_cached_array;
    uint32 _array_size, _chunk_size;
    float _change_threshold;
    double _total_time;


protected:
    void setup_impl() throw (AuroraException);
    void init_impl() throw (AuroraException);
    void run_impl(QBoxInvocation&) throw (AuroraException);

    // packing and unpacking states
    void set_pending_box_state(ptr<AbstractBoxState> packed_box);
    ptr<AbstractBoxState> pack_state(bool delta_only = false);
    void unpack_state(ptr<AbstractBoxState> box_state) {};
    ~DeltaDetectRecurseQBox();
};



class DeltaDetectRecurseBoxState : public AbstractBoxState
{
 public:
  DeltaDetectRecurseBoxState() {}
  ~DeltaDetectRecurseBoxState() {}
};

BOREALIS_NAMESPACE_END

#endif
