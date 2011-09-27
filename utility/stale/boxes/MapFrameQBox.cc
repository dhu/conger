#include "MapFrameQBox.h"
#include "tupleGenerator.h"
#include "DataPath.h"
#include "DynamicArray.h"

BOREALIS_NAMESPACE_BEGIN

void MapFrameQBox::setup_impl() throw (AuroraException)
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

void MapFrameQBox::init_impl() throw (AuroraException)
{
}

void MapFrameQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{

  int contrastIdx, irIdx, ccdIdx, selectedIdx;
  int contrast, array_addr, in_offset, out_offset;
  DynamicArray *array_info;
  uint8 *array_ptr;


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

        DEBUG << "MapFrameQBox " << get_name()
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
          irIdx = in_desc.index_of_field(string("ir_frame"));
          ccdIdx = in_desc.index_of_field(string("ccd_frame"));
          selectedIdx = out_desc.index_of_field(string("selected_frame"));
          // sanity checking
          if(contrastIdx != -1 && irIdx != -1 && ccdIdx != -1 && selectedIdx != -1)
        {
          in_offset = in_desc.get_offset(contrastIdx);
          
          contrast = *(int32 *) (((const char*)data) + in_offset);
          DEBUG << "Contrast: " << contrast;
          if(contrast < _contrast_threshold)
            {
              // select IR frame
              in_offset = in_desc.get_offset(irIdx);
              out_offset = out_desc.get_offset(selectedIdx);

              array_addr = *(int32 *) (((const char*)data) + in_offset);
              array_info = (DynamicArray *)((*(int32 *) (((char *)data) + in_offset)));
              array_ptr = (uint8 *)(array_info + DynamicArray::header_size());
              
              *(int32 *)(out+out_offset) = array_addr;
              
            }
          else
            {
              // select CCD frame
              in_offset = in_desc.get_offset(ccdIdx);
              out_offset = out_desc.get_offset(selectedIdx);
              
              array_addr = *(int32 *) (((const char*)data) + in_offset);
              array_ptr = (uint8 *)((*(int32 *) (((char *)data) + in_offset)));
              
              *(int32 *)(out+out_offset) = array_addr;

            }

            
            out_offset = out_desc.get_offset(selectedIdx);
            uint8 *array_ptr = (uint8 *)((*(int32 *) (out + out_offset))); 
            array_info = (DynamicArray *)((*(int32 *) (out + out_offset)));

            uint8 *array_start = array_ptr+DynamicArray::header_size();
            DEBUG << "First vals Selected: " << (uint) *(array_start) << ", " <<  (uint) *(array_start+1) << ", " <<  (uint) *(array_start+2);
            
            array_info->modify_array_reference(1);
        }
          
          
            }
        }

        // Lovely, pass along...
        DEBUG << "MapFrameQBox " << get_name()
            << " OUT: " << out_desc.tuple_to_string(ei.tuple());
        outputted = true;
        ++ei;
        ++di;
    }

    if (outputted)
    get_output(0).notify_enq();
}

BOREALIS_NAMESPACE_END
