/**
 * TryBoxProcessor.cc
 *
 *  Created on: May 13, 2012
 *      Author: hellojinjie
 */
#include "QueryProcessor.h"
#include "CongerDiagram.h"      // 继承自  Diagram
BOREALIS_NAMESPACE_BEGIN

void QueryProcessor::try_filter()
{
    _ongoing_dynamic_modification = true;

    /***************** 增加 schema ****************************/
    /*
     <schema name="inputpacket">
     <field name="time" type="int"/>
     <field name="price" type="double"/>
     <field name="trade_volume" type="int"/>
     <field name="trade_amount" type="int"/>
     <field name="bid_ask" type="string" size="1"/>
     </schema>
     */
    list<SchemaFieldType> schema_fields;
    SchemaFieldType schema_field;
    schema_field.fieldName = "stock";
    schema_field.typeName = "string";
    schema_field.size = 6;
    schema_fields.push_back(schema_field);

    schema_field.fieldName = "time";
    schema_field.typeName = "int";
    schema_field.size = 0;
    schema_fields.push_back(schema_field);

    schema_field.fieldName = "price";
    schema_field.typeName = "double";
    schema_field.size = 0;
    schema_fields.push_back(schema_field);

    schema_field.fieldName = "trade_volume";
    schema_field.typeName = "int";
    schema_field.size = 0;
    schema_fields.push_back(schema_field);

    schema_field.fieldName = "trade_amount";
    schema_field.typeName = "int";
    schema_field.size = 0;
    schema_fields.push_back(schema_field);

    schema_field.fieldName = "bid_ask";
    schema_field.typeName = "string";
    schema_field.size = 1;
    schema_fields.push_back(schema_field);

    CatalogSchema *add_schema;
    add_schema = _local_catalog.add_conger_schema("inputpacket", schema_fields);

    vector<SchemaField> fields = add_schema->get_schema_field();

    DEBUG << "schema fields size: " << fields.size() << " schema name "
    << add_schema->get_schema_name();
    vector<SchemaField>::iterator iter = fields.begin();
    for (; iter != fields.end(); iter++)
    {
        DEBUG << "field name: " << iter->get_name() << " type: " << iter->get_type();
    }

    // add schema to engine's structures
    vector<CatalogSchema*> schemas;
    schemas.push_back(add_schema);

    _aurora.add_schema(schemas);

    /****************** input ***********************************/
    string stream_name = "inputstream";
    string schema_name = "inputpacket";

    CatalogStream *new_stream;

    vector<CatalogSubscription> subscription;

    new_stream = _local_catalog.add_conger_stream(stream_name, schema_name);
    DEBUG << "Opening a general purpose input data path for stream " << new_stream->get_stream_name();

    _data_path.add_input_path(new_stream->get_stream_name());

    try
    {
        // add stream to engine's structures -- yna
        vector<CatalogStream*> stream;
        stream.push_back(new_stream);
        _aurora.add_stream(stream);
    }
    catch (AuroraException &ae)
    {
        DEBUG << ae.getMessage();
    }

    /*****************  query  *************************/
    string query_name = "try_filter";

    map<string, string> box_parameters;
    box_parameters["expression.0"] = "price>13.0";
    box_parameters["pass-on-false-port"] = "0";

    add_conger_box("try_filter", "filter", "inputstream",
            "outputstream", box_parameters);

    /*******************   subscribe    *************************/
    CatalogSubscription subscriber;
    subscriber = _local_catalog.add_conger_subsribe("outputstream", "127.0.0.1:25000", "");

    vector<CatalogSubscription> subs;
    subs.push_back(subscriber);

    AsyncRPC<void> completion;
    completion.post(true);
    local_subscribe_streams(completion, subs);

    /* 和上面的 true 配对 */
    _ongoing_dynamic_modification = false;
}

void QueryProcessor::try_map()
{
    _ongoing_dynamic_modification = true;

    /***************** 增加 schema ****************************/
    /*
     <schema name="inputpacket">
     <field name="time" type="int"/>
     <field name="price" type="double"/>
     <field name="trade_volume" type="int"/>
     <field name="trade_amount" type="int"/>
     <field name="bid_ask" type="string" size="1"/>
     </schema>
     */
    list<SchemaFieldType> schema_fields;
    SchemaFieldType schema_field;
    schema_field.fieldName = "stock";
    schema_field.typeName = "string";
    schema_field.size = 6;
    schema_fields.push_back(schema_field);

    schema_field.fieldName = "time";
    schema_field.typeName = "int";
    schema_field.size = 0;
    schema_fields.push_back(schema_field);

    schema_field.fieldName = "price";
    schema_field.typeName = "double";
    schema_field.size = 0;
    schema_fields.push_back(schema_field);

    schema_field.fieldName = "trade_volume";
    schema_field.typeName = "int";
    schema_field.size = 0;
    schema_fields.push_back(schema_field);

    schema_field.fieldName = "trade_amount";
    schema_field.typeName = "int";
    schema_field.size = 0;
    schema_fields.push_back(schema_field);

    schema_field.fieldName = "bid_ask";
    schema_field.typeName = "string";
    schema_field.size = 1;
    schema_fields.push_back(schema_field);

    CatalogSchema *add_schema;
    add_schema = _local_catalog.add_conger_schema("inputpacket", schema_fields);

    vector<SchemaField> fields = add_schema->get_schema_field();

    DEBUG << "schema fields size: " << fields.size() << " schema name "
    << add_schema->get_schema_name();
    vector<SchemaField>::iterator iter = fields.begin();
    for (; iter != fields.end(); iter++)
    {
        DEBUG << "field name: " << iter->get_name() << " type: " << iter->get_type();
    }

    // add schema to engine's structures
    vector<CatalogSchema*> schemas;
    schemas.push_back(add_schema);

    _aurora.add_schema(schemas);

    /****************** input ***********************************/
    string stream_name = "inputstream";
    string schema_name = "inputpacket";

    CatalogStream *new_stream;

    vector<CatalogSubscription> subscription;

    new_stream = _local_catalog.add_conger_stream(stream_name, schema_name);
    DEBUG << "Opening a general purpose input data path for stream " << new_stream->get_stream_name();

    _data_path.add_input_path(new_stream->get_stream_name());

    try
    {
        // add stream to engine's structures -- yna
        vector<CatalogStream*> stream;
        stream.push_back(new_stream);
        _aurora.add_stream(stream);
    }
    catch (AuroraException &ae)
    {
        DEBUG << ae.getMessage();
    }

    /*****************  query  *************************/
    string query_name = "try_map";

    map<string, string> box_parameters;
    box_parameters["expression.0"] = "time";
    box_parameters["output-field-name.0"] = "time";
    box_parameters["expression.1"] = "price / 6.3";
    box_parameters["output-field-name.1"] = "usd";

    add_conger_box("try_map", "map", "inputstream",
            "outputstream", box_parameters);

    /*******************   subscribe    *************************/
    CatalogSubscription subscriber;
    subscriber = _local_catalog.add_conger_subsribe("outputstream", "127.0.0.1:25000", "");

    vector<CatalogSubscription> subs;
    subs.push_back(subscriber);

    AsyncRPC<void> completion;
    completion.post(true);
    local_subscribe_streams(completion, subs);

    /* 和上面的 true 配对 */
    _ongoing_dynamic_modification = false;

}

void QueryProcessor::try_union()
{

}

void QueryProcessor::try_filter_map()
{
    _ongoing_dynamic_modification = true;

    /***************** 增加 schema ****************************/
    /*
     <schema name="inputpacket">
     <field name="time" type="int"/>
     <field name="price" type="double"/>
     <field name="trade_volume" type="int"/>
     <field name="trade_amount" type="int"/>
     <field name="bid_ask" type="string" size="1"/>
     </schema>
     */
    list<SchemaFieldType> schema_fields;
    SchemaFieldType schema_field;
    schema_field.fieldName = "stock";
    schema_field.typeName = "string";
    schema_field.size = 6;
    schema_fields.push_back(schema_field);

    schema_field.fieldName = "time";
    schema_field.typeName = "int";
    schema_field.size = 0;
    schema_fields.push_back(schema_field);

    schema_field.fieldName = "price";
    schema_field.typeName = "double";
    schema_field.size = 0;
    schema_fields.push_back(schema_field);

    schema_field.fieldName = "trade_volume";
    schema_field.typeName = "int";
    schema_field.size = 0;
    schema_fields.push_back(schema_field);

    schema_field.fieldName = "trade_amount";
    schema_field.typeName = "int";
    schema_field.size = 0;
    schema_fields.push_back(schema_field);

    schema_field.fieldName = "bid_ask";
    schema_field.typeName = "string";
    schema_field.size = 1;
    schema_fields.push_back(schema_field);

    CatalogSchema *add_schema;
    add_schema = _local_catalog.add_conger_schema("inputpacket", schema_fields);

    vector<SchemaField> fields = add_schema->get_schema_field();

    DEBUG << "schema fields size: " << fields.size() << " schema name "
    << add_schema->get_schema_name();
    vector<SchemaField>::iterator iter = fields.begin();
    for (; iter != fields.end(); iter++)
    {
        DEBUG << "field name: " << iter->get_name() << " type: " << iter->get_type();
    }

    // add schema to engine's structures
    vector<CatalogSchema*> schemas;
    schemas.push_back(add_schema);

    _aurora.add_schema(schemas);

    /****************** input ***********************************/
    string stream_name = "inputstream";
    string schema_name = "inputpacket";

    CatalogStream *new_stream;

    vector<CatalogSubscription> subscription;

    new_stream = _local_catalog.add_conger_stream(stream_name, schema_name);
    DEBUG << "Opening a general purpose input data path for stream " << new_stream->get_stream_name();

    _data_path.add_input_path(new_stream->get_stream_name());

    try
    {
        // add stream to engine's structures -- yna
        vector<CatalogStream*> stream;
        stream.push_back(new_stream);
        _aurora.add_stream(stream);
    }
    catch (AuroraException &ae)
    {
        DEBUG << ae.getMessage();
    }

    /***************** begin query  *************************/
    string query_name = "try_filter_map";

    map<string, string> box_parameters;
    box_parameters["expression.0"] = "price>13.0";
    box_parameters["pass-on-false-port"] = "0";

    add_conger_box("try_filter_map_0", "filter", "inputstream",
            "intermediate", box_parameters);

    box_parameters.clear();
    box_parameters["expression.0"] = "time";
    box_parameters["output-field-name.0"] = "time";
    box_parameters["expression.1"] = "price / 6.3";
    box_parameters["output-field-name.1"] = "usd";

    add_conger_box("try_filter_map_1", "map", "intermediate",
            "outputstream", box_parameters);
    /*********************  end query  **************************/


    /*******************   subscribe    *************************/
    CatalogSubscription subscriber;
    subscriber = _local_catalog.add_conger_subsribe("outputstream", "127.0.0.1:25000", "");

    vector<CatalogSubscription> subs;
    subs.push_back(subscriber);

    AsyncRPC<void> completion;
    completion.post(true);
    local_subscribe_streams(completion, subs);

    /* 和上面的 true 配对 */
    _ongoing_dynamic_modification = false;

}

BOREALIS_NAMESPACE_END
