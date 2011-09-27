#include "QueryProcessor.h"
#include "parseutil.h"

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
//  Add a network update in Xml to a Borealis node.
//
AsyncRPC<void>  QueryProcessor::add_xml_string(// Node update in XML.
                                               string   update_xml)
{
    AsyncRPC<void>     completion;

    string             tag;
    Name               name;
    const DOMElement  *root;
//
//                      Update XML into a Borealis node:
//
//    <component    name={"shedder", "revision", "availability"} />
//    <region   endpoint={rip:rport} />
//    <head     endpoint={hip:hport} />
//    <subscribe  stream={output name}  endpoint={monitor}   [gap={size}] />
//    <input      stream={stream name}  schema={schema name} [node={sender}] />
//
//    <schema name={schema name} >
//        <field name={field name} type={field type} [size={string size}] />  ...
//    </schema>
//
//    <box name={box name} type={box type} [node={endpoint}] >
//        <in   stream={stream name} />  ...
//        <out  stream={stream name} />  ...
//        <parameter name={parameter name}  value={parameter value} />  ...
//    </box>
//
//    <table  name={table name}   schema="{schema name}" >
//        <key     field="{field name}"  />  ...
//       [<index   field="{field name}"  />  ...]
//       [<parameter  name={parameter name}   value={parameter value} />  ...]
//    </table>
//
//    <connection_point_view name={view name} stream={stream name} >
//        <order field={field name} />?
//      ( <size  value={number of tuples} />
//      | <range start={start tuple}  end={end tuple} /> )
//    </connection_point_view>
//
//    <replica_set  name="<set name" />
//        <node ... />  ...
//    </replica_set>
//
//    <delete  query={query list}  box={box list} />
//
//    <move  (box={box list} | table={table list})  node={destination} />
//
//    <modify  (box={box name} | table={table name}) >
//       <parameter  name={parameter name}  value={parameter value} /> ...
//    </modify>
//
//    <disconnect  stream={new stream} />
//
//    <connect (box={destination box}
//            [port={zero based destination port}]  [upstream={0 | 1}])
//           | node={node with the destination box} >
//
//        <box name={box to insert}  type={box type} >
//            <in   stream={existing source stream} />
//            <out  stream={new destination stream} />
//                    :
//        </Box>
//    </connect>
//
//  In cases where the completion fails, the Xml should not be reflected???
//..............................................................................


    _ongoing_dynamic_modification = true; // -- tatbul

    DEBUG << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$";
    DEBUG << "Update XML:  " << update_xml;

    Status  status = _local_catalog.parse_element(update_xml);

    if (status)
    {   root = _local_catalog.get_element();
        tag  = to_string(root->getTagName());

        try
        {   if (tag == "region")
            {   add_region_element(root);
                completion.post(true);
            }
            else if (tag == "head")
            {   add_head_element(root);
                completion.post(true);
            }
            else if (tag == "box")
            {   name = xml_attribute(root, "name", ATTR_REQUIRED);
                add_box_element(root);

                if ( root == NULL )
                {   DEBUG << "Root disappeared after add_box_element.";
                }
                
                bool primary = (HAType::BACKUP != xml_attribute(root, "ha_type", ATTR_NOT_REQUIRED));

                if (primary)
                {   reflect_region_update(update_xml);
                }

                completion.post(true);
            }
            else if (tag == "table")
            {   add_table_element(root);
                reflect_region_update(update_xml);
                completion.post(true);
            }
            else if (tag == "schema")
            {   // Unless already in the catalog, add the schema.  When
                // moving boxes, schemas may be re-deployed within a region.
                //
                name = xml_attribute(root, "name", ATTR_REQUIRED);

                if (!_local_catalog.get_schema(name))
                {   add_schema_element(root, update_xml);   // Contains field elements.
                    reflect_region_update(update_xml);
                }

                completion.post(true);
            }
            else if (tag == "input")
            {   add_input_element(root, completion, update_xml);

                if (completion.stat())
                {   reflect_region_update(update_xml);
                }
            }
            else if (tag == "subscribe")
            {   add_subscribe_element(root, completion);
                //reflect_region_update(update_xml);
            }
            else if (tag == "connection_point_view")
            {   add_view_element(root);
                reflect_region_update(update_xml);
                completion.post(true);
            }
            else if (tag == "replica_set")
            {   add_replica_element(root);
                completion.post(true);
            }
            else if (tag == "move")
            {   move_element(root, completion);
            }
            else if (tag == "delete")
            {   delete_element(root, completion);

                if (completion.stat())
                {   //reflect_global_update(update_xml);  // NYI
                }
            }
            else if (tag == "modify")
            {
                modify_element(root, completion);

                if (completion.stat())
                {   reflect_region_update(update_xml);
                    completion.post(true);
                }
            }
            else if (tag == "connect")
            {   connect_element(root, update_xml, completion);
            }
            else if (tag == "disconnect")
            {   disconnect_element(root, completion);
            }
            else if (tag == "component")
            {   component_element(root);
                completion.post(true);
            }
            else
            {   status = "Unknown update element:  " + tag;
            }
        }
        catch(AuroraException  &e)
        {   status = e.as_string();
            DEBUG << status;
        }
    }

    if (!status)
    {   DEBUG << "Failed parsing update XML:  " << status;
        _ongoing_dynamic_modification = false; // -- tatbul
        return(RPCFault("Unable to parse update XML: " + to_string(status)));
    }

    if (completion.avail())
    {   DEBUG << "******** completion:  " << completion.stat();
    }
    else
    {   DEBUG << "******** completion is not available yet.";
    }

    _ongoing_dynamic_modification = false; // -- tatbul

    return(completion);
}



////////////////////////////////////////////////////////////////////////////////
//
//  Add a vector of network update in Xml to a Borealis node.
//
Status  QueryProcessor::add_xml_batch(// Node updates in XML.
                                      vector< string >  update_xml,
 
                                      // Provide access to the QueryProcessor.
                                      QueryProcessor   &query_processor)
{
    AsyncRPC<void>  rpc;

    Status     status = true;
//
//..............................................................................


    for (uint32 i = 0; i < update_xml.size(); i++)
    {
        rpc = query_processor.add_xml_string(update_xml[i]);
        DEBUG << "add_xml_string " << update_xml[i] << ": " << rpc.stat();

        status = rpc.stat();
        if (!status) break;
    }

    return(status);
}



////////////////////////////////////////////////////////////////////////////////
//
//  Establish a port to reflect updates to a regional optimizer.
//
void  QueryProcessor::add_region_element(const DOMElement  *Region)
                                   throw(AuroraBadEntityException)
{
    string         endpoint;
    string         region;
    UpdateClient  *client;
//
//  <region  endpoint="<region ip>:<region port>" />
//..............................................................................


    xml_expect_tag(Region, "region");
    endpoint = xml_attribute(Region, "endpoint", ATTR_REQUIRED);

    if (is_dom_attribute_present(*Region, "node"))
    {   Throw(AuroraBadEntityException,
              "Region node attribute not allowed in update XML.");
    }

    DEBUG  << "<region  endpoint=\"" << endpoint << "\">";

    // Include the regional component as a region containing this node.
    // Subsequent update XML will be reflected to the region.
    //
    client = new UpdateClient(InetAddress(endpoint));
    _region_client.push_back(client);

    // Inform the region that this node is joining.
    //
    region = "<region  node=\"" + to_string(_addr) + "\" />";
    reflect_update_xml(region, client);

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Establish a port to reflect updates to the Head.
//
void  QueryProcessor::add_head_element(const DOMElement  *Head)
                                 throw(AuroraBadEntityException)
{
    string   endpoint;
    string   head;
//
//  <head  endpoint="<head ip>:<head port>" />
//..............................................................................


    xml_expect_tag(Head, "head");
    endpoint = xml_attribute(Head, "endpoint", ATTR_REQUIRED);

    DEBUG  << "<head  endpoint=\"" << endpoint << "\">";

    if (_head_client)
    {   Throw(AuroraBadEntityException,
              "The connection to the Head has already been established.");
    }

    _head_client = new UpdateClient(InetAddress(endpoint), "BigGiantHead");

    if (!_head_client->is_connected())
    {   delete  _head_client;
        _head_client = NULL;

        FATAL << "I can't find my Head.  Where is my Head?";
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Establish a set of replicas in a local catalog.
//
void  QueryProcessor::add_replica_element(const DOMElement  *replica)
                                    throw(AuroraBadEntityException)
{
    string   name;
    string   endpoint;

    vector<DOMElement *>            node;
    vector<DOMElement *>::iterator  iterator;
//
//    <replica_set  name="<set name" />
//        <node ... />  ...
//    </replica_set>
//..............................................................................


    xml_expect_tag(replica, "replica_set");
    name = xml_attribute(replica, "name", ATTR_REQUIRED);

    DEBUG  << "<replica_set  name=\"" << name << "\">";
    xml_child_elements(node, replica, "node");

    for ( iterator = node.begin(); iterator != node.end(); ++iterator )
    {
        // Parse the stream name and endpoint.
        //
        xml_expect_tag(*iterator, "node");
        endpoint = xml_attribute(*iterator, "endpoint",  ATTR_REQUIRED);

        _local_catalog.add_replica(endpoint);

        DEBUG << " <node endpoint=\"" << endpoint << "\" />";
    }

    DEBUG  << "</replica_set>";

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Add a box to a Borealis node.
//
void  QueryProcessor::add_box_element(const DOMElement  *box)
                                throw(AuroraException)
{
    Status       status = true;
    Query        query;
    CatalogBox  *add_box;
    Boolean      forward;

    CatalogStream::StreamStar::iterator  stream;
               DeferSubscribe::iterator  subscribe;
          vector<InetAddress>::iterator  endpoint;
//
//      <box name={box name} type={box type} [node=<endpoint>] >
//          <in   stream={stream name} />  ...
//          <out  stream={stream name} />  ...
//          <parameter name={parameter name}  value={parameter value} />  ...
//      </box>
//
//  A query passed to a Borealis node only contains boxes.
//  Tables and connection points are added separately.
//
//  Exit:  AuroraBadEntityException - Parse error.
//         AuroraException          - Semantic error.
//..............................................................................


    _local_catalog.clear_add_query();             // Signal no query wrapper.

    // yna -- by default all additions into the engine are considered as dynamic
    // since the scheduler thread always runs....
    add_box = _local_catalog.add_box_element(box, true);

    bool primary = (HAType::BACKUP != xml_attribute(box, "ha_type", ATTR_NOT_REQUIRED));

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
        handle_box_creation(b, primary);
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
                //status = local_subscribe_stream(name, *endpoint, &forward);

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

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Add a table to a Borealis node.
//
void  QueryProcessor::add_table_element(const DOMElement  *table)
                                  throw(AuroraException)
{
    Query          query;
    CatalogTable  *add_table;
//
//      <table  name={table name}   schema="{schema name}" >
//          <key     field="{field name}"  />  ...
//         [<index   field="{field name}"  />  ...]
//         [<parameter  name={parameter name}   value={parameter value} />  ...]
//      </table>
//
//  Exit:  AuroraBadEntityException - Parse error.
//         AuroraException          - Semantic error.
//..............................................................................


    _local_catalog.clear_add_query();             // Signal no query wrapper.
    add_table = _local_catalog.add_table_element(table);

    DEBUG  << "table=" << add_table->get_table_name();

    // add tables to engine. --yna
    //
    vector<CatalogTable*> tables;
    tables.push_back(add_table);

    _aurora.add_table(tables);

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Add a schema declaration to a Borealis node.
//
void  QueryProcessor::add_schema_element(const DOMElement  *schema,
                                         string update_xml)
                                   throw(AuroraException)
{
    CatalogSchema  *add_schema;
//
//  <schema name={schema name} >
//      <field name={field name} type={field type} [size={string size}] />  ...
//  </schema>
//
//  Exit:  AuroraBadEntityException - Parse error.
//         AuroraException          - Semantic error.
//..............................................................................


    add_schema = _local_catalog.add_schema_element(schema);

    // add schema to engine's structures -- yna
    vector<CatalogSchema*> schemas;
    schemas.push_back(add_schema);

    _aurora.add_schema(schemas);

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Add a stream declaration to a Borealis node.
//  If a node is included, subscribe to an intermediate stream.
//
void  QueryProcessor::add_input_element(const DOMElement  *input,
                                        AsyncRPC<void>    &completion,
                                        string             update_xml)
                                  throw(AuroraBadEntityException)
{
    CatalogStream  *new_stream;
    string          node;

    vector<CatalogSubscription>  subscription;
//
// Entry:  The schema must already have been added.
//
//  <input  stream={stream name} schema={schema name} [node={sender}] />
//
//  If the node attribute the stream is an intermediate stream and a
//  subscriber is set up.  Otherwise the stream is a genuine input stream.
//  Parse errors throw an exception; semantic errors post an invalid completion.
//..............................................................................


    // this could parse the node, too???
    new_stream = _local_catalog.add_input_element(input);

    node = xml_attribute(input, "node", ATTR_NOT_REQUIRED);

    // Moving the subscription stuff outside the legacy code
    // We create an input path for each system-wide input stream
    DEBUG << "Opening a general purpose input data path for stream " << new_stream->get_stream_name();

    _data_path.add_input_path(new_stream->get_stream_name());

    try
    {   // add stream to engine's structures -- yna
        vector<CatalogStream*> stream;
        stream.push_back(new_stream);

        // add to the set of input streams.  Not used by QP???
        //_local_catalog.add_input(to_string(new_stream->get_stream_name()));

        _aurora.add_stream(stream);

        if (!node.empty())
        {   // if a subscription do an add subscriber
            DEBUG << "node=" << node;

            InetAddress  endpoint(node);
            CatalogSubscription sub(endpoint, new_stream->get_stream_name());
            subscription.push_back(sub);

            Callback<void, RPC<void> > dummy;
            local_add_subscriptions(subscription, dummy);
            DEBUG  << "... back from local_add_subscriptions.";
        }

        completion.post(true);
    }
    catch (AuroraException ae)
    {   completion.post(RPCFault(ae.getMessage()));
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Add a subscriber to a Borealis node.
//
void QueryProcessor::add_subscribe_element(const DOMElement  *sub,
                                           AsyncRPC<void>    &completion)
                                     throw(AuroraBadEntityException)
{
    CatalogSubscription subscriber;
//
//  Parse errors throw an exception; semantic errors post an invalid completion.
// .............................................................................


    subscriber = _local_catalog.add_subscribe_element(sub);

    vector<CatalogSubscription>  subs;
    subs.push_back(subscriber);

    local_subscribe_streams(completion, subs);

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Add a connection point to a Borealis node.
//
void  QueryProcessor::add_view_element(const DOMElement  *view)
                                 throw(AuroraException)
{
    CPViewDescription   view_object;
    CatalogView        *add_view;
//
//  <connection_point_view name={view name} stream={stream name} >
//      <order field={field name} />?
//    ( <size  value={number of tuples} />
//    | <range start={start tuple}  end={end tuple} />
//    )
//  </connection_point_view>
//
//  This syntax is stale and is kept for compatibility.
//  Replace with the parser in the distributed catalog???
//
//  Exit:  AuroraBadEntityException - Parse error.
//         AuroraException          - Semantic error.
//..............................................................................


    add_view = _local_catalog.add_view_element(view);

    // CPViewDescription SETUP REASONING
    // An empty string if ordering by tuple.
    // field -> ordering field, empty if ordering by TUPLE
    // start -> start value of range or size of window
    // end -> end value of range, ignored if ordering by TUPLE
    //
    StreamDef cp_stream(add_view->get_view_stream_name());  // stream the CP is on
    view_object.from_catalog( add_view ); 
    _aurora.create_cpview(view_object, cp_stream);

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Relocate a box or table.
//
void QueryProcessor::move_element(const DOMElement  *move,
                                  AsyncRPC<void>    &completion)
                            throw(AuroraBadEntityException)
{
    string        box;
    string        node;
    string        table;
    vector<Name>  list;
//
//    <move  (box={box list} | table={table list})  node={destination} />
//
//  Exit:  Parse errors throw an exception
//         Semantic errors post an invalid completion.
//         The completion will not be posted true until all stages run okay.
//         Update XML is reflected in the last stage.
//..............................................................................


    xml_expect_tag(move, "move");
    box   = xml_attribute(move, "box",   ATTR_NOT_REQUIRED);
    table = xml_attribute(move, "table", ATTR_NOT_REQUIRED);
    node  = xml_attribute(move, "node",  ATTR_REQUIRED);

    if (!table.empty())
    {   Throw(AuroraBadEntityException, "NYI:  move table");
    }

    DEBUG  << "<move  box=\"" << box << "\"  node=\"" << node << "\">";

    split_name(box, list);

    if (list.empty())
    {   Throw(AuroraBadEntityException, "No boxes to move.");
    }

    // The target node must not be this node.
    //
    if (node == to_string(_id))
    {   DEBUG  << "node=" << node << "  self=" << _data_add << "  id=" << _id;
        Throw(AuroraBadEntityException, "The move node is this node.");
    }

    local_move_boxes(completion, list, MedusaID(node));

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Delete a query or boxes.  Note:  We may also want to delete streams.
//
void QueryProcessor::delete_element(const DOMElement  *remove,
                                    AsyncRPC<void>    &completion)
                              throw(AuroraBadEntityException)
{
    string        box;
    string        table;
    string        query;
    vector<Name>  list;
//
//    <delete  [query={query list}]  [box={box list})] [table={table list})] />
//
//  In update XML the query attribute is invalid.
//  Parse errors throw an exception; semantic errors post an invalid completion.
//..............................................................................


    xml_expect_tag(remove, "delete");

    box   = xml_attribute(remove, "box",   ATTR_NOT_REQUIRED);
    table = xml_attribute(remove, "table", ATTR_NOT_REQUIRED);
    query = xml_attribute(remove, "query", ATTR_NOT_REQUIRED);

    if (!table.empty())
    {   Throw(AuroraBadEntityException, "NYI:  delete table");
    }

    if (!query.empty())
    {   Throw(AuroraBadEntityException,
              "delete query not allowed in update XML.");
    }

    split_name(box, list);

    if (list.empty())
    {   Throw(AuroraBadEntityException, "No boxes to delete.");
    }

    local_remove_boxes(completion, list);

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Modify parameters for a box or table.
//
void QueryProcessor::modify_element(const DOMElement  *modify,
                                    AsyncRPC<void>    &completion)
                              throw(AuroraException)
{
    string   box;
    string   table;
    string   stream;
    string   value;

    CatalogBox           *catalog_box;

    vector<string>        choked_box;
    vector<DOMElement *>  parameter;

    vector<DOMElement *>::iterator  iterator;

    vector< pair<CatalogBox*, pair<uint16, bool> > >   boxes_and_swaps;
//
//    <modify  (box={box name} | table={table name}) >
//      <parameter  name={parameter name}  value={parameter value} /> ...
//    </modify>
//
//  NYI:  modify a table.
//  Parse errors throw an exception; semantic errors post an invalid completion.
//..............................................................................


    try
    {   xml_expect_tag(modify, "modify");

        box   = xml_attribute(modify, "box",   ATTR_NOT_REQUIRED);
        table = xml_attribute(modify, "table", ATTR_NOT_REQUIRED);

        if (box.empty())
        {   Throw(AuroraException, "NYI:  Table modification.");
        }
        else if (!table.empty())
        {   Throw(AuroraException, "modify has both a box and table attribute.");
        }
        else
        {   DEBUG  <<  "Modify box=" << box;
            catalog_box = _local_catalog.get_box(box);

            if (!catalog_box)
            {   Throw(AuroraException,
                      "Can not find the box (" + to_string(box)
                                               + ") to modify.");
            }

            // Choke dest box as early as possible to minimize its
            // growth in queue size, and to prevent it being scheduled to run.
            //
            choked_box.push_back(box);
            _aurora.choke_boxes(choked_box);
 
            // Modify parameters in the local catalog.
            //
            _local_catalog.add_modify_element(modify);

            xml_child_elements(parameter, modify, "parameter");

            if (parameter.begin() != parameter.end())
            {   _aurora.update_box(catalog_box);     // Modify parameters only.
            }

            // Resume
            //
            if (choked_box.empty())
            {   FATAL << "choked_box.empty()";
            }

            _aurora.resume_boxes(choked_box);

            NOTICE << "boxes resumed from modify";

        }
    }
    catch(AuroraException ae)
    {   completion.post(RPCFault(ae.getMessage()));
    }

    return;
}


////////////////////////////////////////////////////////////////////////////////
//
// Deploy a box upstream on another node to a box port on this node.
//
void  QueryProcessor::connect_element(const DOMElement  *connect,
                                      string             update_xml,
                                      AsyncRPC<void>    &completion)
                                throw(AuroraException)
{
    Status    status = true;

    string    new_stream;      // New stream from insert to destination box.
    string    old_stream;      // Existing stream from source to insert box.
    string    add_box;         // Box being inserted.
    string    dest_box;        // Destination box.
    string    dest_port;       // Index to the In port on the destination box.
    string    connect_xml;
    string    modify_xml;
    string    upstream;

    uint16    port = 0;

    CatalogStream        *catalog_stream;  // Entry for the existing stream.
    CatalogBox           *catalog_box;     // Entry for the destination box.
    CatalogBox           *catalog_add;     // Entry for the new box.
    BoxPort              *box_port;

    vector<string>        choked_box;
    AuroraNode::PortSwaps swaps;
    
    RPC<void>             rpc;
    InetAddress           node;
    string::size_type     index;

    vector<DOMElement *>  box;
    vector<DOMElement *>  out;
    vector<DOMElement *>  in;
//
//    <connect (box={destination box}
//            [port={zero based destination port}]  [upstream={0 | 1}])
//           | node={node with the destination box} >
//
//        <box name={box to insert}  type={box type} >
//            <in   stream={existing source stream} />
//            <out  stream={new destination stream} />
//                    :
//        </box>
//    </connect>
//
//
//     Before   -----S---->[ destination box ]
//
//     After    --S-->[ insert box ]--N-->[ destination box ]
//
//
//                     Upstream Node      |          This Node
//                                        |
//     Before   -------------S------------|--S-->[ destination box ]
//                                        |
//     After    --S-->[ insert box ]--N-->|--N-->[ destination box ]
//                                        |
//  Parse errors throw an exception; semantic errors post an invalid completion.
//..............................................................................


    xml_expect_tag(connect, "connect");

    // We could potentially connect multiple boxes at once here???
    // The local node needs the new stream and the destination box and port.
    //
    xml_child_elements(box, connect, "box");
 
    if (box.size() != 1)
    {   Throw(AuroraException, "One box must be in a connect.");
    }

    add_box = xml_attribute(*box.begin(), "name", ATTR_REQUIRED);
    lowercase(add_box);

    xml_child_elements(out, *box.begin(), "out");

    if (out.size() != 1)
    {   Throw(AuroraException, "The box must have one Out stream.");
    }

    xml_child_elements(in, *box.begin(), "in");

    if (in.size() != 1)
    {   Throw(AuroraException, "The box must have one In stream.");
    }

    old_stream = xml_attribute(*in.begin(),  "stream", ATTR_REQUIRED);
    new_stream = xml_attribute(*out.begin(), "stream", ATTR_REQUIRED);
    dest_box   = xml_attribute(connect,      "box",    ATTR_NOT_REQUIRED);

    // We may want to move casing into the regional component???
    lowercase(old_stream);
    lowercase(new_stream);
    lowercase(dest_box);

    if (!dest_box.empty())
    {   // This block performs the connect on the local (This) node
        // containing the destination box.
        //
        dest_port = xml_attribute(connect, "port", ATTR_NOT_REQUIRED);

        if (!dest_port.empty())
        {   port = atoi(dest_port.c_str());
        }

        // Reference the destination box and the existing stream.
        //
        DEBUG << "destination box=" << dest_box << " port=" << port;
        catalog_box = _local_catalog.get_box(Name(dest_box));

        if (!catalog_box)
        {   Throw(AuroraException,
                  "The destination box is not cataloged:  " + dest_box);
        }

        if (old_stream !=
            catalog_box->get_in_stream(port)->get_stream_name_string())
        {
            Throw(AuroraException,
                  "The old stream in not currently connected("
                       + to_string(old_stream) + ").");
        }

        catalog_stream = _local_catalog.get_stream(old_stream);

        if (!catalog_stream)
        {   Throw(AuroraException,
                  "Can not find old stream (" + to_string(old_stream)
                                                  + ").");
        }

        DEBUG << "new_stream=" << new_stream
              << "  dest_box=" << dest_box   <<  "  dest_port=" << port;

        DEBUG << "old sink size=" << catalog_stream->get_sink_port()->size();

        upstream = xml_attribute(connect, "upstream", ATTR_NOT_REQUIRED);

        if (upstream == "")
        {   upstream = "0";
        }
        else if (upstream == "1")
        {   if (!catalog_stream->is_input_stream())
            {   Throw(AuroraException,
                      "Can not place a box upstream on internal stream ("
                           + to_string(old_stream) + ").");
            }
        }
        else if (upstream != "0")
        {   Throw(AuroraException,
                  "upstream must be 0 or 1; not (" + upstream + ").");
        }

        if (upstream == "0")
        {   // local connect:
            // The filter and the destination boxes are on the same node.
            //
            // Catalog the new box; catalogs the new stream with a null schema.
            // The dynamic flag is set to disable adding sink ports for
            // the existing In streams.
            //
            // NYI:  If other new box streams need to be connected they need to
            //       be connected by adding more code here.
            //
            _local_catalog.clear_add_query();        // Signal no query wrapper.
            catalog_add = _local_catalog.add_box_element(*box.begin(), True);

            // The new stream schema is the same as the old.
            //
            _local_catalog.get_stream(Name(new_stream))
                 ->set_stream_schema(catalog_stream->get_stream_schema());

            DEBUG << "new stream schema="
                  << _local_catalog.get_stream(Name(new_stream))
                         ->get_stream_schema()->as_string();

            // Validate the output schema using inference.
            //
            status = catalog_add->infer_box_out(_local_catalog.get_schema_map());

            if (!status)
            {   Throw(AuroraException,
                      "The new box Out schema does not match the In schema.");
            }

            // Choke destination box as early as possible to minimize its
            // growth in queue size, and to prevent it being scheduled to run.
            //
            choked_box.push_back(dest_box);
            _aurora.choke_boxes(choked_box);

            // The size is still 1 here.
            DEBUG << "sink size=" << catalog_stream->get_sink_port()->size();

            // Find the old stream sink connection to the destination box.
            // Reset the old stream sink connection to the inserted box.
            //
            box_port = catalog_stream->find_sink_box_port(catalog_box, 0);

            if (!box_port)
            {   Throw(AuroraException,
                      "Could not find the old stream sink connection to the destination box.");
            }

            box_port->set_box_port(catalog_box, 0);

            // Modify the engine to connect the new box.
            //
            _aurora.connect_box(
                _local_catalog.get_stream(Name(new_stream)), // New stream
                               catalog_box,                  // Destination box
                               catalog_add,                  // insert box
                               catalog_stream,               // Existing stream.
                               port);                        // Box In port.
 
            DEBUG << "sink size=" << catalog_stream->get_sink_port()->size();

            // Resume the destination box.
            //
            _aurora.resume_boxes(choked_box);

            NOTICE << "boxes resumed";

            DEBUG << "Finished inserting a box.";

            reflect_region_update(update_xml);
            completion.post(true);                // The local connect worked.
        }
        else
        {   ////////////////////////////////////////////////////////////////
            //
            // This block performs the local portion of an upstream connect
            // on the local (This) node containing the destination box.
            //
            // Phase I on the local node:
            //     Add the new stream locally and subscribe to it.
            //     Do first; when remote box resumes it catches the new data.
            //
            //     Assert no multiple subs to the old stream.
            //     RPC the new box to the remote node.
            //
            // On the remote node:
            //    Stop the source box.
            //    Add the filter box (it starts) and connect the streams.
            //    Unpublish the old stream to the node.
            //    Publish the new stream
            //    resume the source box (data flows to the new subscription).
            //
            // Phase II on the local node:
            //    Connect the In on the destination box???
            //    Reflect the connect to the region
            //
            // Do we remove the local copy of the old stream???
            //..............................................................


            index = update_xml.find(">", 0);   // At the end of the opening connect.

            if (index == string::npos)
            {   completion.post(RPCFault("Invalid connect:  " + update_xml));
            }
            else
            {   // Replace the top line in the connext XML with:
                //     <connect  node={data path for this node}>
                //
                connect_xml = "<connect  node=\"" + to_string(_data_add) + "\" >"
                            + update_xml.substr(index + 1, update_xml.length());

                DEBUG << "connect_xml=" << connect_xml;
 
                // Check for multiple subs to the old stream.
                // If multiple then the box shouldn't go upstream.
                //
                if (catalog_stream->get_sink_port()->size() != 1)
                {   Throw(AuroraException,
                          "Can't put a box upstream when other consumers.");
                }

                ///////////////////////////////////////////////////////////////
                // Add the new stream locally and subscribe to it.
                // First so when the remote box resumes it catches the new data.
                //
                DEBUG << "Open a general purpose input data path for stream "
                      << new_stream;

                _data_path.add_input_path(new_stream);

                // Catalog the new stream and add it to the engine's structures.
                // The new stream schema is the same as the old.
                //
                vector<CatalogStream*> streams;

                streams.push_back(_local_catalog.add_stream(
                                      Name(new_stream),
                                      Name(catalog_stream->get_schema_name())));

                DEBUG << "new stream schema="
                      << _local_catalog.get_stream(Name(new_stream))
                             ->get_stream_schema()->as_string();

                _aurora.add_stream(streams);
                //
                //....................................................


                // Determine the upstream node using the old stream's subscription.
                //
                node = _local_catalog.get_subscription(old_stream)->get_subscription_endpoint();
                DEBUG << "node=" << node << "  old_stream=" << old_stream;

                Remote<QueryProcessor> qp = qp_for(MedusaID(to_string(node)));

                if (!qp)
                {   Throw(AuroraException,
                          "Failed to get QP for " + to_string(node));
                }

                //  The remote qp adds the box, create the new stream
                //      and pub1lishes it (see the block below).
                //  Then connect_element2 is called on this node to connect
                //      the new stream to the target box.
                //
                modify_xml  = "<modify  box=\"" + dest_box + "\" >\n";

                modify_xml += " <in  stream=\"" + new_stream
                                + "\"  port=\"" + port + "\" />\n";

                modify_xml += "</modify>";
                DEBUG  << modify_xml;

                // Reflect the upstream portion of the connect.
                //reflect_region_update(update_xml);  // Too early for this???

                qp.add_xml_string(wrap(this,
                                       &QueryProcessor::connect_element2,
                                       completion, modify_xml),
                                  connect_xml);
            }
        }
    }
    else
    {   // This block performs the remote portion of the connect on the
        // upstream node.
        //
        //    Stop the source box.
        //    Add the filter box (it starts) and connect the streams.
        //    If unpublishing; unpublish the old stream to the node.
        //    Publish the new stream
        //    resume the source box (data flows to the new subscription).
        //
        node = xml_attribute(connect, "node", ATTR_REQUIRED);
        DEBUG << "node=" << node << "  old_stream=" << old_stream;

        catalog_stream = _local_catalog.get_stream(old_stream);
        catalog_box = catalog_stream->get_stream_source_box();
        DEBUG << "source box=" << catalog_box->get_box_name();

        // Choke source??? box as early as possible to minimize its
        // growth in queue size, and to prevent it being scheduled to run.
        //
        choked_box.push_back(to_string(catalog_box->get_box_name()));
        _aurora.choke_boxes(choked_box);

        // Catalog the box being inserted; adds new stream with a null schema.
        //
        add_box_element(*box.begin());

        // The new stream schema is the same as the old.
        //
        _local_catalog.get_stream(Name(new_stream))
             ->set_stream_schema(catalog_stream->get_stream_schema());

        DEBUG << "new stream schema="
              << _local_catalog.get_stream(Name(new_stream))
                     ->get_stream_schema()->as_string();


        // Validate the output schema using inference.
        //
        catalog_add = _local_catalog.get_box(Name(add_box));

        DEBUG << "added box=" << catalog_add->get_box_name();
        DEBUG <<    "  type=" << catalog_add->get_box_type();

        status = catalog_add->infer_box_out(_local_catalog.get_schema_map());

        if (!status)
        {   Throw(AuroraException,
                  "The new box Out schema does not match the In schema.");
        }

        // Add the new stream subscription to the catalog.
        //
        CatalogSubscription  subscriber;
        DEBUG << "new stream=" << new_stream << " node=" << node;

        subscriber = _local_catalog.add_subscribe(new_stream,
                                                  to_string(node), 0);

        _local_catalog.get_stream(Name(new_stream))->
            subscribe_sink(_local_catalog.get_subscriber(node,
                                                         Name(new_stream)));
        vector<CatalogSubscription>  subs;
        subs.push_back(subscriber);

        // Publish new_stream (unless already published).
        //
        DEBUG << "publish  new_stream=" << new_stream;

        local_subscribe_streams(completion, subs);

        // Unpublish old_stream if only 1.
        // If only instance
        //     Remove old stream local_unsubscribe_stream
        //
        DEBUG << "NYI:  un-publish old_stream=" << old_stream << " @ " << node;
        CatalogSubscription  *suber;

        suber = _local_catalog.get_subscriber(node,
                                              Name(old_stream));

        if (!suber)
        {   DEBUG << "Could not find old publication.";
        }
        else
        {   //  This requires the data port.
            local_unsubscribe_stream(completion, *suber);
        }

        // subscriptions - downstream  subscribe - upstream
        //                             subscriber - actual upstream copy

        // resume the source box.
        //
        _aurora.resume_boxes(choked_box);

        NOTICE << "boxes resumed";

        // Reflect the modified connect (downstream node; no target box, port).
        reflect_region_update(update_xml);
        completion.post(true);    // The remote portion worked???
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// <<< work in progress >>>
//  Subscribe to the new stream and modify the target box in for the new stream.
//  The old stream may not be published and needs removal???
//  reflect update_xml to the region (done via modify ond remote connect???)
//
void  QueryProcessor::connect_element2(AsyncRPC<void>  completion,

                                       // <modify  box={target box name} >
                                       //  <in stream={new stream} port={port index} />
                                       // </modify>
                                       //
                                       string          modify_xml,

                                       RPC<void>       result)
{
    uint16    port;
    Name      old_stream;
    string    new_stream;
    string    box;
    string    value;

    const  DOMElement    *modify;
    vector<DOMElement *>  in;
    CatalogSubscription   subscriber;
    CatalogBox           *catalog_box;           // Destination box.
    CatalogStream        *catalog_stream;
    InetAddress           node;
    vector<string>        choked_box;

    vector<CatalogSubscription>  subs;
//..............................................................................


    DEBUG << "modify_xml=" << modify_xml << "  result.stat=" << result.stat();

    if (result.stat())
    {   Status  status = _local_catalog.parse_element(modify_xml);

        if (status)
        {   modify = _local_catalog.get_element();

            try
            {   xml_expect_tag(modify, "modify");
                box = xml_attribute(modify, "box", ATTR_REQUIRED);
 
                xml_child_elements(in, modify, "in");

                value = xml_attribute(*in.begin(), "port",   ATTR_REQUIRED);
                port  = uint16(atoi(value.c_str()));

                DEBUG << "modify box=" << box << "  port=" << port;
                new_stream = xml_attribute(*in.begin(), "stream", ATTR_REQUIRED);
                
                // Determine the source node using the old stream's subscription.
                //
                old_stream = _local_catalog.get_box(Name(box))
                                 ->get_in_stream(port)->get_stream_name();

                // This is the control endpoint for the remote upstream node.
                //
                node = _local_catalog.get_subscription(old_stream)
                           ->get_subscription_endpoint();

                DEBUG << "node=" << node << "  old_stream=" << old_stream;

                // Choke destination box as early as possible to minimize its
                // growth in queue size, and to prevent it being scheduled to run.
                //
                choked_box.push_back(box);
                _aurora.choke_boxes(choked_box);
 
                // Modify the In stream on the destination box in the local catalog.
                //
                catalog_stream = _local_catalog.get_stream(Name(new_stream));
                catalog_box    = _local_catalog.get_box(Name(box));
                //catalog_box->reset_in_stream(port, catalog_stream); done by _aurora.update_box_in

                // Modify the destination box In to connect the new stream.
                //
                DEBUG << "Modify box in to stream=" << new_stream;
                _aurora.update_box_in(catalog_box, catalog_stream, port);
                DEBUG << "... Modified port=" << port;

                //  Subscribe to the new stream.
                //
                vector<CatalogSubscription>  subscription;
                // this uses the control endpoint???

                InetAddress  endpoint(node);
                CatalogSubscription sub(endpoint, new_stream);
                subscription.push_back(sub);

                _local_catalog.add_subscription(sub);

                CatalogSubscription *catalog_sub;
                catalog_sub = _local_catalog.get_subscription(endpoint,
                                                              new_stream);

                catalog_stream->subscribe_source(catalog_sub);

                // Magda added: For each new upstream neighbor, ask for its replica_set
                request_replicas(endpoint, new_stream);

                DEBUG << "Opening an input path for stream "
                      << new_stream << " from " << endpoint;

                // Moving data path code out of the legacy code
                // Every time we subscribe to an input stream, we need to open an input path
                //
                _data_path.add_input_path(new_stream, endpoint);
                DEBUG  << "... back from local_add_subscriptions.";

                // Remove the subscription from the old stream.
                //
                vector<CatalogSubscription  *>   unsubscribe_streams;
                CatalogSubscription  *suber;

                AsyncRPC<void>   remove_subs_completion;

                // Needs the data port???
                suber = _local_catalog.get_subscription(node, old_stream);

                if (!suber)
                {   DEBUG << "Could not find subscriber:  "
                          << old_stream << " @ " << node;
                }
                else
                {   _local_catalog.get_stream(Name(old_stream))
                                       ->unsubscribe_source();

                    _local_catalog.remove_subscription(suber);

                   // Moving code for data path outside of the legacy code
                   // No longer need to send data to this client on this data path
                   DEBUG << "Removing input path for stream " << old_stream;
                   _data_path.remove_input_path(old_stream);
                }

                DEBUG << "Removed stale subscriptions.";

                // Resume the destination box.
                //  
                if (choked_box.empty())
                {   FATAL << "choked_box.empty()";
                }

                _aurora.resume_boxes(choked_box);

                NOTICE << "boxes resumed";

                //The upstream reflection should occur here.
                // Need:  The filter box is not local??? pass in connect???
                //        old_stream - got it.
                //        destination box and port - okay.

                //DEBUG << "target box=" << box << "  in port=" << port;

                // Downstream node reflects the modify.
                reflect_region_update(modify_xml);

                //  The old stream may not be published and needs removal???
                //  reflect update_xml to the region (done via modify ond remote con
                //  The modify does a reflect???
                //
                completion.post(true);
            }
            catch (AuroraException  e)
            {   DEBUG << "The local connect modify box operation failed: "
                      << e.getMessage();

                completion.post(RPCFault(e.getMessage()));
            }
        }
        else
        {   DEBUG << "The connect modify element could not be parsed: "
                  << result.stat();

            completion.post(RPCFault(result.stat()));
        }
    }
    else
    {   DEBUG << "The remote connect operation failed: "
              << result.stat();

        completion.post(RPCFault(result.stat()));
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Disconnect an inserted box.
//
void QueryProcessor::disconnect_element(const DOMElement  *disconnect,
                                        AsyncRPC<void>    &completion)
                                  throw(AuroraException)
{
    string          stream;
    string          source_box;
    string          upstream;
    string          modify_xml;
    string          disconnect_xml;
    InetAddress     node;

    CatalogStream  *catalog_stream;
    CatalogStream  *old_stream;
    CatalogBox     *box;
    BoxPort        *box_port;

    CatalogSubscription  subscriber;

    vector<Name>    list;
    vector<string>  choked_box;
//
//  It needs to be modified to use the output stream:
//      <disconnect  stream={new stream}  [upstream={old stream}]/>
//
//  Entry:   --old--> [Filter] --new--> [Sink Box]
//
//   Exit:   --old--> [Sink Box]
//
//  Parse errors throw an exception; semantic errors post an invalid completion.
//..............................................................................


    xml_expect_tag(disconnect, "disconnect");
    stream = xml_attribute(disconnect, "stream", ATTR_REQUIRED);

    DEBUG << "disconnect added stream=" << stream;
    catalog_stream = _local_catalog.get_stream(stream);

    if (!catalog_stream)
    {   Throw(AuroraException,
              "Can not find added stream (" + stream + ") to disconnect.");
    }

    // Get the Filter box if it was inserted locally.
    //
    box = catalog_stream->get_stream_source()->get_port_box();

    if (box == NULL)  // catalog_stream->is_subscribed_stream()
    {
        // This block handles the initial remote disconnect request sent to the
        // downstream node.
        //
        node = catalog_stream->get_source_subscription()->get_subscription_endpoint();
        upstream = xml_attribute(disconnect, "upstream", ATTR_REQUIRED);
        DEBUG << "node=" << node << "  upstream=" << upstream;


        ///////////////////////////////////////////////////////////////
        // Restore the old stream locally and subscribe to it.
        // First so when the remote box resumes it catches the new data.
        //
        DEBUG << "Open a general purpose input data path for stream "
              << upstream;

        _data_path.add_input_path(upstream);

        // Catalog the original stream and add it to the engine's structures.
        // The original stream schema is the same as the new one in place.
        //
        vector<CatalogStream*> streams;

        //  Stream ( source ) is already cataloged.  remove on connect???
        //streams.push_back(_local_catalog.add_stream(
        //                              Name(upstream),
        //                              Name(catalog_stream->get_schema_name())));
        streams.push_back(_local_catalog.get_stream(Name(upstream)));

        DEBUG << "original stream schema="
              << _local_catalog.get_stream(Name(upstream))
                                    ->get_stream_schema()->as_string();

        _aurora.add_stream(streams);
        //....................................................


        Remote<QueryProcessor> qp = qp_for(MedusaID(to_string(node)));

        if (!qp)
        {   Throw(AuroraException,
                  "Failed to get QP for " + to_string(node));
        }

        // Forward the disconnect to the upstream node.
        //
        disconnect_xml = "<disconnect  stream=\"" + stream + "\" />";

        //  The remote qp removes the box and the new stream
        //      and publishes the old stream (see the block below).
        //  Then connect_element2 is called on this node to connect
        //      the old stream to the target box.
        //
        box_port    = catalog_stream->get_sink_box_port(0);
        box         = box_port->get_port_box();

        modify_xml  = "<modify  box=\"" + to_string(box->get_box_name()) + "\" >\n";

        modify_xml += " <in  stream=\"" + upstream
                        + "\"  port=\"" + box_port->get_port() + "\" />\n";

        modify_xml += "</modify>";
        DEBUG  << modify_xml;

        qp.add_xml_string(wrap(this,
                               &QueryProcessor::connect_element2,
                               completion, modify_xml),
                          disconnect_xml);
    }
    else if (catalog_stream->is_output_stream())
    {   // This block runs on the upstream node and receives the disconnect
        // forwarded from the downstream node.

        ////////////////////////////////////////////////////////////////////
        // based on connect:
        //    Stop the filter on the upstream box???
        //    remove filter from catalog; also added stream
        //    add original stream to catalog
        //    Publish original stream (unless already published).
        //    unpublish added stream
        //    resume the source box.
        //..................................................................


        // Determine the old stream from the Filter box's In port 0.
        //
        old_stream = box->get_in_stream(0);

        DEBUG << "disconnect upstream box=" << box->get_box_name()
              << " old stream=" << old_stream->get_stream_name();

        // Stop the filter or the upstream box.
        //
        source_box = to_string(old_stream->get_stream_source_box()->get_box_name());
        choked_box.push_back(source_box);
        _aurora.choke_boxes(choked_box);

        // Determine the downstream node from the added stream's subscription.
        //
        vector<CatalogSubscription*> *subscribers = catalog_stream->get_subscribers();
        node = (*subscribers)[0]->get_subscription_endpoint();

        DEBUG << "original stream=" << old_stream->get_stream_name()
             << " downstream node=" << node;

        // Disconnect the filter box in the engine.
        //
        DEBUG << "Attempting to disconnect boxes";
        //_aurora.disconnect_box(disconnect_boxes);  // For local only.
        vector<CatalogBox*> disconnect_boxes;
        disconnect_boxes.push_back(box);
        _aurora.remove_box(disconnect_boxes);
        DEBUG << "Finished disconnecting boxes";

        // Add the original stream subscription to the catalog.
        //
        subscriber = _local_catalog.add_subscribe(
                                       to_string(old_stream->get_stream_name()),
                                       to_string(node), 0);

        _local_catalog.get_stream(old_stream->get_stream_name())->
            subscribe_sink(_local_catalog.get_subscriber(node,
                                                         old_stream->get_stream_name()));

        // Publish original stream (unless already published).
        //
        vector<CatalogSubscription>  subs;
        subs.push_back(subscriber);

        DEBUG << "publish  original stream=" << old_stream->get_stream_name();

        local_subscribe_streams(completion, subs);

        // Unpublish added stream if only 1.
        // If only instance
        //     Remove old stream local_unsubscribe_stream
        //
        DEBUG << "un-publish added stream=" << stream << " @ " << node;
        CatalogSubscription  *suber;

        suber = _local_catalog.get_subscriber(node, Name(stream));

        if (!suber)
        {   DEBUG << "Could not find added publication.";
        }
        else
        {   //  This requires the data port.
            local_unsubscribe_stream(completion, *suber);
        }

        //  Remove filter from catalog; also added Out stream.
        _local_catalog.remove_filter(box->get_box_name());

        // Resume the source box.
        //  
        if (choked_box.empty())
        {   FATAL << "choked_box.empty()";
        }

        _aurora.resume_boxes(choked_box);

        NOTICE << "boxes resumed";

        // The reflection removes the inserted filter box and added stream.
        // Both nodes needs to reflect it because the downstream
        // node may not be included in the region.
        //
        disconnect_xml = "<disconnect  stream=\"" + stream
                              + "\"  upstream=\"" + old_stream + "\" />";

        reflect_region_update(disconnect_xml);


        // Control is passed back to the downstream node at connect_element2.
        //
        completion.post(true);
    }
    else
    {   // This block performs a simple local disconnect.
        // The filter box was placed on the same node as the destination box.
        //
        list.push_back(box->get_box_name());

        DEBUG << "disconnect filter box=" << box->get_box_name() << "  stream=" << stream;
        local_disconnect_box(completion, list);
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Include optional Borealis components.
//
void  QueryProcessor::component_element(const DOMElement  *component)
                                  throw(AuroraBadEntityException)
{
    string     name;
    string     message;
//
//    <component    name={"base", "shedder", "revision", "availability"} />
//
//  Parse errors throw AuroraBadEntityException.
//..............................................................................


    xml_expect_tag(component, "component");

    name = xml_attribute(component, "name",  ATTR_REQUIRED);
    lowercase(name);

    if (name == "base")
    {   reduce_tuple_header_size(21);
    }
    else if (name == "shedder")
    {   reduce_tuple_header_size(33);
    }
    else if (name == "revision")
    {   reduce_tuple_header_size(45);
    }
    else if (name == "availability")
    {   reduce_tuple_header_size(53);
    }
    else
    {   message = "Unrecognized component:  " + name;
        DEBUG << message;

        Throw(AuroraBadEntityException, message);
    }

    return;
}




////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
RPC<void> QueryProcessor::notify_takeover()
{
    /********************************** yna create_query
    // Use the catalog instead.
    string xml;
    set<ptr<QBox> > boxes = _aurora.get_boxes();
    string node = _id.as_string();

    for (set<ptr<QBox> >::iterator i = boxes.begin(); i != boxes.end(); i++)
    {
        ptr<QBox> b = *i;
        string box_name = b->get_name();
        xml = "<move box=\"" + box_name + "\" node=\"" + node + "\"/>";
        reflect_region_update(xml);
    }
    ***********************************/
    return true;
}


////////////////////////////////////////////////////////////////////////////////
//
//  Reflect code to regional components covering this node.
//
void   QueryProcessor::reflect_region_update(string   update_xml)
{
    vector<UpdateClient *>::iterator  client;
//..............................................................................


    for (client  = _region_client.begin();
         client != _region_client.end(); ++client)
    {
        DEBUG  << "Reflect to region:  " << update_xml;
        reflect_update_xml(update_xml, *client);
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Reflect an update element to a region and to the head.
//
void   QueryProcessor::reflect_global_update(string   update_xml)
{
    RPC<void>  rpc;
//..............................................................................


    reflect_region_update(update_xml);

    if (_head_client)
    {   //DEBUG << _head_client->get_endpoint();
        rpc = _head_client->add_xml_string(update_xml);
        DEBUG << "reflect to head:  add_xml_string( " << update_xml << " ): " << rpc;

        if (!rpc.stat())
        {   WARN << "add_xml_string failed.  status = " << rpc;
        }
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Reflect an update to single regional compnent or the head.
//
void   QueryProcessor::reflect_update_xml(string         update_xml,
                                          UpdateClient  *Client)
{
    RPC<void>  rpc;
//..............................................................................


    rpc = Client->add_xml_string(update_xml);
    DEBUG << "reflect:  add_xml_string( " << update_xml << " ): " << rpc;

    if (!rpc.stat())
    {   WARN << "add_xml_string failed.  status = " << rpc;
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Subscription management.
//
//..............................................................................


////////////////////////////////////////////////////////////////////////////////
//
// Add subscriptions
//
// TODO: once the engine uses Catalog metadata for subscriptions, these should
// execute in a critical section.
//
void QueryProcessor::local_add_subscriptions(
                                    vector<CatalogSubscription>  &subscriptions,
                                   Callback<void, RPC<void> >    cb)
                                       throw(AuroraException)
{
    CatalogSubscription  *catalog_sub;
    CatalogStream        *stream;
    Name                  stream_name;
    InetAddress           endpoint;

    map<InetAddress, vector<CatalogSubscription> >            node_sub;
    map<InetAddress, vector<CatalogSubscription> >::iterator  node_sub_it;
                       vector<CatalogSubscription>::iterator  sub;
//..............................................................................


    DEBUG << "Locally subscribe to " << subscriptions.size() << " inputs.";

    for (sub = subscriptions.begin(); sub != subscriptions.end(); sub++)
    {
        stream = _local_catalog.get_stream(sub->get_subscription_stream_name());

        if (stream)
        {   // Mark those subscriptions newly added.
            // Adding existing subscriptions raises an exception.
            //
            try
            {   stream_name = sub->get_subscription_stream_name();
                endpoint    = sub->get_subscription_endpoint();

                DEBUG << "Adding subscription to stream "
                      << stream_name << "@" << endpoint;

                _local_catalog.add_subscription(*sub);

                catalog_sub = _local_catalog.get_subscription(endpoint,
                                                              stream_name);

                stream->subscribe_source(catalog_sub);

                node_sub[endpoint].push_back(CatalogSubscription(_data_add,
                                                                 stream_name));

                // Magda added: For each new upstream neighbor, ask for its replica_set
                request_replicas(endpoint,stream_name);

                DEBUG << "Opening an input path for stream "
                      << stream_name << " from " << endpoint;

                // Moving data path code out of the legacy code
                // Every time we subscribe to an input stream, we need to open an input path
                //
                _data_path.add_input_path(stream_name, endpoint);
            }
            catch (AuroraBadEntityException& abe)
            {   DEBUG << "Subscription to stream " << stream->get_stream_name()
                      << " already exists in the catalog.";
            }
        }
    }

    DEBUG << "Actually subscribing to " << node_sub.size() << " input streams.";

    // Iterate over recorded subscriptions; create output paths at remote sites.
    //
    for (node_sub_it  = node_sub.begin();
         node_sub_it != node_sub.end(); node_sub_it++)
    {
        Remote<QueryProcessor> qp =
            qp_for(MedusaID(to_string(node_sub_it->first)));

        if (!qp)
        {   Throw(AuroraException,
                  "Failed to get QueryProcessor for "
                     + to_string(node_sub_it->first));
        }

        DEBUG << "Call remote subscribe_streams for " << node_sub_it->first;

        qp.subscribe_streams(wrap(this,
                                 &QueryProcessor::remote_subscribe_check,
                                  node_sub_it->first),
                             node_sub_it->second);
    }

    if (cb) cb(true);
}



////////////////////////////////////////////////////////////////////////////////
//
AsyncRPC<void> QueryProcessor::add_subscriptions(
                                       vector<CatalogSubscription> subs_to_add)
{
    AsyncRPC<void> completion;
//..............................................................................


    try
    {   local_add_subscriptions(subs_to_add, completion.propagate());
        completion.post(true);            // Not in original???
    }
    catch (AuroraException& ae)
    {   completion.post(RPCFault(ae.getMessage()));
    }

    return(completion);
}



////////////////////////////////////////////////////////////////////////////////
//
void QueryProcessor::remote_subscribe_check(InetAddress source, RPC<void> result)
{
//..............................................................................


    if (!result.stat())
    {   WARN << "Failed to subscribe to streams from " << source;
    }
}



////////////////////////////////////////////////////////////////////////////////
//
// Remove subscriptions
//
void QueryProcessor::local_remove_subscriptions(AsyncRPC<void> completion,
                                                vector<CatalogSubscription*>& subs)
{
    vector<CatalogSubscription*>::iterator unsubscribe_it = subs.begin();
    vector<CatalogSubscription*>::iterator unsubscribe_end = subs.end();
//..............................................................................


    // Unsubscribe from remote inputs.
    //
    for (; unsubscribe_it != unsubscribe_end; ++unsubscribe_it)
    {
        CatalogSubscription *sub = *unsubscribe_it;
        Name stream_name = sub->get_subscription_stream_name();

        DEBUG << "Removing subscription to " << stream_name.as_string()
              << "@" << sub->get_subscription_endpoint().as_string();

        Remote<QueryProcessor> sub_qp =
            qp_for(MedusaID(to_string(sub->get_subscription_endpoint())));

        if (!sub_qp)
        {   completion.post(RPCFault("Failed to get QP for " +
                                     to_string(sub->get_subscription_endpoint())));

            return;
        }

        CatalogSubscription rsub(_data_add, stream_name);

        sub_qp.unsubscribe_stream
            (wrap(this, &QueryProcessor::remote_unsubscribe_check, rsub), rsub);

        CatalogStream* cstream = sub->get_subscription_stream();

        if (!cstream)
        {   cstream = _local_catalog.get_stream(stream_name);

            if (!(cstream->get_source_subscription() &&
                  cstream->get_source_subscription() == sub))
            {
                cstream = NULL;
            }
        }

        if (cstream)
        {   cstream->unsubscribe_source();
        }

        _local_catalog.remove_subscription(sub);


        // Moving code for data path outside of the legacy code
        // No longer need to send data to this client on this data path
        DEBUG << "Removing input path for stream " << stream_name;
        _data_path.remove_input_path(stream_name);
    }

    completion.post(true);
}



////////////////////////////////////////////////////////////////////////////////
//
AsyncRPC<void> QueryProcessor::remove_subscriptions(
                                   vector<CatalogSubscription> subs_to_delete)
{
    AsyncRPC<void> completion;
    vector<CatalogSubscription>::iterator deletion_it = subs_to_delete.begin();
    vector<CatalogSubscription>::iterator deletion_end = subs_to_delete.end();
    vector<CatalogSubscription*> existing_subs;
//..............................................................................


    for (; deletion_it != deletion_end; ++deletion_it)
    {
        CatalogSubscription* csub =
            _local_catalog.get_subscription(
                deletion_it->get_subscription_endpoint(),
                deletion_it->get_subscription_stream_name());

        if (csub)
        {   existing_subs.push_back(csub);
        }
        else
        {   return RPCFault("Subscription to " +
                            deletion_it->get_subscription_stream_name().as_string() +
                            " at " + deletion_it->get_subscription_endpoint().as_string() +
                            " not found");
        }
    }

    local_remove_subscriptions(completion, existing_subs);
    return completion;
}



////////////////////////////////////////////////////////////////////////////////
//
void QueryProcessor::remote_unsubscribe_check(CatalogSubscription sub, RPC<void> result)
{
//..............................................................................


    if (!result.valid())
    {
        ERROR << "Failed to unsubscribe from stream "
              << sub.get_subscription_stream_name()
              << " at " << sub.get_subscription_endpoint()
              << " , failure is " << result.stat() ;
    }
}



////////////////////////////////////////////////////////////////////////////////
//
// Subscriber management.
//

////////////////////////////////////////////////////////////////////////////////
//
// Adds a subscriber to a local stream
//
void QueryProcessor::local_subscribe_streams(AsyncRPC<void>  &completion,
                                vector<CatalogSubscription>  &subs)
{
    Name         name;
    InetAddress  endpoint;
    Status       status = true;
    Boolean      forward_subs = false;

    vector<CatalogSubscription>::iterator sub_it  = subs.begin();
    vector<CatalogSubscription>::iterator sub_end = subs.end();
//..............................................................................


    for (; sub_it != sub_end; ++sub_it)
    {   
        name     = sub_it->get_subscription_stream_name();
        endpoint = sub_it->get_subscription_endpoint();

        DEBUG << "Valid history on subscription is " << sub_it->is_valid_last_insertion_tuple();

        if (!_local_catalog.get_stream(name))
        {   
            DEBUG << "Defer subscription of " << name << "@" << endpoint;

            // Deferred until the producer box is added.
            //
            _defer_subscribe[name].push_back(endpoint);
        }
        else
        {   
            // Magda: MUST add exactly the subscription we receive.
            // Otherwise, all parameters of the subscription are lost!!!! There's info about history there!
            status = local_subscribe_stream(*sub_it, &forward_subs);
            //status = local_subscribe_stream(name, endpoint, &forward_subs);
            // End Magda

            if (!status)
            {   break;
            }
        }
    }

    if (status)
    {   if (forward_subs)
        {   // If the node has replicas, they must open the same output paths
            //
            forward_subs_to_replicas(subs);
        }

        completion.post(true);
    }
    else
    {   completion.post(RPCFault(status));
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Add a subscription for a single stream.
//
Status  QueryProcessor::local_subscribe_stream(
                                               CatalogSubscription subscriber,
                                               //Name         name,
                                               //InetAddress  endpoint,
                                               Boolean     *forward)
{
    Status                status = true;
    CatalogSubscription  *subscription;
    CatalogStream        *stream;

    // Magda: extract some values from CatalogSubscription object for convenience.
    Name name = subscriber.get_subscription_stream_name();
    InetAddress endpoint = subscriber.get_subscription_endpoint();

//..............................................................................


    DEBUG << "Add subscriber for " << name << "@" << endpoint;

    DEBUG << "sink flag = " << subscriber.get_sink_flag(); // -- tatbul

    try
    {   // Only need to do all this for new output paths
        //
        if (!_data_path.has_output_path(name))
        {
            // Open a new data path on the desired stream.
            //
            _data_path.add_output_path(name);

            // Start to monitor the new output path
            //
            _local_catalog.monitor_local_stream(name);

            // Update the state advertised by the Availability monitor to
            // include the new stream.
            //
            _avail_mon.set_state(wrap(this,
                                     &QueryProcessor::avail_monitor_response,
                                      string("update_state")),
                                 _local_catalog.get_local_stream_state_map());

            // Only replica that gets the first subscriber for a new output
            // stream forwards to others. We do this outside of the loop.
            //
            if (endpoint)
            {   *forward = true;
            }
        }

        // If the endpoint is valid, add a client to the output path
        // Otherwise, we were only supposed to create a new output path
        //
        if (endpoint)
        {   DEBUG << "There is an endpoint, must add to catalog";
            subscription = _local_catalog.get_subscriber(endpoint, name);

            if (!subscription)        
            {   // Magda: don't *create* a new CatalogSubscription object, used the one we got from downsteam node.
                //CatalogSubscription  subscriber(endpoint, name);
                //
                _local_catalog.add_subscriber(subscriber);
                subscription = _local_catalog.get_subscriber(endpoint, name);
                stream       = _local_catalog.get_stream(name);
                stream->subscribe_sink(subscription);
                DEBUG << "Actually added client";
            }
            else
            {   DEBUG << "Setting the sink flag..";
                subscription->set_sink_flag(); // -- tatbul

                DEBUG << "Client is already in catalog";
            }

            // Add a new downstream client.
            //
            DEBUG << "Adding client for data path " << name
                                      << " client " << endpoint;

            _data_path.add_output_path_client(*subscription);
        }
    }
    catch (AuroraBadEntityException& abe)
    {   status = abe.getMessage();
        DEBUG << status;
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
// Remote site wants to subscribe to our streams
//
AsyncRPC<void> QueryProcessor::subscribe_streams(
                                   vector<CatalogSubscription>  subs)
{
    AsyncRPC<void>  completion;
//..............................................................................


    local_subscribe_streams(completion, subs);

    return(completion);
}



////////////////////////////////////////////////////////////////////////////////
//
// Remove subscribers for our local streams.
//
void QueryProcessor::local_unsubscribe_stream(AsyncRPC<void> completion,
                                              CatalogSubscription& sub)
{
// TODO:
// -- remove statistics collection
// -- remove subscriber from stream's remote sinks (need to block engine).
//..............................................................................


    DEBUG << "Removing subscriber for "
          << sub.get_subscription_stream_name().as_string()
          << "@" << sub.get_subscription_endpoint().as_string();

    CatalogSubscription* csub =
        _local_catalog.get_subscriber(sub.get_subscription_endpoint(),
                                      sub.get_subscription_stream_name());

    if ( ! csub )
    {
        RPCFault error("Did not find the subscription to remove " + to_string(sub));
        completion.post(error);
        return;
    }

    CatalogStream *cstream = csub->get_subscription_stream();

    cstream->unsubscribe_sink(csub);

    if (!_local_catalog.remove_subscriber(csub))
    {   DEBUG << "missing subscriber.";
    }

    // No longer need to send data to this client on this data path
    _data_path.remove_output_path_client( sub );


    // Magda: NO. Don't garbage collect or we loose our output queues.
    // This should never be needed here.
    // Garbage collect output paths otherwise we fail to subscribe later.
    //if ( cstream->get_subscribers()->size() == 0 )
    //{
    //  //remove_output_path_from_legacy_catalog(sub.get_subscription_stream_name());
    //  DEBUG << "No more subscribers, so cleaning output path " << sub.get_subscription_stream_name();
    //  _data_path.remove_output_path(sub.get_subscription_stream_name());
    //}

    completion.post(true);
}



////////////////////////////////////////////////////////////////////////////////
//
// Remote site wants to unsubscribe from a local stream
//
AsyncRPC<void> QueryProcessor::unsubscribe_stream(CatalogSubscription sub)
{
    AsyncRPC<void> completion;
//..............................................................................


    local_unsubscribe_stream(completion, sub);

    return completion;
}



////////////////////////////////////////////////////////////////////////////////
//
// Methods for replication-based fault-tolerance
// Monitor the consistency of remote input streams and local streams
// Changing input subscriptions as needed
//
RPC< vector<string> >  QueryProcessor::get_replica_set()
{
//..............................................................................


    return (*_local_catalog.get_replica_set());
}



////////////////////////////////////////////////////////////////////////////////
//
RPC<bool>  QueryProcessor::authorize_reconciliation(MedusaID replica)
{
//..............................................................................


    return _constman.authorize_reconciliation(replica);
}



////////////////////////////////////////////////////////////////////////////////
//
void QueryProcessor::request_replicas(InetAddress node, Name stream_name)
{
//..............................................................................


    DEBUG << "Requesting replicas for " << node << " and stream " << stream_name;
    Remote<QueryProcessor> remote_qp = qp_for(MedusaID(to_string(node)));

    if (!remote_qp)
    {   WARN << "No QP for " << node;
        return;
    }

    remote_qp.get_replica_set( wrap (this, &QueryProcessor::got_upstream_replica_set, stream_name) );
}



////////////////////////////////////////////////////////////////////////////////
//
void QueryProcessor::got_upstream_replica_set(Name stream_name, RPC< vector<string> > replica_set)
{
//..............................................................................


    DEBUG << "Got list of replicas that produce stream " << stream_name;

    // If upstream neighbor has a replica_set, add that information to the new catalog
    // Ask the AvailabilityMonitor to monitor all the new nodes
    //
    if ( !replica_set.valid() )
    {
        ERROR << "Failed while asking upstream neighbor for its replica_set";
        return;
    }

    if ( (*replica_set).size() > 1 )
    {
        DEBUG << "Replica set exists, enabling monitoring ";

        vector<string>::iterator i;
        for ( i = (*replica_set).begin(); i != (*replica_set).end(); ++i )
        {

            // Add Nodes to catalog
            InetAddress node(*i);

            DEBUG << "Adding replica " << node << " for stream " << stream_name;

            // Start to monitor a remote input stream
            _local_catalog.add_remote_input_stream(stream_name,node);

            // Ask the availability monitor to monitor all these nodes
            _avail_mon.add_node(wrap(this, &QueryProcessor::avail_monitor_response,string("add_node")),
                                node);
        }

        // Start monitoring process, in case it was not already started
        _avail_mon.start(wrap(this, &QueryProcessor::avail_monitor_response,string("start")));
    }
}



////////////////////////////////////////////////////////////////////////////////
//
void QueryProcessor::forward_subs_to_replicas(vector<CatalogSubscription> subs)
{
//..............................................................................


    DEBUG << "Forwarding subscriptions to replicas.";

    // Clear subscription endpoint info
    vector<CatalogSubscription>::iterator sub_iter;

    for ( sub_iter = subs.begin(); sub_iter != subs.end(); ++sub_iter)
    {
        sub_iter->set_subscription_endpoint(InetAddress());
    }

    // Send empty subscriptions to all our replicas
    vector<string> *replicas = _local_catalog.get_replica_set();

    // No replica set, then there is nothing to do
    // was:  if ( ! replicas )
    //
    if (replicas->empty())
    {   return;
    }

    vector<string>::iterator replica_iter;

    for ( replica_iter = replicas->begin(); replica_iter != replicas->end(); ++replica_iter)
    {
        InetAddress node = InetAddress(*replica_iter);

        // Don't send it to ourselves, of course
        if ( MedusaID(to_string(node)) != _id )
        {
            Remote<QueryProcessor> remote_qp = qp_for(to_string(node));

            if (!remote_qp)
            {
                ERROR << "No QP for " << node;
                continue;
            }

            remote_qp.subscribe_streams( wrap (this, &QueryProcessor::forward_subs_check, node), subs );
        }
    }

}



////////////////////////////////////////////////////////////////////////////////
//
void  QueryProcessor::forward_subs_check(InetAddress replica, RPC<void> result)
{
//..............................................................................


    if (!result.valid())
    {   ERROR << "Failed forwarding an empty subscription to replica " << replica
              << " because " << result.stat();
    }

}



////////////////////////////////////////////////////////////////////////////////
//
void  QueryProcessor::avail_monitor_response(string command, RPC<void> result)
{
//..............................................................................


    if (!result.valid())
    {   ERROR << "Operation " << command << " on Availability Monitor failed";
    }
}



////////////////////////////////////////////////////////////////////////////////
//
// Fault-tolerance: updating the state of local streams that will be
// advertised to our downstream neighbors.
//
void  QueryProcessor::update_local_stream_states(ConsistencyState new_state)
{
//..............................................................................


    _local_catalog.update_local_stream_states(new_state);

    _avail_mon.set_state(wrap(this,
                              &QueryProcessor::avail_monitor_response,
                              string("update_state")),
                         _local_catalog.get_local_stream_state_map());

    DEBUG << "Should update the state of streams to " << new_state;

}



////////////////////////////////////////////////////////////////////////////////
//
// Fault-tolerance: Most recent state of input streams as produced by
// the replicas of all upstream neighbors.
//
void  QueryProcessor::update_input_stream_states( RPC< RStreamStatesIndex > changed_nodes)
{
    AffectedSubscriptions affected_subscriptions;

    RStreamStatesIndex::iterator node_iter;
//..............................................................................


    // For all upstream nodes that experienced a change in the state of their output streams
    //
    for ( node_iter = (*changed_nodes).begin(); node_iter != (*changed_nodes).end(); ++node_iter )
    {
        // For all the streams that we are getting from a replica of that node
        InetAddress node = node_iter->first;
        PerOutputStreamStates::iterator stream_iter;

        for ( stream_iter = node_iter->second.begin(); stream_iter != node_iter->second.end(); ++stream_iter )
        {

            // Update the state of the stream and mark the corresponding subscription as affected
            Name stream_name = stream_iter->first;
            ConsistencyState stream_state = stream_iter->second;
            bool update_performed = _local_catalog.update_input_stream_state(stream_name,node,stream_state);
            if ( update_performed )
            {
                DEBUG << "Adjusted state of input stream " << stream_name << " at " << node << " -> " << stream_state;

                // Adding affected subscription to list (could also do this below since we need to look up the info)
                CatalogSubscription* sub = _local_catalog.get_subscription(stream_name);
                InetAddress node = sub->get_subscription_endpoint();
                ConsistencyState state = _local_catalog.get_input_stream_state(stream_name,node);
                affected_subscriptions[stream_name] = pair<InetAddress,ConsistencyState>(node,state);

            }
        }
    }

    // Update subscriptions if needed
    modify_subscriptions(affected_subscriptions);
}



////////////////////////////////////////////////////////////////////////////////
//
void  QueryProcessor::modify_subscriptions( AffectedSubscriptions& affected_subscriptions)
{
//..............................................................................


    // Ask the consistency manager if we need to change a subscription
    map<Name, pair<InetAddress,ConsistencyState> >::iterator i;
    for ( i = affected_subscriptions.begin(); i != affected_subscriptions.end(); ++i )
    {
        // 1 Select the best replica to serve as our main subscription
        Name stream_name = i->first;
        PerNodeStreamStates& stream_replicas = _local_catalog.get_all_input_stream_replicas(stream_name);
        pair<InetAddress,ConsistencyState> old_sub = i->second;


        // XXX Should merge DataPath info with Catalog
        InetAddress old_corr = _data_path.get_input_path_source(stream_name,InputType::CORRECTIONS);
        if ( ! old_corr )
        { old_corr = _data_path.get_input_path_source(stream_name,InputType::CORRECTED);
        }
        ConsistencyState old_corr_state = _local_catalog.get_input_stream_state(stream_name,old_corr);
        pair<InetAddress,ConsistencyState> corr_sub(old_corr,old_corr_state);


        pair<InetAddress,ConsistencyState> new_sub = _constman.pick_best_input_replica(old_sub,corr_sub,stream_replicas);
        DEBUG << "Subscription stream " << stream_name << " old " << old_sub.first << " " << old_sub.second
              << " new " << new_sub.first << " " << new_sub.second;


        // 2 Select a background reconciliation stream. Currently we do not store info about this stream in Catalog
        InetAddress new_corr = _constman.pick_corrections_input_replica(new_sub,corr_sub,stream_replicas);
        DEBUG << "Corrections stream " << stream_name << " old " << old_corr << " new " << new_corr;

        // If anything changed, we switch all the subscriptions around
        if ((new_sub.first != old_sub.first)  ||  (new_corr != old_corr))
        {   switch_subscriptions(stream_name,
                                 old_sub.first, new_sub.first,
                                 old_corr, new_corr);
        }
    }
}



////////////////////////////////////////////////////////////////////////////////
//
void  QueryProcessor::switch_subscriptions(Name stream_name,
                                     InetAddress old_sub_endpoint,
                                     InetAddress new_sub_endpoint,
                                     InetAddress old_corr_endpoint,
                                     InetAddress new_corr_endpoint)
{
//..............................................................................


    DEBUG << "Updating subscriptions";

    // Update the main subscription
    if (  new_sub_endpoint != old_sub_endpoint )
    {

        DEBUG << " Removed sub " << old_sub_endpoint << " from catalog";

        CatalogSubscription *old_sub = _local_catalog.get_subscription(old_sub_endpoint,stream_name);

        // Unsubscribe only if we don't use the subscription as background either
        if ( old_sub_endpoint != new_corr_endpoint )
        {
            _data_path.remove_input_path_source(stream_name,old_sub_endpoint);
            remote_unsubscribe(old_sub_endpoint,*old_sub);
            DEBUG << " Actually unsubscribed " << old_sub->get_subscription_endpoint();
        }

        // Remove old subscription from catalog ( we don't store the background subscriptions in the catalog )
        _local_catalog.remove_subscription(old_sub); // This will delete the subscription
        old_sub = NULL; // Make sure to remember!

        // Add subscription to local catalog
        DEBUG << " Added sub " << new_sub_endpoint << " to catalog";
        CatalogSubscription new_sub(new_sub_endpoint, stream_name);
        _local_catalog.add_subscription(new_sub);
        _data_path.add_input_path_source(stream_name,new_sub_endpoint);

        // Actually subscribe only if we have not done so before
        if ( new_sub_endpoint != old_corr_endpoint)
        {
            // Subscribe to new stream.
            CatalogSubscription add_sub(_data_add, stream_name);

            // Add history information
            _data_path.update_input_history(add_sub);

            remote_subscribe(new_sub_endpoint, add_sub);
            DEBUG << " Actually subscribed with " << new_sub_endpoint;
        }
    }


    // Update the background subscription
    if (new_corr_endpoint != old_corr_endpoint)
    {
        CatalogSubscription corr_sub(_data_add,stream_name);

        if ((old_corr_endpoint)  &&  (old_corr_endpoint != new_sub_endpoint))
        {
            DEBUG << " Unsubscribed from background stream " << old_corr_endpoint;
            _data_path.remove_input_path_source(stream_name,old_corr_endpoint);
            remote_unsubscribe(old_corr_endpoint,corr_sub);
        }

        if (new_corr_endpoint)
        {
            DEBUG << " Added background stream " << new_corr_endpoint;
            _data_path.add_input_path_source(stream_name,new_corr_endpoint,InputType::CORRECTIONS);

            if (old_sub_endpoint != new_corr_endpoint)
            {
                // Add history information
                _data_path.update_input_history(corr_sub);

                DEBUG << " Subscribed to background stream " << new_corr_endpoint;
                remote_subscribe(new_corr_endpoint,corr_sub);
            }
        }
    }
}



////////////////////////////////////////////////////////////////////////////////
//
void  QueryProcessor::remote_unsubscribe(InetAddress old_sub_endpoint, CatalogSubscription old_sub)
{
//..............................................................................


    Name stream_name = old_sub.get_subscription_stream_name();
    ConsistencyState state = _local_catalog.get_input_stream_state(stream_name,old_sub_endpoint);

    INFO << "Unsubscribing from stream " << stream_name << " that has state " << state << " for endpoint " << old_sub_endpoint;

    // Nothing to do if node failed
    if ( state != ConsistencyState::FAILURE )
    {
        INFO << "Sending request to unsubscribe";

        // Unsubscribe from old stream
        CatalogSubscription delete_sub(_data_add,stream_name);

        Remote<QueryProcessor> old_remote_qp = qp_for(MedusaID(to_string(old_sub_endpoint)));
        if ( ! old_remote_qp )
        { ERROR << "QueryProcessor for " << old_sub_endpoint << " is gone??";
        }
        else
        { old_remote_qp.unsubscribe_stream( wrap(this, &QueryProcessor::remote_unsubscribe_check, old_sub),
                                            delete_sub);
        }

    }
}


////////////////////////////////////////////////////////////////////////////////
//
void  QueryProcessor::remote_subscribe(InetAddress new_sub_endpoint, CatalogSubscription add_sub)
{
    vector<CatalogSubscription> list_subs;
//..............................................................................


    list_subs.push_back(add_sub);
    Remote<QueryProcessor> new_remote_qp = qp_for(MedusaID(to_string(new_sub_endpoint)));

    if (!new_remote_qp)
    {   ERROR << "QueryProcessor for " << new_sub_endpoint << " is gone??";
    }
    else
    {   new_remote_qp.subscribe_streams(wrap(this,
                                             &QueryProcessor::remote_subscribe_check,
                                             new_sub_endpoint),
                                        list_subs);
    }
}



////////////////////////////////////////////////////////////////////////////////
//
// Disconnect an existing box from between two boxes.
// Assumptions:
// - Box is one-input/one-output (e.g., drop box)
//
void QueryProcessor::local_disconnect_box(AsyncRPC<void>  &completion,
                                          vector<Name>     box_names)
{
    vector<Name>::iterator        name_it  = box_names.begin();
    vector<Name>::iterator        name_end = box_names.end();

    vector<CatalogBox*>           disconnect_boxes;
    vector<CatalogBox*>           dest_boxes;
    vector<CatalogSubscription*>  dest_subs;
    vector<string>                choked_boxes;
//
//..............................................................................


    try
    {   // First verify unary box constraints on all given boxes.
        //
        for (; name_it != name_end; ++name_it)
        {
            Name &box_name = *name_it;
            CatalogBox *disconnect_box = _local_catalog.get_box(box_name);

            if (!disconnect_box)
            {
                completion.post(RPCFault("Box \"" + to_string(box_name)
                                + "\" does not exist in the local catalog."));
                return;
            }

            CatalogStream *in, *out;

            if (disconnect_box->get_box_in())
            {
                assert(disconnect_box->get_box_in()->size() == 1); // unary box
                in = disconnect_box->get_in_stream(0);
            }
            else
            {
                completion.post(RPCFault("Box \"" + to_string(box_name)
                            + "\" to be disconnected has no input streams"));
                return;
            }

            CatalogBox *dest_box = NULL;
            CatalogSubscription *dest_sub = NULL;

            if (disconnect_box->get_box_out())
            {
                // yna -- changed from assertion
                // since we don't have a good assertion handler.
                // Failures need to be more graceful....

                // unary output
                //assert(disconnect_box->getBoxOut()->size() == 1);
                if (disconnect_box->get_box_out()->size() != 1)
                {
                    completion.post(RPCFault("Box \"" + to_string(box_name)
                        + " has more than one output stream,"
                        + " could not disconnect."));
                    return;
                }


                out = disconnect_box->get_out_stream(0);

                if (out->get_sink_port()->size() == 1)
                {
                    dest_box = out->get_sink_box_port(0)->get_port_box();
                }
                else if (out->get_subscribers()->size() == 1)
                {
                    dest_sub = out->get_subscribers()->at(0);
                }
                else
                {
                    completion.post(RPCFault("Box \"" + to_string(box_name)
                        + " has incorrect number of sinks or remote sinks"));
                    return;
                }
            }
            else
            {
                completion.post(RPCFault("Box \"" + to_string(box_name) +
                                "\" to be disconnected has no output streams"));
                return;
            }

            // Record modifications to process.
            disconnect_boxes.push_back(disconnect_box);
            dest_boxes.push_back(dest_box);
            dest_subs.push_back(dest_sub);
            choked_boxes.push_back(name_it->as_string());

            DEBUG << "Going ahead and disconnecting " << *name_it;
        }

        // Disconnect stream modifications
        //
        // -- add source_box's output as input stream of dest_box
        // -- disconnect_box's output stream is deleted as part of removal
        //    from catalog.
        // yna -- now done in AuroraNode::swapBoxPorts()


        // Choke box before changing ports.
        //

        // Choking alone is not enough, leads to lost tuples.
        // Instead: start buffering the input; drain the drop; choke the drop.
        // -- tatbul
        //
        // TODO: note choking inputs unneccessary since choking a box chokes its inputs....
        // should also make drain a parameter of AuroraNode::createAddTask
        // -- yna

        DEBUG << "Choking disconnect boxes";
        _aurora.choke_boxes(choked_boxes);
        DEBUG << "Draining disconnect boxes";
        _aurora.drain_boxes(choked_boxes);
        DEBUG << "Finished draining disconnect boxes";

        vector< pair<CatalogStream*, uint16> > disconnected_box_inputs;

        for (unsigned int i = 0; i < dest_boxes.size(); ++i)
        {
            if (dest_boxes[i])
            {
                uint16 dest_port = disconnect_boxes[i]->get_out_stream(0)
                                        ->get_sink_box_port(0)->get_port();
                disconnected_box_inputs.push_back(make_pair(
                            disconnect_boxes[i]->get_in_stream(0), dest_port));
            }
            else
            {   disconnected_box_inputs.push_back(pair<CatalogStream*, uint16>(
                                                                    NULL, 0));
            }
        }

        // disconnect
        //
        DEBUG << "Attempting to disconnect boxes";
        _aurora.disconnect_box(disconnect_boxes);
        DEBUG << "Finished disconnecting boxes";

        // Disconnect port modifications
        //
        // -- swap dest_box ports
        //    + uninstall port from disconnect_box, install port from source_box
        //
        // -- remove disconnect_box ports
        //    + remove as sink of source stream
        //
        // yna -- now performed by AuroraNode::disconnectBoxes.


        // Resume dest box's input streams
        //
        choked_boxes.clear();

        for (unsigned int i = 0; i < dest_boxes.size(); ++i)
        {
            if (dest_boxes[i])
            {
                choked_boxes.push_back(
                    dest_boxes[i]->get_box_name().as_string());
            }
        }

        _aurora.resume_boxes(choked_boxes);

        NOTICE << "boxes resumed";

        // Remove the boxes.
        //
        name_it = box_names.begin();

        for ( ; name_it != name_end; ++name_it)
        {
            // Reflect update to the regional catalog.
            //
            string update_xml;

            update_xml = "<disconnect stream=\""
                       + _local_catalog.get_box(*name_it)->
                                get_out_stream(0)->get_stream_name_string()
                       + "\" />";

            //update_xml = "<disconnect box=\"" + to_string(*name_it) + "\" />";

            _local_catalog.remove_box(*name_it);

            DEBUG << "reflecting update xml:";
            DEBUG << update_xml;

            reflect_region_update(update_xml);
        }

        completion.post(true);
    }
    catch (AuroraException ae)
    {   completion.post(RPCFault(ae.getMessage()));
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Add a box.
//
// Notes:
// -- each box:
//    * adds itself as a sink of any input
//    * becomes the source of any outputs.
//
// -- if a box is fed from an existing stream, the stream's source is choked.
//    * this is not set by catalog.addBox
//
// -- boxes must be in network order (i.e. upstream->downstream) if they are connected
//
//
void QueryProcessor::local_add_boxes(AsyncRPC<void>    &completion,
                                     vector<CatalogBox> boxes,
                                     BoxPackage         packed_boxes,
                                     set<Name>          potential_subscriptions,
                                     InetAddress        src)
{
    vector<CatalogBox*>           catalog_boxes;
    vector<CatalogSubscription*>  unsubscribe_streams;
    CatalogBox                   *source_box;
    CatalogBox                   *catalog_box;
    vector<string>                choked_boxes;

    vector<CatalogBox>::iterator  box_it  = boxes.begin();
    vector<CatalogBox>::iterator  box_end = boxes.end();
//
//..............................................................................


    DEBUG << "Locally adding boxes....";

    try
    {   for (; box_it != box_end; ++box_it)
        {
            // Check if the box already exists in the catalog.
            catalog_box = _local_catalog.get_box(box_it->get_box_name());

            if ( !catalog_box )
            {   // Add the box to the local catalog.
                //
                _local_catalog.add_box(*box_it);
                catalog_box = _local_catalog.get_box(box_it->get_box_name());
            }

            if (catalog_box)
            {   CatalogStream::StreamStar *ins = catalog_box->get_box_in();
                CatalogStream::StreamStar::iterator in_it  = ins->begin();
                CatalogStream::StreamStar::iterator in_end = ins->end();

                for( ; in_it != in_end; ++in_it)
                {
                    // No need to buffer input streams because of
                    // ModifyNetworkTask's locked listener update.
                    //
                    if ( !(*in_it)->is_input_stream() )
                    {
                        source_box = (*in_it)->get_stream_source_box();

                        choked_boxes.push_back(
                                       source_box->get_box_name().as_string());
                    }
                }

                CatalogStream::StreamStar *outs = catalog_box->get_box_out();
                CatalogStream::StreamStar::iterator out_it = outs->begin();
                CatalogStream::StreamStar::iterator out_end = outs->end();

                for( ; out_it != out_end; ++out_it)
                {
                    // Remove any previous local subscriptions to new outputs.
                    //
                    if ( (*out_it)->is_subscribed_stream() )
                    {
                        DEBUG << "Stream " << (*out_it)->as_string()
                              << " remote source "
                              << ((*out_it)->is_subscribed_stream() ? "true" : "false");

                        unsubscribe_streams.push_back(
                            (*out_it)->get_source_subscription());
                    }
                }

                catalog_boxes.push_back(catalog_box);
            }
            else
            {   completion.post(RPCFault("Failed to add box to the local catalog (catalog error)."));
                return;
            }
        }

        DEBUG << "Adding boxes to the engine...";

        // Modify the engine.
        //
        _aurora.add_box(catalog_boxes, packed_boxes);

        DEBUG << "Managing subscriptions for new boxes...";

        AsyncRPC<void> remove_subs_completion;
        local_remove_subscriptions(remove_subs_completion, unsubscribe_streams);

        if (remove_subs_completion.avail() && !remove_subs_completion.stat())
        {
            DEBUG << "Failed to remove stale subscriptions.";
            return;
        }

        DEBUG << "Removed stale subscriptions.";

        if ( !potential_subscriptions.empty() )
        {
            // Subscribe to streams necessary.
            // -- for now subscriptions schedule tasks.
            // TODO: batch these subscribe tasks.
            vector<CatalogSubscription> new_subscriptions;

            set<Name>::iterator subs_name_it =
                potential_subscriptions.begin();

            set<Name>::iterator subs_name_end =
                potential_subscriptions.end();

            vector<CatalogSubscription> subscriptions_to_add;

            for (; subs_name_it != subs_name_end ; ++subs_name_it )
            {
                CatalogStream* stream = _local_catalog.get_stream(
                    *subs_name_it);

                if ( stream
                     && stream->is_input_stream()
                     && !stream->is_subscribed_stream() )
                {
                    subscriptions_to_add.push_back(CatalogSubscription(src, *subs_name_it));
                }
            }

            local_add_subscriptions(subscriptions_to_add, completion.propagate());
            //completion.post(true);  //not in original???
        }
        else
        {   completion.post(true);
        }
    }
    catch (AuroraException& ae)
    {   completion.post(RPCFault(ae.getMessage()));
    }

    return;
}


////////////////////////////////////////////////////////////////////////////////
//
// add_boxes RPC interface
//
AsyncRPC<void> QueryProcessor::add_boxes(vector<CatalogBox> boxes,
                                         BoxPackage packed_boxes,
                                         set<Name> potential_subscriptions,
                                         InetAddress prev_node)
{
    AsyncRPC<void> completion;
//..............................................................................


    local_add_boxes(completion, boxes, packed_boxes,
                    potential_subscriptions, prev_node);

    return(completion);
}


////////////////////////////////////////////////////////////////////////////////
//
// Remove a box, leaving stuff dangling.
//
// Notes:
// -- deletes its output streams
//
// -- removes the appropriate sink port for any input stream.
//
void QueryProcessor::local_remove_boxes(AsyncRPC<void>  &completion,
                                        vector<Name>     box_names)
{
    vector<string>          box_str_names;
    vector<string>          upstream_box_names;
    vector<CatalogBox*>     boxes;
    CatalogBox             *catalog_box;

    vector<Name>::iterator  name_it = box_names.begin();
    vector<Name>::iterator  name_end = box_names.end();

//..............................................................................


    try
    {   for (; name_it != name_end; ++name_it)
        {
            box_str_names.push_back(name_it->as_string());
            catalog_box = _local_catalog.get_box(*name_it);

            if (catalog_box)
            {   boxes.push_back(catalog_box);

                CatalogStream::StreamStar* ins = catalog_box->get_box_in();
                CatalogStream::StreamStar::iterator in_it = ins->begin();
                CatalogStream::StreamStar::iterator in_end = ins->end();

                for(; in_it != in_end; ++in_it)
                {
                    if ( !((*in_it)->is_input_stream()) )
                    {
                        string up_name = (*in_it)->get_stream_source_box()->
                                                 get_box_name().as_string();

                        upstream_box_names.push_back(up_name);
                    }
                }
            }
            else
            {   completion.post(RPCFault("Could not remove box " +
                                         name_it->as_string() +
                                         " , not found."));
                return;
            }
        }

        // Remove
        _aurora.remove_box(boxes);

        vector<CatalogSubscription*> streams_to_unsubscribe;
        vector<CatalogBox*>::iterator box_it = boxes.begin();
        vector<CatalogBox*>::iterator box_end = boxes.end();

        for (; box_it != box_end; ++box_it)
        {
            // Remove this box as a sink for its inputs.
            CatalogStream::StreamStar* ins = (*box_it)->get_box_in();
            CatalogStream::StreamStar::iterator in_it = ins->begin();
            CatalogStream::StreamStar::iterator in_end = ins->end();

            for(; in_it != in_end; ++in_it)
            {

                // Remove any subscriptions if we're the last sink.
                if ( (*in_it)->is_subscribed_stream()
                     && (*in_it)->get_sink_port()->size() == 0 )
                {
                    streams_to_unsubscribe.push_back(
                        (*in_it)->get_source_subscription() );
                }

                // TODO: garbage collect inputs,
                // provided it's not an injected stream.
            }
        }

        AsyncRPC<void> remove_subs_completion;
        local_remove_subscriptions(remove_subs_completion, streams_to_unsubscribe);

        if ( remove_subs_completion.avail() && !remove_subs_completion.stat() )
            return;

        // Modify the catalog.
        //
        name_it = box_names.begin();

        for (; name_it != name_end; ++name_it)
        {
            _local_catalog.remove_box(*name_it);
        }
    }
    catch (AuroraException& ae)
    {   completion.post(RPCFault(ae.getMessage()));
        return;
    }

    completion.post(true);
}



////////////////////////////////////////////////////////////////////////////////
//
// Move boxes to a remote node.
//
void QueryProcessor::local_move_boxes(AsyncRPC<void>  &completion,
                                      vector<Name>     box_names,
                                      MedusaID         dest)
{
    string   message;

    // choke
    vector<string>       box_str_names;
    vector<CatalogBox*>  catalog_boxes;
    vector<CatalogBox>   box_copies;

    // Inputs from local boxes or injected inputs.
    set<Name> fed_inputs;

    // Outputs to local boxes
    set<Name> feed_outputs;

    // Inputs to redirect (i.e. input streams that are injected elsewhere).
    set<Name> redirect_inputs;

    // Query outputs to redirect
    set<Name> redirect_outputs;

    vector<Name>::iterator name_it = box_names.begin();
    vector<Name>::iterator name_end = box_names.end();
//
//  Exit:  completion - A fault is posted or chained through add_boxes
//                      and back through local_move_boxes_orphan_subs.
//                      Check upon return for an immediate fault with:
//                          completion.avail() && !completion.stat()
//                      Otherwise it posts a delayed fault or true.
//..............................................................................


    DEBUG << "Moving boxes to " << to_string(dest);

    try
    {   for (; name_it != name_end; ++name_it)
        {
            box_str_names.push_back(name_it->as_string());
            CatalogBox* cbox = _local_catalog.get_box(*name_it);

            if (cbox)
            {   InetAddress dest_addr(dest.get_id());

                catalog_boxes.push_back(cbox);

                // Record streams to subscribe to.
                CatalogStream::StreamStar *ins = cbox->get_box_in();
                CatalogStream::StreamStar::iterator in_it = ins->begin();
                CatalogStream::StreamStar::iterator in_end = ins->end();

                for (; in_it != in_end; ++in_it)
                {
                    if (!(*in_it)->is_input_stream())
                    {
                        // Streams fed from a local box require a subscription.
                        //
                        CatalogBox* source_box = (*in_it)->get_stream_source()
                                                              ->get_port_box();

                        vector<Name>::iterator source_found =
                            find(box_names.begin(), box_names.end(),
                                 source_box->get_box_name());

                        if (source_found == box_names.end())
                        {   fed_inputs.insert((*in_it)->get_stream_name());
                        }
                    }
                    else if ((*in_it)->is_injected_stream())
                    {   // Injected streams have their input forwarded
                        // Need to disconnect the input???
                        //
                        fed_inputs.insert((*in_it)->get_stream_name());
                    }
                    else if ( (*in_it)->get_source_subscription()->get_subscription_endpoint() != dest_addr)
                    {   // Non-injected inputs cause a subscription to their injection point
                        // unless that endpoint is our destination.
                        //
                        redirect_inputs.insert( (*in_it)->get_stream_name() );
                    }
                }

                CatalogStream::StreamStar *outs = cbox->get_box_out();
                CatalogStream::StreamStar::iterator out_it = outs->begin();
                CatalogStream::StreamStar::iterator out_end = outs->end();

                for (; out_it != out_end; ++out_it)
                {
                    if ( !(*out_it)->is_output_stream() )
                    {
                        vector<BoxPort>* sinks = (*out_it)->get_sink_port();
                        vector<BoxPort>::iterator sink_it = sinks->begin();
                        vector<BoxPort>::iterator sink_end = sinks->end();

                        for (; sink_it != sink_end; ++sink_it)
                        {
                            if ( sink_it->get_port_box() )
                            {
                                vector<Name>::iterator sink_found =
                                      find(box_names.begin(), box_names.end(),
                                           sink_it->get_port_box()->get_box_name());

                                // local downstream boxes need a subscription.
                                if ( sink_found == box_names.end() )
                                {
                                    feed_outputs.insert( (*out_it)
                                                           ->get_stream_name() );
                                }
                            }
                        }
                    }

                    // add to list of outputs to redirect
                    //
                    if ((*out_it)->get_subscribers()->size() > 0)
                    {
                        redirect_outputs.insert( (*out_it)->get_stream_name() );
                    }
                }

                box_copies.push_back(*cbox);
            }
            else
            {   message = "Could not move box \"" + name_it->as_string()
                        + "\"; box not found.";

                WARN << message;
                completion.post(RPCFault(message));
                return;
            }
        }

        // Note we don't explicitly resume inputs -- this is now done in
        // AuroraNode::uninstallBoxPorts, which gets called by AuroraNode::removeBox.
        _aurora.choke_boxes(box_str_names);

        DEBUG << "Move box choked";

        // pack
        BoxPackage  packed_boxes;
        _aurora.pack_boxes(box_str_names, packed_boxes);

        DEBUG << "Move box packed";

        // Add stats for streams that the remote site will subscribe to.
        _aurora._monitor.add_out_stream_stats((void *)&(_aurora._deqs),
                                              _aurora.sm,
                                              fed_inputs);

        DEBUG << "Move box added output stream stats";

        // Add boxes remotely
        Remote<QueryProcessor> dst_qp = qp_for(dest);

        if (!dst_qp)
        {   message = "No such moveBoxes destination " + to_string(dest);
            WARN << message;
            completion.post(RPCFault(message));
        }
        else
        {   DEBUG << "Move box remotely adding...";

            MoveBoxData move_data(catalog_boxes, dest, feed_outputs,
                                  redirect_inputs, redirect_outputs);

            dst_qp.add_boxes(wrap((QueryProcessor*)this,
                                  &QueryProcessor::local_move_boxes_orphan_subs,
                                  completion, move_data),
                             box_copies, packed_boxes, fed_inputs, _addr);

            DEBUG << "Move box finished remotely adding.";
        }
    }
    catch (AuroraException& ae)
    {   completion.post(RPCFault(ae.getMessage()));
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Move boxes stage 2
// -- setting up subs to feed orphaned boxes.
//
void QueryProcessor::local_move_boxes_orphan_subs(AsyncRPC<void> completion,
                                                  MoveBoxData  data,
                                                  RPC<void>    result)
{
//..............................................................................


    DEBUG << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^";

    if (!result.stat())
    {   DEBUG << "Move boxes failed to set up boxes remotely: "
              << result.stat();

        completion.post(RPCFault(result.stat()));
    }
    else
    {   DEBUG << "Creating " << data.feed_outputs.size()
              << " subscriptions for outputs fed by moved boxes.";

        // Create subscriptions for previous outputs.
        //
        InetAddress source(data.dest.get_id());
        vector<CatalogSubscription>  incoming_subs;

        set<Name>::iterator name_it  = data.feed_outputs.begin();
        set<Name>::iterator name_end = data.feed_outputs.end();

        for (; name_it != name_end; ++name_it)
        {   incoming_subs.push_back(CatalogSubscription(source, *name_it));
        }

        try
        {   local_add_subscriptions(incoming_subs,
                                    wrap(this,
                                         &QueryProcessor::local_move_boxes_redirect_streams,
                                         completion, data));
        }
        catch (AuroraException& ae)
        {   completion.post(RPCFault(ae.getMessage()));
        }
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Move boxes stage 3
// -- redirecting site links
void QueryProcessor::local_move_boxes_redirect_streams(AsyncRPC<void> completion,
                                                       MoveBoxData data,
                                                       RPC<void> result)
{
//..............................................................................


    if (!result.stat())
    {   DEBUG << "Move boxes failed to add local subscriptions: " << result.stat();
        completion.post(RPCFault(result.stat()));
        return;
    }

    /// Move box step 3.

    DEBUG << "Redirecting " << data.redirect_inputs.size()
          << " input subscriptions for moved boxes.";

    InetAddress dest = InetAddress(data.dest.get_id());

    // redirect non-injected inputs
    // TODO:
    // -- garbage collect if no other box uses the subscription.
    // -- okay to do after removing box, unless removing a box garbage collects input streams (NYI).

    set<Name>     &redirect_inputs = data.redirect_inputs;
    set<Name>::iterator red_in_it  = redirect_inputs.begin();
    set<Name>::iterator red_in_end = redirect_inputs.end();

    vector<CatalogSubscription> redirect_input_subs;

    vector<CatalogBox*>& moved_boxes = data.catalog_boxes;
    vector<CatalogSubscription*> input_subs_to_delete;

    for (; red_in_it != red_in_end; ++red_in_it)
    {
        CatalogStream *stream = _local_catalog.get_stream(*red_in_it);

        if (stream)
        {   CatalogSubscription *csub = stream->get_source_subscription();

            CatalogSubscription sub(csub->get_subscription_endpoint(),
                                    csub->get_subscription_stream_name());

            redirect_input_subs.push_back(sub);

            DEBUG << "Redirecting input " << stream->get_stream_name() << " from "
                  << csub->get_subscription_endpoint();

            vector<BoxPort>             *sinks = stream->get_sink_port();
            vector<BoxPort>::iterator sink_it  = sinks->begin();
            vector<BoxPort>::iterator sink_end = sinks->end();

            for (; sink_it != sink_end; ++sink_it)
            {
                vector<CatalogBox*>::iterator found =
                    find(moved_boxes.begin(), moved_boxes.end(), sink_it->get_port_box());

                if (found == moved_boxes.end())
                {   break;
                }
            }

            if (sink_it == sink_end)
            {
                DEBUG << "Garbage collecting input subscription for stream "
                      << stream->get_stream_name() << "@" << csub->get_subscription_endpoint();

                input_subs_to_delete.push_back(stream->get_source_subscription());
            }
        }
        else
        {   WARN << "Could not redirect input, unknown input stream "
                 << red_in_it->as_string();

            completion.post(RPCFault("Unknown input stream \""
                                     + red_in_it->as_string() + "\""));

            return;
        }
    }

    AsyncRPC<void> input_unsub_completion;
    local_remove_subscriptions(input_unsub_completion, input_subs_to_delete);

    if (input_unsub_completion.avail() && !input_unsub_completion.stat())
    {
        completion.post(RPCFault("Failed to garbage collect input subscriptions"));
        return;
    }


    // Perform actual redirection.
    Remote<QueryProcessor> qp = qp_for(MedusaID(to_string(dest)));

    if (!qp)
    {   WARN << "Failed to get QueryProcessor at " << dest;

        completion.post(RPCFault("Failed to get QueryProcessor for " +
                                 to_string(dest)));

        return;
    }

    if (!redirect_input_subs.empty())    // Not in original???
    {   DEBUG << "remote add_subscriptions " << redirect_input_subs.size() << " ...";
        qp.add_subscriptions(redirect_input_subs);
    }

    DEBUG << "Redirecting " << data.redirect_outputs.size()
          << " output subscriptions for moved boxes.";

    // redirect previous outputs
    // -- this must be done before removing boxes from the local catalog
    vector<CatalogSubscription> redirect_output_subs;

    set<Name> &redirect_outputs = data.redirect_outputs;
    set<Name>::iterator red_out_it = redirect_outputs.begin();
    set<Name>::iterator red_out_end = redirect_outputs.end();

    for (; red_out_it != red_out_end; ++red_out_it)
    {
        CatalogStream *stream = _local_catalog.get_stream(*red_out_it);

        if (stream)
        {   vector<CatalogSubscription*> *subscribers = stream->get_subscribers();
            vector<CatalogSubscription*>::iterator subbers_it = subscribers->begin();
            vector<CatalogSubscription*>::iterator subbers_end = subscribers->end();

            DEBUG << "subscribers size = " << subscribers->size();

            // No need to remove from stream, since box outputs are deleted.
            //
            for (; subbers_it != subbers_end; ++subbers_it)
            {
                DEBUG << "dest=" << dest
                      << "  ep=" << (*subbers_it)->get_subscription_endpoint()
                    << "  name=" << (*subbers_it)->get_subscription_stream_name();

                if ((*subbers_it)->get_subscription_endpoint() != dest)
                {
                    // TODO: check CatalogSubscription copy constructor.
                    redirect_output_subs.push_back(*(*subbers_it));
                }

                if (!_local_catalog.remove_subscriber(*subbers_it))
                {   DEBUG << "Missing subscriber to stream " << stream->get_stream_name();
                }

                //subbers_it = subscribers->begin();  // reset test hack???
            }
        }
        else
        {   WARN << "Could not redirect output, unknown output stream "
                 << red_out_it->as_string();

            completion.post(RPCFault("Unknown output stream \"" +
                                     red_out_it->as_string() + "\""));

            return;
        }
    }

    // qp is already set as RPC stub for dest above.
    //
    qp.subscribe_streams(
          wrap(this, &QueryProcessor::local_move_boxes_engine_and_catalog,
               completion, data),
          redirect_output_subs);

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Move boxes stage 4.
// -- removing boxes from the local engine and catalog.
//
void QueryProcessor::local_move_boxes_engine_and_catalog(AsyncRPC<void> completion,
                                                         MoveBoxData data,
                                                         RPC<void> result)
{
//..............................................................................


    if (!result.stat())
    {   DEBUG << "Move boxes failed to redirect subscriptions: " << result.stat();
        completion.post(RPCFault(result.stat()));
        return;
    }

    DEBUG << "Move boxes modifying engine.";

    try
    {   //////////////////////////////////////////////////
        // Engine and catalog modification.

        set<Name> &feed_outputs = data.feed_outputs;
        InetAddress dest = InetAddress(data.dest.get_id());

        vector<Name> box_names;
        //vector<string> box_str_names;

        vector<CatalogBox*> &catalog_boxes = data.catalog_boxes;
        vector<CatalogBox*>::iterator box_it = catalog_boxes.begin();
        vector<CatalogBox*>::iterator box_end = catalog_boxes.end();

        for (; box_it != box_end; ++box_it)
        {
            box_names.push_back((*box_it)->get_box_name());
            //box_str_names.push_back((*box_it)->getBoxName().as_string());

            // TODO: fix
            // Hack for now, since XML interface does not set up inter-site subscriptions.
            CatalogStream::StreamStar::iterator out_it = (*box_it)->get_box_out()->begin();
            CatalogStream::StreamStar::iterator out_end = (*box_it)->get_box_out()->end();

            for (; out_it != out_end; ++out_it)
            {
                DEBUG << "Removing out path for stream " << (*out_it)->get_stream_name();
                _data_path.remove_output_path((*out_it)->get_stream_name());
            }
        }

        // Stop running boxes.

        // Resume buffered streams from before packing.
        // -- do this here otherwise data might be lost before subscriptions are set up.

        // Remove boxes.
        _aurora.remove_box(catalog_boxes, feed_outputs);

        DEBUG << "Move boxes garbage collecting local subscriptions";

        set<CatalogSubscription*> uniq_unsubscribe;
        box_it = catalog_boxes.begin();

        for (; box_it != box_end; ++box_it)
        {
            CatalogStream::StreamStar* ins = (*box_it)->get_box_in();
            CatalogStream::StreamStar::iterator in_it = ins->begin();
            CatalogStream::StreamStar::iterator in_end = ins->end();

            for(; in_it != in_end; ++in_it)
            {
                // Remove any subscriptions if we're the last sink.
                //
                if ((*in_it)->is_subscribed_stream()  &&
                    (*in_it)->get_sink_port()->size() == 0)
                {
                    uniq_unsubscribe.insert( (*in_it)->get_source_subscription() );
                }

                // TODO: garbage collect inputs,
                // provided it's not an injected stream.
            }
        }

        vector<CatalogSubscription*> streams_to_unsubscribe;
        set<CatalogSubscription*>::iterator unsub_it = uniq_unsubscribe.begin();
        set<CatalogSubscription*>::iterator unsub_end = uniq_unsubscribe.end();

        for (; unsub_it != unsub_end; ++unsub_it)
        {   streams_to_unsubscribe.push_back(*unsub_it);
        }

        // Unsubscribe from remote inputs.
        AsyncRPC<void> remove_subs_completion;
        local_remove_subscriptions(remove_subs_completion, streams_to_unsubscribe);

        if (remove_subs_completion.avail()  &&  !remove_subs_completion.stat())
        {   return;
        }

        DEBUG << "Move boxes modifying catalog.";


        // Modify the local catalog and reflect moves to the regional catalog.
        //

        string update_xml;

        vector<Name>::iterator name_it = box_names.begin();
        vector<Name>::iterator name_end = box_names.end();

        for (; name_it != name_end; ++name_it)
        {
            _local_catalog.remove_box(*name_it, feed_outputs);

            update_xml = "<move box=\"" + name_it->as_string()
                       + "\" node=\"" + dest.as_string() + "\"/>";

            DEBUG << "reflect:  " << update_xml;
            reflect_global_update(update_xml);
        }
    }
    catch (AuroraException& ae)
    {   DEBUG << "Move boxes failed: " << ae.getMessage();
        completion.post(RPCFault(ae.getMessage()));
        return;
    }

    completion.post(true);
}



////////////////////////////////////////////////////////////////////////////////
//////////////////////   Deprecated Code Below   ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
// move_boxes RPC interface.    DEPRECATED
//
AsyncRPC<void> QueryProcessor::move_boxes(vector<Name> box_names, MedusaID dest)
{
    AsyncRPC<void> completion;
//..............................................................................


    // completion not modified in the original; which is wrong???
    local_move_boxes(completion, box_names, dest);

    return(completion);
}


BOREALIS_NAMESPACE_END
