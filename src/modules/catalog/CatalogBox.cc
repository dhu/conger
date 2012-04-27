#include  "CatalogQuery.h"
#include  "CatalogBox.h"
#include  "Expression.h"
#include  "SQLSelect.h"
#include  "Aggregate.h"
#include  "util.h"

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
//  Infer the type of Box out streams from in streams.
//  If an out stream already has a schema it is validated.
//  If the box is an unknown external type, the return status will be false
//  with no message.
//  Upon entry, all in streams must have schemas and the box has been validated.
//
Status  CatalogBox::infer_box_out(// Map to save the composite schema.
                                  CatalogSchema::SchemaMap  *schema_map)
{
    Status    status = true;
    string    type;
    uint16    index;

    CatalogStream  *in  = NULL;
    CatalogStream  *out = NULL;
//..............................................................................


    type = get_box_type();

    if (!get_box_in()->empty())
    {   in = get_in_stream(0);            // First in stream (must be present).
    }

    if (!get_box_out()->empty())
    {   out = get_out_stream(0);          // First out stream (if present).
    }

    DEBUG << "box name=" << get_box_name() << "  type=" << type;

    if (!in)
    {   status = "No In streams are connected to box ("
               + to_string(get_box_name())  + ").";
    }
    else if ((type == "filter")  || (type == "filter_array")    // in = out+
                                 || (type == "revisionfilter"))
    {
        status = infer_filter_out(in, out, schema_map);
    }
    else if ((type == "bsort" )             ||            // in = out
             (type == "lock"  )             ||
             (type == "unlock")             ||
             (type == "random_drop")        ||
             (type == "window_drop"))
    {
        status = match_box_schema(in, out);
    }
    else if (type == "map")
    {   status = infer_map_out(in, out, schema_map);
    }
    else if (type == "revisionmap")
    {   status = infer_revisionmap_out(in, out, schema_map);
    }    
    else if (type == "union")                              // in+ = out
    {   status = match_box_schema(in, out);
        index  = get_box_in()->size();
        DEBUG << "index=" << to_string(index);

        while (status  &&  index)
        {   index--;
            status = match_box_schema(get_in_stream(index), out);
        }
    }
    else if (type == "aggregate")                   // in = {f(in), ...}
    {   status = infer_aggregate_out(in, out, schema_map);
    }
    else if (type == "revisionaggregate")           // in = {f(in), ...} & revised_field in out
    {   status = infer_revisionaggregate_out(in, out, schema_map);
    }
    else if ((type == "join" )  ||                  // in2 = {f(in2), ...}
             (type == "sjoin")  ||  (type == "aurorajoin")  ||
             (type == "joinstatic"))
    {
        status = infer_join_out(out, schema_map);
    }
    else if (type == "waitfor")                     // in2, out   in0 = out
    {   status = match_box_schema(in, out);
    }
    else if (type == "soutput")                     //  in = out2
    {   status = infer_soutput_out(in, out, schema_map);
    }
    else if (type == "sunion")                      // in+ = out, control
    {   
        // The control output and first data output of SUnion follow the same
        // rules those of SOutput.
        //
        status = infer_soutput_out(in, out, schema_map);

        if (status)
        { 
            // SUnion can have many inputs with different schemas and 
            // either one or multiple outputs with possibly different
            // schemas so we need a fancy check.
            //
            status = infer_sunion_out();
        }      
    }
    else    // external boxes:  udb, bdb
    {   status = infer_udb_out(schema_map, in, out);

        if (status.is_false())
        {  status = infer_bdb_out(schema_map, in, out);
        }

        if (status.is_false())
        {   status = infer_array_out(schema_map, in, out);
        }

        DEBUG << "exit status=" << status;
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Infer the out stream schemas for a filter box.
//
Status  CatalogBox::infer_filter_out(// In stream.
                                     CatalogStream  *in,

                                     //  In - First Out stream; schema is optional.
                                     // Out - Unchanged or schema is added.
                                     CatalogStream  *out,

                                     // Map to save the composite schema.
                                     CatalogSchema::SchemaMap  *schema_map)
{
    Status    status = true;
    uint16    index;
//
// 1 In stream.
// expression.<#> is an expression over In.
// There is 1 Out per expression with the same schema as In.
// If pass-on-false-port is defined and is true then
//    there is an additional Out with the same schema as In.
//..............................................................................


    index = get_box_out()->size();

    while (status  &&  index)
    {   index--;
        status = match_box_schema(in, get_out_stream(index));
    }

    return(status);
}



////////////////////////////////////////////////////////////////////////////////
//
//  Infer the out stream schema for a join box.
//
Status  CatalogBox::infer_join_out(//  In - First Out stream; schema is optional.
                                   // Out - Unchanged or schema is added.
                                   CatalogStream  *out,

                                   // Map to save the composite schema.
                                   CatalogSchema::SchemaMap   *schema_map)
{
    Status     status = true;
    uint16     width  = 0;
    uint16     size;
    DataType   type;
    string     field;
    string     value;

    CatalogSchema      composite;
    CatalogSchema     *out_schema;
    
    BoxParameter      *map;
    ExprContext        context;
    ptr<Expression>    expression;
//
// 2 In streams; 1 Out stream.
// out-field.name.<#> is the name of a field in In0 or In1.
// out-field.<#> is an output value expression.
//
// ( out-field-name-<0>  type( out-field-<1>), ... )
// else (In0, In1)
//..............................................................................


    map   = get_box_parameter();
    field = map->parameter("out-field-name.0");
    DEBUG << "out-field-name.0:  (" << field << ")";

    if (!field.empty())
    {   while (!field.empty())
        {   value = map->parameter("out-field." + to_string(width));
            DEBUG << "join field " << field << " = " << value;

            if (value.empty())
            {   status = "Missing out-field parameter.";
                break;
            }

            context.set_named_schema_info("left",  get_in_schema(0), 0);
            context.set_named_schema_info("right", get_in_schema(1), 1);
        
            try
            {   expression = Expression::parse(value, context);
            }
            catch(AuroraException  &e)  // ExprException???
            {   status = "Could not parse a join expression:  " + e.as_string();
                break;
            }

            DEBUG << "Returned from parse().";

            size = 0;
            type = expression->getType();

            if (type == DataType::STRING)
            {   size = expression->getStringLength();
            }

            DEBUG << "field=" << field << " type=" << type << " size=" <<size;
            vector<SchemaField>  &in_field = composite.get_schema_field();
            in_field.push_back(SchemaField(field,type,size));

            width++;
            field = map->parameter("out-field-name." + to_string(width));
        }
    }
    else
    {   // Concatonate all of the fields in the two input stream schemas.
        //
      DEBUG << "just concat'ing";
        out_schema = get_in_schema(0);
    
        copy(out_schema->get_schema_field().begin(),
             out_schema->get_schema_field().end(),
             back_inserter(composite.get_schema_field()));

        out_schema = get_in_schema(1);

        copy(out_schema->get_schema_field().begin(),
             out_schema->get_schema_field().end(),
             back_inserter(composite.get_schema_field()));
    }

    if (status)
    { 
      DEBUG << "Checking match";
      status = match_composite_schema(out, &composite, schema_map);
    }

    return(status);
}



////////////////////////////////////////////////////////////////////////////////
//
//  Infer the out stream schema for a map box.
//
Status  CatalogBox::infer_map_out(// In stream.
                                  CatalogStream  *in,

                                  //  In - First Out stream; schema is optional.
                                  // Out - Unchanged or schema is added.
                                  CatalogStream  *out,
                       
                                  // Map to save the composite schema.
                                  CatalogSchema::SchemaMap  *schema_map)
{
    Status     status = true;
    uint16     width  = 0;
    uint16     size;
    DataType   type;
    string     field;
    string     value;

    CatalogSchema      composite;
    
    BoxParameter      *map;
    ExprContext        context;
    ptr<Expression>    expression;
//
// 1 In stream; 1 Out stream.
// expression.<#> is an expression over In.
// output-field-name.<#> is the out field name.
//..............................................................................


    map   = get_box_parameter();
    value = map->parameter("expression.0");
    DEBUG << "expression.0:  (" << value << ")";

    if (value.empty())
    {   status = "Map box has no out expression prarameters.";
    }
    else
    {   while (!value.empty())
        {   field = map->parameter("output-field-name." + to_string(width));
            DEBUG << "map field " << field << " = " << value;

            if (field.empty())
            {   status = "Missing output-field-name parameter.";
                break;
            }

            context.set_context_schema(*get_in_schema(0));

            try
            {   expression = Expression::parse(value, context);
            }
            catch(AuroraException  &e)             // ExprException
            {   status = "Could not parse a map expression:  " + e.as_string();
                break;
            }

            size = 0;
            type = expression->getType();

            if (type == DataType::STRING)
            {   size = expression->getStringLength();
            }

            DEBUG << "field=" << field << " type=" << type << " size=" <<size;
            vector<SchemaField>  &in_field = composite.get_schema_field();
            in_field.push_back(SchemaField(field,type,size));

            width++;
            value = map->parameter("expression." + to_string(width));
        }

        if (status)
        {   status = match_composite_schema( out, &composite, schema_map );
        }
    }
 
    return(status);
}


////////////////////////////////////////////////////////////////////////////////
//
//  Infer the out stream schema for a revisionmap box.
//
Status  CatalogBox::infer_revisionmap_out(// In stream.
                                  CatalogStream  *in,

                                  //  In - First Out stream; schema is optional.
                                  // Out - Unchanged or schema is added.
                                  CatalogStream  *out,
                       
                                  // Map to save the composite schema.
                                  CatalogSchema::SchemaMap  *schema_map)
{
    Status     status = true;
    uint16     width  = 0;
    uint16     size;
    DataType   type;
    string     field;
    string     value;

    CatalogSchema      composite;
    
    BoxParameter      *map;
    ExprContext        context;
    ptr<Expression>    expression;

    uint16 max_field_size = 0;
//
// 1 In stream; 1 Out stream.
// expression.<#> is an expression over In.
// output-field-name.<#> is the out field name.
//..............................................................................


    map   = get_box_parameter();
    value = map->parameter("expression.0");
    DEBUG << "expression.0:  (" << value << ")";

    if (value.empty())
    {   status = "Map box has no out expression prarameters.";
    }
    else
    {   while (!value.empty())
        {   field = map->parameter("output-field-name." + to_string(width));
            DEBUG << "map field " << field << " = " << value;

            if (field.empty())
            {   status = "Missing output-field-name parameter.";
                break;
            }

            context.set_context_schema(*get_in_schema(0));

            try
            {   expression = Expression::parse(value, context);
            }
            catch(AuroraException  &e)             // ExprException
            {   status = "Could not parse a map expression:  " + e.as_string();
                break;
            }

            size = 0;
            type = expression->getType();

            if (type == DataType::STRING)
            {   size = expression->getStringLength();
            }

            DEBUG << "field=" << field << " type=" << type << " size=" <<size;
            vector<SchemaField>  &in_field = composite.get_schema_field();
            in_field.push_back(SchemaField(field,type,size));
        max_field_size = size > max_field_size ? size : max_field_size;

            width++;
            value = map->parameter("expression." + to_string(width));
        }


    // Add in a revised_value field of the maximum field size
    //  aggregate_field = SchemaField("revised_value", DataType::STRING, max_field_size, 0);
    composite.get_schema_field().push_back(SchemaField("revised_value", DataType::STRING, max_field_size, 0));


        if (status)
        {   status = match_composite_schema( out, &composite, schema_map );
        }
    }
 
    return(status);
}


////////////////////////////////////////////////////////////////////////////////
//
//  Infer the out stream schema for an aggregate box.
//
Status  CatalogBox::infer_aggregate_out(// In stream.
                                        CatalogStream  *in,

                                        //  In - First Out stream; schema is optional.
                                        // Out - Unchanged or schema is added.
                                        CatalogStream  *out,

                                        // Map to save the composite schema.
                                        CatalogSchema::SchemaMap  *schema_map)
{
    Status     status = true;
    Boolean    order_on_field = False;
    int32      index;
    string     value;
    string     name;
    DataType   type;

    BoxParameter      *map;
    CatalogSchema     *in_schema;
    CatalogSchema      composite;
    SchemaField        aggregate_field;
    ExprContext        context;
    ptr<Expression>    expression;
    ptr<Aggregate>     aggregate;
    vector<string>     group_by;

    vector<string>::iterator  field;
//
// 1 In stream; 1 Out stream.
// aggregate-function.<#> is a function over In.
// aggregate-function-output-name.<#> is an optional Out field name for
//    aggregate-function.<#>; else agg_<#> if not given.
//    The type is the type of aggregate-function.<#>
//    Functions may have multiple outputs;
//       which appends _<#> to the field name.
//     
// group-by is an optional comma-separated list of expressions over In.
// order-on-field is optional and has a single field.
//    order-on-field is required when window-size-by=VALUES or order-by=FIELD;
//                   otherwise it is not permitted (or ignored).
//
// order-by = "FIELD"
//    Out = ( <group-by fields>*, order-on-field, <function field <#>>+ )
//
// order-by = "TUPLENUM"
//    Out = ( <group-by fields>*, [order-on-field] <function field <#>>+ )
//..............................................................................


    // Compose group-by fields.
    //
    if (status)
    {   in_schema = in->get_schema();

        map   = get_box_parameter();
        value = map->parameter("group-by");

        if (!value.empty())
        {   vector<string> parts;
            split(value, ',', group_by);

            for (field = group_by.begin(); field != group_by.end(); field++)
            {   DEBUG << "Parsing field " << *field;
                index = in_schema->index_of_field(*field);

                if (index < 0)
                {   status = "Invalid order-on-field name (" + *field + ")";
                }
                else
                {   composite.get_schema_field().push_back(
                                      *(in_schema->get_schema_field(index)));
                }
            }
        }
    }

    // Compose the optional order-on-field parameter field.
    //
    if (status)
    {   value = map->parameter("order-by");

        if (value.empty())
        {   status = "The order-by parameter is missing.";
        }
        else if (value == "FIELD")
        {   order_on_field = True;
        }
        else if (value != "TUPLENUM")
        {   status = "The order-by parameter must be FIELD or TUPLENUM; got ("
                   + value + ")";
        }
    }

    if (status)
    {   value = map->parameter("window-size-by");

        if (value == "VALUES")
        {   order_on_field = True;
        }
    
        value = map->parameter("order-on-field");

        if (value.empty())
        {   if (order_on_field)
            {   status = "The order-on-field parameter is missing.";
            }
        }
        else if (order_on_field)
        {
            DEBUG << "order-on-field value: " << value;
            DEBUG << "in_scheam == NULL " << (in_schema == NULL);
            DEBUG << "field count of schema " << in_schema->get_schema_name()
                    << " " << in_schema->get_schema_field().size();
            index = in_schema->index_of_field(value);

            if (index < 0)
            {   status = "Invalid order-on-field name (" + value + ")";
            }
            else
            {   composite.get_schema_field().push_back(
                                  *(in_schema->get_schema_field(index)));
            }
        }
        else
        {   status = "The order-on-field parameter is not allowed.";
        }
    }

    // Compose aggregate function fields.
    //
    if (status)
    {   value = map->parameter("aggregate-function.0");

        if (value.empty())
        {   status = "There must be at least one aggregate function parameter.";
        }
        else
        {   index = 0;
            context.set_context_schema(*in_schema);

            while (!value.empty())
            {   name = map->parameter("aggregate-function-output-name."
                                          + to_string(index));

                if (name.empty())
                {   name = "agg_" + to_string(index);
                }

                try
                {   aggregate = Aggregate::parse(value, context);

                    // multiple out fields: FidLateTrackerFunc
                    //                      LRAccidentAggFunc1
                    //                      LRCarCount
                    //                      LRSegmentStatisticsFunc2
                    //
                    for (uint16  j = 0; j < aggregate->numOutputFields(); j++)
                    {   aggregate_field = aggregate->get_output_field(j);

                        if (aggregate->numOutputFields() == 1)
                        {   aggregate_field.set_name(name);
                        }
                        else
                        {   aggregate_field.set_name(name + "_" + to_string(j));
                        }

                        composite.get_schema_field().push_back(aggregate_field);
                    }

                    //  See if another aggregate function.
                    //
                    index++;

                    value = map->parameter("aggregate-function."
                                               + to_string(index));
                }
                catch (ExprException  &e)
                {   status = "Could not parse an aggregate function:  "
                           + to_string(e);

                    DEBUG << status;
                    break;
                }
            }
        }
    }

    if (status)
    {   status = match_composite_schema(out, &composite, schema_map);
    }
 
    return(status);
}


////////////////////////////////////////////////////////////////////////////////
//
//  Infer the out stream schema for a revisionaggregate box.
//
Status  CatalogBox::infer_revisionaggregate_out(// In stream.
                                        CatalogStream  *in,

                                        //  In - First Out stream; schema is optional.
                                        // Out - Unchanged or schema is added.
                                        CatalogStream  *out,

                                        // Map to save the composite schema.
                                        CatalogSchema::SchemaMap  *schema_map)
{
    Status     status = true;
    Boolean    order_on_field = False;
    int32      index;
    string     value;
    string     name;
    DataType   type;

    BoxParameter      *map;
    CatalogSchema     *in_schema;
    CatalogSchema      composite;
    SchemaField        aggregate_field;
    ExprContext        context;
    ptr<Expression>    expression;
    ptr<Aggregate>     aggregate;
    vector<string>     group_by;

    vector<string>::iterator  field;

    uint16   max_field_size = 0;
//
// 1 In stream; 1 Out stream.
// aggregate-function.<#> is a function over In.
// aggregate-function-output-name.<#> is an optional Out field name for
//    aggregate-function.<#>; else agg_<#> if not given.
//    The type is the type of aggregate-function.<#>
//    Functions may have multiple outputs;
//       which appends _<#> to the field name.
//     
// group-by is an optional comma-separated list of expressions over In.
// order-on-field is optional and has a single field.
//    order-on-field is required when window-size-by=VALUES or order-by=FIELD;
//                   otherwise it is not permitted (or ignored).
//
// order-by = "FIELD"
//    Out = ( <group-by fields>*, order-on-field, <function field <#>>+ )
//
// order-by = "TUPLENUM"
//    Out = ( <group-by fields>*, [order-on-field] <function field <#>>+ )
//..............................................................................


    // Compose group-by fields.
    //
    if (status)
    {   in_schema = in->get_schema();

        map   = get_box_parameter();
        value = map->parameter("group-by");

        if (!value.empty())
        {   vector<string> parts;
            split(value, ',', group_by);

            for (field = group_by.begin(); field != group_by.end(); field++)
            {   DEBUG << "Parsing field " << *field;
                index = in_schema->index_of_field(*field);

                if (index < 0)
                {   status = "Invalid order-on-field name (" + *field + ")";
                }
                else
                {   composite.get_schema_field().push_back(
                                      *(in_schema->get_schema_field(index)));
                    max_field_size = in_schema->get_schema_field(index)->get_size() > max_field_size ?
                                     in_schema->get_schema_field(index)->get_size() : max_field_size;
                }
            }
        }
    }

    // Compose the optional order-on-field parameter field.
    //
    if (status)
    {   value = map->parameter("order-by");

        if (value.empty())
        {   status = "The order-by parameter is missing.";
        }
        else if (value == "FIELD")
        {   order_on_field = True;
        }
        else if (value != "TUPLENUM")
        {   status = "The order-by parameter must be FIELD or TUPLENUM; got ("
                   + value + ")";
        }
    }

    if (status)
    {   value = map->parameter("window-size-by");

        if (value == "VALUES")
        {   order_on_field = True;
        }
    
        value = map->parameter("order-on-field");

        if (value.empty())
        {   if (order_on_field)
            {   status = "The order-on-field parameter is missing.";
            }
        }
        else if (order_on_field)
        {   index = in_schema->index_of_field(value);

            if (index < 0)
            {   status = "Invalid order-on-field name (" + value + ")";
            }
            else
            {   composite.get_schema_field().push_back(
                                  *(in_schema->get_schema_field(index)));
                max_field_size = in_schema->get_schema_field(index)->get_size() > max_field_size ?
                                 in_schema->get_schema_field(index)->get_size() : max_field_size;
            }
        }
        else
        {   status = "The order-on-field parameter is not allowed.";
        }
    }


    // Compose aggregate function fields.
    //
    if (status)
    {   value = map->parameter("aggregate-function.0");

        if (value.empty())
        {   status = "There must be at least one aggregate function parameter.";
        }
        else
        {   index = 0;
            context.set_context_schema(*in_schema);

            while (!value.empty())
            {   name = map->parameter("aggregate-function-output-name."
                                          + to_string(index));

                if (name.empty())
                {   name = "agg_" + to_string(index);
                }

                aggregate = Aggregate::parse(value, context);

                // multiple out fields: FidLateTrackerFunc
                //                      LRAccidentAggFunc1
                //                      LRCarCount
                //                      LRSegmentStatisticsFunc2
                //
                for (uint16  j = 0; j < aggregate->numOutputFields(); j++)
                {   aggregate_field = aggregate->get_output_field(j);

                    if (aggregate->numOutputFields() == 1)
                    {   aggregate_field.set_name(name);
                    }
                    else
                    {   aggregate_field.set_name(name + "_" + to_string(j));
                    }

                   composite.get_schema_field().push_back(aggregate_field);
                    max_field_size = aggregate_field.get_size() > max_field_size ? aggregate_field.get_size() : max_field_size;
                }

                //  See if another aggregate function.
                //
                index++;

                value = map->parameter("aggregate-function."
                                           + to_string(index));
            }
        }
    }

    // Add in a revised_value field of the maximum field size
    aggregate_field = SchemaField("revised_value", DataType::STRING, max_field_size, 0);
    composite.get_schema_field().push_back(aggregate_field);

    DEBUG << *out;
    DEBUG << composite;

    if (status)
    {   status = match_composite_schema(out, &composite, schema_map);
    }
 
    return(status);
}



////////////////////////////////////////////////////////////////////////////////
//
//  Infer the out stream schema for an soutput box.
//
Status  CatalogBox::infer_soutput_out(// In stream.
                                      CatalogStream  *in,

                                      // Empty control schema.
                                      CatalogStream  *out,

                                      // Map to save the composite schema.
                                      CatalogSchema::SchemaMap  *schema_map)
{
    CatalogSchema   composite;
//
// SOutput and SUnion both inherit from SControl so they both have that empty
// control output stream as their FIRST output stream
// Other than this, SOutput is also like a simple filter...
// so it has one output stream that has the same schema as its input stream.
//..............................................................................


    // First check if the data output stream matches the data input stream.
    // The second output is the data output.
    //
    Status  status = match_box_schema(in, get_out_stream(1));

    if (status)
    {   
        // Now check if the control output stream has an empty schema.
        // For some reason, this does not work. So just checking if control.
        //
        status = match_composite_schema(out, &composite, schema_map);
    }

    return(status);
}



////////////////////////////////////////////////////////////////////////////////
//
//  Infer the out streams schemas for an sunion box.
//  We assume that the control output and first data output have been 
//  checked already by infer_soutput_outputs
//
Status  CatalogBox::infer_sunion_out()
{
    // The total number of input streams.
    int32 nb_data_inputs  = get_box_in()->size();

    // There is one control output stream.
    int32 nb_control_outputs = 1;

    // In SUnion, data outputs start after the contol output streams.
    int32 first_data_output_index = nb_control_outputs;
    int32 nb_data_outputs = get_box_out()->size() - first_data_output_index;

    int32  index;
    Status status = true;
//
//..............................................................................


    // If many outputs, then each data output must correspond to one data input.
    //
    if (nb_data_outputs > 1)
    {
        if ( nb_data_outputs != nb_data_inputs )
        { 
            status = "Error in box (" + to_string(get_box_name()) + "): " +
                " SUnion must have either one data output stream or one data output per input stream";
            DEBUG << status;
        }

        index = nb_data_outputs;

        while ( status && index)
        { 
            index--;
            DEBUG << "Matching schema at input " << index << " with output schema " << first_data_output_index+index;
            status = match_box_schema(get_in_stream(index),get_out_stream(first_data_output_index + index));
        }
    }

    // Otherwise, all inputs must match the one output.
    //
    else 
    {
        index = nb_data_inputs;  

        while (status && index)
        {
            index--;
            DEBUG << "Matching schema at input " << index << " with output schema " << first_data_output_index;
            status = match_box_schema(get_in_stream(index),get_out_stream(first_data_output_index));
        }
    }

    return status;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////  Box Utility Methods  /////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//  Match a stream to the reference stream (unchanged).
//  If the stream argument has no schema it is assigned the schema.
//
Status  CatalogBox::match_composite_schema(// Output stream to validate.
                                           CatalogStream  *out,

                                           // Expected out schema.
                                           CatalogSchema  *composite,

                                           // Map to save the composite scheam.
                                 CatalogSchema::SchemaMap  *schema_map)
{
    Status          status = true;
    Name            name;
    CatalogSchema  *out_schema;
//
//..............................................................................


    if (!out)
    {   status = "Missing Out stream on box ("
               + to_string(get_box_name()) + ").";
    }
    else
    {   out_schema = out->get_stream_schema();

        if (!out_schema)
        {   // Name it after the box that created it.  Multple outs???
            //
            name = "/" + to_string(get_box_name());
            composite->set_schema_name(name);

            // Declare in_schema in the catalog.
            //
            out_schema = &(*schema_map)[name];        // Insert a new schema.
            out_schema->clone(*composite);
            out->set_stream_schema(out_schema);

            DEBUG << "********** Added schema " << name
                  << " to stream " << out->get_stream_name_string();
        }
        else if (!composite->equal_schema(out_schema))
        {   status = "On box (" + to_string(get_box_name())
                   + ") the Out stream (" + out->get_stream_name_string()
                   + ") schema (" + out_schema->get_schema_name()
                   + ") does not match the expected schema.";
        }
    }

    return( status );
}



////////////////////////////////////////////////////////////////////////////////
//
//  Match a stream to the reference stream (unchanged).
//  If the stream argument has no schema it is assigned the schema.
//
Status  CatalogBox::match_box_schema(// Stream to match up.
                                     CatalogStream  *in,

                                     // Output stream to validate.
                                     CatalogStream  *out)
{
    Status    status = true;
//
//..............................................................................


    if (out)
    {   DEBUG <<    "in=" <<  in->get_stream_name()
              << "  out=" << out->get_stream_name();
    }

    if (!out)
    {   status = "No Out streams are connected to box ("
               + to_string(get_box_name()) + ").";
    }
    else if (!in->match_stream_schema(out))
    {   status = "In box (" + to_string( get_box_name())
               + ") the schema of the Out stream (" + out->get_stream_name_string()
               + ") does not match schema of the In stream ("
               + in->get_stream_name_string() + ").";
    }

    return( status );
}


Status  CatalogBox::match_box_schema_two_in(// Stream to match up.
                                     CatalogStream  *in1,
                                     CatalogStream  *in2,

                                     // Output stream to validate.
                                     CatalogStream  *out)
{
    Status    status = true;
//
//..............................................................................


    if (out)
    {   DEBUG <<    "in=" <<  in1->get_stream_name()
              <<    "in=" <<  in2->get_stream_name()
              << "   out=" << out->get_stream_name();
    }

    if (!out)
    {   status = "No Out streams are connected to box ("
               + to_string(get_box_name()) + ").";
    }
    else if (!in1->match_stream_schema(out))
    {   status = "In box (" + to_string( get_box_name())
               + ") the schema of the Out stream (" + out->get_stream_name_string()
               + ") does not match schema of the In stream ("
               + in1->get_stream_name_string() + ").";
    }
    else if (!in2->match_stream_schema(out))
    {   status = "In box (" + to_string( get_box_name())
               + ") the schema of the Out stream (" + out->get_stream_name_string()
               + ") does not match schema of the In stream ("
               + in2->get_stream_name_string() + ").";
    }

    return( status );
}


Status CatalogBox::get_in_stream_index(CatalogStream* in, uint16& idx)
{
    CatalogStream::StreamStar::iterator in_it = _in.begin();
    CatalogStream::StreamStar::iterator in_end = _in.end();
    
    for (unsigned int i = 0; in_it != in_end; ++in_it, ++i) {
        if ( *in_it == in )
            idx = i;
    }
    
    if ( in_it == in_end )
        return "Input stream not found";

    return true;
}

Status CatalogBox::get_out_stream_index(CatalogStream* out, uint16& idx)
{
    CatalogStream::StreamStar::iterator out_it = _out.begin();
    CatalogStream::StreamStar::iterator out_end = _out.end();
    
    for (unsigned int i = 0; out_it != out_end; ++out_it, ++i) {
        if ( *out_it == out )
            idx = i;
    }
    
    if ( out_it == out_end )
    {   return "Output stream not found";
    }

    return true;
}



////////////////////////////////////////////////////////////////////////////////
//
// Copy the top level box settings.
//
void CatalogBox::shallow_copy(const CatalogBox &box)
{
//..............................................................................


    _name = box.get_box_name();
    _type = box.get_box_type();
    _node = box.get_box_node();
    BoxParameter bp;
    bp.set_parameter_map(box.get_box_parameter_map_const());
    _parameter = bp;
    _table = NULL;
    _query = NULL;
    _in_queue.clear();
    _in.clear();
    _out.clear();
}



////////////////////////////////////////////////////////////////////////////////
//
// Add an input stream to this box.
//
void CatalogBox::add_in_stream(CatalogStream  *stream,
                               InQueue         in_queue)
{
//..............................................................................


    _in.push_back(stream);
    _in_queue.push_back(in_queue);
}



////////////////////////////////////////////////////////////////////////////////
//
// Change an box input stream to a different stream.
//
void CatalogBox::reset_in_stream(uint16 port, CatalogStream *stream)
{
//..............................................................................


    _in[port] = stream;
}



////////////////////////////////////////////////////////////////////////////////
//
// Remove an input stream from this box.
//
void  CatalogBox::remove_in_stream(Name  stream)
{
//..............................................................................


    CatalogStream::StreamStar::iterator i;

    for (i = _in.begin(); i != _in.end(); i++)
    {
        if ((*i)->get_stream_name() == stream)
        {   break;
        }
    }

    if (i != _in.end())
    {   _in.erase(i);
        //_queue.erase(i); ???
    }
    else 
    {   DEBUG << "No such input stream!";
    }
}



////////////////////////////////////////////////////////////////////////////////
//
// Add an output stream to this box.
//
void CatalogBox::add_out_stream(CatalogStream *stream)
{
//..............................................................................


    _out.push_back(stream);
}



////////////////////////////////////////////////////////////////////////////////
//
// Remove an output stream from this box.
//
void CatalogBox::remove_out_stream(Name stream)
{
//..............................................................................


    CatalogStream::StreamStar::iterator i;

    for (i = _out.begin(); i != _out.end(); i++)
    {
        if ((*i)->get_stream_name() == stream)
        {
            break;
        }
    }

    if (i != _out.end())
    {
        _out.erase(i);
    }
    else 
    {
        DEBUG << "No such output stream!";
    }
}



////////////////////////////////////////////////////////////////////////////////
//
// Get query.
//
CatalogQuery *CatalogBox::get_box_query() const
{
//..............................................................................


    return(_query);
}



////////////////////////////////////////////////////////////////////////////////
//
// Remove the sink box port from my specified input stream,
// whose destination box is me. Also compare port numbers in case
// I am a binary box that takes its both inputs from the same stream,
// e.g., self-join.
//
void CatalogBox::remove_in_sink_port(Name in_stream, uint16 port_num)
{
//..............................................................................


    for (unsigned int i = 0; i < _in.size(); i++)
    {
        DEBUG << "remove_in_sink_port i: " << i << " stream: " << _in[i]->get_stream_name();

        if ((_in[i]->get_stream_name() == in_stream) && (i == port_num))
        {
            vector<BoxPort> *sinks = _in[i]->get_sink_port();
            vector<BoxPort>::iterator iter;

            for (iter = sinks->begin(); iter != sinks->end(); iter++)
            {
                if (iter->get_port_box())
                {
                    if ((iter->get_port_box()->get_box_name() == _name) &&
                        (iter->get_port() == i))
                    {
                        break;
                    }
                }
            }

            if (iter != sinks->end())
            {   sinks->erase(iter);
            }
            else
            {   DEBUG << "Boxport to be removed not found!";
            }

            return;
        }
    }

    DEBUG << "CatalogBox " << _name.as_string()
          << " does not have input stream named \""
          << to_string(in_stream) << "\"";
}



////////////////////////////////////////////////////////////////////////////////
//
// Remove all sink box ports from my specified input stream,
// whose destination box is me (regardless of port numbers).
//
void CatalogBox::remove_in_sink_port(Name in_stream)
{
//..............................................................................


    for (unsigned int i = 0; i < _in.size(); i++)
    {
        if (_in[i]->get_stream_name() == in_stream)
        {
            vector<BoxPort> *sinks = _in[i]->get_sink_port();
            vector<BoxPort>::iterator iter;

            for (iter = sinks->begin(); iter != sinks->end(); iter++)
            {
                if (iter->get_port_box())
                {
                    if (iter->get_port_box()->get_box_name() == _name)
                    {   break;
                    }
                }
            }
            if (iter != sinks->end())
            {
                sinks->erase(iter);

                // If I am a self-join or self-union,
                // there may be one more matching port; loop again.
                //
                for (iter = sinks->begin(); iter != sinks->end(); iter++)
                {
                    if (iter->get_port_box())
                    {
                        if (iter->get_port_box()->get_box_name() == _name)
                        {   break;
                        }
                    }
                }
                if (iter != sinks->end())
                {
                    sinks->erase(iter);
                }
            }
            else
            {   DEBUG << "Boxport to be removed not found!";
            }

            return;
        }
    }

     DEBUG << "CatalogBox " << _name.as_string()
           << " does not have input stream named \""
           << to_string(in_stream) << "\"";
}



////////////////////////////////////////////////////////////////////////////////
//
//  Create an XML string for a box.
//
string  CatalogBox::form_box_xml(HAType ha_type)
{
    string     element;
    string     stream_name;
    uint16     port;

    BoxParameter::ParameterMap::iterator  parameter;
     CatalogStream::StreamStar::iterator  stream;
//
//..............................................................................


    element += "<box name=\"" + Util::full_name(get_box_name())
               + "\" type=\"" + get_box_type() + "\"";

    if ( !get_box_node().empty() )
    {   element += " node=\"" + get_box_node() + "\"";
    }

    if (ha_type == HAType::BACKUP)
    {   element += " ha_type=\"" + HAType::BACKUP.as_string() + "\"";
    }

    element += " >\n";
    port = 0;

    // Box input ports.
    //
    for (stream  = get_box_in()->begin();
         stream != get_box_in()->end(); stream++)
    {
        stream_name = Util::full_name((*stream)->get_stream_name());
        element += " <in  stream=\"" + stream_name;

        if (get_cpview(stream_name) != "") // if cpview exists, add to xml
        {   element += "\" connection_point_view=\"" + get_cpview(stream_name);
        }

        if (is_update_queue(port))
        {   
            element += "\"  update_queue=\"1";
            element += "\"  queue_type=\"" + to_string(get_in_queue_type(port));

            if (get_in_queue_key(port))
            {   
                element += "\"  key=\"" + to_string(get_in_queue_key(port));
            }
        }

        element += "\" />\n";
     
        port++;
    }

    // Box output ports.
    //
    for (stream  = get_box_out()->begin();
         stream != get_box_out()->end(); stream++)
    {
        element += " <out  stream=\"" + Util::full_name((*stream)->get_stream_name())
                      + "\"/>\n";
    }

    if (get_box_table())
    {   element += " <access table=\"" + Util::full_name(get_box_table()->get_table_name())
                      + "\"/>\n";
    }

    // Box parameters.
    //
    BoxParameter::ParameterMap* params = get_box_parameter_map();

    for (parameter  = params->begin();
         parameter != params->end(); parameter++)
    {
        // Escape the parameter value for the engine's parser.
        //
        element += " <parameter  name=\"" + parameter->first
                          + "\" value=\"" + xml_escape(parameter->second)
                          + "\"/>\n";
    }

    element += "</box>\n";
    DEBUG << element;

    return(element);
}



////////////////////////////////////////////////////////////////////////////////
//
void CatalogBox::freeze(::NMSTL::OSerial &out) const
{
//..............................................................................


    out && out.begin_container(*this) && (out << _name << _type << _node)
    && freeze_ptr_shallow_impl(out, _query)

#ifdef ENABLE_CONTAINER_SERIALIZATION
    //&& BOREALIS_FREEZE_PTR_FIELD(BoxStar)
    && freeze_ptr_shallow_impl(out, _in)
    && freeze_ptr_shallow_impl(out, _out)
#endif
    && (out  << _parameter)
    && freeze_ptr_shallow_impl(out, _table)
    && out.end_container(*this);
}



////////////////////////////////////////////////////////////////////////////////
//
void CatalogBox::unfreeze(::NMSTL::ISerial &in)
{
//..............................................................................


    in && in.begin_container(*this)
    && (::NMSTL::__iserial_adaptor(in) << _name << _type << _node)
    && unfreeze_ptr_impl(in, _query)

#ifdef ENABLE_CONTAINER_SERIALIZATION
    //&& BOREALIS_UNFREEZE_PTR_FIELD(BoxStar)
    && unfreeze_ptr_impl(in, _in)
    && unfreeze_ptr_impl(in, _out)
#endif

    && (::NMSTL::__iserial_adaptor(in) << _parameter)
    && unfreeze_ptr_impl(in, _table)
    && in.end_container(*this);
}


/******************************** unused???
////////////////////////////////////////////////////////////////////////////////
//
// Is this box invertible or not
// Invertible is if input tuple(s) can be reconstructed from output tuple
//
bool CatalogBox::is_invertible()
{
  if (_type == "filter" || _type == "revisionfilter" || _type == "filter_array" || _type == "map_frame" || _type == "map_frame_composite")
    {
      if (_parameter.parameter("pass-on-false-port", "none") == "1")
    return true;
      else
    return false;
    }
  else if (_type == "map" || _type == "revisionmap")
    {
      unsigned int number_of_expressions = 0, number_of_inverses = 0;
      // count the number of expressions
      for (unsigned int i = 0; ; ++i)
    {
      if (_parameter.parameter(("expression." + to_string(i)), "empty") != "empty")
        ++number_of_expressions;
      else
        break;
    }
      // count the number of inverses
      for (unsigned int i = 0; i < number_of_expressions; ++i)
    {
      if (_parameter.parameter(("inverse." + to_string(i)), "empty") != "empty")
        ++number_of_inverses;
    }
      // invertible if number of expression and inverses are the same
      if (number_of_expressions == number_of_inverses)
    return true;
      else
    return false;
    }
  
  else if (_type == "aggregate" || _type == "revisionaggregate")
    return false;
  
  else // everyone else
    return false;
}


////////////////////////////////////////////////////////////////////////////////
//
// Returns the window_size for Aggreate, 0 for others
// 
int CatalogBox::get_window_size()
{
  if (_type == "aggregate" || _type == "revisionaggregate")
    return atoi(_parameter.parameter("window-size").c_str());
  else 
    {
      ERROR << "Asking for window size of non-aggregate box";
      return 0;
    }
}



vector<CatalogBox*> CatalogBox::get_upstream_boxes()
{
    vector<CatalogBox*> boxes;
    for(CatalogStream::StreamStar::iterator i = _in.begin() ; i != _in.end(); i++)
    {
        CatalogBox* box = (*i)->get_stream_source_box();
        if (box)
            boxes.push_back(box);
    }
    return boxes;
}



vector<CatalogBox*> CatalogBox::get_downstream_boxes()
{
    vector<CatalogBox*> boxes_so_far;
    for(CatalogStream::StreamStar::iterator i = _out.begin() ; i != _out.end(); i++)
    {
        vector<CatalogBox*> boxes = (*i)->get_stream_sink_boxes();
        boxes_so_far.insert(boxes_so_far.end(), boxes.begin(), boxes.end()); 
    }
    return boxes_so_far;
}



vector<CatalogBox*> CatalogBox::get_neighboring_boxes()
{
    vector<CatalogBox*> boxes_so_far = get_upstream_boxes();
    vector<CatalogBox*> boxes = get_downstream_boxes();
    boxes_so_far.insert(boxes_so_far.end(),  boxes.begin(), boxes.end());
    return boxes_so_far;
}
*********************/


BOREALIS_NAMESPACE_END
