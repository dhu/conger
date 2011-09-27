#include "MapFrameCompositeQBox.h"
#include "tupleGenerator.h"
#include "DataPath.h"
#include "DynamicArray.h"


BOREALIS_NAMESPACE_BEGIN
#define FRAME_WIDTH 352
#define FRAME_HEIGHT 288
#define FRAME_DEPTH 3
#define CHUNK_X 44
#define CHUNK_Y 44

void remap_pixel(char *src, char *dst, int x, int y)
{
  *(dst + (y * FRAME_WIDTH + x)*FRAME_DEPTH) = *(src + (y*FRAME_WIDTH+x)*FRAME_DEPTH);
  *(dst + (y * FRAME_WIDTH + x)*FRAME_DEPTH+1) = *(src + (y*FRAME_WIDTH+x)*FRAME_DEPTH+1);
  *(dst + (y * FRAME_WIDTH + x)*FRAME_DEPTH+2) = *(src + (y*FRAME_WIDTH+x)*FRAME_DEPTH+2);
}

void remap_chunk(char *src, char *dst, int startX, int startY)
{

  for (int y = 0; y < CHUNK_Y; ++y)
    {
      for(int x = 0; x < CHUNK_X; ++x)
    {
      remap_pixel(src, dst, startX+x, startY+y);
    }
    }
}


void MapFrameCompositeQBox::setup_impl() throw (AuroraException)
{


    typed_param("contrast_threshold", _contrast_threshold);
    if (get_num_inputs() != 1)
    {
        Throw(aurora_typing_exception,
              "Map requires exactly one input stream (not " +
              to_string(get_num_inputs()) + ")");
    }

    for (unsigned int i = 0; ; ++i)
    {
        string expr = param("expression." + to_string(i));
        if (expr.empty())
        {
            if (i == 0)
            {
                Throw(aurora_typing_exception,
                      "Map requires at least one expression parameter. \
                      (\"expression\" is deprecated)");
            }
            else
            {
                break;
            }
        }
    
        ExprContext ctxt;
        //ctxt.setTupleDescription(TupleDescription(getInDescription(0)));
        ctxt.set_context_schema(*get_in_schema(0));
        _expressions.push_back(Expression::parse(expr, ctxt));
    }  

    /*vector<string> field_names;
    for (unsigned int i = 0; i < _expressions.size(); ++i)
    {
        field_names.push_back(param("output-field-name." + to_string(i), 
                                    PARAM_NON_EMPTY));
    }

    // Build the output TupleDescription based on the expressions
    //
    // NOTE: Expression::getReturnTypes() returns a vector of DataTypes, but
    //       map function can *only* return one type. This, we assume to be 
    //       true (any other return types would mess things up due to 
    //       getReturnedSize forcing the copy of more than needed)

    vector<SchemaField> fields;
    for (unsigned int i = 0; i < _expressions.size(); ++i)
    {
        string name = param("output-field-name." + to_string(i),
                            PARAM_NON_EMPTY);
        fields.push_back(_expressions[i]->get_schema_field(name));
    } */
    
    
    //TupleDescription t(fields.begin(), fields.end());
    CatalogSchema in  = *get_in_schema(0);
    
    // Set output description (there's only one output stream)
    vector<SchemaField> fields;

      // copy the first two fields of the in schema
    fields.push_back(*in.get_schema_field(0));
    fields.push_back(*in.get_schema_field(1));
    fields[1].set_name("selected_frame");
    
    TupleDescription t(fields.begin(), fields.end());
    set_out_description(0, t);
}

void MapFrameCompositeQBox::init_impl() throw (AuroraException)
{
}

void MapFrameCompositeQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{

  int contrastIdx, irIdx, ccdIdx, selectedIdx, oContrastIdx;
  int array_addr, in_offset, out_offset;
  char *iContrastPtr, *oContrastPtr;
  DynamicArray *array_info;
  uint8 *irPtr, *ccdPtr;


    DeqIterator di = deq(0);
    EnqIterator ei = enq(0);
    bool outputted = false;

    TupleDescription in_desc(  get_in_schema( 0 ));
    TupleDescription out_desc( get_out_schema( 0 ));

    while (inv.continue_dequeue_on(di, 0))
    {
        const void *data = di.tuple();
        Tuple input_tuple = Tuple::factory(data);
        char *out = (char*)ei.tuple();

        DEBUG << "MapFrameCompositeQBox " << get_name()
            << " IN: " << in_desc.tuple_to_string(data);

        if (input_tuple.get_fake_flag()) // if a fake tuple, just pass along
        {
            memcpy(out, data, HEADER_SIZE);
            char *p = out + HEADER_SIZE;
            _ctxt.reset();
            _ctxt.set_tuple((const char *)data);
            int32 data_size = get_out_schema(0)->get_size();
            memset(p, 0, data_size);
        }
        else                           // otherwise, evaluate the function
        {   
            memcpy(out, data, HEADER_SIZE);
            char *p = out + HEADER_SIZE;
            _ctxt.reset();
            _ctxt.set_tuple((const char *)data);

            // If tuple is a BOUNDARY or a RECONCILIATION_DONE, 
            // keep the header, set all output fields to zero and propagate
            if ((input_tuple.get_tuple_type() == TupleType::BOUNDARY) ||
               (input_tuple.get_tuple_type() == TupleType::RECONCILIATION_DONE))
            { 
                int32 data_size = get_out_schema(0)->get_size(); 
                                        // Size of data part of output tuple
                memset(p,0,data_size);
            }
            else // Otherwise, perform actual mapping
            { 
          //Frame work instead of expression
          //Expression::eval_vector_into(_expressions, p, _ctxt);

          contrastIdx = in_desc.index_of_field(string("contrast"));
          oContrastIdx = out_desc.index_of_field(string("contrast"));
          irIdx = in_desc.index_of_field(string("ir_frame"));
          ccdIdx = in_desc.index_of_field(string("ccd_frame"));
          selectedIdx = out_desc.index_of_field(string("selected_frame"));
          // sanity checking
          if(contrastIdx != -1 && irIdx != -1 && ccdIdx != -1 && selectedIdx != -1 && oContrastIdx != -1)
        {
          in_offset = in_desc.get_offset(contrastIdx);
          out_offset = out_desc.get_offset(oContrastIdx);
          oContrastPtr = out+out_offset;
          iContrastPtr = ((char *)data+in_offset);
          for(int x = 0; x < 48; ++x)
            oContrastPtr[x] = iContrastPtr[x];
          
          // assign out ptr to ccd  by default
           in_offset = in_desc.get_offset(ccdIdx);
           out_offset = out_desc.get_offset(selectedIdx);
              
           array_addr = *(int32 *) (((const char*)data) + in_offset);
           ccdPtr = (uint8 *)((*(int32 *) (((char *)data) + in_offset)));
           *(int32 *)(out+out_offset) = array_addr;
    
           in_offset = in_desc.get_offset(irIdx);
           irPtr = (uint8 *)((*(int32 *) (((char *)data) + in_offset)));
    
           for (int i = 0; i < 6; ++i)
             {
               
               for(int j = 0; j < 8; ++j)
             {
               if((uint8) oContrastPtr[i*8+j] < _contrast_threshold)
                 {
                   DEBUG << "Remapping chunk " << i*8+j << "!";
                   remap_chunk((char *) irPtr, (char *)ccdPtr, j*CHUNK_X, i*CHUNK_Y);
                 }
             }
             }
           


          
          // Manage reference count
            out_offset = out_desc.get_offset(selectedIdx);
            array_info = (DynamicArray *)((*(int32 *) (out + out_offset)));
            
            array_info->modify_array_reference(1);
        }
          
          
            }
        }

        // Lovely, pass along...
        DEBUG << "MapFrameCompositeQBox " << get_name()
            << " OUT: " << out_desc.tuple_to_string(ei.tuple());
        outputted = true;
        ++ei;
        ++di;
    }

    if (outputted)
    get_output(0).notify_enq();
}

BOREALIS_NAMESPACE_END
