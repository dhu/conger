/**
 * CongerDiagram.cc
 *
 *  Created on: Apr 17, 2012
 *      Author: hellojinjie
 */

#include "CongerDiagram.h"
#include "parseutil.h"

BOREALIS_NAMESPACE_BEGIN

CongerDiagram::CongerDiagram()
{

}

CongerDiagram::~CongerDiagram()
{
}

/**
 * 简化的 add schema，没有考虑设定 type size 的情况，也没有考虑 array 的情况
 */
CatalogSchema* CongerDiagram::add_conger_schema(string schema_name,
        list<SchemaFieldType> schema_fields)
{
    TypeArray::iterator             in_array;

    string    message;
    string    name;
    string    type;
    string    size;

    DataType  data;
    uint8     data_size;

    try
    {
        name = schema_name;
        lowercase(name);

        begin_schema(name);

        list<SchemaFieldType>::iterator field_iterator = schema_fields.begin();
        for ( ; field_iterator != schema_fields.end(); field_iterator++)
        {
            name = field_iterator->fieldName;
            type = field_iterator->typeName;
            DEBUG << "add schema: " << name << " type: " << type;
            lowercase(name);
            /* 如果 type 是 string， 下面的方法会返回 0 */
            data_size = data.from_string(type);
            if (data_size == 0)
            {
                /* 对于 string， field 的长度使用 xml 里定义的长度 */
                data_size = field_iterator->size;
            }
            add_field(name, data, data_size);
        }
    }
    catch(AuroraException  &e)
    {
        message = "Failed parsing a schema from XML:\n   " + e.as_string();
        DEBUG << message;
        Throw(AuroraBadEntityException, message);
    }
    return  _add_schema;
}

/**
 * 这里我们只解析 input stream， output stream 是不解析的
 */
CatalogStream* CongerDiagram::add_conger_stream(string stream_name, string schema_name)
{
    CatalogStream   *result;

    string    message;
    string    stream;
    string    schema;

    // Parse the stream and schema names.
    //
    try
    {
        stream = stream_name;
        lowercase(stream);

        schema = schema_name;
        result = add_stream(stream, schema);

    }
    catch(AuroraException  &e)
    {
        message = "Failed parsing an input from XML:\n   " + e.as_string();
        DEBUG << message;
        Throw(AuroraBadEntityException, message);
    }

    return  result;
}

void CongerDiagram::add_conger_query(string query_name,
        map<string, string> query_parameters)
{

}

CatalogBox* CongerDiagram::add_conger_box(string box_name, string type,
        string in_streams, string out_stream, map<string, string> box_parameters)
{
    CatalogSchema::SchemaMap* schema_map = get_schema_map();
    CatalogSchema::SchemaMap::iterator iter = schema_map->begin();
    for (; iter != schema_map->end(); iter++)
    {
        DEBUG << "list schema map:  " << iter->first << "  " << iter->second.get_schema_name();
    }

    string   message;
    string   name;
    string   table;
    string   update;
    string   key;
    string   stream;
    string   value;
    Boolean  single_box_query;
    string   cpview;

    CatalogBox  *add_box = NULL;

    try
    {
        name = box_name;

        lowercase(name);
        lowercase(type);

        // Detect a box outside a query.
        single_box_query = False;

        if (_add_query == NULL)
        {   DEBUG << "Use the box name (" << name << ") for the query";

           // Use the box name for the query.
           begin_query(name);

           // Inherit default queue parameters from borealis.
           _add_query->set_in_queue(_in_queue);

           single_box_query = True;
        }

        begin_box(name, type);

        // Inherit default queue parameters from the query.
        //
        _add_box->set_box_in_queue(
                parse_conger_inqueue(box_parameters, _add_query->get_in_queue()));

        // _add_box->set_box_node(_add_node);  // Inherit the node???
        // XXX 这里没有处理好。。。
        string node = box_parameters["node"];

        if (!node.empty())
        {
            node = Util::form_endpoint(node, DEFAULT_BOREALIS_PORT);
            lowercase(node);

            _add_box->set_box_node(node);

            DEBUG  << "<box  name=\"" << name
                  << "\"  type=\""   << type
                  << "\"  node=\""   << node << "\"/>";
        }
        else
        {   _add_box->set_box_node(_add_node);

           DEBUG  << "<box  name=\"" << name
                  <<   "\"  type=\"" << type << "\"/>";
        }

        char* in_char = const_cast<char* >(in_streams.c_str());
        char* pch;
        pch = strtok(in_char, ":");
        while (pch != NULL)
        {
            stream = string(pch);
            add_in(stream,
                    parse_conger_inqueue(box_parameters, _add_query->get_in_queue()),
                    False);
            DEBUG << "connect stream " << stream << " to box " << name;
            pch = strtok(NULL, ":");
        }

        add_out(out_stream);

        // add_parameter_element(box);
        // box 的参数
        //....................................................
        string   message;
        string   name;
        string   value;


        try
        {
            map<string, string>::iterator iter = box_parameters.begin();
            for (; iter != box_parameters.end(); iter++)
            {
                add_parameter(iter->first, iter->second);
            }
        }
        catch(AuroraException  &e)
        {   message = "Failed parsing a parameter from XML:\n   " + e.as_string();
            DEBUG << message;

            Throw(AuroraBadEntityException, message);
        }
        //......................................................
        // end box 参数

        add_box = _add_box;

        if (single_box_query)
        {   end_query();
        }

        DEBUG  << "</box>";
    }
    catch(AuroraException  &e)
    {   message = "Failed parsing a box from XML:\n   " + e.as_string();
       DEBUG << message;

       Throw(AuroraBadEntityException, message);
    }
    return  add_box;
}


CatalogBox::InQueue CongerDiagram::parse_conger_inqueue(
        map<string, string> box_parameters,
        CatalogBox::InQueue in_queue)
{
    string      update_queue;
    string      queue_type;
    string      key;
    string      message;

    CatalogBox::InQueue  result;


    try
    {
      queue_type = "*";

      key = "*";
    }
    catch(AuroraException  &e)
    {   message = "Failed parsing an update queue from XML:\n   " + e.as_string();

      DEBUG << message;
      Throw(AuroraBadEntityException, message);
    }

    result._update_queue = in_queue._update_queue;
    result._queue_type = in_queue._queue_type;

    if (update_queue == "0")
    {   result._update_queue = False;
    }
    else if (update_queue == "1")
    {   result._update_queue = True;
    }
    else if (!update_queue.empty())
    {   message = "update_queue attribute must be 0 or 1.";

      DEBUG << message;
      Throw(AuroraBadEntityException, message);
    }

    if (queue_type == "*")
    {
      result._queue_type = in_queue._queue_type;
    }
    else if (queue_type == "0")
    {
      result._queue_type = 0;
    }
    else if (queue_type == "1")
    {
      result._queue_type = 1;
    }
    else if (queue_type == "2")
    {
      result._queue_type = 2;
    }
    else if (queue_type == "3")
    {
      result._queue_type = 3;
    }
    else if (queue_type == "4")
    {
      result._queue_type = 4;
    }
    else if (queue_type == "5")
    {
      result._queue_type = 5;
    }
    else if (queue_type == "6")
    {
      result._queue_type = 6;
    }
    else if (queue_type == "7")
    {
      result._queue_type = 7;
    }
    else if (queue_type == "8")
    {
      result._queue_type = 8;
    }
    else
    {
      message = "queuetype attribute must be 0-8.";
      DEBUG << message;
      Throw(AuroraBadEntityException, message);
    }

    if (key == "*")
    {   result._key = in_queue._key;
    }
    else
    {   result._key = key;
    }

    DEBUG << "update=" << int(result._update_queue);
    DEBUG << " queue_type=" << result._queue_type;
    DEBUG << " key=" << result._key;

    if ((update_queue == "0")  &&  (key != "*"))
    {   message = "Invalid:  update_queue=0 and key=\""
              + to_string(result._key) + "\"";

      DEBUG << message;
      Throw(AuroraBadEntityException, message);
    }

    if ((update_queue == "0")  && (queue_type != "*"))
    {
      message = "Invalid:  update_queue=0 and queue_type=\""
              + to_string(result._queue_type) + "\"";

      DEBUG << message;
      Throw(AuroraBadEntityException, message);
    }

    return  result;
}

CatalogSubscription CongerDiagram::add_conger_subsribe(string stream,
        string endpoint, string gap)
{

    string    message;

    uint16    size;
    CatalogSubscription  subscription;
//
//  <subscribe  stream=<output name>  [endpoint=<monitor>  [gap=<size>]] />
//..............................................................................


    try
    {
        DEBUG << "*** <subscribe  stream=\"" << stream
              << "\" endpoint=\"" << endpoint
              << "\"      gap=\"" << gap << "\" />";

        if (gap.empty())
        {   size = 0xFFFF;
        }
        else
        {   size = atoi(gap.c_str());
        }

        if (_client_monitor.empty())
        {   endpoint = form_endpoint(endpoint, DEFAULT_MONITOR_PORT);
        }
        else
        {   if (!endpoint.empty())
            {   DEBUG << "A subscribe element within a client may not specify an endpoint.";

                Throw(AuroraBadEntityException,
                      "A subscribe element within a client may not specify an endpoint.");
            }

            DEBUG << "endpoint=" << endpoint << "  _client_monitor=" << _client_monitor;
            endpoint = _client_monitor;
        }

        DEBUG << "<subscribe  stream=\"" << stream
              << "\" endpoint=\"" << endpoint
              << "\"      gap=\"" << gap << "\" />";

        lowercase(stream);

        ///////////////////////////////////  experimental
        vector<Name>  list;

        split_name(endpoint, list);

        if (list.size() > 1)
        {   DEBUG << "output stream=" << stream << " partition=" << list.size();
            _partition_output[stream] = endpoint;
        }
        /////////////////////////////////////////

        subscription = add_subscribe(stream, endpoint, size);
        subscription.set_sink_flag();
        DEBUG << "sink flag = " << subscription.get_sink_flag();

        // Brad says: We may want to move this into the regional.
        //
        if (!get_stream( Name(stream)))
        {   DEBUG << "Invalid stream: " << stream;

            Throw( AuroraBadEntityException,
                   "Invalid stream: " + stream );
        }
        else
        {   get_stream( Name( stream ))->
                subscribe_sink( get_subscriber( InetAddress( endpoint ),
                                                Name( stream )));
        }
    }
    catch(AuroraException  &e)
    {   message = "Failed parsing an XML subscribe element:\n   " + e.as_string();
        DEBUG << message;

        Throw(AuroraBadEntityException, message);
    }

    return  subscription;
}

BOREALIS_NAMESPACE_END
