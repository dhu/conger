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
#include "antlr3.h"             // antlr3 的 runtime 的头文件 /opt/borealis-tools/antlrs/include
#include "CongerCQLLexer.h"     // antlr3 自动生成的文件
#include "CongerCQLParser.h"    // antlr3 自动生成的文件

BOREALIS_NAMESPACE_BEGIN

typedef struct CQLWindowStruct
{
    int range;
    int slide;
} CQLWindow;

typedef struct CQLSelectStruct
{
    string function;
    string field;
} CQLSelect;

typedef struct CQLContextStruct
{
    string cql;
    list<CQLSelect> select_list;
    list<string> from_list;
    CQLWindow window;

} CQLContext;

CQLContext parse_select(string cql_string);

void parse_tree_node(CQLContext &context, pANTLR3_BASE_TREE node);

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
        CQLContext context = parse_select(cql);
        DEBUG << "context.cql: " << "\n" << context.cql;
        DEBUG << "context.输入流: " << context.from_list.front();

        map<string, string> box_parameters;
        box_parameters["aggregate-function.0"] = "max(price)";
        box_parameters["aggregate-function-output-name.0"] = "price";
        box_parameters["window-size-by"] = "VALUES";
        box_parameters["window-size"] = "600";
        box_parameters["advance"] = "10";
        box_parameters["order-by"] = "FIELD";
        box_parameters["order-on-field"] = "time";

        add_conger_box("cql_test", "aggregate", "inputstream",
                "outputstream", box_parameters);
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

void parse_tree_node(CQLContext &context, pANTLR3_BASE_TREE node)
{

}

CQLContext parse_select(string cql_string)
{
    pANTLR3_INPUT_STREAM input;
    pCongerCQLLexer lxr;
    pANTLR3_COMMON_TOKEN_STREAM tstream;
    pCongerCQLParser psr;

    const char * inputString = cql_string.c_str();

    input = antlr3StringStreamNew((uint8_t *) inputString, ANTLR3_ENC_UTF8,
            strlen(inputString), (uint8_t *) "test_statement");
    lxr = CongerCQLLexerNew(input);
    tstream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT,
            TOKENSOURCE(lxr));
    psr = CongerCQLParserNew(tstream);

    CongerCQLParser_statement_return statementAST = psr->statement(psr);

    pANTLR3_BASE_TREE root = statementAST.tree;
    pANTLR3_BASE_TREE treeNode;

    treeNode = (pANTLR3_BASE_TREE) root->getChild(root, 0);

    ANTLR3_UINT32 treeType = treeNode->getType(treeNode);
    if (treeType == TOK_SFW)
    {
        DEBUG << "this is a SFW token";
    }
    else
    {
        DEBUG << "this is not SFW token. it is " << treeType;
    }

    CQLContext context;
    /* 对 TOK_SFW 的子节点进行遍历 */
    DEBUG << "TOK_SFW 的子节点个数: " << treeNode->getChildCount(treeNode);
    pANTLR3_BASE_TREE sfwNode = treeNode;
    for (uint32 i = 0; i < sfwNode->getChildCount(sfwNode); i++)
    {
        pANTLR3_BASE_TREE currentNode;
        pANTLR3_BASE_TREE parentNode;
        currentNode = (pANTLR3_BASE_TREE) sfwNode->getChild(sfwNode, i);
        treeType = currentNode->getType(currentNode);

        DEBUG << "tree node type: " << treeType;

        ANTLR3_UINT32 currentType;
        CQLSelect cql_select;
        CQLWindow window;
        string from_field;
        context.cql = cql_string;
        switch (treeType)
        {
        case TOK_SELECT:
            currentNode = (pANTLR3_BASE_TREE) currentNode->getChild(currentNode, 0);
            currentType = currentNode->getType(currentNode);
            if (currentType != TOK_PROJTERM_LIST)
            {
                DEBUG << "unexpected token, we expect TOK_PROJTERM_LIST";
            }
            DEBUG << "we met TOK_PROJTERM_LIST";
            currentNode = (pANTLR3_BASE_TREE) currentNode->getChild(currentNode, 0);
            currentType = currentNode->getType(currentNode);
            if (currentType != TOK_PROJTERM)
            {
                DEBUG << "unexpected token, we expect TOK_PROJTERM";
            }
            DEBUG << "we met TOK_PROJTERM";
            currentNode = (pANTLR3_BASE_TREE) currentNode->getChild(currentNode, 0);
            currentType = currentNode->getType(currentNode);
            if (currentType != TOK_ARITH_EXPR)
            {
                DEBUG << "unexpected token, we expect TOK_ARITH_EXPR";
            }
            DEBUG << "we met TOK_ARITH_EXPR";
            currentNode = (pANTLR3_BASE_TREE) currentNode->getChild(currentNode, 0);
            currentType = currentNode->getType(currentNode);
            if (currentType != TOK_AGGR)
            {
                DEBUG << "unexpected token, we expect TOK_AGGR";
            }
            DEBUG << "we met TOK_AGGR";
            parentNode = currentNode;
            currentNode = (pANTLR3_BASE_TREE) currentNode->getChild(currentNode, 0);
            currentType = currentNode->getType(currentNode);
            if (currentType != KW_MAX)
            {
                DEBUG << "unexpected token, we expect KW_MAX";
            }
            DEBUG << "we met KW_MAX";
            cql_select.function = "max";
            *currentNode = *parentNode;
            currentNode = (pANTLR3_BASE_TREE) currentNode->getChild(currentNode, 1);
            currentType = currentNode->getType(currentNode);
            if (currentType != TOK_ARITH_EXPR)
            {
                DEBUG << "unexpected token, we expect TOK_ARITH_EXPR";
            }
            DEBUG << "we met TOK_ARITH_EXPR";
            currentNode = (pANTLR3_BASE_TREE) currentNode->getChild(currentNode, 0);
            cql_select.field = (char*)currentNode->getText(currentNode)->chars;
            context.select_list.push_back(cql_select);
            break;
        case TOK_FROM:
            currentNode = (pANTLR3_BASE_TREE) currentNode->getChild(currentNode, 0);
            currentType = currentNode->getType(currentNode);
            if (currentType != TOK_RELATION_LIST)
            {
                DEBUG << "unexpected token, we expect TOK_RELATION_LIST";
            }
            DEBUG << "we met TOK_RELATION_LIST";
            currentNode = (pANTLR3_BASE_TREE) currentNode->getChild(currentNode, 0);
            currentType = currentNode->getType(currentNode);
            if (currentType != TOK_RELATION_VARIABLE)
            {
                DEBUG << "unexpected token, we expect TOK_RELATION_VARIABLE";
            }
            DEBUG << "we met TOK_RELATION_VARIABLE";
            parentNode = currentNode;
            currentNode = (pANTLR3_BASE_TREE) currentNode->getChild(currentNode, 0);
            from_field = string((char*)currentNode->getText(currentNode)->chars);
            context.from_list.push_back(from_field);

            currentNode = parentNode;
            currentNode = (pANTLR3_BASE_TREE) currentNode->getChild(currentNode, 1);
            if (currentType != TOK_WINDOW)
            {
                DEBUG << "unexpected token, we expect TOK_WINDOW";
            }
            DEBUG << "we met TOK_WINDOW";
            parentNode = currentNode;
            currentNode = (pANTLR3_BASE_TREE) currentNode->getChild(currentNode, 1);
            DEBUG << "before atoi";
            window.range = atoi((char*)currentNode->getText(currentNode)->chars);
            currentNode = parentNode;
            currentNode = (pANTLR3_BASE_TREE) currentNode->getChild(currentNode, 4);
            window.slide = atoi((char*)currentNode->getText(currentNode)->chars);
            context.window = window;
            break;
        default:
            DEBUG << "unhandled tree type";
            break;
        }
    }

    input->close(input);
    lxr->free(lxr);
    tstream->free(tstream);
    psr->free(psr);
    return context;

}

BOREALIS_NAMESPACE_END
