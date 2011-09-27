#include  "Diagram.h"
#include  "DataType.h"
#include  "parseutil.h"
#include  "util.h"

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
//  Parse a Borealis Network Diagram represented as an XML file into a
//  catalog structure.
//
Status  Diagram::parse_file(// File containing an XML Borealis network diagram.
                            string    file)
{
//..............................................................................


    // Parse the stream and schema names.
    //
    try
    {   _doc = parse_xml_file(file);
        parse_xml();
    }
    catch(AuroraException  &e)
    {   DEBUG << "Failed parsing a diagram from xml:" << e;
        return  e.as_string();
    }

    return  true;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Parse a Borealis Network Diagram represented as an XML string into a
//  catalog structure.
//
Status  Diagram::parse_string(// A string containing an XML network diagram.
                              string    diagram)
{
//..............................................................................


    try
    {   _doc = parseXmlString(diagram);
        parse_xml();
    }
    catch(AuroraException  &e)
    {   DEBUG << "Failed parsing a diagram from XML:" << e;
        return  e.as_string();
    }

    return  true;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Parse a single XML schema as a string.
//
Status  Diagram::parse_xml_schema(// An element to be parsed.
                                  string    schema_xml,

                                  // Return the schema from the catalog.
                                  CatalogSchema  **schema
                                 )
{
//..............................................................................


    Status  status = parse_element(schema_xml);

    if (status)
    {   try
        {   *schema = add_schema_element(_root);
        }
        catch(AuroraException  &e)
        {   status = "Failed parsing schema from XML";
        }
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Parse a single XML query as a string.
//
Status  Diagram::parse_xml_query(// An element to be parsed.
                                 string     query_xml,

                                 // Return the schema from the catalog.
                                 CatalogQuery  **query,

                                 // Whether this is a dynamic query addition (default no).
                                 Boolean dynamic
                                )
{
//..............................................................................


    Status  status = parse_element(query_xml);

    if (status)
    {   try
        {   *query = add_query_element(_root, dynamic);
        }
        catch(AuroraException  &e)
        {   status = "Failed parsing a query from XML";
        }
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Parse a single XML connection point view as a string.
//
Status  Diagram::parse_xml_view(// An XML connection point view to be parsed.
                                string    view_xml,

                                // Return the view from the catalog.
                                CatalogView  **view)
{
//..............................................................................


    Status  status = parse_element(view_xml);

    if (status)
    {   try
        {   *view = add_view_element(_root);
        }
        catch(AuroraException  &e)
        {   status = "Failed parsing a connection point view from XML";
        }
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Parse a single XML subscribe element as a string.
//
Status  Diagram::parse_xml_subscribe(// An XML subscribe element to be parsed.
                                     string    subscribe_xml,

                                     // Return a copy of the subscription.
                                     CatalogSubscription  *subscription )
{
//..............................................................................


    Status  status = parse_element(subscribe_xml);

    if (status)
    {   try
        {   *subscription = add_subscribe_element(_root);
        }
        catch(AuroraException  &e)
        {   status = "Failed parsing a subscribe element from XML";
        }
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Parse a single XML element as a string.
//
Status  Diagram::parse_element(// An element to be parsed.
                               string    element)
{
//..............................................................................

    try
    {   _doc  = parseXmlString(element);
        _root = _doc->getDocumentElement();
    }
    catch(AuroraException  &e)
    {   DEBUG << "Failed parsing an element from XML:" << e;
        return(e.as_string());
    }

    return  true;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Parse a Borealis Network or Deployment Diagram represented as an XML
//  string into a catalog structure.  Called from parseFile or parseString.
//
void  Diagram::parse_xml()
                   throw(AuroraBadEntityException)
{
    string   tag;

    vector<DOMElement *>            borealis;
    vector<DOMElement *>::iterator  iterator;
//
//..............................................................................


    try
    {   _root = _doc->getDocumentElement();
        DEBUG  << "root tag: " << to_string( _root->getTagName() );
        tag = to_string( _root->getTagName() );

        if ( tag == "borealis" )
        {   _borealis = True;
            parse_xml_borealis( _root );
        }
        else if ( tag == "deploy" )
        {   _borealis = False;
            parse_xml_deploy( _root );
        }
        else
        {   Throw(AuroraBadEntityException, "Unknown file type:  " + tag);
        }
    }
    catch(AuroraException  &e)
    {   DEBUG << "Failed parsing a diagram from XML:\n   " << e;
        Throw(AuroraBadEntityException, e.as_string());
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Parse a Borealis Network Diagram represented as an XML string into a
//  catalog structure.  Called from parseFile or parseString.
//
void  Diagram::parse_xml_deploy(const DOMElement   *root)
                          throw(AuroraBadEntityException)
{
//  <deploy>
//      :
//  </deploy>
//..............................................................................


    try
    {   xml_expect_tag(root, "deploy");
        DEBUG << "<deploy>";

        parse_replica(    root);
        parse_node(       root);
        parse_region(     root);
        parse_client(     root);
        parse_publish(    root);
        parse_subscribe(  root);
        parse_unsubscribe(root);

        DEBUG << "</deploy>";
    }
    catch(AuroraException  &e)
    {   DEBUG << "Failed parsing deployment from XML:\n   " << e;

        Throw(AuroraBadEntityException, e.as_string());
    }

    return;
}




////////////////////////////////////////////////////////////////////////////////
//
//  Parse a replica_set element to locates queries on a set of Borealis nodes.
//
void  Diagram::parse_replica(const DOMElement   *root)
                       throw(AuroraBadEntityException)
{
    vector<DOMElement *>            replica;
    vector<DOMElement *>::iterator  iterator;

    string    message;
    string    name;
    string    query;
//
//    <replica_set  name="<set name"  <query="<name> ..." ... />
//       <node ... />
//    </replica_set>
//..............................................................................


    try
    {   xml_child_elements(replica, root, "replica_set");

        for (iterator = replica.begin(); iterator != replica.end(); iterator++)
        {
            // Parse the stream name and enpoint.
            //
            xml_expect_tag(*iterator, "replica_set");
            name  = xml_attribute(*iterator, "name",  ATTR_REQUIRED);
            query = xml_attribute(*iterator, "query", ATTR_NOT_REQUIRED);

            lowercase(query);

            DEBUG << "<replica_set  name=\"" << name
                           << "\"  query=\"" << query << "\" >";

            // Parse the node elements within a replica set.
            //
            _replica_query = query;
            parse_node(*iterator);
            _replica_query = "";

            DEBUG << "</replica_set>";
        }
    }
    catch(AuroraException  &e)
    {   _replica_query = "";
        message = "Failed parsing a replica_set from XML:\n   " + e.as_string();
        DEBUG << message;

        Throw(AuroraBadEntityException, message);
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Parse a Node element that locates queries on a Borealis node.
//
void  Diagram::parse_node(const DOMElement   *root)
                    throw(AuroraBadEntityException)
{
    vector<DOMElement *>            node;
    vector<DOMElement *>::iterator  iterator;

    vector<Name>  list;

    string    id;
    string    endpoint;
    string    query;
    string    replica;
    string    message;

    uint32    index;
//
//    <node  endpoint="<host>[:endpoint]"
//            [<query="<name1> <name2> ..." ... />]
//
// Entry:  _replica_query is set when parsing a replica set.
//..............................................................................


    try
    {   xml_child_elements(node, root, "node");
        replica = "";

        for (iterator = node.begin(); iterator != node.end(); iterator++)
        {
            // Parse the stream name and enpoint.
            //
            xml_expect_tag( *iterator, "node" );
            endpoint = xml_attribute( *iterator, "endpoint", ATTR_REQUIRED );
            query    = xml_attribute( *iterator, "query",    ATTR_NOT_REQUIRED );

            lowercase( query );
            endpoint = form_endpoint( endpoint, DEFAULT_BOREALIS_PORT );

            DEBUG << "<node  endpoint=\"" << endpoint
                  <<       "\"  query=\"" << query << "\" />";

            // if a replica and 1st node -> establish as first; locate there.
            // if a replica and more -> add to the map.
            // else locate to primary.
            //
            if (_replica_query.empty())
            {   _add_node = endpoint;
            }
            else if (replica.empty())
            {   replica   = endpoint;        // Save the first replica.
                _add_node = endpoint;        // Locate the queries there.
                query = _replica_query;

                DEBUG << "Replica (" << endpoint << ") is the first in a set.";
            }
            else
            {   DEBUG << "The node (" << endpoint
                      << ") is a replica of (" << replica << ").";

                _replica_map[replica].insert(endpoint);
                _add_node = "";             // Skip locating replicated queries.
            }

            if (!_add_node.empty())
            {   split_name(query, list);

                for (index = 0; index < list.size(); index++)
                {   locate_query(list[index]);
                }
            }
        }
    }
    catch(AuroraException  &e)
    {   message = "Failed parsing a node from XML:\n   " + e.as_string();
        DEBUG << message;

        Throw(AuroraBadEntityException, message);
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Include a Borealis Node in a region.
//
void  Diagram::parse_region(const DOMElement   *root)
                      throw(AuroraBadEntityException)
{
    vector<DOMElement *>            region;
    vector<DOMElement *>::iterator  iterator;

    string    endpoint;
    string    node;
    string    message;
//
//     <region  [node="<bip:bport>"] [endpoint="<rip>[:rport]"] />
//..............................................................................


    try
    {   xml_child_elements(region, root, "region");

        for (iterator = region.begin(); iterator != region.end(); ++iterator)
        {
            // Parse the region node and endpoint to receive update Xml.
            //
            xml_expect_tag(*iterator, "region");
            node     = xml_attribute(*iterator, "node",     ATTR_NOT_REQUIRED);
            endpoint = xml_attribute(*iterator, "endpoint", ATTR_NOT_REQUIRED);

            node     = form_endpoint(node,     DEFAULT_BOREALIS_PORT);
            endpoint = form_endpoint(endpoint, DEFAULT_REGION_PORT);

            DEBUG << "<region  node=\"" << node
                  <<  "\"  endpoint=\"" << endpoint << "\" />";

            _region_map[ node ].insert( endpoint );    // Map the endpoint.
        }
    }
    catch(AuroraException  &e)
    {   message = "Failed parsing a region from XML:\n   " + e.as_string();
        DEBUG << message;

        Throw(AuroraBadEntityException, message);
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Associate publish and subscribe elements with clients.
//
void  Diagram::parse_client( const DOMElement  *root )
    throw( AuroraBadEntityException )
{
    vector<DOMElement *>            client;
    vector<DOMElement *>::iterator  iterator;

    string    endpoint;
    string    prefix;
    string    message;
//
//      <client  [prefix="<class prefix>"]  [endpoint="<mip:mport>"] >
//          <publish ... />  ...
//          <subscribe ... />  ...
//      </client>
//..............................................................................


    try
    {   xml_child_elements(client, root, "client");

        for (iterator = client.begin(); iterator != client.end(); ++iterator)
        {
            // Parse the client prefix and endpoint to receive data.
            //
            xml_expect_tag(*iterator, "client");
            prefix   = xml_attribute(*iterator, "prefix",   ATTR_NOT_REQUIRED);
            endpoint = xml_attribute(*iterator, "endpoint", ATTR_NOT_REQUIRED);

            ///////////////////////////////////  experimental
            vector<Name>  list;
            uint32        index;

            split_name(endpoint, list);

            _client_monitor = Util::form_endpoint(to_string(list[0]),
                                                  DEFAULT_MONITOR_PORT);

            for (index = 1; index < list.size(); index++)
            {   DEBUG << "endpoint=" << list[index];

                _client_monitor += " "
                                 + Util::form_endpoint(to_string(list[index]),
                                                       DEFAULT_MONITOR_PORT);
            }

            //_client_monitor = Util::form_endpoint(endpoint, DEFAULT_MONITOR_PORT);
            /////////////////////////////////////////

            DEBUG << "<client  prefix=\"" << prefix
                  <<  "\"  endpoint=\"" << _client_monitor << "\" >";

            parse_subscribe(*iterator);
            // _client_monitor = "";   //org???

            parse_publish(*iterator);
            _client_monitor = "";

            DEBUG << "</client>";
        }
    }
    catch(AuroraException  &e)
    {   message = "Failed parsing an XML client element:\n   " + e.as_string();
        DEBUG << message;

        Throw(AuroraBadEntityException, message);
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Parse publish elements to deploy inputs into a catalog structure.
//
void  Diagram::parse_publish(const DOMElement  *root)
                       throw(AuroraBadEntityException)
{
    vector<DOMElement *>            publish;
    vector<DOMElement *>::iterator  iterator;

    vector<BoxPort>           *port;
    vector<BoxPort>::iterator  box;

    string    stream;
    string    node;
    string    infer;
    string    message;

    CatalogStream  *catalog_stream;
//
// <publish stream={stream name}  [node={endpoint}] />
//
// Currently this is only used by the marshal program.
//..............................................................................


    try
    {   xml_child_elements(publish, root, "publish");

        for (iterator = publish.begin(); iterator != publish.end(); ++iterator)
        {
            // Parse the stream name and enpoint.
            //
            xml_expect_tag(*iterator, "publish");
            stream = xml_attribute(*iterator, "stream", ATTR_REQUIRED);
            node   = xml_attribute(*iterator, "node",   ATTR_NOT_REQUIRED);

            lowercase(stream);

            DEBUG << "<publish stream=\"" << stream
                  << "\"  node=\"" << node << "\"/>";

            ///////////////////////////////////  experimental
            vector<Name>  list;

            split_name(_client_monitor, list);

            if (list.size() > 1)
            {   DEBUG << "input stream=" << stream << " partition=" << list.size();
                _partition_input[stream] = list.size();
            }
            /////////////////////////////////////////

            // A stream can publish to multiple nodes.
            // The publish location is determined by the box locations.
            // If a node is given it must match one of the box nodes, if any.
            //
            catalog_stream = get_stream(stream);

            if (catalog_stream == NULL)
            {   message = "A published stream (" + stream
                        + ") is not declared.";

                DEBUG << message;
                Throw(AuroraBadEntityException, message);
            }
 
            port = catalog_stream->get_sink_port();

            if (port->empty())
            {    DEBUG << "<publish stream=\"" << stream
                       << "\"  node=\"" << node << "\"/>";

                 node = form_endpoint(node, DEFAULT_BOREALIS_PORT);
                 add_publish(stream, node);
            }
            else
            {   for (box = port->begin(); box != port->end(); box++)
                {   infer = box->get_port_box()->get_box_node();

                    DEBUG << "stream=" << stream << "  infer=" << infer;

                    //infer = form_endpoint(infer, DEFAULT_BOREALIS_PORT);

                    DEBUG << "<publish stream=\"" << stream
                          << "\"  node=\"" << infer << "\"/>";

                    add_publish(stream, infer);

                    if (!node.empty()  &&
                        (infer == form_endpoint(node, DEFAULT_BOREALIS_PORT)))
                    {
                        node = "";
                    }
                }

                if (!node.empty())
                {   message = "A publish node does not contain a connected box.";
                    DEBUG << message;

                    Throw(AuroraBadEntityException, message);
                }
            }
        }
    }
    catch(AuroraException  &e)
    {   message = "Failed parsing an XML publish element:\n   " + e.as_string();
        DEBUG << message;

        Throw(AuroraBadEntityException, message);
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  An application monitor subscribes to an output stream.
//
void  Diagram::parse_subscribe(const DOMElement   *root)
                         throw(AuroraBadEntityException)
{
    vector<DOMElement *>            subscribe;
    vector<DOMElement *>::iterator  iterator;
//
//  <subscribe  stream=<output name>  [gap=<size>]  [endpoint=<monitor>]   />
//
//  The endpoint attribute is only used internally in update XML.
//..............................................................................


    try
    {   xml_child_elements(subscribe, root, "subscribe");

        for (iterator  = subscribe.begin();
             iterator != subscribe.end(); ++iterator)
        {
            add_subscribe_element(*iterator);
        }
    }
    catch(AuroraException  &e)
    {   DEBUG << e;
        Throw(AuroraBadEntityException, e.as_string());
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Add a single subscribe element top the catalog.
//
CatalogSubscription  Diagram::add_subscribe_element(const DOMElement *subscribe)
                                              throw(AuroraBadEntityException) 
{
    string    stream;
    string    endpoint;
    string    gap;
    string    message;

    uint16    size;
    CatalogSubscription  subscription;
//
//  <subscribe  stream=<output name>  [endpoint=<monitor>  [gap=<size>]] />
//..............................................................................


    try
    {   xml_expect_tag(subscribe, "subscribe");

        stream   = xml_attribute(subscribe, "stream",   ATTR_REQUIRED);
        endpoint = xml_attribute(subscribe, "endpoint", ATTR_NOT_REQUIRED);
        gap      = xml_attribute(subscribe, "gap",      ATTR_NOT_REQUIRED);

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



////////////////////////////////////////////////////////////////////////////////
//
//  An application monitor removes a subscription to an output stream.
//
void  Diagram::parse_unsubscribe(const DOMElement   *root)
                           throw(AuroraBadEntityException)
{
    vector<DOMElement *>            unsubscribe;
    vector<DOMElement *>::iterator  iterator;

    string    stream;
    string    message;
//
//     <unsubscribe  stream="<stream name>"  [endpoint=<monitor address>] />
//..............................................................................


    try
    {   xml_child_elements(unsubscribe, root, "unsubscribe");

        for (iterator  = unsubscribe.begin();
             iterator != unsubscribe.end(); ++iterator)
        {
            xml_expect_tag(*iterator, "unsubscribe");
            stream = xml_attribute(*iterator, "stream",  ATTR_REQUIRED);

            NOTICE << "TO DO:  <unsubscribe  stream=\"" << stream << "\" />";
        }
    }
    catch(AuroraException  &e)
    {   message = "Failed parsing an XML unsubscribe element:\n   " + e.as_string();
        DEBUG << message;

        Throw(AuroraBadEntityException, message);
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Assign a query's primary node to each box contained in the query.
//
void  Diagram::locate_query(// Name of the query.
                            Name   name)
                      throw(AuroraBadEntityException)
{
    CatalogQuery  *query;
    string         lower;

    CatalogBox::BoxStar::iterator      box;
    CatalogTable::TableStar::iterator  table;
    QueryMap::iterator                 in_query;
//
//  Entry:  _add_node has the node IP address.
//..............................................................................


    lower = to_string(name);
    lowercase(lower);
    DEBUG  << "query name = " << lower;

    in_query = _topology._query_map.find(lower);

    if (in_query == _topology._query_map.end())
    {   Throw(AuroraBadEntityException,
              "Undefined query:  " + lower);
    }

    query = &in_query->second;

    for (box  = query->get_query_box()->begin();
         box != query->get_query_box()->end(); box++)
    {
        (*box)->set_box_node(_add_node);
        DEBUG << "locate box " << (*box)->get_box_name() << " on " << _add_node;
    }

    for (table  = query->get_query_table()->begin();
         table != query->get_query_table()->end(); table++)
    {
        (*table)->set_table_node(_add_node);
        DEBUG << "locate table " << (*table)->get_table_name() << " on " << _add_node;
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Parse a Borealis Network Diagram represented as an XML string into a
//  catalog structure.  Called from parseFile or parseString.
//
void  Diagram::parse_xml_borealis(const DOMElement   *root)
                            throw(AuroraBadEntityException)
{
    string    update_queue;
    string    key;
//
//  <borealis  [updatequeue={0 | 1}] 
//             [queuetype={0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8}] 
//             [key={field name}] >
//      :
//  </borealis>
//..............................................................................


    try
    {   xml_expect_tag(root, "borealis");
        _in_queue = parse_in_queue(root, CatalogBox::InQueue());

        DEBUG << "<borealis update_queue=\"" << int(_in_queue._update_queue)
              << "\" queue_type=\"" << int(_in_queue._queue_type)
              << "\" key=\"" << _in_queue._key << "\" />";

        parse_type(  root);
        parse_schema(root);
        parse_input( root);
        parse_output(root);
        parse_view(  root);
        parse_box(   root);
        parse_query( root);
        parse_delete(root);    // Should this be first or last???

        DEBUG << "</borealis>";
    }
    catch(AuroraException  &e)
    {   DEBUG << "Failed parsing a diagram from XML:\n   " << e;
        Throw(AuroraBadEntityException, e.as_string());
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Parse inputs in XML into a catalog structure.
//
void  Diagram::parse_input(const DOMElement   *root)
                     throw(AuroraBadEntityException)
{
    vector<DOMElement *>            input;
    vector<DOMElement *>::iterator  iterator;

    string    stream;
    string    schema;
    string    message;
//
// <input stream={stream name} schema={schema name} />
//..............................................................................


    try
    {   xml_child_elements(input, root, "input");

        for (iterator = input.begin(); iterator != input.end(); ++iterator)
        {
            ///////////////////////// use add_input_element???
            // Parse the stream name and enpoint.
            //
            xml_expect_tag( *iterator, "input" );
            stream = xml_attribute(*iterator, "stream", ATTR_REQUIRED);
            schema = xml_attribute(*iterator, "schema", ATTR_NOT_REQUIRED);

            if (!xml_attribute(*iterator, "node", ATTR_NOT_REQUIRED).empty())
            {   message = "The node attribute is not allowed on borealis input elements.";
                DEBUG << message;
                Throw(AuroraBadEntityException, message);
            }

            lowercase(schema);
            lowercase(stream);

            DEBUG << "<input  stream=\"" << stream
                  << "\" schema=\"" << schema << "\"/>";

            add_stream(stream, schema);
            ///////////////////////

            add_input(stream);
         }
    }
    catch(AuroraException  &e)
    {   message = "Failed parsing an input from XML:\n   " + e.as_string();
        DEBUG << message;

        Throw(AuroraBadEntityException, message);
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Parse outputs in XML into a catalog structure.
//
void  Diagram::parse_output(const DOMElement  *root)
                      throw(AuroraBadEntityException)
{
    vector<DOMElement *>            output;
    vector<DOMElement *>::iterator  iterator;

    string    stream;
    string    schema;
    string    message;
//
//  <output  stream={stream name}  schema={schema name} />
//
// This construct is only used by the Marshal program.
// Until schema inferencing is implemented, output schemas need to be declared.
// Even then we may want to retain this element so that marshaling code can be
// generated for incomplete networks.
// The endpoint attribute is retained for legacy Xml that does not use
// deployment Xml.
// For all other purposes this element has been replaced by the subscribe
// and unsubscribe deployment elements.
//..............................................................................


    try
    {   xml_child_elements(output, root, "output");

        for (iterator = output.begin(); iterator != output.end(); ++iterator)
        {
            // Parse the stream name and enpoint.
            //
            xml_expect_tag(*iterator, "output");
            stream = xml_attribute(*iterator, "stream",   ATTR_REQUIRED);
            schema = xml_attribute(*iterator, "schema",   ATTR_NOT_REQUIRED);

            lowercase(schema);
            lowercase(stream);

            DEBUG << "<output stream=\"" << stream
                  << "\" schema=\"" << schema << "\"/>";

            add_stream(stream, schema);
        }
    }
    catch(AuroraException  &e)
    {   message = "Failed parsing an output from XML:\n   " + e.as_string();
        DEBUG << message;

        Throw(AuroraBadEntityException, message);
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Parse aliases for arrays in XML into a map.
//
void  Diagram::parse_type(const DOMElement   *root)
                    throw(AuroraBadEntityException)
{
    vector<DOMElement *>            type;
    vector<DOMElement *>::iterator  iterator;
//
//  <type  name={alias name}   array={type(...)}  [size={string size}] />
//..............................................................................


    try
    {   xml_child_elements(type, root, "type");

        for (iterator = type.begin(); iterator != type.end(); ++iterator)
        {   add_type_element(*iterator);
        }
    }
    catch(AuroraException  &e)
    {   DEBUG << e;
        Throw(AuroraBadEntityException, e.as_string());
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Add a single parsed schema into a catalog structure.
//
void  Diagram::add_type_element(const DOMElement  *type)
                          throw(AuroraBadEntityException)
{
    string    message;
    string    name;
    string    array;
    string    size;
//
//  <type  name={alias name}   array={type(...)}  [size={string size}] />
//..............................................................................


    try
    {   xml_expect_tag(type, "type");
        name  = xml_attribute(type, "name",  ATTR_REQUIRED);
        array = xml_attribute(type, "array", ATTR_REQUIRED);
        size  = xml_attribute(type, "size",  ATTR_NOT_REQUIRED);

        lowercase(name);

        if (array.find("(", 0) == string::npos)
        {   message = "Array types require a range.";
            DEBUG << message;

            Throw(AuroraBadEntityException, message);
        }
        else if (!size.empty())
        {   message = "Array types for strings is not yet implemented.";
            DEBUG << message;

            Throw(AuroraBadEntityException, message);
        }
        else if (_array_type.find(name) != _array_type.end())
        {   message = "Duplicate array type declaration:  " + name;
            DEBUG << message;

            Throw(AuroraBadEntityException, message);
        }

        _array_type[name] = array;

        DEBUG  << "<type  name=\"" << name << "\"  array=\"" << array << "\" />";
    }
    catch(AuroraException  &e)
    {   message = "Failed parsing an array type declaration from XML:\n   " + e.as_string();
        DEBUG << message;

        Throw(AuroraBadEntityException, message);
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Parse schemas in XML into a catalog structure.
//
void  Diagram::parse_schema(const DOMElement   *root)
                      throw(AuroraBadEntityException)
{
    vector<DOMElement *>            schema;
    vector<DOMElement *>::iterator  iterator;
//
//  <schema  name={schema name} />
//      <field  name={field name}  type={field type}  [size={string size}]/> ...
//  </schema>
//..............................................................................


    try
    {   xml_child_elements(schema, root, "schema");

        for (iterator = schema.begin(); iterator != schema.end(); ++iterator)
        {   add_schema_element(*iterator);
        }
    }
    catch(AuroraException  &e)
    {   DEBUG << e;
        Throw(AuroraBadEntityException, e.as_string());
    }

    return;
}




////////////////////////////////////////////////////////////////////////////////
//
//  Add a single parsed schema into a catalog structure.
//
CatalogSchema  *Diagram::add_schema_element(const DOMElement  *schema)
                                      throw(AuroraBadEntityException)
{
    vector<DOMElement *>            field;
    vector<DOMElement *>::iterator  item;

    TypeArray::iterator             in_array;

    string    message;
    string    name;
    string    type;
    string    size;

    DataType  data;
    uint8     data_size;
//
//  <schema  name={schema name}  />
//      <field  name={field name}  type={field type}  [size={string size}]/> ...
//  </schema>
//..............................................................................


    try
    {   xml_expect_tag(schema, "schema");
        name = xml_attribute( schema, "name", ATTR_REQUIRED);
        lowercase(name);

        DEBUG  << "<schema  name=\"" << name << "\" >";

        begin_schema(name);
        xml_child_elements(field, schema, "field");

        for (item = field.begin(); item != field.end(); ++item)
        {
            // Parse the field name and type.
            //
            name = xml_attribute(*item, "name", ATTR_REQUIRED);
            type = xml_attribute(*item, "type", ATTR_REQUIRED);
            size = xml_attribute(*item, "size", ATTR_NOT_REQUIRED);

            lowercase(name);
 
            in_array = _array_type.find(type);

            if (in_array != _array_type.end())
            {   type = (*in_array).second;
            }
 
            if (size.empty())
            {   DEBUG << "  <field  name=\"" << name
                      << "\"  type=\"" << type << "\"/>";

                size = "0";
            }
            else
            {   DEBUG << "  <field  name=\"" << name
                      << "\"  type=\"" << type
                      << "\"  size=\"" << size << "\"/>";
            }

            if (type.find("(", 0) == string::npos)
            {   // Add a scalar or string field.
                //
                if (type == "array")
                {   message = "Unknown field type:  array";
                    DEBUG << message;

                    Throw(AuroraBadEntityException, message);
                }

                data_size = data.from_string(type);
                //DEBUG  << "data_size=" << to_string((uint32)data_size);

                if (data_size == 0)
                {   // Add a scalar field with a string type.
                    //
                    add_field(name, data, string_to_int(size));
                }
                else if (size == "0")
                {   // Add a scalar field with a numeric type.
                    //
                    add_field(name, data, data_size);
                }
                else
                {   message = "In field (" + name
                            + ") the size attribute can only be used with a string type.";

                    DEBUG << message;
                    Throw(AuroraBadEntityException, message);
                }
            }
            else
            {   // Add an array field.
                //
                DEBUG << "add_field for an array ..." << type;
                add_field(name, DataType::ARRAY, 4);

                DEBUG << "add_array ...";
                add_array(type, size);
            }
        }

        DEBUG  << "</schema>";
    }
    catch(AuroraException  &e)
    {   message = "Failed parsing a schema from XML:\n   " + e.as_string();
        DEBUG << message;

        Throw(AuroraBadEntityException, message);
    }

    return  _add_schema;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Parse connection point views in XML into a catalog structure.
//
void Diagram::parse_view(const DOMElement   *root)
                   throw(AuroraBadEntityException)
{
    vector<DOMElement *>            view;
    vector<DOMElement *>::iterator  iterator;
//
//  <connection_point_view name={view name} stream={stream name} >
//      <order field={field name} />?
//    ( <size  value={number of tuples} />
//    | <range start={start tuple}  end={end tuple} />
//    )
//      <function name={predication function name} />  (proposed; NYI)
//  </connection_point_view>
//..............................................................................


    try
    {   xml_child_elements(view, root, "connection_point_view");

        for (iterator = view.begin(); iterator != view.end(); ++iterator)
        {   add_view_element(*iterator);
        }
    }
    catch(AuroraException  &e)
    {   DEBUG << e;
        Throw(AuroraBadEntityException, e.as_string());
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Add a single parsed connection point view into a catalog structure.
//
CatalogView  *Diagram::add_view_element(const DOMElement   *view)
                                  throw(AuroraBadEntityException)
{
    const DOMElement    *order, *size, *range, *radius;
    CatalogView         *result;

    string   name;
    string   stream;
    string   field;
    string   start;
    string   end;
    string   rad_val;
    string   message;
//
//  <connection_point_view name={view name} stream={stream name} >
//      <order field={field name} />?
//    ( <size  value={number of tuples} />
//    | <range start={start tuple}  end={end tuple} />
//    )
//  </connection_point_view>
//..............................................................................


    try
    {   xml_expect_tag(view, "connection_point_view");

        name   = xml_attribute(view, "name",   ATTR_REQUIRED);
        stream = xml_attribute(view, "stream", ATTR_REQUIRED);

        lowercase(name);
        lowercase(stream);

        DEBUG << "<connection_point_view name=\"" << name
              << "\"   stream=\"" << stream << "\"/>";

        order = xml_only_tag(view, "order", ATTR_NOT_REQUIRED);

        if ( order )
        {   xml_expect_tag(order, "order");

            field = xml_attribute(order, "field", ATTR_REQUIRED);
            lowercase(field);

            DEBUG << "<order  field=\"" << field << "\"/>";;
        }

        radius = xml_only_tag(view, "radius", ATTR_NOT_REQUIRED);

        if ( radius )
        {   xml_expect_tag(radius, "radius");

            rad_val = xml_attribute(radius, "value", ATTR_REQUIRED);

            DEBUG << "<radius value=\"" << rad_val << "\"/>";;
        }
        else
        {   rad_val = "0";
        }

        size = xml_only_tag(view, "size", ATTR_NOT_REQUIRED);

        if (size)
        {   xml_expect_tag(size, "size");

            start = xml_attribute(size, "value", ATTR_REQUIRED);
            end   = "0";
            DEBUG << "<size  value=\"" << start << "\"/>";;
        }

        range = xml_only_tag(view, "range", ATTR_NOT_REQUIRED);

        if (range)
        {   xml_expect_tag(range, "range");

            start = xml_attribute(range, "start", ATTR_REQUIRED);
            end   = xml_attribute(range, "end",   ATTR_REQUIRED);

            DEBUG << "<range  start=\"" << start
                          << "  end=\"" << end << "\"/>";;
        }

        DEBUG << "</connection_point_view>";
        DEBUG  << "name=(" << name << ")";
        DEBUG  << "stream=(" << stream << ")";

        result = add_view(name, stream, field,
                          string_to_int(start),
                          string_to_int(end),
                          string_to_int(rad_val));

        add_parameter_element(view);
    }
    catch(AuroraException  &e)
    {   message = "Failed parsing a connection point view from XML:\n   " + e.as_string();
        DEBUG << message;

        Throw(AuroraBadEntityException, message);
    }

    return  result;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Parse boxes in XML into a catalog structure.
//
void  Diagram::parse_box(const DOMElement  *root,

                         // Disable adding new In stream connections.
                         // The caller will manage them explicitly.
                         //
                         Boolean   dynamic)
                   throw(AuroraBadEntityException)
{
    vector<DOMElement *>            box;
    vector<DOMElement *>::iterator  iterator;
//
//  <box  name={box name}    type={transform}  [node={IP address}] >/>
//      <in   stream={input stream name}   />  ...
//      <out  stream={output stream name}  />  ...
//      <parameter  name={parameter name}   value={parameter value} />  ...
//  </box>
//..............................................................................


    try
    {   xml_child_elements(box, root, "box");

        for (iterator = box.begin(); iterator != box.end(); iterator++)
        {   add_box_element( *iterator, dynamic );
        }
    }
    catch(AuroraException  &e)
    {   DEBUG << e;
        Throw(AuroraBadEntityException, e.as_string());
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//
//
CatalogBox  *Diagram::add_box_element(const DOMElement  *box,

                                      // Disable adding new In stream connections.
                                      // The caller will manage them explicitly.
                                      //
                                      Boolean   dynamic)
                                throw(AuroraBadEntityException)
{
    vector<DOMElement *>            in, out, parameter, access;
    vector<DOMElement *>::iterator  item;

    string   message;
    string   name;
    string   table;
    string   type;
    string   update;
    string   key;
    string   stream;
    string   value;
    Boolean  single_box_query;
    string   cpview;

    CatalogBox  *add_box = NULL;
//..............................................................................


    try
    {
        xml_expect_tag(box, "box");
        name = xml_attribute(box, "name", ATTR_REQUIRED);
        type = xml_attribute(box, "type", ATTR_REQUIRED);

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
                      parse_in_queue(box, _add_query->get_in_queue()));

        // _add_box->set_box_node(_add_node);  // Inherit the node???
        string node = xml_attribute(box, "node", ATTR_NOT_REQUIRED);

        if (!node.empty())
        {   node = Util::form_endpoint(node, DEFAULT_BOREALIS_PORT);
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

        xml_child_elements(in, box, "in");

        for (item = in.begin(); item != in.end(); item++)
        {
            // Parse the in stream name.
            //
            stream = xml_attribute(*item, "stream", ATTR_REQUIRED);
            lowercase(stream);

            // if there is a view on the input of this box, what is it?
            // we'll create CatalogView after the box is done
            cpview = xml_attribute( *item, "connection_point_view", ATTR_NOT_REQUIRED );   
            lowercase(cpview);
          
            if (cpview != "")
            {
                DEBUG << "  <in  stream=\"" << stream << "\" connection_point_view=\"" << cpview << "\" />";
            }
            else 
            {
                DEBUG << "  <in  stream=\"" << stream << "\" />";
            }

            add_in(stream,
                   parse_in_queue(*item, _add_box->get_box_in_queue()),
                   dynamic);
        }

        xml_child_elements( out, box, "out" );

        for (item = out.begin(); item != out.end(); ++item)
        {
            // Parse the out stream name.
            //
            stream = xml_attribute( *item, "stream", ATTR_REQUIRED );
            lowercase(stream);

            DEBUG << "  <out  stream=\"" << stream << "\"/>";
            add_out(stream);
        }

        add_parameter_element(box);

        xml_child_elements(access, box, "access");

        for (item = access.begin(); item != access.end(); ++item)
        {
            table = xml_attribute(*item, "table", ATTR_REQUIRED);
            lowercase(table);

            DEBUG << "  <access  table=\"" << table << "\"/>";
            add_table(table);
        }

        // iter through in's again to create the views
        // creating that CatalogView if there is a cpview on any "in" stream
        xml_child_elements(in, box, "in");

        for (item = in.begin(); item != in.end(); ++item)
        {
            //    cpview = "";
            // Parse the in stream name.
            //
            stream = xml_attribute( *item, "stream", ATTR_REQUIRED );
            lowercase( stream );
        
            // if there is a view on the input of this box, what is it?
            cpview = xml_attribute( *item, "connection_point_view", ATTR_NOT_REQUIRED );   
            lowercase( cpview );
        
            // add the view
            if ( cpview != "" ) 
            {
                _add_box->set_cpview(stream, cpview);
                add_view(cpview, name, stream);
            }
        }

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



////////////////////////////////////////////////////////////////////////////////
//
//  Parse tables in XML into a catalog structure.
//
void  Diagram::parse_table(const DOMElement   *root)
                     throw(AuroraBadEntityException)
{
    vector<DOMElement *>            table;
    vector<DOMElement *>::iterator  iterator;
//
//  <table  name={table name}   schema="{schema name}" >
//      <key     field="{field name}"  />  ...
//     [<index   field="{field name}"  />  ...]
//
//      <parameter  name={parameter name}   value={parameter value} />  ...
//  </table>
//..............................................................................


    try
    {   xml_child_elements( table, root, "table" );

        for (iterator = table.begin(); iterator != table.end(); ++iterator)
        {
            add_table_element(*iterator);
        }
    }
    catch(AuroraException  &e)
    {   DEBUG << e;
        Throw(AuroraBadEntityException, e.as_string());
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Add a single parsed table into a catalog structure.
//
CatalogTable  *Diagram::add_table_element(const DOMElement  *table)
                                    throw(AuroraBadEntityException)
{
    vector<DOMElement *>            index, parameter;
    vector<DOMElement *>::iterator  item;

    string   message;
    string   name;
    string   schema;
    string   field;
    string   value;
    Boolean  single_table_query;

    CatalogTable  *add_table = NULL;
//
//..............................................................................


    try
    {
        xml_expect_tag( table, "table" );
        name   = xml_attribute( table, "name",   ATTR_REQUIRED );
        schema = xml_attribute( table, "schema", ATTR_REQUIRED );

        lowercase(name);
        lowercase(schema);

        // Detect a table outside a query.
        single_table_query = False;

        if (_add_query == NULL)
        {   // Pass through any defaults???
            begin_query(name);   // Use the table name for the query.
            single_table_query = True;
        }

        begin_table(name, schema);

        _add_table->set_table_node(_add_node);
        DEBUG  << "<table  name=\"" << name << "\"/>";

        xml_child_elements(index, table, "key");

        for (item = index.begin(); item != index.end(); item++)
        {
            // Parse a key name.
            //
            field = xml_attribute( *item, "field", ATTR_REQUIRED );
            lowercase(field);

            DEBUG << "  <key  field=\"" << field << "\"/>";
            // yna -- added for comma separated keys.
            if ( field.find(',') != string::npos )
            {
                vector<string> keys;
                split(field, ',', keys);

                vector<string>::iterator key_it = keys.begin();
                vector<string>::iterator key_end = keys.end();
                for (; key_it != key_end; ++key_it)
                    add_key(*key_it);
            }
            else {
                add_key(field);
            }
        }

        xml_child_elements(index, table, "index");

        for (item = index.begin(); item != index.end(); item++)
        {
            // Parse a field name.
            //
            field = xml_attribute(*item, "field", ATTR_REQUIRED);
            lowercase(field);

            DEBUG << "  <index  field=\"" << field << "\"/>";
            add_index( field );
        }

        add_parameter_element(table);
        add_table = _add_table;

        if ( single_table_query )
        {   end_query();
        }

        DEBUG  << "</table>";
    }
    catch(AuroraException  &e)
    {   message = "Failed parsing a table from XML:\n   " + e.as_string();
        DEBUG << message;

        Throw(AuroraBadEntityException, message);
    }

    return  add_table;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Add parameters to a box or table.
//
void  Diagram::add_parameter_element(const DOMElement  *root)
                               throw(AuroraBadEntityException)
{
    string   message;
    string   name;
    string   value;

    vector<DOMElement *>            index, parameter;
    vector<DOMElement *>::iterator  item;
//
//  Note:  The following parameter values might contain references to case
//         insensitive names.  They are converted to lower case.
//
//            key      sql     group-by     order-on-field
//            expression.<#>   aggregate-function.<#>
//            out-field.<#>    output-expression.<#>
//..............................................................................


    try
    {   xml_child_elements( parameter, root, "parameter" );

        for ( item = parameter.begin(); item != parameter.end(); ++item )
        {
            // Parse a parameter name and value.
            //
            name  = xml_attribute( *item, "name",  ATTR_REQUIRED );
            value = xml_attribute( *item, "value", ATTR_REQUIRED );

            lowercase( name );

            if ((name == "key")  ||  (name == "group-by")  ||
                (name == "sql")  ||  (name == "order-on-field"))
            {
                lowercase(value);
            }
            else if ((name.substr(0, 11) == "expression.")          ||
                     (name.substr(0, 10) == "out-field.")           ||
                     (name.substr(0, 15) == "out-field-name.")      ||
                     (name.substr(0, 19) == "aggregate-function.")  ||
                     (name.substr(0, 18) == "output-expression."))
            {
                //TO DO:  Expressions may contain quoted items.
                //        The quoted items should not be lowercased.
                //
                lowercase(value);
            }

            DEBUG << "  <parameter  name=\"" << name
                  <<  "\"  value=\"" << value << "\"/>";

            add_parameter(name, value);
        }

    }
    catch(AuroraException  &e)
    {   message = "Failed parsing a parameter from XML:\n   " + e.as_string();
        DEBUG << message;

        Throw(AuroraBadEntityException, message);
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Parse modify element to change parameters of an existing box or table.
//  Not yet in use.
//
void  Diagram::parse_modify(const DOMElement   *root)
                      throw(AuroraBadEntityException)
{
    vector<DOMElement *>            modify;
    vector<DOMElement *>::iterator  iterator;
//
//  <modify  (box={box name} | table={table name}) >
//      <parameter name={parameter name}  value={parameter value} />  ...
//  </modify>
//..............................................................................


    try
    {   xml_child_elements(modify, root, "modify");

        for (iterator = modify.begin(); iterator != modify.end(); ++iterator)
        {
            add_modify_element(*iterator);
        }
    }
    catch(AuroraException  &e)
    {   DEBUG << e;
        Throw( AuroraBadEntityException, e.as_string());
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Modify parameters of an existing box or table.
//
void  Diagram::add_modify_element(const DOMElement  *modify)
                                  throw(AuroraBadEntityException)
{
    string   box;
    string   table;
    string   message;

    //string   stream;
    //string   port;

    //uint16   index;
    //BoxPort  box_port;

    //CatalogStream      *catalog_stream;

    //vector<DOMElement *>            in;
    //vector<DOMElement *>::iterator  item;
//
//..............................................................................


    xml_expect_tag(modify, "modify");

    box   = xml_attribute(modify, "box",   ATTR_NOT_REQUIRED);
    table = xml_attribute(modify, "table", ATTR_NOT_REQUIRED);

    // Access the box or table to be modified.
    //
    if (box.empty())
    {   if (table.empty())
        {   Throw(AuroraBadEntityException,
                  "Modify requires a Box or Table.");
        }
        else
        {   modify_table_node(Name(table));
        }
    }
    else if (table.empty())
    {   modify_box_node(Name(box));
    }
    else
    {   Throw(AuroraBadEntityException,
              "Can not modify both a Box and Table.");
    }

    add_parameter_element(modify);        // Modify any given parameters.

    /************************** moved to RegionalCatalog???
    // Modify any given in streams.
    //
    try
    {   xml_child_elements(in, modify, "in");
 
        for (item = in.begin(); item != in.end(); item++)
        {
            stream = xml_attribute(*item, "stream", ATTR_REQUIRED);
            port   = xml_attribute(*item, "port",   ATTR_NOT_REQUIRED);

            if (port.empty())
            {   port = "0";
            }

            index = atoi(port.c_str());

            // Modify the Box In stream for a given port.
            //
            DEBUG << "box=" << box << " stream=" << stream << " port=" << port;
            catalog_stream = get_stream(Name(stream));

            // If downstream only then the stream needs to be added.
            //
            if (!catalog_stream)
            {   DEBUG << "add stream=" << stream;

                add_stream(stream,
                           _add_box->get_in_schema(index)->get_schema_name());
            }

            _add_box->reset_in_stream(index, catalog_stream);

            // Remove the old stream???

            // Record the sink connection.
            // What about remote_sink???
            //
            box_port.set_box_port(_add_box, index);
            catalog_stream->add_sink_port(box_port);
        }
    }
    catch(AuroraException &e)
    {   message = "Failed parsing modify xml:\n   " + e.as_string();
        DEBUG << message;

        Throw(AuroraBadEntityException, message);
    }
    **********************/

    close_prior();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Parse streams in XML into a catalog structure.
//
void  Diagram::parse_query(const DOMElement   *root)
                     throw(AuroraBadEntityException)
{
    vector<DOMElement *>            query;
    vector<DOMElement *>::iterator  iterator;
//
//..............................................................................


    try
    {   xml_child_elements(query, root, "query");

        for (iterator = query.begin(); iterator != query.end(); iterator++)
        {   add_query_element(*iterator);
        }
    }
    catch(AuroraBadEntityException  &e)
    {   DEBUG << e;
        Throw(AuroraBadEntityException, e.as_string());
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Add a single parsed query into a catalog structure.
//
CatalogQuery  *Diagram::add_query_element(const DOMElement  *query,
                                          Boolean dynamic)
                                    throw(AuroraBadEntityException)
{
    CatalogQuery   *result;

    string    message;
    string    name;
    string    node;
    string    type;
    string    in_queue;
//
//  <query  name={query name}  [updatequeue={0 | 1}]  
//                             [queuetype={0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8}]  
//                             [key={field name}] />
//     ( box+ | input* | output* | schema* | type* | table* | view* )
//  </query>
//..............................................................................


    try
    {   xml_expect_tag(query, "query");
        name = xml_attribute( query, "name", ATTR_REQUIRED );

        DEBUG  << "<query  name=\"" << name << "\">";
        lowercase(name);
        begin_query(name);

        // Inherit default queue parameters from borealis.
        //
        _add_query->set_in_queue(parse_in_queue(query, _in_queue));

        parse_type(  query);
        parse_schema(query);
        parse_input( query);
        parse_output(query);
        parse_table( query);
        parse_box(   query, dynamic);
        parse_view(  query);

        result = _add_query;
        end_query();
        _add_node = "";

        DEBUG  << "</query>";
    }
    catch(AuroraException  &e)
    {   message = "Failed parsing a query from XML:\n   " + e.as_string();
        DEBUG << message;

        Throw(AuroraBadEntityException, message);
    }

    return  result;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Add a single parsed input stream into a catalog structure.
//
CatalogStream  *Diagram::add_input_element(const DOMElement  *input)
                                     throw(AuroraBadEntityException)
{
    CatalogStream   *result;

    string    message;
    string    stream;
    string    schema;
//
//  <input  stream={stream name} schema={schema name} />
//..............................................................................


    // Parse the stream and schema names.
    //
    try
    {   xml_expect_tag(input, "input");
        stream = xml_attribute(input, "stream", ATTR_REQUIRED);
        lowercase(stream);

        schema = xml_attribute(input, "schema", ATTR_REQUIRED);
        result = add_stream(stream, schema);

        DEBUG << "<input  stream=\"" << stream
                  << "\"  schema=\"" << schema << "\" />";
    }
    catch(AuroraException  &e)
    {   message = "Failed parsing an input from XML:\n   " + e.as_string();
        DEBUG << message;

        Throw(AuroraBadEntityException, message);
    }

    return  result;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Parse XML to delete queries from a catalog structure.
//
void  Diagram::parse_delete(const DOMElement   *root)
                      throw(AuroraBadEntityException)
{
    vector<DOMElement *>            element;
    vector<DOMElement *>::iterator  iterator;

    string   name;
    string   message;
//
//  <delete  query={query name} />
//..............................................................................


    try
    {   xml_child_elements(element, root, "delete");

        for (iterator = element.begin(); iterator != element.end(); iterator++)
        {   // remove all boxes and the query from the catalog.
            name = xml_attribute(*iterator, "query", ATTR_REQUIRED);

            DEBUG  << "NYI:  <delete  query=\"" << name << "\" />";
        }
    }
    catch(AuroraException  &e)
    {   message = "Failed parsing a delete from XML:\n   " + e.as_string();
        DEBUG << message;

        Throw(AuroraBadEntityException, message);
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Parse XML to parse update queue and key attributes.
//
CatalogBox::InQueue  Diagram::parse_in_queue(const DOMElement    *root,

                                             // Default queue options.
                                             CatalogBox::InQueue  in_queue)
                                       throw(AuroraBadEntityException)
{
    string      update_queue;
    string      queue_type;
    string      key;
    string      message;

    CatalogBox::InQueue  result;
//
//..............................................................................


    try
    {   update_queue = xml_attribute(root, "update_queue", ATTR_NOT_REQUIRED);

        // The default "*" is used to signal inheritance when there is no type.
        //
        queue_type = xml_attribute(root, "queue_type", "*");

        // The default "*" is used to signal inheritence when there is no key.
        //
        key = xml_attribute(root, "key", "*");
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


////////////////////////////////////////////////////////////////////////////////
//
// Compose a complete cononical endpoint:  <dotted ip address>:<port>
//
string  Diagram::form_endpoint(// An enpoint as:  [<host>][:[<port>]]
                               string   endpoint,

                               // A default port number.
                               uint16   Port)
{
    string    form;
    string    host;
    string    mixed = "";

    uint16    port;
    uint16    delta = 15;

    string::size_type  index;
//
// The default port is a static assignment for now.
// It could be assigned by scanning available channels???
//..............................................................................


    if (_deploy_local)
    {   port  = Port;                                // Assume the default port.
        host  = endpoint;                            // Assume just a host.
        index = endpoint.find(":", 0);               // See if an optional port.

        if (index != string::npos)
        {   host = endpoint.substr(0, index);        // Host before the colon.

            if (index != endpoint.size() - 1)
            {   port = atoi(endpoint.substr(         // Extract a port number.
                                            index + 1,
                                            endpoint.size() ).c_str());
            }
        }

        if ((host == "127.0.0.1") || host.empty())
        {   host = "localhost";                      // Canonical local host.
        }

        while (True)
        {   string  &map = _local_endpoint[port];

            if (map.empty())
            {   map = host;
                break;
            }
            else if (map == host)
            {   if (!mixed.empty())
                {   WARN << "Assuming " << host << " is not " << mixed << ".";
                }

                break;
            }

            port += delta;

            if ((!is_dotted_host(host))  ||
                (!is_dotted_host(map)))
            {
                if (mixed.empty())
                {   mixed = map;
                }
                else
                {   mixed += " or " + map;
                }
            }
        }

        form = Util::get_host_address("")            // Dotted local address
             + ":" + to_string(port);                // and port number.

        DEBUG << "Using local address:  " << form;
    }
    else
    {   form = Util::form_endpoint(endpoint, Port); // Dotted host address.
    }

    return  form;
}



////////////////////////////////////////////////////////////////////////////////
//
//  See if a host name is a non-local dotted IP address.
//
Boolean  Diagram::is_dotted_host( string  host )
{
    Boolean   dotted = False;
    string::size_type  index;
//
//  Just looks for 3 dots in the name.  Should also see if they are integers.
//..............................................................................


    if (host != "127.0.0.1")
    {   index = host.find(".", 0);

        if (index != string::npos)
        {   index = host.find( ".", index + 1 );

            if (index != string::npos)
            {   index = host.find(".", index + 1);

                if (index != string::npos)
                {   dotted = True;
                }
            }
        }
    }

    return  dotted;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Automatically fill in deployment for a network.
//
Status  Diagram::generate_deployment()
{
    Status   status = true;
    string   endpoint;

    CatalogSubscription        subscription;
    StreamMap::const_iterator  stream;
    QueryMap::iterator         query;
//..............................................................................


    for (query  = _topology._query_map.begin();
         query != _topology._query_map.end(); ++query)
    {
        _add_node = (query->second).get_query_node();

        if (_add_node.empty())
        {
            // Locate query boxes and tables on the local host.
            //
            _add_node = Util::form_endpoint("", DEFAULT_BOREALIS_PORT);
            locate_query(query->first);
            _add_node = "";
        }
    }

    DEBUG << "stream_map = " << _topology._stream_map.size();

    for (stream  = get_stream_map()->begin();
         stream != get_stream_map()->end(); stream++)
    {

        if ((*stream).second.is_input_stream())
        {   add_publish(to_string((*stream).first),
                        form_endpoint("", DEFAULT_BOREALIS_PORT ));

            DEBUG << "generate publish for " << (*stream).first;
        }
        else if ( (*stream).second.is_output_stream() )
        {   endpoint = form_endpoint("", DEFAULT_MONITOR_PORT);

            subscription = add_subscribe(to_string((*stream).first),
                                         endpoint, 0xFFFF);

            get_stream((*stream).first)->
                subscribe_sink(get_subscriber(InetAddress(endpoint),
                                              (*stream).first));

            DEBUG << "generate subscribe for " << (*stream).first;
        }
    }

    return  status;
}


BOREALIS_NAMESPACE_END

//
////////////////////////////////////////////////////////////////////////////////
