#ifndef ADJ_FACE_DETECT_QBOX_H
#define ADJ_FACE_DETECT_QBOX_H

#include "cv.h"
#include "highgui.h"
#include "cxcore.h"

#include "QBox.h"
#include "Expression.h"
#include "DataPath.h"
#include "DynamicArray.h"


BOREALIS_NAMESPACE_BEGIN

class FaceDetectAdjQBox : public QBox
{
  friend class FaceDetectAdjBoxState;

private:
    unsigned int _output_tuple_size;
    vector<uint32> _array_dimensions;
    vector<uint32> _chunks_in_each_dimension;
    vector<uint32> _the_coords;
    uint32 _num_chunks;
    uint32 _array_size;
    uint32 _frame_offset;
    DynamicArray *_cached_array;
    uint32 _min_x, _max_x, _min_y, _max_y;
    CvMemStorage* _storage;
    CvHaarClassifierCascade* _cascade;
    double _total_time;    
    ~FaceDetectAdjQBox();
    void add_coord(uint32 dim, uint32 n, uint32 &g);
    void map_chunk(DynamicArray *);
    void outline_roi(DynamicArray *the_array);
    void set_pixel_red(uint32 x, uint32 y, DynamicArray *);
    void detect_on_region();
    
    
protected:
    void setup_impl() throw (AuroraException);
    void init_impl() throw (AuroraException);
    void run_impl(QBoxInvocation&) throw (AuroraException);

    // packing and unpacking states
    void set_pending_box_state(ptr<AbstractBoxState> packed_box);
    ptr<AbstractBoxState> pack_state(bool delta_only = false);
    void unpack_state(ptr<AbstractBoxState> box_state) {};
};



class FaceDetectAdjBoxState : public AbstractBoxState
{
 public:
  FaceDetectAdjBoxState() {}
  ~FaceDetectAdjBoxState() {}
};

BOREALIS_NAMESPACE_END

#endif
