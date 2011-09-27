#ifndef DELTA_ADJ_QBOX_H
#define DELTA_ADJ_QBOX_H

#include "QBox.h"
#include "Expression.h"
#include "DataPath.h"
#include "DynamicArray.h"


BOREALIS_NAMESPACE_BEGIN

typedef struct t_bb {
    uint32 x_start;
    uint32 y_start;
    uint32 x_length;
    uint32 y_length;
} bounding_box;


class DeltaDetectAdjQBox : public QBox
{
  friend class DeltaDetectAdjBoxState;

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
    DynamicArray *create_new_chunk(int, int, int, int);
    vector<DynamicArray *> create_batches(vector<int> the_chunks);
    vector<int> upper_left_of_chunk(int chunk);
    bool is_adjacent(bounding_box &, uint32, uint32);
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
    ~DeltaDetectAdjQBox();
};



class DeltaDetectAdjBoxState : public AbstractBoxState
{
 public:
  DeltaDetectAdjBoxState() {}
  ~DeltaDetectAdjBoxState() {}
};

BOREALIS_NAMESPACE_END

#endif
