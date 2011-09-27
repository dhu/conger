#ifndef CHUNK_ASSEMBLER_QBOX_H
#define CHUNK_ASSEMBLER_QBOX_H

#include "QBox.h"
#include "Expression.h"
#include "DataPath.h"
#include "DynamicArray.h"

BOREALIS_NAMESPACE_BEGIN

class ChunkAssemblerQBox : public QBox
{
  friend class ChunkAssemblerBoxState;

private:
    unsigned int _output_tuple_size;
    vector<uint32> _array_dimensions;
    vector<uint32> _chunks_in_each_dimension;
    vector<uint32> _the_coords;
    uint32 _num_chunks;
    uint32 _array_size;
    uint32 _frame_offset;
    DynamicArray *_cached_array;
    ~ChunkAssemblerQBox();
    void add_coord(uint32 dim, uint32 n, uint32 &g);
    void map_chunk(DynamicArray *);
protected:
    void setup_impl() throw (AuroraException);
    void init_impl() throw (AuroraException);
    void run_impl(QBoxInvocation&) throw (AuroraException);

    // packing and unpacking states
    void set_pending_box_state(ptr<AbstractBoxState> packed_box);
    ptr<AbstractBoxState> pack_state(bool delta_only = false);
    void unpack_state(ptr<AbstractBoxState> box_state) {};
};



class ChunkAssemblerBoxState : public AbstractBoxState
{
 public:
  ChunkAssemblerBoxState() {}
  ~ChunkAssemblerBoxState() {}
};

BOREALIS_NAMESPACE_END

#endif
