#ifndef GATHER_QBOX_H
#define GATHER_QBOX_H

#include "QBox.h"
#include "DynamicArray.h"

BOREALIS_NAMESPACE_BEGIN

class GatherQBox : public QBox
{

 protected:

    uint32   _output_tuple_size;


 protected:

    void setup_impl() throw (AuroraException);
    void init_impl() throw (AuroraException);
    void run_impl(QBoxInvocation&) throw (AuroraException);

    
 private:

    bool process_input_tuple(DynamicArray *, uint32 input);

    queue<uint8 *>  _lhs_arrays;
    queue<uint8 *>  _rhs_arrays;

    uint8           _output_array[320];
    uint32          _in_array_offset, _out_array_offset;

    /// Number of workers sending streams to merge.
    ///
    uint16          _worker_count;
};


BOREALIS_NAMESPACE_END

#endif
