#include "FaceDetectSpatialQBox.h"
#include "timer.h"
#include "DynamicArray.h"
#include <Magick++.h>

BOREALIS_NAMESPACE_BEGIN

void FaceDetectSpatialQBox::setup_impl() throw (AuroraException)
{
  vector<SchemaArray::DimensionInformation> theDimInfo;

    if ( get_num_inputs() != 1 )
    {   Throw(aurora_typing_exception,
              "FaceDetectSpatial requires exactly one input stream (not " +
              to_string(get_num_inputs()) + ")");
    }


    set_out_description( 0, get_in_stream( 0 ));
 


    DEBUG << "FaceDetectSpatialQBox input schema " << get_in_schema(0)->as_string();
    DEBUG << "FaceDetectSpatialQBox output schema " << get_out_schema(0)->as_string();

    TupleDescription in_td(get_in_schema(0));
    int the_field_index = in_td.index_of_field(string("frame"));
    _frame_offset = in_td.get_offset(the_field_index);
    
    //the_field_index = in_td.index_of_field(string("frame_size"));
    //_size_offset = in_td.get_offset(the_field_index);

    const SchemaField *the_array = in_td.get_schema_field(the_field_index);
    _array_schema  = the_array->get_array();
    /*theDimInfo = the_array_info->get_all_dimensions();    
    for(unsigned int x = 0; x < theDimInfo.size(); ++x)
      {
    dim_size = (uint32)(theDimInfo[x]._upper - theDimInfo[x]._lower) + 1;
        _array_dimensions.push_back(dim_size);
      } */
    uint32 x_dim, y_dim, z_dim;    
    typed_param("dimension.0", x_dim);
    typed_param("dimension.1", y_dim);
    typed_param("dimension.2", z_dim);
    DEBUG << "Dimensions: " << x_dim << ", " << y_dim << ", " << z_dim;
    _array_size = x_dim*y_dim*z_dim;
    _array_dimensions.push_back(x_dim);
    _array_dimensions.push_back(y_dim);
    _array_dimensions.push_back(z_dim);
    
    const char *cascade_name = "/u/jennie/Array_Research/opencv-1.0.0/data/haarcascades/haarcascade_frontalface_alt2.xml";
    _cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );
    if(_cascade == 0)
        DEBUG << "Failed to load cascade";
    _storage = cvCreateMemStorage(0);
    _detection_count = 0;    
    _time_fd = _time_prep = 0.0;
}

void FaceDetectSpatialQBox::init_impl() throw (AuroraException)
{
  TupleDescription out_td(get_out_schema(0));
  _output_tuple_size = out_td.get_size();

}

void FaceDetectSpatialQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{
    uint32 size, out_size;
    
    
    //BYTE *buffer;
   
    char *buffer[_array_size];
    //RGBQUAD current_pixel;
    IplImage *ocv_image;
    timer timer_fd, timer_prep;
    Magick::Blob in_blob, out_blob;
    Magick::Image in_image, out_image;
    //--------------------------------------------------
    DeqIterator my_deq_iterator = deq(0);

    TupleDescription in_td(get_in_schema(0));
    TupleDescription out_td(get_out_schema(0));

    while (inv.continue_dequeue_on(my_deq_iterator, 0))
    {

        const void *data = my_deq_iterator.tuple();
                
       // unconditionally pass through
    timer_prep.start();
       memcpy(enq(0).tuple(), data, _output_tuple_size);
       DynamicArray *my_array = (DynamicArray *) (*(uint32 *) ((((char *)data) + _frame_offset)));
       uint8 *the_array_data = (uint8 *) (((uint8*)my_array) + DynamicArray::header_size());
       
       size = my_array->array_size(_array_schema) / _array_schema->array_size();

       in_blob = Magick::Blob(the_array_data, size);
       in_image.read(in_blob);
       in_image.write(0, 0, _array_dimensions[0],_array_dimensions[1], "RGB", Magick::CharPixel, buffer);
       
       ocv_image = cvCreateImage( cvSize(_array_dimensions[0],_array_dimensions[1]),  IPL_DEPTH_8U, 3);
       
       ocv_image->imageData = (char *) buffer;
       timer_prep.stop();
       _time_prep += timer_prep.elapsed_time();
       timer_fd.start();
       detect_on_region(ocv_image);
       timer_fd.stop();
       _time_fd += timer_prep.elapsed_time();
       out_image = Magick::Image(_array_dimensions[0],_array_dimensions[1], "RGB", Magick::CharPixel, ocv_image->imageData);
       out_image.magick("JPEG");
       out_image.write(&out_blob);
       //current_pixel = in_image.GetPixelColor(0, 0);
       //DEBUG << "First cx pixels: " << (uint) current_pixel.rgbBlue << ", " << (uint) current_pixel.rgbGreen << ", " << (uint) current_pixel.rgbRed; 
       //DEBUG << "First pixels: " << (int) (image->imageData)[0] << ", " << (int) (image->imageData)[1] << ", " << (int) (image->imageData)[2];
       /*for(unsigned int x = 0; x < _array_dimensions[0]; ++x)
          for(unsigned int y = 0; y < _array_dimensions[1]; ++y)
          {
            // TODO: Only update regions where there were detections; don't copy at all w/no detections
             current_pixel.rgbBlue = (BYTE) ((uchar*)(image->imageData + image->widthStep*y))[x*3]; 
             current_pixel.rgbGreen = (BYTE) ((uchar*)(image->imageData + image->widthStep*y))[x*3+1];
             current_pixel.rgbRed = (BYTE) ((uchar*)(image->imageData + image->widthStep*y))[x*3+2];
             in_image.SetPixelColor(x, y, current_pixel);
          } */
        //cvReleaseImage(&image);
        //CxMemFile memfile;
        //memfile.Open();
        //in_image.Rotate180();
        //in_image.Encode(&memfile, CXIMAGE_FORMAT_JPG);
        //buffer = memfile.GetBuffer();
        //out_size = memfile.Size();
        //in_image.magick("JPEG");
        //in_image.write(&out_blob);
        out_size = out_blob.length();
        DynamicArray *out_array;
        out_array = DynamicArray::allocate_array(out_size);
        out_array->set_count_size(1, out_size);
        uint8 *out_array_data = ((uint8 *)out_array) + DynamicArray::header_size();
        memcpy(out_array_data, (uint8 *) out_blob.data(), out_size);
        uint32 *element = (uint32 *)((uint8 *)enq(0).tuple() + _frame_offset); 
        *element  = (uint32) out_array;        
        //uint32 *size_field = (uint32 *)((uint8 *)enq(0).tuple() + _size_offset);
        //*size_field = out_size;
        // clean up
        //free(buffer);
        my_array->modify_array_reference(-1);


       ++ (enq(0));
       
     ++my_deq_iterator;
     get_output(0).notify_enq();
     //NOTICE << "Total detections: " << _detection_count;
     //DEBUG << "Sent tuple size=" << out_size << " bytes " << to_hex_string(the_elements, 20);
    }
    NOTICE << "Prep time: " << _time_prep << " fd time: " << _time_fd;
 }


void FaceDetectSpatialQBox::detect_on_region(IplImage *img)
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
    DEBUG << "Created grayscale with: " << img->width << ", " << img->height; 
    IplImage* small_img = cvCreateImage( cvSize( cvRound (img->width/scale),
                         cvRound (img->height/scale)),
                     8, 1 );
   DEBUG << "Created color!";
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

    //cvShowImage( "result", img );
    cvReleaseImage( &gray );
    cvReleaseImage( &small_img );   
}

void FaceDetectSpatialQBox::set_pending_box_state(ptr<AbstractBoxState> packed_box)
{
    //FaceDetectSpatialBoxState* actual_packed_box = static_cast<FaceDetectSpatialBoxState*>(packed_box.get());
    //_pending_box_state.set(new FaceDetectSpatialBoxState(*actual_packed_box));
    _pending_box_state = packed_box.dynamic_cast_to<FaceDetectSpatialBoxState>();
}

ptr<AbstractBoxState> FaceDetectSpatialQBox::pack_state(bool delta_only)
{
  INFO << "Packing state for FaceDetectSpatialQBox named [" << get_name() << "]";
  ptr<FaceDetectSpatialBoxState> filter_box_state(new FaceDetectSpatialBoxState());
  return filter_box_state;
}


BOREALIS_NAMESPACE_END
