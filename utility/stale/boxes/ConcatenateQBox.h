#ifndef CONCATENATE_QBOX_H
#define CONCATENATE_QBOX_H

#include "QBox.h"
#include <vector>
#include "Expression.h"
#include "DynamicArray.h"

BOREALIS_NAMESPACE_BEGIN

class ConcatenateQBox : public QBox
{
private:
    uint32 _window_size, _slide, _overlap;
    uint32 _max_x, _max_y, _max_z; // for individual frames
    uint32 _out_y;
    uint32 _slide_count;
    bool _first_flag;
    uint32 _tuple_count;
    DynamicArray *_cached_view;
    uint8 *_cached_view_values;
    uint32 _in_array_offset, _out_array_offset;
    uint32 _full_frame_size, _partial_frame_size;
    uint8 *_cached_view_pos;
    uint32 _cache_size;
	void slide_up_and_reset();
	uint32 _output_tuple_size;
	void print_cvv();
protected:
    void setup_impl() throw (AuroraException);
    void init_impl() throw (AuroraException);
    void run_impl(QBoxInvocation&) throw (AuroraException);
};

BOREALIS_NAMESPACE_END

#endif
