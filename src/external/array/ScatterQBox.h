#ifndef SCATTER_QBOX_H
#define SCATTER_QBOX_H

#include "QBox.h"
#include "Expression.h"
#include "DynamicArray.h"

BOREALIS_NAMESPACE_BEGIN

class ScatterQBox : public QBox
{
      friend class ScatterBoxState;

  private:

    string      _array_field;
    uint32      _array_offset;
    uint32      _output_tuple_size;
    uint32      _input_tuple_size;

    /// Number of workers to receive partitioned streams.
    ///
    uint16      _worker_count;


  protected:

    void setup_impl() throw (AuroraException);
    void init_impl() throw (AuroraException);
    void run_impl(QBoxInvocation&) throw (AuroraException);

    // packing and unpacking states
    void  set_pending_box_state(ptr<AbstractBoxState> packed_box);
    ptr<AbstractBoxState> pack_state(bool delta_only = false);
    void unpack_state(ptr<AbstractBoxState> box_state) {};
    void *create_output_tuple(const void *data, uint32 i);

    DynamicArray *clone_array(DynamicArray *orig);
    DynamicArray *halve_array(DynamicArray *orig, int out_stream);

    void *iterate_over_and_null_vals(const void *data, int i);

    uint32     _slice_offset, _chunk_offset, _id_offset;
    uint32     _seq_no;
};



class ScatterBoxState : public AbstractBoxState
{
  public:
    ScatterBoxState() {}
   ~ScatterBoxState() {}
};


BOREALIS_NAMESPACE_END

#endif
