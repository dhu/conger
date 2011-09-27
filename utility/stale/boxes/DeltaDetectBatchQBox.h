#ifndef DELTA_BATCH_QBOX_H
#define DELTA_BATCH_QBOX_H

#include "QBox.h"
#include "Expression.h"
#include "DataPath.h"
#include "DynamicArray.h"

BOREALIS_NAMESPACE_BEGIN

class DeltaDetectBatchQBox : public QBox
{
  friend class DeltaDetectBatchBoxState;

private:
    unsigned int _output_tuple_size;

    // add-ins
    uint32 _frame_offset;
    vector<uint32> _array_dimensions;
    vector<uint32> _chunks_in_each_dimension;
    uint32 _num_chunks;
    float _delta_threshold;
    uint32 _min_x, _min_y, _max_x, _max_y;

    void addCoord(uint32 dim, uint32 n, uint32 &g);
    float calculatePercentChanged(uint32 chunk_number);
    uint8 getValue(unsigned int, unsigned int, unsigned int, char *);
    void setValue( unsigned int, unsigned int, unsigned int, char *, uint8);
    void map_chunk(int);
    void fix_bounds(uint);
    void set_pixel_red(uint32 x, uint32 y, DynamicArray *the_out);
    DynamicArray *create_new_chunk();

    vector<uint32> theCoords;
    uint8 _first_flag;
    DynamicArray *_current_array;
    DynamicArray *_cached_array;
    uint32 _array_size, _chunk_size;
    float _change_threshold;


protected:
    void setup_impl() throw (AuroraException);
    void init_impl() throw (AuroraException);
    void run_impl(QBoxInvocation&) throw (AuroraException);

    // packing and unpacking states
    void set_pending_box_state(ptr<AbstractBoxState> packed_box);
    ptr<AbstractBoxState> pack_state(bool delta_only = false);
    void unpack_state(ptr<AbstractBoxState> box_state) {};
    ~DeltaDetectBatchQBox();
};



class DeltaDetectBatchBoxState : public AbstractBoxState
{
 public:
  DeltaDetectBatchBoxState() {}
  ~DeltaDetectBatchBoxState() {}
};

BOREALIS_NAMESPACE_END

#endif
