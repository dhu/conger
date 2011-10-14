#include  "DataType.h"
#include  "DeployDiagram.h"

#include  "parseutil.h"


BOREALIS_NAMESPACE_BEGIN

// ??? Replicate streams and schemas over all nodes that use them.
// ??? We may have to subscribe to box outputs for streams over a node.


////////////////////////////////////////////////////////////////////////////////
//
//  Deploy an initial Diagram on Borealis nodes.
//
Status  DeployDiagram::deploy()
{
    DeployMap::iterator  node;
//
//..............................................................................


    Status  status = locate();        // Locate pending elements on nodes.

    if (status)
    {   status = infer_schema();      // Infer schemas for intermediate streams.
        DEBUG  << "infer_schema status = " << status;
    }

    if (status)
    {   {   for (node = _pending.begin(); node != _pending.end(); node++)
            {   DEBUG  << "Scan node " << (*node).first;
                status = deploy_pending_node(node);

                if (!status)
                {   break;
                }
            }
        }
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Determine the node for which each element is to be deployed.
//
Status  DeployDiagram::locate()
{
    Status          status = true;
    string          node;
    string          name;
    DeployNode     *pending;
    CatalogBox     *box;
    CatalogStream  *view_stream;

          QueryMap::iterator   query;
           ViewMap::iterator   view;
         RegionMap::iterator   region;
    vector<string>::iterator   region_ep;
//
// Boxes are deployed to the node to which they are assigned.
//
// Output schemas are not deployed.
// Input schemas are deployed to nodes corresponding to streams.
// The same schema may be deployed to several nodes.
//
// Limits:  Inputs can not be connected to outputs.
//          Boxes within a query must be on the same node.
//          Streams connected to inputs need to have their other ends
//             connected to box ins on the same node.
//
// What happens if a query contains only a table???
//..............................................................................


    DEBUG << "locate elements ===========================";


    // When a query is deployed to a node, it's location is passed on to it's
    // boxes and tables.  It is in turn picked up here.
    //
    for (query  = _topology._query_map.begin();
         query != _topology._query_map.end(); ++query)
    {
        // Each query is located where the first box or table is located.  Wrong???
        node = (query->second).get_query_node();

        if (node.empty())
        {   status = "Query (" + to_string((query->second).get_query_name())
                               + "has no node assignment.";

            break;
        }

        DEBUG << "locate query " << query->first << " on " << node;
        pending = &_pending[node];
        pending->_region = False;                   // Assume no regions.
        //pending->_connected = False;                   // Never been connected.


        // Locate subscriptions by the box out stream connected to their source.
        //
        CatalogBox::BoxStar *query_boxes = query->second.get_query_box();
        CatalogBox::BoxStar::iterator query_box_it = query_boxes->begin();
        CatalogBox::BoxStar::iterator query_box_end = query_boxes->end();

        // Iterate over boxes, checking output streams
        //
        for (; query_box_it != query_box_end; ++query_box_it)
        {
            node = (*query_box_it)->get_box_node();

            if (node.empty())
            {   status = "Box (" + to_string((*query_box_it)->get_box_name())
                                 + ") has no node assignment.";

                break;
            }

            // Save the name of each new box needing type inferencing.
            //
            pending->_box_infer.push_back( (*query_box_it)->get_box_name() );
            DEBUG << "Pending inference: " << (*query_box_it)->get_box_name();

            CatalogStream::StreamStar::iterator out_it =
                    (*query_box_it)->get_box_out()->begin();

            CatalogStream::StreamStar::iterator out_end =
                (*query_box_it)->get_box_out()->end();

            for (; out_it != out_end; ++out_it)
            {
                // Locate subscriptions for deployment.
                //
                StreamLinkMap::iterator subs_it  = _overlay._subscriber_map.begin();
                StreamLinkMap::iterator subs_end = _overlay._subscriber_map.end();

                // Iterate over subscriber endpoints.
                //
                for (; subs_it != subs_end; ++subs_it)
                {
                    SubscriptionMap &ep_subscribed_streams = subs_it->second;

                    // If endpoint subscribes to this output stream,
                    // add to current DeployNode.
                    //
                    SubscriptionMap::iterator subs_found =
                        ep_subscribed_streams.find( (*out_it)->get_stream_name() );

                    if ( subs_found != ep_subscribed_streams.end() )
                    {   pending->_output[(*out_it)->get_stream_name()]
                               = subs_found->second;
                    }
                }
            }
        }

        if (!status)
        {   break;
        }
    }

    if (status)
    {   // Locate connection point views by the stream to which they are attached.
        //
        for ( view = get_view_map()->begin(); view != get_view_map()->end(); ++view )
        {
            // Each view is located where it's stream is located.
            //
            view_stream = (view->second).get_view_stream();

            // Streams whose source is a box are co-located with the box.
            //
            box = view_stream->get_stream_source()->get_port_box();

            // Streams whose source is an input get their location from boxes
            // attached the sink.  They all need to be on the same node.
            //
            if ( !box )
            {   if ( view_stream->get_sink_port()->size() > 0 )
                {   box = view_stream->get_sink_box_port(0)->get_port_box();
                }
                else
                {   FATAL << "could not locate stream " << view_stream->get_stream_name()
                          << ", stream has no source or sinks";
                }
            }

            pending = &_pending[ box->get_box_node() ];
            pending->_region    = False;                   // Assume no regions.
            //pending->_connected = False;                   // Never been connected.
            pending->_view.push_back( &(view->second) );

            DEBUG << "locate view " << view->first << " on " << box->get_box_node();
        }
    }


    if (status)
    {   // Flag regions containing each node in case a node without queries.
        //
        for ( region = _region_map.begin(); region != _region_map.end(); ++region )
        {
            node    = region->first;                // Borealis node address.
            pending = &_pending[ node ];

            pending->_region    = True;             // Regions are on this node.
            //pending->_connected = False;          // Never been connected.

            DEBUG << "Node " << node << " is in a region.";
        }
    }

    DEBUG << "Done locating elements. ===========================";

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Determine the schemas for pending intermediate streams.
//  List boxes in upstream deployment order.
//  Check for cyclic deployment.
//
Status  DeployDiagram::infer_schema()
{
    Status       status = true;
    Boolean      cyclic;
    Boolean      infer;
    Boolean      done = False;
    CatalogBox  *box;

                    DeployMap::iterator  pending;
                   list<Name>::iterator  box_name;
    CatalogStream::StreamStar::iterator  in;
    CatalogStream::StreamStar           *box_in;
//..............................................................................


    // do until all streams have schemas,
    //
    while (( status )  &&  ( !done ))
    {   cyclic = False;
        done   = True;
        DEBUG << "------------------ " << _pending.size() << " pending nodes ...";

        // Do over pending nodes,
        //
        for (pending  = _pending.begin();
             pending != _pending.end(); pending++)
        {
            //  Do over boxes pending inferencing on each node,
            //
            for (box_name  = (*pending).second._box_infer.begin();
                 box_name != (*pending).second._box_infer.end(); box_name++)
            {
                DEBUG  << "box_name=" << (*box_name);
                cyclic = True;
                infer  = True;
                done   = False;
                box    = get_box(*box_name);
                box_in = box->get_box_in();

                // See if all Ins have schemas.
                //
                for (in  = box_in->begin();
                     in != box_in->end(); in++)
                {
                    DEBUG << "In stream (" + to_string((*in)->get_stream_name()) + ")";

                    if (!(*in)->get_stream_schema())
                    {
                        infer = False;
                        break;
                    }
                }

                // If all Ins have schemas, infer Outs; enqueue in deployment order.
                //
                if (infer)
                {   cyclic = False;
                    DEBUG  << "box_name=" << (*box_name);
                    status = box->infer_box_out(get_schema_map());

                    if (!status)
                    {   if (status.is_false())
                        {   status = "The box (" + to_string(box->get_box_name())
                                   + ") has an unknown external type ("
                                   + to_string(box->get_box_type()) + ").";
                        }

                        break;
                    }

                    // Move inferred boxes from the pending list to the
                    // ordered deployment list.  Restart the pending list scan.
                    //
                    (*pending).second._box_deploy.push_back( *box_name );
                    (*pending).second._box_infer.erase( box_name );
                    break;
                }

                DEBUG << "Skip box (" << (*box_name) << ")";
            }

            if (!done  &&  (!status  ||  !cyclic))
            {   break;
            }
        }

        // Assert at least one box was resolved each round.
        //
        if (cyclic)
        {   status = "Cyclic network.";
        }
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Deploy all pending elements to nodes.
//
Status  DeployDiagram::deploy_pending_node( DeployMap::iterator  node )
{
    ReplicaMap::iterator       replica;
    vector<string>::size_type  size = 0;
    vector<string>::size_type  index;
    set<string>::iterator      member;

    string     endpoint;
    string     element;

    Status     status = true;
    RPC<void>  rpc;
//..............................................................................


    endpoint = (*node).first;
    _node = &node->second;            // Node for deployment.

    replica = _replica_map.find(endpoint);

    if (replica != _replica_map.end())
    {   size = (*replica).second.size();

        DEBUG  << "first replica=" << (*replica).first
               << " with " << size << " copies.";

        element = "<replica_set  name=\"not used\" >\n <node endpoint=\""
                + endpoint + "\" />\n";

        for (member  = (*replica).second.begin();
             member != (*replica).second.end(); member++)
        {
           element += " <node endpoint=\"" + *member + "\" />\n";
        }

        element += "</replica_set>\n";
        DEBUG << element;

        member = (*replica).second.begin();
    }

    index = 0;

    while (status)
    {   status = open_client(endpoint);     // Initial deployment for the node.

        if (status)
        {   if (_node->_region)
            {   status = deploy_region(endpoint);   // Deploy any regions.
            }
        }

        // Deploy the replica set to each node in the set, BEFORE deploying anything else
        // Replicas will talk to each other during deployment
        //
        if (status  &&  size)
        {   rpc = _node->_client->add_xml_string( element );
            DEBUG << "add_xml_string " << element << ": " << rpc;
            status = rpc.stat();
        }

        // Deploy inputs before tables and boxes as they may reference inputs.
        //
        if (status)
        {   status = deploy_input((*node).first);    // Deploy inputs and schemas.
        }

        if (status)
        {   status = deploy_table_schema(endpoint);   // Deploy table-only schemas.
        }

        // Deploy boxes after tables as they may access a table.
        //
        if (status)
        {   status = deploy_box_node(_node, endpoint); // Deploy boxes on the node.
        }

        // Deploy output after boxes so Box Out declarations are available.
        //
        if (status)
        {   status = deploy_output();     // Deploy output subscriptions to this node.
        }

        // Deploy views after all streams as they may reference any stream.
        //
        if (status)
        {   status = deploy_view();       // Views may use streams in the query.
        }

        close_client();                   // Does nothing for now.

        if (index == size)
        {   break;                        // Done if all replicas are deployed.
        }

        if (status)
        {   endpoint = *member;
            member++;
            index++;

            DEBUG << "Deploy replica at:  " << endpoint;
        }
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Connect to a client node and establish reflections to the Head.
//
Status  DeployDiagram::open_client(// Target node in _pending.
                                   string   endpoint)
{
    Status     status = true;
    RPC<void>  rpc;
    string     element;
//
//  Call only once for each client.
//..............................................................................


    DEBUG  << "Create client for node: " << endpoint;
    _node->_client = (ptr<MedusaClient>)new MedusaClient(node_address(endpoint));

    // Unless static deployment, set up reflections to the Head.
    //
    if (!get_head().empty())
    {   element = "<head  endpoint=\"" + get_head() + "\" />";
        rpc = _node->_client->add_xml_string(element);

        DEBUG << "add_xml_string " << element << ": " << rpc;
        status = rpc.stat();
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Disconnect a client node.
//
Status  DeployDiagram::close_client()
{
    Status     status = true;
//..............................................................................


    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Add all regions to a single node.
//
Status  DeployDiagram::deploy_region(// Target node in _pending.
                                     string   endpoint)
{
    set<string>::iterator   region;

    set<string>  *member;
    string        element;
    Status        status = true;
    RPC<void>     rpc;
//..............................................................................


    member = &_region_map[endpoint];
    NOTICE << "Deploying " << member->size() << " regions to node " << endpoint;

    if (!member->empty())
    {   for (region = member->begin();  region != member->end(); region++)
        {   DEBUG << "Deploy region " << *region << " to node " << endpoint;

            element = "<region endpoint=\"" + *region + "\" />";
            rpc = _node->_client->add_xml_string( element );
            DEBUG << "add_xml_string " << element << ": " << rpc;
            status = rpc.stat();

            if (!status)
            {   break;
            }
        }
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Deploy schemas for all input streams to a single node (in _node).
//
Status  DeployDiagram::deploy_input(string  endpoint)
{
    Status          status = true;
    CatalogSchema  *schema;
    string          element;
    string          node;
    RPC<void>       rpc;

    SubscriptionMap::iterator  iterator;
    CatalogStream             *stream;
    StreamLinkMap::iterator    published;

    SubscriptionMap    map;
    InetAddress        address;
//
// This deploys all input schemas to a node.
// It needs to only deploy schemas for inputs used by that node.
// The publish endpoint should dominate the sink.
//..............................................................................


    DEBUG << "Node:  " << _node->_client->as_string()
          << " publisher map size = " << _overlay._publisher_map.size();

    //address   = InetAddress( _node->_client->as_string() );
    address   = endpoint;
    published = _overlay._publisher_map.find(address);

    if (published != _overlay._publisher_map.end())
    {   map = (*published).second;
        DEBUG << "published map size = " << map.size();

        for ( iterator = map.begin(); iterator != map.end(); ++iterator )
        {   node = _node->_client->as_string();
            DEBUG  << "deploy stream " << (*iterator).first << " at " << node;

            stream = &_topology._stream_map[ (*iterator).first ];
            schema = (stream)->get_stream_schema();

            if (_deployed_stream.find(node) == _deployed_stream.end())
            {
                DEBUG << "Initializing the set of deployed streams for node " << node;
                StreamSet     set;
                _deployed_stream[ node ] = set;
            }

            StreamSet  &stream_set = _deployed_stream[node];

            // See if the stream is not yet deployed.
            //
            if (stream_set.find(stream) == stream_set.end())
            {
                DEBUG  << "deploy stream " << (stream)->get_stream_name()
                             << " schema " << schema->get_schema_name();

                status = deploy_schema(schema);

                if (!status)
                {   break;
                }

                // Add the input to the set of deployed streams.
                //
                stream_set.insert(stream);

                // Always deploy input streams; deploy intermediates on edges in boxes.

                if (_partition_input.find((stream)->get_stream_name_string()) == _partition_input.end())
                {
                    element = stream->form_input_xml();
                    DEBUG << element;

                    rpc = _node->_client->add_xml_string(element);
                    NOTICE << "add_xml_string " << (stream)->get_stream_name() << ": " << rpc;
                    status = rpc.stat();

                    if (!status)
                    {   break;
                    }
                }
                else     ///////// experimental
                {   uint16  size = _partition_input[(stream)->get_stream_name_string()];

                    for (uint16  i = 1; i <= size; i++)
                    {   element = "<input  stream=\""
                                + stream->get_stream_name_string() + to_string(i)
                                + "\"  schema=\"" + schema->get_schema_name()
                                + "\" />";

                        DEBUG << element << " size=" << size;
                        rpc = _node->_client->add_xml_string(element);
                        NOTICE << "add_xml_string " << (stream)->get_stream_name() << ": " << rpc;
                        status = rpc.stat();

                        if (!status)
                        {   break;
                        }
                    }
                }
            }
        }
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Deploy a single schema a single node (in _node).
//
Status  DeployDiagram::deploy_schema(CatalogSchema  *schema)
{
    string        node;
    Status        status = true;

    CatalogSchema        *element;
    vector<SchemaField>  *field;

    vector<SchemaField>::iterator  f;

//
// This deploys all input schemas to a node.
// It needs to only deploy schemas for inputs used by that node???
//..............................................................................


    node = node_address(_node->_client->as_string()).as_string();

    if (_deployed_schema.find(node) == _deployed_schema.end())
    {
        SchemaSet     set;
        _deployed_schema[node] = set;
    }

    SchemaSet  &schema_set = _deployed_schema[node];

    // See if the schema has not been deployed to this node.
    //
    if (schema_set.find(schema) == schema_set.end())
    {
        // Scan fields for arrays with schemas and deploy them.
        // for field / if array / if not intrinsic / if not deployed
        // exit if a !status
        //
        field = &(schema->get_schema_field());

        for (f = field->begin(); f != field->end(); f++)
        {
            // See if an array of tuples.
            //
            if (f->get_type() == DataType::ARRAY)
            {
                element = f->get_array()->get_element_schema();

                if (element)
                {
                    // Deploy the element schema first.
                    //
                    if (schema_set.find(element) == schema_set.end())
                    {
                        status = send_schema(element);

                        // Keep track of schemas that have been deployed.
                        //
                        if (status)
                        {  schema_set.insert(schema);
                        }
                    }
                }
            }
        }

        if (status)
        {   status = send_schema(schema);

            // Keep track of schemas that have been deployed.
            //
            if (status)
            {  schema_set.insert(schema);
            }
        }
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Send a single schema as update XML.
//
Status  DeployDiagram::send_schema(CatalogSchema  *schema)
{
    string        element;
    Status        status = true;
    RPC<void>     rpc;
//
// This deploys all input schemas to a node.
// It needs to only deploy schemas for inputs used by that node???
//..............................................................................


    element = schema->form_schema_xml();

    rpc = _node->_client->add_xml_string(element);
    NOTICE << "add_xml_string " << schema->_name << ": " << rpc;
    status = rpc.stat();

    return  status;
}




////////////////////////////////////////////////////////////////////////////////
//
//  Deploy additional schemas for tables on a single node.
//
Status  DeployDiagram::deploy_table_schema(// Target node in _pending.
                                           string   endpoint)
{
    Status          status = true;
    CatalogSchema  *schema;
    CatalogTable   *table;
    string          element;
    RPC<void>       rpc;

    TableMap::iterator  table_it;
//
// This deploys all table schemas to a node.
// It needs to only deploy schemas for inputs used by that node???
//..............................................................................


    // Deploy schemas only used in tables.
    //
    for (table_it  = _topology._table_map.begin();
         table_it != _topology._table_map.end(); ++table)
    {
        table = &(*table_it).second;

        DEBUG << "++++++++++ table "
              << to_string(table->get_table_name())
              << " on " << endpoint
              << " to " << table->get_table_node();

        if (endpoint == table->get_table_node())
        {   schema = table->get_table_schema();

            DEBUG  << "deploy table " << table->get_table_name()
                   << " schema " << schema->get_schema_name();

            status = deploy_schema(schema);

            if (status)
            {
               if (_deployed_table.find(table) == _deployed_table.end())
               {
                    element = table->form_table_xml();
                    rpc = _node->_client->add_xml_string(element);
                    status = rpc.stat();

                    if (status)
                    {   // Remember views that have been deployed.
                        //
                        _deployed_table.insert(table);
                    }
                }
            }

            if ( !status )
            {   break;
            }
        }
    }


    /**********************
    // Deploy tables.  Each is wrapped in a query until queries go away???
    //
    for ( table  = _topology._table_map.begin();
          table != _topology._table_map.end(); table++ )
    {
        element  = "<query  name=\""
                 + Util::full_name( (*table).second.get_table_name() ) + "\">\n";

        element += (*table).second.form_table_xml();
        element += "</query>\n";
        DEBUG << element;

        rpc = _node->_client->add_xml_string( element );
        NOTICE << "add_query_xml " << (*table).second.get_table_name() << ": " << rpc;
        if ( !rpc.stat() ) return( rpc.stat() );
    }
    *************************/

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Deploy all boxes on a node and start them.
//
Status  DeployDiagram::deploy_box_node(DeployNode  *deploy,

                                       // This is needed with replica_set
                                       //
                                       string       endpoint)
{
    Status       status = true;
    CatalogBox  *box;

    list<Name>::iterator  name;
//..............................................................................


    DEBUG << "deploying " << deploy->_box_deploy.size() << " boxes...";

    // Scan ordered boxes for the node.
    //
    for (name  = deploy->_box_deploy.begin();
         name != deploy->_box_deploy.end(); name++)
    {
        //DEBUG << "box name = " << *name;
        box = &_topology._box_map[*name];

        if (_deployed_box.find(box) == _deployed_box.end())
        {
            // Deploy and start the box on the target node.
            // Could send all boxes in one query and start???
            //
            DEBUG << "deploy (" << *name << ")";
            status = deploy_box(box, endpoint);

            if ( !status )
            {   break;
            }
        }
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Deploy a single box to a node and start it.
//
Status  DeployDiagram::deploy_box(// Box to deploy.
                                  CatalogBox  *box,

                                  // This is needed with replica_set
                                  //
                                  string       endpoint)
{
    RPC<void>  rpc;
    string     element;
    Status     status = true;
    string     input_stream_node;
    string     box_node;

    CatalogStream::StreamStar::iterator  in;
//
// Box In schemas that run between nodes are deployed.
// Until queries are removed from nodes, wrap each box in its own query.
//..............................................................................


    // if any box ins are from another node, deploy the stream and schema, too.
    //
    for ( in  = box->get_box_in()->begin();
          in != box->get_box_in()->end(); in++ )
    {
        DEBUG << "Deploy: now examining input stream " << (*in)->get_stream_name();

        // See if there is an upstream box.
        //
        if (!(*in)->is_input_stream())
        {
            // Location of the In stream source (upstream box).
            //

            //box_node = box->get_box_node(); --> This does not work when there are multiple replicas of a node
            //DEBUG << "The box node is " << box_node << " while the endpoint is " << endpoint;
            box_node = endpoint; // -> Make sure to use correct replica

            // That's the default node, but if this is box_node's replica, then the stream is at this replica too
            input_stream_node = (*in)->get_stream_source()->get_port_box()->get_box_node();
            ReplicaMap::iterator replica_iter = _replica_map.find(input_stream_node);

            if ( replica_iter != _replica_map.end() )
            {
                set<string>::iterator member_iter;

                for (member_iter = replica_iter->second.begin(); member_iter != replica_iter->second.end(); member_iter++)
                {
                    if ( (*member_iter) == box_node )
                    {   input_stream_node = box_node;
                    }
                }
            }

            DEBUG << "The stream is not an input. It is located at: " << input_stream_node;

            //if (node != box->get_box_node())
            if ( input_stream_node != box_node )
            {

                DEBUG << "Deploying schema ";
                status = deploy_schema((*in)->get_stream_schema());

                if (!status)
                {   break;
                }

                if (_deployed_stream.find(box_node) == _deployed_stream.end())
                {
                    DEBUG << "Initializing the set of deployed streams for node " << box_node;
                    StreamSet     set;
                    _deployed_stream[ box_node ] = set;
                }

                StreamSet  &stream_set = _deployed_stream[box_node];

                // See if the stream is not yet deployed.
                //
                if (stream_set.find(*in) == stream_set.end())
                {
                    // Deploy intermediate in streams.
                    //
                    element = "<input  stream=\"" + Util::full_name((*in)->get_stream_name())
                                + "\"  schema=\"" + Util::full_name((*in)->get_schema_name())
                                + "\"  node=\"" + input_stream_node + "\" />\n";

                    DEBUG << element;

                    DEBUG << "Deploying stream " << (*in)->get_stream_name();
                    rpc = _node->_client->add_xml_string(element);
                    NOTICE << "add_xml_string " << (*in)->get_stream_name() << ": " << rpc;
                    status = rpc.stat();

                    if ( !status)
                    {   break;
                    }

                    stream_set.insert(*in);
                }
            }
        }
    }

    ///////////////////////////////// experimental
    string  stream;
 
    if (box->get_box_type() == "scatter")
    {   element = "<box name=\"" + to_string(box->get_box_name())
                + "\" type=\"scatter\" node=\""
                + box->get_box_node() + "\" >\n";

        stream = box->get_in_stream(0)->get_stream_name_string();
        element += "<in  stream=\"" + stream + "\" />\n";

        stream = box->get_out_stream(0)->get_stream_name_string();
        vector<Name>  list;
        split_name(_partition_output[stream], list);

        for (uint16  i = 1; i <= list.size(); i++)
        {
            element += "<out  stream=\"" + stream + to_string(i) + "\"/>\n";
        }

        element += "</box>";
    }
    else if (box->get_box_type() == "gather")
    {   element = "<box name=\"" + to_string(box->get_box_name())
                + "\" type=\"gather\" node=\""
                + box->get_box_node() + "\" >\n";

        stream = box->get_in_stream(0)->get_stream_name_string();

        for (uint16 i  = 1; i <= _partition_input[stream]; i++)
        {
            element += "<in   stream=\"" + stream + to_string(i) + "\"/>\n";
        }

        stream = box->get_out_stream(0)->get_stream_name_string();
        element += "<out  stream=\"" + stream + "\" />\n";

        element += "</box>";
    }
    else
    ///////////////////////////////////////////////
    {   element = box->form_box_xml();
    }

    DEBUG << element;

    if ( status )
    {   rpc = _node->_client->add_xml_string( element );
        NOTICE << "add_xml_string " << element << ": " << rpc;
        status = rpc.stat();
    }

    /********************************** yna create_query
    if ( status )
    {   // Run each query after it is deployed.
        //
        rpc = _node->_client->start_query( Util::full_name(box->get_box_name());
        NOTICE << "start_query( " << box->get_box_name() << " ): " << rpc;
        status = rpc.stat();
    }
    *************************************/

    if ( status )
    {   // Remember boxes that have been deployed.
        //
        _deployed_box.insert(box);
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Deploy schemas for all output streams to a single node (in _node).
//
Status  DeployDiagram::deploy_output()
{
    string     element;
    string     stream;
    string     node;

    Status     status = true;
    RPC<void>  rpc;

    DeployOutput::iterator    subscriber;
//
// This deploys all output schemas to a node.
// It needs to only deploy schemas for outputs used by that node.
// Ouput elements are not deployed.  Subscribe elements are.
//..............................................................................


    if ( !_node->_output.empty() )
    {   subscriber = _node->_output.begin();

        for (; subscriber != _node->_output.end(); ++subscriber )
        {
            CatalogSubscription &sub = subscriber->second;

            stream = Util::full_name( subscriber->first );
            node   = sub.get_subscription_endpoint().as_string();

            if (_deployed_stream.find(node) == _deployed_stream.end())
            {
                DEBUG << "Initializing the set of deployed streams for node " << node;
                StreamSet     set;
                _deployed_stream[ node ] = set;
            }

            StreamSet  &stream_set = _deployed_stream[node];

            // See if the stream is not yet deployed.
            //
            if (stream_set.find(sub.get_subscription_stream()) == stream_set.end())
            {
                DEBUG << "stream=" << stream;
                if (_partition_output.find(stream) == _partition_output.end())
                {
                    element = sub.form_subscribe_xml();

                    DEBUG  << element;
                    rpc = _node->_client->add_xml_string( element );
                    NOTICE << "add_xml_string " << subscriber->first << ": " << rpc;
                    status = rpc.stat();
                }
                else     ///////// experimental
                {   string  endpoint = _partition_output[stream];

                    vector<Name>  list;
                    split_name(endpoint, list);

                    for (uint16  i = 1; i <= list.size(); i++)
                    {   element = "<subscribe  stream=\""
                                + stream + to_string(i)
                                + "\"  endpoint=\"" + list[i - 1]
                                + "\" />";

                        DEBUG << element << " size=" << list.size();
                        rpc = _node->_client->add_xml_string(element);
                        NOTICE << "add_xml_string " << stream << ": " << rpc;
                        status = rpc.stat();

                        if (!status)
                        {   break;
                        }
                    }
                }

                if (status)
                {   // Add the output to the set of deployed streams.
                    //
                    stream_set.insert(sub.get_subscription_stream());
                }
            }
        }
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Deploy all views for a single node (in _node).
//
Status  DeployDiagram::deploy_view()
{
    Status        status = true;
    string        element;
    RPC<void>     rpc;

    vector<CatalogView *>::iterator  view;
//..............................................................................


    for (view  = _node->_view.begin();
         view != _node->_view.end(); view++)
    {
        if (_deployed_view.find(*view) == _deployed_view.end())
        {
            element = (*view)->form_view_xml();

            rpc = _node->_client->add_xml_string(element);
            NOTICE << "add_xml_string " << element << ": " << rpc;
            status = rpc.stat();

            if (!status)
            {   break;
            }

            // Remember views that have been deployed.
            //
            _deployed_view.insert(*view);
        }
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Virtual override to add only new boxes; ignore existing boxes.
//
CatalogBox  *DeployDiagram::add_box_element(const DOMElement  *box,
                                                     Boolean   dynamic)
                                      throw(AuroraBadEntityException)
{
    CatalogBox       *add_box = NULL;
//..............................................................................


    add_box = Diagram::add_box_element(box, dynamic);

    return  add_box;
}



////////////////////////////////////////////////////////////////////////////////
//
// If the IP address does not contain the port specification, then add the
// default port, 15000.
//
InetAddress  DeployDiagram::node_address(// A node as:  <ip address>[:<port>]
                                         string   node )
{
    // The default borealis port is a static assignment for now.
    // It needs to be assigned by scanning available channels???
    //
    string    endpoint;
//..............................................................................


    endpoint = Util::form_endpoint( node, DEFAULT_BOREALIS_PORT );
    DEBUG << "Complete address is " << endpoint;

    return  InetAddress(endpoint);
}


BOREALIS_NAMESPACE_END

//
////////////////////////////////////////////////////////////////////////////////
