#include "cv.h"
#include "highgui.h"
#include "cxcore.h"

#include "QBox.h"

#include "ContrastQBox.h"
#include "ContrastCompositeQBox.h"
#include "DeltaDetectQBox.h"
#include "DeltaDetectAdjQBox.h"
#include "DeltaDetectBatchQBox.h"
#include "DeltaDetectRecurseQBox.h"
#include "DeltaDetectSampleQBox.h"

//#include "ChunkAssemblerQBox.h"
#include "FaceDetectQBox.h"
#include "FaceDetectChunkQBox.h"
#include "FaceDetectBatchQBox.h"
#include "FaceDetectAdjQBox.h"
#include "FaceDetectRecurseQBox.h"

#include "RegridQBox.h"
#include "RenderQBox.h"
#include "DeltaAisQBox.h"
#include "MergeSketchQBox.h"
#include "RenderDeltasQBox.h"
#include "FaceDetectSpatialQBox.h"

#include "MapFrameQBox.h"
#include "MapFrameCompositeQBox.h"

//#include "JoinFitsQBox.h"
#include "ConcatenateQBox.h"


BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
// Create a new instance of a user defined box.
//
QBox  *QBox::instantiate_array(// Type name of the box.
                               const string  &type)
{
//..............................................................................


    if      (type == "contrast")            return  new ContrastQBox;
    else if (type == "contrast_composite")  return  new ContrastCompositeQBox;
    else if (type == "delta_detect")        return  new DeltaDetectQBox;
    else if (type == "delta_detect_adj")    return  new DeltaDetectAdjQBox;
    else if (type == "delta_detect_batch")  return  new DeltaDetectBatchQBox;
    else if (type == "delta_detect_sample") return  new DeltaDetectSampleQBox;
    else if (type == "delta_detect_recurse") return new DeltaDetectRecurseQBox;
    //else if (type == "chunk_assembler")     return  new ChunkAssemblerQBox;
    else if (type == "face_detect")         return  new FaceDetectQBox;
    else if (type == "face_detect_recurse") return  new FaceDetectRecurseQBox;
    else if (type == "face_detect_spatial") return  new FaceDetectSpatialQBox;
    else if (type == "face_detect_batch")   return  new FaceDetectBatchQBox;
    else if (type == "face_detect_adj")     return  new FaceDetectAdjQBox;
    else if (type == "face_detect_chunk")   return  new FaceDetectChunkQBox;
    else if (type == "map_frame")           return  new MapFrameQBox;
    else if (type == "map_frame_composite") return  new MapFrameCompositeQBox;
    else if (type == "regrid")              return  new RegridQBox;
    else if (type == "merge")               return  new MergeQBox;
    //    else if (type == "join_fits")               return  new JoinFitsQBox;
    else if (type == "concatenate")               return  new ConcatenateQBox;
    else if (type == "render")               return  new RenderQBox;
    else if (type == "render_deltas")               return  new RenderDeltasQBox;
    else if (type == "delta_ais")               return  new DeltaAisQBox;
    

    return  NULL;
}


BOREALIS_NAMESPACE_END
