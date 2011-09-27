#include  "CatalogQuery.h"
#include  "CatalogBox.h"

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
//  Infer the type of Box out streams from in streams.
//  If an out stream already has a schema it is validated.
//  Upon entry, all in streams must have schemas and the box has been validated.
//
Status  CatalogBox::infer_array_out(// Map to save the composite schema.
                                    CatalogSchema::SchemaMap  *schema_map,

                                    CatalogStream  *in,
                                    

                                    CatalogStream  *out)
{
    Status    status = false;
    string    type;
//..............................................................................


    type = get_box_type();

    if (type == "delta_detect" || type == "chunk_assembler" || type == "delta_detect_batch" || type == "delta_detect_adj" || type == "delta_detect_recurse"  || type == "delta_detect_sample" || type == "delta_ais")
    {
       status = match_box_schema(in, out);
    }
    else if (type == "merge")
    {
        CatalogStream *in2 = get_in_stream(1);
        status = match_box_schema_two_in(in, in2, out);
    }
    else if (type == "join_fits")
    {
        CatalogSchema output;
        output.clone(*get_in_schema(0));
        output.set_schema_name("tricolor_frame");
        SchemaField img_field("tri_frame", DataType::ARRAY, 4, get_tuple_header_size());
        SchemaArray *img_info = new SchemaArray(DataType::BYTE, 1, NULL);
        // TODO: make it copy the first two dimensions from its predecessor
        img_info->add_dimension(0, 1023);
        img_info->add_dimension(0, 744);
        img_info->add_dimension(0, 2);
        img_field.set_array(img_info);
        output._field[0] = img_field;

        if(output._field.size() == 1)
        {
            SchemaField seq_no_f("seq_no", DataType::INT, 4, get_tuple_header_size() + 4);
            output._field.push_back(seq_no_f);
        }

        DEBUG << "FitsJoin output schema: " << output.as_string();
        status =  match_composite_schema(out, &output, schema_map);
         
        
    }
    else if (type == "concatenate")
    {
        CatalogSchema output;
        output.clone(*get_in_schema(0));
        output.set_schema_name("scrolling_frame");
        SchemaField img_field("scroll_frame", DataType::ARRAY, 4, get_tuple_header_size());
        SchemaArray *img_info = new SchemaArray(DataType::BYTE, 1, NULL);
        // TODO: make it copy the first two dimensions from its predecessor
        img_info->add_dimension(0, 1023);
        img_info->add_dimension(0, 2787); // TODO: check this - or base off of the types params if we can get them here
        img_info->add_dimension(0, 2);
        img_field.set_array(img_info);
        output._field[0] = img_field;
        
        DEBUG << "FitsJoin output schema: " << output.as_string();
        status =  match_composite_schema(out, &output, schema_map);
    }
    else if (type == "face_detect" || type == "face_detect_chunk" || type == "face_detect_batch" || type == "face_detect_spatial" || type == "face_detect_adj" || type== "face_detect_recurse")
    {
        status = match_box_schema(in, out);
    }
    else if (type == "edge_detect")
    {
       status = match_box_schema(in, out);
    }
    else if (type == "contrast" || type == "contrast_composite")
    {
       status = match_box_schema(in, out);
    }
    else if(type == "regrid") // this will be more complex later
    {
        status = match_box_schema(in, out);
    }
    else if(type == "render"  || type == "render_deltas")
    {
        // an arbitrary match like map_frame_*
        // this is bad because it presupposes that the array must be the first tuple field
        CatalogSchema      composite;

        composite.clone(*get_in_schema(0));    
        SchemaField map_field("map", DataType::ARRAY, 4, get_tuple_header_size());
        SchemaArray *map_info = new SchemaArray(DataType::BYTE, 1, NULL);
        map_info->add_dimension(0, 639);
        map_info->add_dimension(0, 479);
        map_info->add_dimension(0, 2);
        map_field.set_array(map_info);
        composite._field[0] = map_field;
        
        status =  match_composite_schema(out, &composite, schema_map);
        
        
        
        //status = match_composite_schema(out, &out_schema, schema_map);
    }
    // hack -- don't try this at home!
    else if (type == "map_frame_composite" || type == "map_frame")
    {
        CatalogSchema      composite;
        CatalogSchema      *in_schema;
        SchemaField theArray;
        vector<SchemaField> theDummy;

        vector<SchemaField>::iterator thePos = get_in_schema(0)->get_schema_field().begin();
        thePos++;

        in_schema = get_in_schema(0);
        copy(in_schema->get_schema_field().begin(), thePos,
             back_inserter(composite.get_schema_field()));

        theArray = *(in_schema->get_schema_field(1));
    
        theArray.set_name("selected_frame");
        theDummy.push_back(theArray);

        copy(theDummy.begin(), theDummy.end(), 
             back_inserter(composite.get_schema_field()));

        status =  match_composite_schema(out, &composite, schema_map);
    }

    return  status;
}


BOREALIS_NAMESPACE_END
