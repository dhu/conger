#include  "Catalog.h"
#include  "parseutil.h"


BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
// If defining a schema, box, or table then see that they are complete.
//
Status  Catalog::prior_element()
{
    Status   status = true;
//..............................................................................


    if (( _add_box )  &&  ( _add_in + _add_out == 0 ))
    {   status = "The box ( " + to_string( _add_box->get_box_name() )
            + " ) has neither in nor out declarations.";
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
//  End any active prior declarations.
//
Status  Catalog::close_prior()
{
    Status status = true;
//..............................................................................


    if( _add_box != NULL )
    {   status = validate_number_of_in(); // type, in_num);

        if ( status )
        {   status = validate_number_of_out();  // name, type, number_of_out);
        }

        //if( status )
        //status = validateOut( name ); //- call this for the last box in the query

        if ( status )
        {   status = validate_table_present(); //  name, type );
        }

        if ( status )
        {  status = validate_number_of_param(); // type,  number of params )
        }

        if ( status )
        {   status = validate_parameter_present(); // name,  number of parameters );
        }
    }

    _add_schema = NULL;
    _add_box    = NULL;
    _add_table  = NULL;
    _add_view   = NULL;

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
// See if a schema can be added to a catalog structure.
//
Status  Catalog::validate_schema( // Schema to create.
                                 Name  name )
{
    Status   status = true;
    CatalogSchema::SchemaMap::iterator in_schema;
//..............................................................................


    in_schema = _topology._schema_map.find( name );

    if ( in_schema != _topology._schema_map.end() )
    {   status = "Schema ( " + to_string( name ) + " ) is already cataloged.";
    }

    if ( status )
    {  status = prior_element();
    }

    DEBUG << "status = " << status;
    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
// See if a field can be added to a schema in a catalog structure.
//
Status  Catalog::validate_field( // Name of the field.
                                string     name,

                                // The type of the field.
                                DataType  type,

                                // Size is required for string types.
                                uint16    size )
{
    Status   status = true;
//..............................................................................


    if ( !_add_schema )
    {   status = "Attempt to add field ( " + name
            + " ) before beginning a schema.";
    }
    else if (( type == DataType::STRING )  &&  ( !size ))
    {   status = "The string field ( " + name + " ) has a size of zero.";
    }
    else if ( type == DataType::NONE )
    {   status = "The field ( " + name + " ) has the NONE type.";
    }

    DEBUG << "status = " << status;
    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
// See if a dataflow stream can be added to the catalog.
//
Status  Catalog::validate_stream( // Stream to create.
                                 Name    name,

                                 // Data types in the stream.
                                 Name    schema
                                )
{
    Status   status = true;
    CatalogSchema::SchemaMap::iterator  in_schema;
    StreamMap::iterator  in_stream;
//..............................................................................


    in_schema = _topology._schema_map.find( schema );

    if ( in_schema == _topology._schema_map.end() )
    {   status = " Can not add the stream ( "        + to_string( name )
            + " ) before declaring the schema ( " + to_string( schema ) + " ).";
    }
    else
    {   in_stream = _topology._stream_map.find( name );

        // Input and output streams are parsed before boxes.
        // Consequently intermediate streams between boxes are defined after
        // this method is invoked so this check is valid.
        //
        if ( in_stream != _topology._stream_map.end() )
        {   status = "Stream ( " + to_string( name ) + " ) is already cataloged.";
        }
    }

    if ( status )
    {  status = prior_element();
    }

    DEBUG << "status = " << status;
    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
// See if an input or output stream can be added to the catalog.
//
Status  Catalog::validate_input_output( // Stream to connect to input or output.
                                       string  stream
                                       )
{
    Status   status = true;
    StreamMap::iterator  in_stream;
//..............................................................................


    in_stream = _topology._stream_map.find( stream );

    if ( in_stream == _topology._stream_map.end() )
    {   status = "Can not add an input or output to an undeclared stream ( "
            + to_string( stream )+ " ).";
    }

    if ( status )
    {  status = prior_element();
    }

    DEBUG << "status = " << status;
    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
// See if a box can be added to the catalog.
//
Status  Catalog::validate_box( // Name of the box.
                              Name    name,

                              // Box operation.
                              string  type
                              )
{
    Status   status = true;
    BoxMap::iterator  in_box;
//..............................................................................


    in_box = _topology._box_map.find( name );

    if ( in_box != _topology._box_map.end() )
    {   status = "Box ( " + to_string( name ) + " ) is already cataloged.";
    }

    if (( status )  && ( !_add_query ))
    {   status = "Box ( " + to_string( name )
            + " ) is defined outside any query.";
    }

    if ( status )
    {   // First check if an intrinsic box.
        //
        if ( !( type == "aggregate"     ||  type == "filter"            ||
        type == "filter_array"  ||
                type == "join"          ||  type == "union"             ||
                type == "bsort"         ||  type == "map"               ||
                type == "update"        ||  type == "insert"            ||
                type == "select"        ||  type == "delete"            ||
                type == "lock"          ||  type == "unlock"            ||
                type == "waitfor"       ||
                type == "sjoin"         ||  type == "scontrol"          ||
                type == "soutput"       ||  type == "sunion"            ||
                type == "window_drop"   ||  type == "random_drop"       ||
                type == "revisionfilter"||  type == "revisionmap"       ||
                type == "aurorajoin"    ||  type == "revisionaggregate" ))
        {
            // Then check if a user defined box 
            // (move to borealis/src/external/).
            //
            if ( !( type == "joinstatic"  ||  type == "combine"  ||
                    type == "delay"       ||  type == "LRWaitFor"||
                    type == "delay_sel"   || // tatbul -- delay with selectivity
            // jennie - array-specific boxes
            type == "edge_detect" || type == "map_frame"  ||
            type == "map_frame_composite"  || 
            type == "contrast" || type == "contrast_composite" ||
            type == "delta_detect" || type == "delta_detect_batch" || 
            type == "delta_detect_recurse" || type == "delta_detect_adj" || 
            type == "chunk_assembler" || type == "face_detect" || 
            type == "face_detect_chunk" || type == "face_detect_batch" || 
            type == "face_detect_spatial" || type == "face_detect_adj" ||
             type == "delta_detect_sample" || type == "face_detect_recurse" || type == "regrid" || type == "merge" || type == "join_fits" 
            || type == "render" || type == "render_deltas" || type == "delta_ais" || type == "concatenate" || type == "scatter" || type == "gather"))
            {
                status = "Unknown box type ( " + type + " ).";
            }
        }
    }

    if ( status )
    {  status = prior_element();
    }

    DEBUG << "status = " << status;
    return  status;
}



///////////////////////////////////////////////////////////////////////////////
//
// See if the number of input streams passed to a given box are correct.
//
Status  Catalog::validate_number_of_in()
{
    Status   status = true;
    string   type;
//.............................................................................


    type = _add_box->get_box_type();

    // only filter box can have more than one output stream connected to it.
    //
    if (( type == "join"  || type == "aurorajoin" || type == "waitfor" || type == "merge" || type == "join_fits" )
        && ( _add_in != 2 ))
    {
        status = "Boxtype " + type + " must have exactly TWO input streams.";
    }

    if ( status )
    {   if (( type == "filter"    ||  type == "filter_array" ||   
          type == "map"       ||  type == "edge_detect" || 
          type == "map_frame" ||  type == "contrast" ||
          type == "contrast_composite" || type == "map_frame_composite"  ||
              type == "lock"      ||  type == "unlock" ||
              type == "insert"    ||  type == "update" ||
              type == "select"    ||  type == "delete" ||
              type == "aggregate" ||  type == "revisionmap" || 
          type == "delta_detect" || type == "delta_detect_adj" || 
          type == "delta_detect_sample" || type == "delta_detect_recurse" ||
          type == "chunk_assembler" ||
          type == "face_detect" || type == "face_detect_chunk" || 
          type == "face_detect_batch" || type == "delta_detect_batch" || type == "face_detect_adj" || 
          type == "face_detect_spatial" || type == "face_detect_recurse" || type == "regrid" || type == "render" || type == "render_deltas" || type == "delta_ais" || type == "concatenate" || type == "scatter")
            && ( _add_in != 1 ))
        {
            status = "Box ( " + to_string( _add_box->get_box_name()) + " ) of type "
                + type + " must have exactly ONE input streams.";
        }
    }
    if(type == "gather"  && _add_in != 2)
      status = "Gather requires two input streams.";
    if ( status )
    {  status = prior_element();
    }

    DEBUG << "status = " << status;
    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
// See if schema for all input streams  match.
//
Status  Catalog::validate_more_in( // first input stream,
                                  Name  stream
                                  )
{
    Status           status = true;
    string           type;
    Name             name;
    Name             in_stream;
    CatalogSchema   *in_schema;
    CatalogStream   *connect;

    CatalogStream::StreamStar     in;
    CatalogStream::StreamStar::iterator i;
//..............................................................................


    type = _add_box->get_box_type();
    name = _add_box->get_box_name();

    // Only union and join can have more than one input stream connected to a box
    //
    if (( _add_in >= 1 )  &&  ( type == "union" ))
    {   in = *_add_box->get_box_in();

        for (i = in.begin(); i != in.end(); i++)
        {
            in_stream = (*i)->get_stream_name();
            in_schema = (*i)->get_stream_schema();

            if ( in_schema != NULL )
            {   connect  = &( _topology._stream_map[ stream ] );

                if ( connect->get_stream_schema() != NULL )
                {
                    if ( connect->get_stream_schema() != in_schema )
                    {
                       /*
                       Jeong-Hyon: I commented this out because it is not the right 
                       way of doing the semantic check!! It doesn't consider thpe
                       equivalence at all. I.e., we should allow those with type equivalance.
                       
                       status = "Schema ( " + to_string( connect->get_schema_name() ) +
                                 " ) of input stream ( "             + to_string( stream ) +
                                 " ) is different from the schema (" + to_string( in_schema->get_schema_name() ) +
                                 " ) of previous input stream ( "    + to_string( in_stream ) +
                                 " ) for box ( "                     + to_string( name )   +
                                 " ) of type ( "                     + _add_box->get_box_type()   +  ").";
                       */                                 
                    }
                }
            }
        }
    }

    //status = priorElement(); // getting called before adding to first input to a box.
    DEBUG << "status = " << status;
    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
// See if number of output streams exceed the allowed limit
//
Status  Catalog::validate_number_of_out()
{
    Status   status = true;
    string  type;
//...............................................................................


    type = _add_box->get_box_type();

    // only filter box can have more than one output stream connected to it.
    //
    if(type == "scatter")
      {
        if(_add_out != 2)
    {
      DEBUG << "Dealing with scatter status in catalog validate";
            status = "Box ( " + to_string( _add_box->get_box_name() ) + " ) of type ( "
            + type + " ) must have two output streams.";
            return status;
    }
        else
      return true;  
      }
      if (    !( type == "filter"  ||
           type == "filter_array" ||
           type == "edge_detect" ||
           type == "contrast" ||
           type == "contrast_composite" || 
           type == "map_frame" || 
           type == "map_frame_composite" ||
               type == "delete"  || 
               type == "select"  || 
               type == "revisionfilter" || 
               type == "sunion" || 
               type == "soutput" ||
               type == "mdimsolver" ||
               type == "splitter" ||
               type == "model_aggregate" || 
           type == "delta_detect" ||
           type == "delta_detect_recurse" ||
           type == "delta_detect_sample" ||
           type == "delta_detect_adj" ||
           type == "delta_detect_batch"  ||
           type == "chunk_assembler" ||
           type == "face_detect" ||
           type == "face_detect_chunk" || 
           type == "face_detect_batch" || 
           type == "face_detect_adj" || 
           type == "face_detect_spatial" || 
           type == "face_detect_recurse" ||
           type == "regrid" || type == "merge" || type == "join_fits" || type == "render" || type == "render_deltas" || type == "delta_ais" || type == "gather")
            && _add_out > 1 )
    {
      DEBUG << "Adding status for box " << type;
        status = "Box ( " + to_string( _add_box->get_box_name() ) + " ) of type ( "
            + type + " ) cannot have more than one output stream.";
    }

    if ( status )
    {  status = prior_element();
    }

    DEBUG << "status = " << status;
    return  status;
}



/////////////////////////////////////////////////////////////////////////////////
//
// See if schema for output stream matches with the schema of input stream
// Call this function for the last box in query after Type Inferencing is implemented
//
Status  Catalog::validate_out(// output stream,
                              Name  stream
                              )
{
    Status   status = true;
    string   type;
    Name     name;
    CatalogStream  *in_stream;
    CatalogStream  *out_connect;
//..............................................................................

    type = _add_box->get_box_type();
    name = _add_box->get_box_name();

    // only union, filter and bsort require to have same schema for input and output
    if ( type == "union" || type == "filter" || type == "filter_array" || type == "edge_detect" || type == "bsort" || type == "revisionfilter" || type == "contrast" || type == "contrast_composite" || type == "delta_detect"  || type == "chunk_assembler" || type == "face_detect" || type == "face_detect_chunk" || type == "face_detect_batch" || type == "delta_detect_batch" || type == "face_detect_spatial" || type == "delta_detect_adj" || type == "face_detect_adj" || type == "delta_detect_sample" || type == "delta_detect_recurse" || type == "face_detect_recurse" || type == "regrid" || type == "delta_ais" || type == "scatter"  || type == "gather")
    {
        for ( uint16  i=0; i<= _add_in; ++i )
        {   in_stream = _add_box->get_in_stream( i );

            //  if( !(in_stream->getStreamSchema()) ) //!= NULL )
            //  {
            out_connect  = &( _topology._stream_map[ stream] );

            if( out_connect->get_stream_schema() != in_stream->get_stream_schema() )
            {  status ="Schema ( "                   + to_string( out_connect->get_schema_name() ) +
                    " ) of input stream ( "         + to_string( stream ) +
                    " ) is different from the schema (" + to_string( in_stream->get_schema_name() ) +
                    " ) of previous input stream ( "    + to_string( in_stream->get_stream_name()) +
                    " ) for box type ("                 + _add_box->get_box_type()   +  ").";
            }
            //  }
        }
    }

    if ( status )
    {  status = prior_element();
    }

    DEBUG << "status = " << status;
    return  status;
}



/////////////////////////////////////////////////////////////////////////////////
///
/// See if the table tag is present for box types insert, update or select
///
Status  Catalog::validate_table_present()
{
    Status   status = true;
    string   type;
//...............................................................................


    type = _add_box->get_box_type();

    if (( type == "select" || type == "insert" ||
          type == "update" || type == "delete" )
        && ( !_add_table ))
    {
        status = "For a box ( " + to_string( _add_box->get_box_name() )
            + " of type " + type
            + " table name must be present inside the box schema.";
    }

    if ( status )
    {  status = prior_element();
    }

    DEBUG << "status = " << status;
    return( status );
}



////////////////////////////////////////////////////////////////////////////////
//
// See if the number of parameter passed to a box are correct.
//
Status  Catalog::validate_number_of_param()
{
    Status     status = true;
    string     type;
    string::size_type  size;
//..............................................................................


    type = _add_box->get_box_type();
    size = _add_box->get_box_parameter_map()->size();

    if ((type != "union")    &&  (type != "sunion")  &&
        (type != "soutput")  &&  (size == 0))
    {
        status = " Box ( " + type + " ) expects at least one predicate ";
    }

    if ( status )
    {  status = prior_element();
    }

    DEBUG << "status = " << status;
    return( status );
}



////////////////////////////////////////////////////////////////////////////////
//
// See if the parameter passed is appropriate for the given box
//
Status Catalog::validate_parameter( // parameter name,
                                   string  parameter,

                                   // Value to assign the parameter.
                                   string  value
                                   )
{
    Status   status = true;
    string   type;
    Name     name;
//..............................................................................


    if ( _add_box )
    {   type = _add_box->get_box_type();
        name = _add_box->get_box_name();

        // making a local copy of value and then converting it to lowercase.
        string val = value;
        lowercase( val );

        if ( type == "aggregate" || type == "revisionaggregate" )
        {   if ( parameter=="order-by"  &&  !  (val=="field"  ||  val=="tuplenum" ) )
            {    status = "For box ( " + to_string( name ) + " ) of type ( " + type
                    + " order-by must have a value defined as either FIELD or TUPLENUM.";
            }

            if ( status )
            {   if ( parameter == "window-size-by"  &&
                     !(val == "values"  ||  val == "tuples" ))
                {
                    status = "For box ( " + to_string( name ) + " ) of type ( " + type
                        + " window-size-by must have a value defined as either VALUES or TUPLES.";
                }
            }

            if ( status )
            {   if ( parameter == "window-size"  ||  parameter == "advance"  ||
                     parameter == "slack"        ||  parameter == "timeout"  )
                {
                    int i = atoi( value.c_str() );

                    if ( i == 0 )
                    {   status = "For box ( " + to_string( name )  + " ) of type  ( " + type
                            + " parameter" + parameter + " is not an integer. ";
                    }
                }
            }
        }
        else if ( type == "join" || type == "aurorajoin" )
        {   if (( parameter=="left-order-by"  || parameter=="right-order-by" )
                &&  !( val=="values"  ||  val=="tuples" ) )
            {
                status = "For a ( join ) box ( " + to_string( name ) + " ) its parameter ( " + parameter
                    + " ) must have a value defined as either VALUES or TUPLES.";
            }
            // yna -- it's okay for one side to be zero size, i.e. an "asymmetric" join
            else if ( //parameter=="left-buffer-size"  ||  parameter=="right-buffer-size" ||
                      parameter=="left-buffer-size"  &&  parameter=="right-buffer-size" ||
                      parameter=="timeout" )
            {
                int i = atoi(val.c_str());


                if( i == 0 )
                {   status = " Parameter ( " + parameter + " ) for box ( " + to_string( name )
                        + " ) is not an integer.";
                }
            }
        }
        else if( type == "filter" || type == "filter_array" || type == "edge_detect" || type == "revisionfilter" || type == "contrast" || type == "contrast_composite" ) 
        {   if (( parameter.substr( 0, 11 ) == "expression." )  &&
                ( parameter.substr( 11 ) != "0" ))
            {
                int i = atoi( parameter.substr( 11 ).c_str() );

                if ( i == 0 )
                {   status = "Parameter ( " + parameter
                        + " ) does not have an integer extension for box ( " + to_string( name )
                        + " ) of type ( Filter ).";
                }
            }
        }
        else if ( type == "map" || type == "revisionmap" )
        {   if (( parameter.substr(0, 11) == "expression." )  &&
                ( parameter.substr(11) != "0" ))
            {
                int i = atoi( parameter.substr(11).c_str() );

                if ( i == 0 )
                {   status = "Parameter ( " + parameter + " ) for box ( " + to_string( name )
                        + " ) does not have an integer extension for box type ( Map ).";
                }
            }
            else if (( parameter.substr(0, 18) == "output-field-name." )  &&
                     ( parameter.substr(18) != "0" ) )
            {
                int i = atoi( parameter.substr(18).c_str() );

                if ( i == 0 )
                {   status = "Parameter " + parameter + " ) for box ( " + to_string( name )
                        + " does not have an integer extension for box type ( Map ).";
                }
            }
        }
        else if (( type == "bsort" && parameter=="slack" ) ||
                 // yna -- commenting out, the key is an expression not a constant int!
//                 (( type == "lock"  || type == "unlock") && parameter=="key" ) ||
                 ( type == "waitfor"  && parameter == "timeout" ))
        {
            int i = atoi( val.c_str() );

            if ( i == 0 )
            {   status = "Parameter ( " + parameter + " ) for Box ( " + to_string( name )
                    + " has not an integer value for box type ( " + type + " ).";
            }
        }
        else if( type == "insert" || type == "update" ||
                 type == "select" || type == "delete" )
        {   if ( parameter == "sql" && val == "" )
            {   status = " A SQL statement is expected for parameter=sql for box ( " + to_string( name )
                    + " ) of type ( " + type + " ).";
            }
        }
        else if ( type == "update" )
        {   if ((( parameter.substr(0, 18) == "output-expression." ) ||
                 ( parameter.substr(0, 18) == "output-field-name." ))  &&
                (  parameter.substr(18) != "0" ))
            {
                int i = atoi( parameter.substr(18).c_str() );

                if ( i == 0 )
                {   status = "Parameter ( " + parameter + " ) for Box ( " + to_string( name ) +
                        " ) does not have an integer extension for box type ( Update ).";
                }
            }
        }
    }

    if ( status )
    {  status = prior_element();
    }

    DEBUG << "status = " << status;
    return( status );
}


////////////////////////////////////////////////////////////////////////////////
//
// See if the required parameters for a specific box are present.
//
Status  Catalog::validate_parameter_present()
{
    Status      status = true;
    CatalogBox *box;
    Name        name;
    string      type;
    string::size_type   size;

    Boolean  order_by       = False;
    Boolean  window_size_by = False;
    Boolean  order_on_field = False;
    Boolean  predicate      = False;
    Boolean  left_order_by  = False;
    Boolean  right_order_by = False;
    Boolean  hit            = False;

    string   order_by_value, window_size_by_value;

    BoxParameter::ParameterMap* param;
    BoxParameter::ParameterMap::iterator  parameter;
//..............................................................................


    box  = _add_box;
    name = _add_box->get_box_name();
    type = box->get_box_type();

    param = box->get_box_parameter_map();

    if ( type == "aggregate" )
    {   for (  parameter  = param->begin();
               parameter != param->end(); ++parameter )
        {
            if ( parameter->first == "order-by" )
            {   order_by = True;   // to show that order-by is present
                order_by_value = parameter->second;
            }

            if ( parameter->first == "window-size-by" )
            {   window_size_by = True;
                window_size_by_value = parameter->second;
            }

            if ( parameter->first == "order-on-field" )
            {   order_on_field = True;
            }
        }

        if ( !order_by )
        {   status = " order-by is a required field for an aggregate box( " + to_string(name) + " ).";
        }
        else if ( !window_size_by )
        {   status = " window-size-by is a required field for an aggregate box( "
                + to_string(name) + " ).";
        }

        lowercase( order_by_value );
        lowercase( window_size_by_value )

            if ((( order_by         &&  order_by_value == "field" )   ||
                 ( window_size_by   &&  window_size_by_value == "values" ))
                && ( !order_on_field ))
            {
                status = " order-on-field is required for an" + type +
                    + " box " + to_string(name) + " when window-size-by=VALUES or order-by=FIELD .";
            }
    }
    else if ( type == "join" || type == "aurorajoin" )
    {   for (  parameter  = param->begin();
               parameter != param->end(); ++parameter )
        {
            if ( parameter->first == "predicate" )
            {  predicate = True;
            }

            if( parameter->first == "left-order-by" )
            {  left_order_by  = True;
            }

            if ( parameter->first == "right-order-by" )
            {   right_order_by = True;
            }
        }

        if ( !predicate )
        {   status = " PREDICATE parameter is required for a join box( "
                + to_string(name) + " ).";
        }
        else if ( !left_order_by )
        {   status = " left-order-by parameter is required for a join box( "
                + to_string(name) + " ).";
        }
        else if( !right_order_by )
        {   status = " right-order-by parameter is required for a join box( "
                + to_string(name) + " ).";
        }
    }
    else if ( type == "filter" || type == "filter_array" || type == "edge_detect" || type == "revisionfilter" || type == "contrast" || type == "contrast_composite" ) 
    {   for ( parameter  = param->begin();
              parameter != param->end(); ++parameter )
        {
            string::size_type pos = parameter->first.find_first_of(" .");

            if ( pos < 10 || parameter->first.length() >18 )
            {   hit = True;
                break;
            }
            else if (( parameter->first.substr(0, 11) == "expression."  ||
                       parameter->first.substr(0, 18) == "pass-on-false-port" ) )
            {
                hit = False;
            }
            else
            {   hit = True;
                break;
            }
        }

        if ( hit )
        {   status = "Parameter ( " + parameter->first
                + " ) is incorrect for a filter box.";
        }
    }
    else if ( type == "map" || type == "revisionmap" )
    {   for (  parameter  = param->begin();
               parameter != param->end(); parameter++ )
        {
            string::size_type pos = parameter->first.find_first_of(" .");

            if ( pos < 10 || pos >17 )
            {   hit = True;
                break;
            }
            else if (( parameter->first.substr(0, 11) == "expression."  ||
                       parameter->first.substr(0, 18) == "output-field-name." ) )
            {
                hit = False;
            }
            else
            {   hit = True;
                break;
            }
        }

        if ( hit )
        {   status = "Parameter ( " + parameter->first
                + " ) is incorrect for a map box.";
        }
    }
    else if ( type == "bsort" )
    {   for (  parameter  = param->begin();
               parameter != param->end(); parameter++ )
        {
            if ( parameter->first == "order-on" )
            {   hit = True;
            }
        }

        if ( !hit )
        {   status = " order-on parameter is required field for a bsort box( "
                + to_string( name ) + " ).";
        }
    }
    else if ( type == "waitfor" )
    {   for (  parameter  = param->begin();
               parameter != param->end(); ++parameter )
        {
            if ( parameter->first == "predicate" )
            {   predicate = True;
            }
        }

        if ( !predicate )
        {   status = " PREDICATE parameter is required for a waitfor box( " + to_string(name) + " ).";
        }
    }
    else if ( type == "lock" || type == "unlock" )
    {   for (  parameter  = param->begin();
               parameter != param->end(); ++parameter )
        {
            if ( parameter->first == "lockset" )
            {   hit = True;
            }
        }

        if( !hit )
        {   status = " lockset parameter is required field for a lock/unlock box( "
                + to_string(name) + " ).";
        }
    }
    else if ( type == "insert" || type == "update" || type == "select")
    {
        for ( parameter  = param->begin();
              parameter != param->end(); ++parameter )
        {
            if ( parameter->first == "sql" )
            {   hit = True;
            }
        }

        if ( !hit )
        {   status = " A SQL parameter is required for box ( " + to_string(name)
                + " ) of type " + type + ".";
        }
    }
    else if ( type == "select" )
    {   size = _add_box->get_box_out()->size();

        for (  parameter  = param->begin();
               parameter != param->end(); ++parameter )
        {
            if( ( parameter->first == "pass-on-no-results" &&
                  ( parameter->second == "true" || parameter->second == "1") ) &&
                ( size !=2 ) )
            {
                hit = True;
            }
        }

        if ( hit )
        {  status = "Box ( " + to_string(name) + " ) of type ( " + type
                + " ) with parameter ( pass-on-no-results ) and value ( true ) must have Exactly TWO output streams connected to the Box.";
        }
    }
    else if ( type == "update" )
    {   for (  parameter  = param->begin();
               parameter != param->end(); ++parameter )
        {
            // update can have either "sql", "output-expression.i" or "output-field-name.i" as parameter names.
            //
            if(  parameter->first.substr(0, 18) == "output-expression."  ||
                 parameter->first.substr(0, 18) == "output-field-name."  ||
                 parameter->first == "sql" )
            {
                hit = False;
            }
            else
            {   hit = True;
                break;
            }
        }

        if ( hit )
        {   status = "Parameter ( " + parameter->first + " ) is incorrect for a box ( "
                + to_string(name) + " ) of type ( " + box->get_box_type() + ").";
        }
    }

    if ( status )
    {  status = prior_element();
    }

    DEBUG << "status = " << status;
    return( status );
}



////////////////////////////////////////////////////////////////////////////////
//
// See if a connection point view can be added to a catalog structure.
//
Status  Catalog::validate_view( // Name of the connection point view.
                               Name    name,

                               // Name of the stream to connect to.
                               Name    stream,

                               // Name of a file to order by.
                               // An empty string if ordering by tuple.
                               string   field,

                               // Field order value or start value of a range.
                               uint32  start,

                               // End value of a range.
                               // Ignored if ordering by field.
                               uint32  end
                               )
{
    Status   status = true;
    ViewMap::iterator  in_view;

//
// validate name and other elements???
//..............................................................................


    in_view = _view_map.find( name );

    if ( in_view != _view_map.end() )
    {
      if (!_view_map[name].is_done())
    status = "incomplete";
      else 
    status = "Connection point view ( " + to_string( name )
      + " ) is already cataloged and complete.";
    }

    if ( status )
    {  status = prior_element();
    }

    DEBUG << "status = " << status;
    return( status );
}


/// See if a connection point view can be added to a catalog structure.
///
Status Catalog::validate_view( /// Name of the connection point view.
                  Name name )
{
  Status status = true;
  ViewMap::iterator  in_view;
//..............................................................................

  in_view = _view_map.find(name);
  if ( in_view != _view_map.end() )
    {
      if (!_view_map[name].is_done())
    status = "incomplete";
      else 
    status = "Connection point view ( " + to_string( name )
      + " ) is already cataloged and complete.";
    }
  
  if ( status ) 
    status = prior_element();

  DEBUG << "status = " << status;
  return ( status );
 
}


////////////////////////////////////////////////////////////////////////////////
//
// See if a table can be added to the catalog.
//
Status  Catalog::validate_table( // Name of the table.
                                Name    name,

                                // Schema for the table.
                                Name    schema
                                )
{
    Status   status = true;

    CatalogSchema::SchemaMap::iterator  in_schema;
    TableMap::iterator  in_table;
//..............................................................................


    in_schema = _topology._schema_map.find( schema );

    if ( in_schema == _topology._schema_map.end() )
    {   status = "Can not add the table ( "
            + to_string( name )
            + " ) before declaring the schema ( "
            + to_string( schema ) + " ).";

    }
    else
    {   in_table = _topology._table_map.find( name );

        if ( in_table != _topology._table_map.end() )
        {   status = "Table ( " + to_string( name ) + " ) is already cataloged.";
        }
        else if ( !_add_query )
        {   status = "Table " + to_string( name )
                + " is defined outside any query.";
        }
    }

    if ( status )
    {   status = prior_element();
    }

    DEBUG << "status = " << status;
    return( status );
}



////////////////////////////////////////////////////////////////////////////////
//
// See if a a query can be added to the catalog.
//
Status  Catalog::validate_query( // Name of the query.
                                Name    name
                                )
{
    Status   status = true;
    QueryMap::iterator  in_query;
//..............................................................................


    in_query = _topology._query_map.find( name );

    if ( in_query != _topology._query_map.end() )
    {   status = "Query ( " + to_string( name ) + " ) is already cataloged.";
    }
    else if ( _add_query )
    {   status = "Query ( " + to_string( name )
            + " ) can not be cataloged because another is being defined.";
    }

    if ( status )
    {  status = prior_element();
    }

    DEBUG << "status = " << status;
    return( status );
}

BOREALIS_NAMESPACE_END
