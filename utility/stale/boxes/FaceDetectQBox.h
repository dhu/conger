#ifndef FACE_DETECT_QBOX_H
#define FACE_DETECT_QBOX_H

#include "cv.h"
#include "highgui.h"
#include "cxcore.h"
#include "QBox.h"
#include "Expression.h"
#include "DataPath.h"



BOREALIS_NAMESPACE_BEGIN

class FaceDetectQBox : public QBox
{
  friend class FaceDetectBoxState;

private:
    unsigned int _output_tuple_size;
    uint32 _array_size;
    vector<uint32> _array_dimensions;
    uint32 _frame_offset;
    uint32 _detection_count;
    CvMemStorage* _storage;
    CvHaarClassifierCascade* _cascade;
	void detect_on_region(IplImage *);
	double _total_time;
protected:
    void setup_impl() throw (AuroraException);
    void init_impl() throw (AuroraException);
    void run_impl(QBoxInvocation&) throw (AuroraException);

    // packing and unpacking states
    void set_pending_box_state(ptr<AbstractBoxState> packed_box);
    ptr<AbstractBoxState> pack_state(bool delta_only = false);
    void unpack_state(ptr<AbstractBoxState> box_state) {};
};



class FaceDetectBoxState : public AbstractBoxState
{
 public:
  FaceDetectBoxState() {}
  ~FaceDetectBoxState() {}
};

BOREALIS_NAMESPACE_END

#endif
