/**
 * CongerProcessor.cc
 *
 *  Created on: Apr 17, 2012
 *      Author: hellojinjie
 */
#include "QueryProcessor.h"
#include "CongerDiagram.h"      // 继承自  Diagram
#include "DeployParser.h"       // 用来解析 conger 的 xml 部署文件
#include "DeployDescript.h"     // 用来描述部署信息的一个结构体， 该结构体可以由 DeployParser 生成
#include "ParseDriver.h"
#include "ParseContext.h"

BOREALIS_NAMESPACE_BEGIN


AsyncRPC<void> QueryProcessor::add_conger_string(string conger_config)
{
    INFO << conger_config;

    AsyncRPC<void> completion;
    completion.post(true);

    if (conger_config == "filter")
    {
        this->try_filter();
    }
    else if (conger_config == "map")
    {
        this->try_map();
    }
    else if (conger_config == "filter_map")
    {
        this->try_filter_map();
    }
    else if (conger_config == "union")
    {
        this->try_union();
    }
    else if (conger_config == "join")
    {
        this->try_join();
    }
    else if (conger_config == "aggregate")
    {
        this->try_aggregate();
    }
    else if (conger_config == "aggregate_join")
    {
        this->try_aggregate_join();
    }
    else
    {
        DeployParser deploy_parser;
        DeployDescript deploy_descript = deploy_parser.parse(conger_config);

        if (deploy_descript.deployName == "Unspecified Deploy Name")
        {
            return completion;
        }

        _ongoing_dynamic_modification = true;

        add_conger_schema(deploy_descript);
        add_conger_input(deploy_descript);
        add_conger_query(deploy_descript);
        add_conger_subscribe(deploy_descript);

        _ongoing_dynamic_modification = false;

    }

    return completion;
}

void QueryProcessor::add_conger_schema(DeployDescript deploy_descript)
{
    map<string, list<SchemaFieldType> >::iterator schema_iterator;
    for (schema_iterator = deploy_descript.schemas.begin();
            schema_iterator != deploy_descript.schemas.end();
            schema_iterator++)
    {
        list<SchemaFieldType> schema_fields = schema_iterator->second;
        string schema_name = schema_iterator->first;

        CatalogSchema *add_schema;

        add_schema = _local_catalog.add_conger_schema(schema_name, schema_fields);

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
    }
    return;
}

void QueryProcessor::add_conger_input(DeployDescript deploy_descript)
{
    DEBUG << "we have " << deploy_descript.inputStreams.size() << " input stream";
    map<string, string>::iterator input_iterator;
    for (input_iterator = deploy_descript.inputStreams.begin();
            input_iterator != deploy_descript.inputStreams.end();
            input_iterator++)
    {
        string stream_name = input_iterator->first;
        string schema_name = input_iterator->second;

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
    }
}

void QueryProcessor::add_conger_query(DeployDescript deploy_descript)
{
    map<string, map<string, string> >::iterator query_iterator;
    for (query_iterator = deploy_descript.querys.begin();
            query_iterator != deploy_descript.querys.end();
            query_iterator++)
    {
        string query_name = query_iterator->first;
        map<string, string> query_parameters = query_iterator->second;

        string cql = query_parameters["cql"];

        ParseDriver driver;
        ParseContext context = driver.parse(query_name, cql);
        this->transform_cql_to_boxes(context);
    }
}

void QueryProcessor::add_conger_box(string box_name, string type, string in_streams,
        string out_stream, map<string, string> box_parameters)
{
    Status       status = true;
    Query        query;
    CatalogBox  *add_box;
    Boolean      forward;

    CatalogStream::StreamStar::iterator  stream;
               DeferSubscribe::iterator  subscribe;
          vector<InetAddress>::iterator  endpoint;

    _local_catalog.clear_add_query();             // Signal no query wrapper.

    // yna -- by default all additions into the engine are considered as dynamic
    // since the scheduler thread always runs....
    add_box = _local_catalog.add_conger_box(box_name, type, in_streams,
            out_stream, box_parameters);
    DEBUG  << "box=" << add_box->get_box_name();
    status = add_box->infer_box_out(_local_catalog.get_schema_map());

    if (!status)
    {   if (status.is_false())
        {   status = "The box (" + to_string(add_box->get_box_name())
                   + ") has an unknown external type ("
                   + to_string(add_box->get_box_type()) + ").";
        }

        DEBUG  << "infer status=" << status;
        Throw(AuroraBadEntityException, to_string(status));
    }

    // add box to engine's structures -- yna
    //
    vector<CatalogBox*> boxes;
    boxes.push_back(add_box);

    _aurora.add_box(boxes, BoxPackage());
    DEBUG << "... back from add_box.";
    ptr<QBox> b = _aurora.get_box(add_box->get_box_name().as_string());

    if (b)
    {
        // false 和 HA 有关, 可能表示它不是主节点 (primary)
        //
        handle_box_creation(b, false);
    }
    else
    {
        NOTICE << "wasn't able to locate " << b->get_name();
    }

    for (stream  = add_box->get_box_out()->begin();
         stream != add_box->get_box_out()->end(); stream++)
    {
        DEBUG << "Try out " << (*stream)->get_stream_name();
        subscribe = _defer_subscribe.find((*stream)->get_stream_name());

        if (subscribe != _defer_subscribe.end())
        {   for (endpoint  = subscribe->second.begin();
                 endpoint != subscribe->second.end(); endpoint++)
            {
                Name name = (*stream)->get_stream_name();

                DEBUG << "Add deferred subscription to " << name
                                               << " at " << *endpoint;

                // Add subscription.
                // Magda: changed local_subscribe_stream signature.
                CatalogSubscription subscriber(*endpoint, name);
                status = local_subscribe_stream(subscriber, &forward);

                if (!status)
                {   DEBUG << status;
                    Throw(AuroraBadEntityException, to_string(status));
                }

                if (forward)
                {   vector<CatalogSubscription> subs;
                    subs.push_back(CatalogSubscription(*endpoint, name));
                    forward_subs_to_replicas(subs);
                }
            }

            // Remove the subscription from the deferred to-do list.
            //
            _defer_subscribe.erase(subscribe);
        }
    }

    // For fault-tolerance purposes, the ConsistencyManager has to inspect the box
    // If the box is an SUnion or an SOutput then the ConsistencyManager must start to monitor the
    // control output stream of this box.
    //
    _constman.check_for_control_stream(add_box);
}

void QueryProcessor::add_conger_subscribe(DeployDescript deploy_descript)
{

    CatalogSubscription subscriber;
    subscriber = _local_catalog.add_conger_subsribe("outputstream", "127.0.0.1:25000", "");

    vector<CatalogSubscription>  subs;
    subs.push_back(subscriber);

    AsyncRPC<void> completion;
    completion.post(true);
    local_subscribe_streams(completion, subs);

}

BOREALIS_NAMESPACE_END
