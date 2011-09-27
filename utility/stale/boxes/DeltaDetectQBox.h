#ifndef DELTA_QBOX_H
#define DELTA_QBOX_H

#include "QBox.h"
#include "Expression.h"
#include "DataPath.h"
#include "DynamicArray.h"

BOREALIS_NAMESPACE_BEGIN

class DeltaDetectQBox : public QBox
{
  friend class DeltaDetectBoxState;

private:
    unsigned int _output_tuple_size;

    // add-ins
    uint32 _frame_offset, _size_offset;
    vector<uint32> _array_dimensions;
    vector<uint32> _chunks_in_each_dimension;
    uint32 _num_chunks;
    float _delta_threshold;

    void addCoord(uint32 dim, uint32 n, uint32 &g);
    float calculate_percent_changed(uint32 chunk_number);
    uint8 getValue(unsigned int, unsigned int, unsigned int, char *);
    void setValue( unsigned int, unsigned int, unsigned int, char *, uint8);
    void map_chunk(int);
    DynamicArray *create_new_chunk(int chunk_number);

    vector<uint32> theCoords;
    uint8 _first_flag;
    DynamicArray *_current_array;
    DynamicArray *_cached_array;
    uint32 _array_size, _chunk_size;
    float _change_threshold;
    uint32 _chunk_x, _chunk_y, _chunk_z;
    double _total_time;


protected:
    void setup_impl() throw (AuroraException);
    void init_impl() throw (AuroraException);
    void run_impl(QBoxInvocation&) throw (AuroraException);

    // packing and unpacking states
    void set_pending_box_state(ptr<AbstractBoxState> packed_box);
    ptr<AbstractBoxState> pack_state(bool delta_only = false);
    void unpack_state(ptr<AbstractBoxState> box_state) {};
    ~DeltaDetectQBox();
};



class DeltaDetectBoxState : public AbstractBoxState
{
 public:
  DeltaDetectBoxState() {}
  ~DeltaDetectBoxState() {}
};

BOREALIS_NAMESPACE_END

#endif
