#include  "CatalogBdb.h"
#include  "Expression.h"
#include  "SQLSelect.h"

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
//  Infer the type of Box out streams from in streams.
//  If an out stream already has a schema it is validated.
//  Upon entry, all in streams must have schemas and the box has been validated.
//
Status  CatalogBox::infer_bdb_out(// Map to save the composite schema.
                                  CatalogSchema::SchemaMap  *schema_map,

                                  CatalogStream  *in,

                                  CatalogStream  *out)
{
    Status    status = false;
    string    type;
//
//  NYI:  Exists
//..............................................................................


    type = get_box_type();

    if (type == "update")                           // in = out | f(in) = out | no out
    {   status = CatalogBdb::infer_update_out(this, in, out, schema_map);
    }
    else if (type == "insert")                      // in = out | no out
    {   status = CatalogBdb::infer_insert_out(this, in, out);
    }
    else if (type == "select")                      // f(in) = out0, in = out1?
    {   status = CatalogBdb::infer_select_out(this, in, out, schema_map);
    }
    else if (type == "delete")
    {   status = CatalogBdb::infer_delete_out(this, in, out, schema_map);
    }
 
    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Infer the out stream schema for an sql update box.
//
Status  CatalogBdb::infer_update_out(// The update box.
                                     CatalogBox     *box,

                                     // Only In stream.
                                     CatalogStream  *in,

                                     //  In - Optional Out stream; schema is optional.
                                     // Out - Unchanged or schema is added.
                                     CatalogStream  *out,

                                     // Map to save the composite schema.
                                     CatalogSchema::SchemaMap  *schema_map)
{
    Status     status = true;
    uint16     width  = 0;
    uint16     size;
    DataType   type;
    Boolean    pass;
    string     field;
    string     value;

    BoxParameter    *map;
    CatalogTable    *table;
    CatalogSchema    composite;

    ExprContext      context;
    ptr<Expression>  expression;
//
//  One In stream.
//  If pass-input is defined and non-zero, Out exists and matches In.
//  Else output-expression.<#> is an expression over in.
//       output-field-name.<#> is the out field name.
//  Else no output.
//..............................................................................


    try
    {   map = box->get_box_parameter();

        if (map->typed_parameter("pass-input", pass,
                                 BoxParameter::NOT_REQUIRED))
        {
            if (pass)
            {   status = box->match_box_schema(in, out);
            }
        }
        else
        {   value = map->parameter("output-expression.0");
            DEBUG << "expression.0:  (" << value << ")";

            if (!value.empty())
            {   table = box->get_box_table();
                context.set_named_schema_info("input", box->get_in_schema(0), 0);
                context.set_named_schema_info("old", table->get_table_schema(), 1);
                context.set_named_schema_info("new", table->get_table_schema(), 2);

                while (!value.empty())
                {   field = map->parameter("output-field-name." + to_string(width));
                    DEBUG << "map field " << field << " = " << value;

                    if (field.empty())
                    {   status = "Missing output-field-name parameter.";
                        break;
                    }

                    try
                    {   //context.set_context_schema(*get_in_schema(0));
                        expression = Expression::parse(value, context);

                        size = 0;
                        type = expression->getType();

                        if (type == DataType::STRING)
                        {   size = expression->getStringLength();
                        }

                        DEBUG << "field=" << field << " type=" << type << " size=" << size;
                        vector<SchemaField>  &in_field = composite.get_schema_field();
                        in_field.push_back(SchemaField(field,type,size));
                    }
                    catch (AuroraException &e)
                    {   status = "Could not parse output value (" + value
                               +"):  " + to_string(e);

                        DEBUG << status;
                        break;
                    }

                    width++;

                    value = map->parameter("output-expression."
                                               + to_string(width));
                }

                if (status)
                {   status = box->match_composite_schema(out, &composite,
                                                         schema_map);
                }
            }
        }
    }
    catch (AuroraException &e)
    {   status = "The pass-input parameter has an invalid value:  "
               + to_string(e);

        DEBUG << status;
    }

    return( status );
}




////////////////////////////////////////////////////////////////////////////////
//
//  Infer the out stream schema for an sql insert box.
//
Status  CatalogBdb::infer_insert_out(// The insert box.
                                     CatalogBox     *box,

                                     // Only In stream.
                                     CatalogStream  *in,

                                     //  In - Optional Out stream; schema is optional.
                                     // Out - Unchanged or schema is added.
                                     CatalogStream  *out)
{
    Status   status = true;
    Boolean  pass;
//
//  One In stream.
//  Out exists if pass-input is non-zero and matches In.
//..............................................................................


    try
    {   if (box->get_box_parameter()->typed_parameter("pass-input", pass,
                                        BoxParameter::NOT_REQUIRED))
        {
            if (pass)
            {   status = box->match_box_schema(in, out);
            }
        }
    }
    catch (AuroraException  &e)
    {   status = "The pass-on-no-result parameter has an invalid value:  "
               + to_string(e);

        DEBUG << status;
    }

    return( status );
}



////////////////////////////////////////////////////////////////////////////////
//
//  Infer the out stream schema for an sql select box.
//
Status  CatalogBdb::infer_select_out(// The select box.
                                     CatalogBox     *box,

                                     // Select In stream.
                                     CatalogStream  *in,

                                     //  In - First Out stream; schema is optional.
                                     // Out - Unchanged or schema is added.
                                     CatalogStream  *out,

                                     // Map to save the composite schema.
                                     CatalogSchema::SchemaMap  *schema_map)
{
    Status   status = true;
    string   sql;
    Boolean  pass;

    BoxParameter   *map;
    CatalogTable   *table;
    ExprContext     context;
    ptr<SQLSelect>  select;
    CatalogSchema  *hack;
//
//  One input stream.
//  sql is an sql expression over (input=In, <table schema>)
//  Out 0 is the schema for the sql expression.
//  Out 1 exists if pass-on-no-results is non-zero and is equal to In.
//..............................................................................


    map = box->get_box_parameter();
    sql = map->parameter("sql");
    DEBUG << "sql=" << sql;

    if ( sql.empty())
    {   status = "Missing or empty sql parameter.";
    }
    else
    {   table = box->get_box_table();

        if (!table)
        {   status = "No table is declared for the ("
                   + to_string(box->get_box_name()) + ") select box.";
        }
    }


    // Match the first output stream to the schema for the sql expression.
    //
    if (status)
    {   context.set_named_schema_info("input", in->get_schema(), 0);
        DEBUG << "input schema:  " << to_string(*in->get_schema());

        context.set_named_schema_info(to_string(table->get_table_name()),
                                      table->get_table_schema(), 1);

        DEBUG << "table (" << to_string(table->get_table_name())
              << ") schema: " << to_string(*table->get_table_schema());

        try
        {    select = SQLSelect::parse(sql, context,
                                       to_string(table->get_table_name()));

             hack   = select->get_result_catalog_schema();
             DEBUG  << "get_result_catalog_schema=" << to_string(*hack);

             DEBUG << "table (" << to_string(table->get_table_name())
                   << ") schema: " << to_string(*table->get_table_schema());

             status = box->match_composite_schema(out,
                          select->get_result_catalog_schema(), schema_map);
        }
        catch (AuroraException &e)
        {   status = "Exception while parsing select box sql parameter: "
                   + to_string(e);

            DEBUG << status;
        }
    }


    // If pass-on-no-result is true then match a second Out stream with In.
    //
    if (status)
    {   try
        {   if (map->typed_parameter("pass-on-no-results", pass,
                                     BoxParameter::NOT_REQUIRED))
            {
                if (pass)
                {   status = box->match_box_schema(in, box->get_out_stream(1));
                }
            }
        }
        catch (AuroraException &e)
        {   status = "The pass-on-no-results parameter has an invalid value:  "
                   + to_string(e);

            DEBUG << status;
        }
    }

    return( status );
}



////////////////////////////////////////////////////////////////////////////////
//
//  Infer the out stream schema for an sql delete box.
//
Status CatalogBdb::infer_delete_out(// The delete box.
                                    CatalogBox     *box,

                                    CatalogStream *in,
                                    
                                    CatalogStream *out,
                                    
                                    CatalogSchema::SchemaMap  *schema_map)
{
    Status            status = true;
    BoxParameter     *map;
    bool              pass_deletions = false;
    bool              pass_input = false;
    CatalogTable     *table;
    CatalogSchema    *table_schema;
    CatalogStream    *pass_input_stream;
//.............................................................................

    map = box->get_box_parameter();
    
    map->typed_parameter<bool>("pass-deletions", pass_deletions);

    if ( pass_deletions )
    {
        table = box->get_box_table();

        if ( !table )
        {   status = "No table is declared for the ("
                   + to_string(box->get_box_name()) + ") delete box.";
        }
        else
        {
            table_schema = table->get_table_schema();
            status = box->match_composite_schema(out, table_schema, schema_map);
        }
    }

    if ( status )
    {   map->typed_parameter<bool>("pass-input", pass_input);

        if ( pass_input )
        {
            unsigned int pass_input_stream_id = (pass_deletions? 1 : 0);
            if ( box->get_box_out()->size() > pass_input_stream_id )
            {
                pass_input_stream = box->get_out_stream(pass_input_stream_id);
                status = box->match_box_schema(in, pass_input_stream);
            }
            else
            {   status = string() +
                    "No output stream was specified to pass on inputs for "+
                    "delete box " + to_string(box->get_box_name());
            }
        }
    }
    
    return( status );
}


BOREALIS_NAMESPACE_END
