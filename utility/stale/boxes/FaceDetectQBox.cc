#include "FaceDetectQBox.h"
#include "timer.h"
#include "DynamicArray.h"

BOREALIS_NAMESPACE_BEGIN

void FaceDetectQBox::setup_impl() throw (AuroraException)
{
  SchemaArray *the_array_info;
  vector<SchemaArray::DimensionInformation> theDimInfo;
  int dim_size;

    if ( get_num_inputs() != 1 )
    {   Throw(aurora_typing_exception,
              "FaceDetect requires exactly one input stream (not " +
              to_string(get_num_inputs()) + ")");
    }


    set_out_description( 0, get_in_stream( 0 ));
 


    DEBUG << "FaceDetectQBox input schema " << get_in_schema(0)->as_string();
    DEBUG << "FaceDetectQBox output schema " << get_out_schema(0)->as_string();

    TupleDescription in_td(get_in_schema(0));
    int the_field_index = in_td.index_of_field(string("frame"));
    _frame_offset = in_td.get_offset(the_field_index);

    const SchemaField *the_array = in_td.get_schema_field(the_field_index);
    the_array_info  = the_array->get_array();
    theDimInfo = the_array_info->get_all_dimensions();    
    for(unsigned int x = 0; x < theDimInfo.size(); ++x)
      {
    dim_size = (uint32)(theDimInfo[x]._upper - theDimInfo[x]._lower) + 1;
        _array_dimensions.push_back(dim_size);
      }
    
    const char *cascade_name = "/u/jennie/Array_Research/opencv-1.0.0/data/haarcascades/haarcascade_frontalface_alt2.xml";
    _cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );
    if(_cascade == 0)
        DEBUG << "Failed to load cascade";
    _storage = cvCreateMemStorage(0);
    _detection_count = 0;
    _total_time = 0.0;
    
}

void FaceDetectQBox::init_impl() throw (AuroraException)
{
  TupleDescription out_td(get_out_schema(0));
  _output_tuple_size = out_td.get_size();

}

void FaceDetectQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{
    //uint8 *image_pixel;
    DeqIterator my_deq_iterator = deq(0);
    timer the_time;

    TupleDescription in_td(get_in_schema(0));
    TupleDescription out_td(get_out_schema(0));

    // Do stuff...
    the_time.start();
    while (inv.continue_dequeue_on(my_deq_iterator, 0))
    {

        const void *data = my_deq_iterator.tuple();
        //Tuple input_tuple = Tuple::factory(data);
        
       // unconditionally pass through
       memcpy(enq(0).tuple(), data, _output_tuple_size);
       DynamicArray *my_array = (DynamicArray *) (*(uint32 *) ((((char *)data) + _frame_offset)));

       uint8 *the_array_pixels = (uint8 *) (((uint8*)my_array) + DynamicArray::header_size());
       IplImage *image = cvCreateImage( cvSize(_array_dimensions[0],_array_dimensions[1]),  IPL_DEPTH_8U, 3);
       image->imageData = (char *) the_array_pixels;       
       
       // fill it with the current image
    detect_on_region(image);
    //DEBUG << "First pixel in region: " << (uint) ((uchar*)(theImage->imageData))[0]
    //    << ", " << (uint)  ((uchar*)(theImage->imageData))[1] << ", " << (uint)  ((uchar*)(theImage->imageData))[2];
            cvReleaseImage(&image);

       ++ (enq(0));
       
       
       
       DEBUG << "Sanity test end: array field offset=" << _frame_offset << " size=" << my_array->array_size(get_in_schema(0)->get_schema_field("frame")->get_array())
                      << " header size=" << my_array->header_size();
     ++my_deq_iterator;
     get_output(0).notify_enq();
     //NOTICE << "Total detections: " << _detection_count;
    }
    the_time.stop();
    _total_time += the_time.elapsed_time();
    NOTICE << "Total time: " << _total_time;
 }


void FaceDetectQBox::detect_on_region(IplImage *img)
{
    DEBUG << "Doing detection!";
static CvScalar colors[] = 
    {
        {{0,0,255}},
        {{0,128,255}},
        {{0,255,255}},
        {{0,255,0}},
        {{255,128,0}},
        {{255,255,0}},
        {{255,0,0}},
        {{255,0,255}}
    };

    double scale = 1.3;
    IplImage* gray = cvCreateImage( cvSize(img->width,img->height), 8, 1 );
    IplImage* small_img = cvCreateImage( cvSize( cvRound (img->width/scale),
                         cvRound (img->height/scale)),
                     8, 1 );
    int i;

    cvCvtColor( img, gray, CV_BGR2GRAY );
    cvResize( gray, small_img, CV_INTER_LINEAR );
    cvEqualizeHist( small_img, small_img );
    cvClearMemStorage( _storage );

    if( _cascade )
    {
        DEBUG << "Have cascade!";
        double t = (double)cvGetTickCount();
        CvSeq* faces = cvHaarDetectObjects( small_img, _cascade, _storage,
                                            1.1, 2, 0/*CV_HAAR_DO_CANNY_PRUNING*/,
                                            cvSize(30, 30) );
        t = (double)cvGetTickCount() - t;
        DEBUG << "detection time = " << ((double)cvGetTickFrequency()*1000.) ;
        for( i = 0; i < (faces ? faces->total : 0); i++ )
        {
            DEBUG << "Drawing detection!";
            CvRect* r = (CvRect*)cvGetSeqElem( faces, i );
            CvPoint center;
            int radius;
            center.x = cvRound((r->x + r->width*0.5)*scale);
            center.y = cvRound((r->y + r->height*0.5)*scale);
            radius = cvRound((r->width + r->height)*0.25*scale);
            cvCircle( img, center, radius, colors[i%8], 3, 8, 0 );
            ++_detection_count;
        }
    }

    cvShowImage( "result", img );
    cvReleaseImage( &gray );
    cvReleaseImage( &small_img );   
}

void FaceDetectQBox::set_pending_box_state(ptr<AbstractBoxState> packed_box)
{
    //FaceDetectBoxState* actual_packed_box = static_cast<FaceDetectBoxState*>(packed_box.get());
    //_pending_box_state.set(new FaceDetectBoxState(*actual_packed_box));
    _pending_box_state = packed_box.dynamic_cast_to<FaceDetectBoxState>();
}

ptr<AbstractBoxState> FaceDetectQBox::pack_state(bool delta_only)
{
  INFO << "Packing state for FaceDetectQBox named [" << get_name() << "]";
  ptr<FaceDetectBoxState> filter_box_state(new FaceDetectBoxState());
  return filter_box_state;
}


BOREALIS_NAMESPACE_END
