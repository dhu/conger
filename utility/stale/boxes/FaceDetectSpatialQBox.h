#ifndef FACE_DETECT_SPATIAL_QBOX_H
#define FACE_DETECT_SPATIAL_QBOX_H

#include "cv.h"
#include "highgui.h"
#include "cxcore.h"
#include "QBox.h"
#include "Expression.h"
#include "DataPath.h"

BOREALIS_NAMESPACE_BEGIN

class FaceDetectSpatialQBox : public QBox
{
  friend class FaceDetectSpaticalBoxState;

private:
    unsigned int _output_tuple_size;
    uint32 _array_size;
    uint32 _detection_count;
    vector<uint32> _array_dimensions;
    uint32 _frame_offset;
    CvMemStorage* _storage;
    CvHaarClassifierCascade* _cascade;
	void detect_on_region(IplImage *);
	double _time_fd, _time_prep;
	SchemaArray *_array_schema;
protected:
    void setup_impl() throw (AuroraException);
    void init_impl() throw (AuroraException);
    void run_impl(QBoxInvocation&) throw (AuroraException);

    // packing and unpacking states
    void set_pending_box_state(ptr<AbstractBoxState> packed_box);
    ptr<AbstractBoxState> pack_state(bool delta_only = false);
    void unpack_state(ptr<AbstractBoxState> box_state) {};
};



class FaceDetectSpatialBoxState : public AbstractBoxState
{
 public:
  FaceDetectSpatialBoxState() {}
  ~FaceDetectSpatialBoxState() {}
};

BOREALIS_NAMESPACE_END

#endif
