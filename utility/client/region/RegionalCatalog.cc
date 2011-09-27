#include  "RegionalCatalog.h"
#include  "rpc_QueryProcessor.h" 
#include  "MedusaClient.h"
#include  "parseutil.h"

#include  <sstream>

BOREALIS_NAMESPACE_BEGIN



////////////////////////////////////////////////////////////////////////////////
//
//  Parse an Update XML element into the Catalog.
//
Status  RegionalCatalog::parse_update_xml(string  update_xml)
{
    string   tag;
    string   name;
//
//    <region  node={borealis node} />
//    <subscribe  stream={output name}  endpoint={monitor}   [gap={size}] />
//    <input      stream={input name}   schema={schema name} [node=<subscriber>] />
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
//       [<function name={predication function name} />]  (proposed; NYI)
//    </connection_point_view>
//
//    NYI:   <delete  box={box list} />
//    HACK:  <delete  box=<drop box name>  stream=<new stream> />
//
//    <move  (box={box list} | table={table list})  node={destination} />
//
//    <modify  (box={box name} | table={table name}) >
//         <parameter name={parameter name}  value={parameter value} /> ...
//    </modify>
//
//    <modify  box={box name} >
//         <in stream={new stream} port={port index} />
//    </modify>
//
//    <disconnect  stream={new stream name} />
//
//    <connect (box={destination box}
//            [port={zero based destination port}]  [upstream={0 | 1}])
//
//        <box name={box to insert}  type={box type} >
//            <in   stream={existing source stream} />
//            <out  stream={new destination stream} />
//                    :
//        </Box>
//    </connect>
//
//    <loaded/>
//    <reset/>
//..............................................................................


    NOTICE << update_xml;
    Status  status = parse_element(update_xml);

    if (status)
    {   tag = to_string(_root->getTagName());

        try
        {   if (tag == "schema")
            {   // Unless already in the catalog, add the schema.  When
                // moving boxes, schemas may be re-deployed within a region.
                //
                name = xml_attribute(_root, "name", ATTR_REQUIRED);

                if (!get_schema(name))
                {   add_schema_element(_root);   // Contains field elements.
                }
            }
            else if (tag == "input")
            {   add_input_element(_root);
            }
            else if (tag == "box")
            {   add_box_element(_root, False); 
                // Contains Box and Table elements.
            }
            else if (tag == "connection_point_view")
            {   add_view_element(_root);
            }
            else if (tag == "region")
            {   add_region_element(_root);
            }
            else if (tag == "delete")
            {   delete_element(_root);
            }
            else if ( tag == "move" )
            {   move_element(_root);
            }
            else if (tag == "subscribe")
            {   add_subscribe_element(_root);

                // Commenting this out until subscriber reflection is
                // properly implemented. Right now, it only works for
                // diagram outputs. -- tatbul
                /*
                CatalogSubscription subscription = add_subscribe_element(_root);

                CatalogStream *stream = get_stream(
                            Name(subscription.get_subscription_streamName()));

                assert(stream);

                CatalogBox *box = stream->get_stream_source_box();
                assert(box);

                string node = box->get_box_node();

                subscription.setSubscription_endpoint(InetAddress(node));
                addSubscriber(subscription);

                stream->subscribe_source(
                    get_subscriber(subscription.get_subscription_endpoint(),
                                  subscription.get_subscription_stream_name()));
                */
            }
            else if (tag == "connect")
            {   connect_element(_root);
            }
            else if (tag == "disconnect")
            {   disconnect_element(_root);
            }
            else if (tag == "modify")
            {   modify_element(_root);
            }
            else if (tag == "loaded")
            {   // Signal a global catalog has finished loading.
                //
                _loaded = True;
            }
            else if (tag == "reset")
            {   // Clear the entire catalog so it can try to reload.
                //
                clear_catalog();
            }
            else
            {   status = "Unknown update element:  " + tag;
            }
        }
        catch(AuroraBadEntityException  &e)
        {   DEBUG << "Failed parsing update xml:" << e;
            status = e.as_string();
        }
        catch(AuroraException  &e)
        {   DEBUG << e;
            status = e.as_string();
        }
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Add an input stream or new subscription to a regional catalog.
//
void  RegionalCatalog::add_input_element(const DOMElement  *input)
                                    throw(AuroraBadEntityException)
{
    string   stream;
    string   schema;
//
//    <input  stream={stream name} schema={schema name} [node=<subscriber>] />
//..............................................................................


    if (xml_attribute(input, "node", ATTR_NOT_REQUIRED).empty())
    {   Diagram::add_input_element(_root);
    }
    else
    {   // An input with a node is a subscription.  The node is ignored for now.
        //
        xml_expect_tag(input, "input");
        stream = xml_attribute(input, "stream", ATTR_REQUIRED);
        lowercase(stream);

        schema = xml_attribute(input, "schema", ATTR_REQUIRED);

        // Add only if this is the first subscription.
        //
        if (!get_stream(Name(stream)))
        {   
            add_stream(stream, schema);
            DEBUG << "add  stream=" << stream << "  schema=" << schema;
        }
    }

    return; 
}



////////////////////////////////////////////////////////////////////////////////
//
//  Add a single parsed region update into a regional catalog structure.
//
void  RegionalCatalog::add_region_element(const DOMElement  *Region)
                                    throw(AuroraBadEntityException)
{
    string   node;
//
//     <region  node={borealis node} />
//..............................................................................


    xml_expect_tag(Region, "region");
    node = xml_attribute(Region, "node", ATTR_REQUIRED);

    if (is_dom_attribute_present(*Region, "endpoint"))
    {   Throw(AuroraBadEntityException,
              "Region endpoint attribute not allowed in update XML.");
    }
    else
    {   DEBUG  << "<region  node=\"" << node << "\">";
        _node.insert(node);
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Delete a single parsed box and/or stream from a regional catalog structure.
//
void  RegionalCatalog::delete_element(const DOMElement  *Delete)
                                throw(AuroraBadEntityException)
{
    string  name;
//
//  Delete drop box:
//     <delete  box=<drop box name>  stream=<new stream> />
//..............................................................................


    xml_expect_tag(Delete, "delete");

    if (is_dom_attribute_present(*Delete, "box"))
    {   name = xml_attribute(Delete, "box", ATTR_REQUIRED);
        // box  = getBox( name );

        if (name != "")
        {   remove_box(name);
            DEBUG << "delete a box:  " << name;
        }
    }

    if (is_dom_attribute_present(*Delete, "stream"))
    {   name = xml_attribute(Delete, "stream", ATTR_REQUIRED);
        // stream = getStream( name );

        DEBUG << "delete a stream:  " << name;
        //Assert:  the new stream must have no sinks.
        //Delete the new stream
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//
//  Delete a single box if the target node is out of the region.
//  Otherwise move the box within the region.
//
void  RegionalCatalog::move_element(const DOMElement  *move)
                              throw(AuroraException)
{
    string  box_name;
    string  table_name;
    string  node;
//
//  Delete box:  Ignored if the box is currently not on the source node.
//     <move  (box={box name} | table={table name})  node={destination} />
//
//  If the _node set is empty then this is a global catalog and
//  all nodes are included.
//..............................................................................


    xml_expect_tag(move, "move");

    box_name   = xml_attribute(move, "box",   ATTR_NOT_REQUIRED);
    table_name = xml_attribute(move, "table", ATTR_NOT_REQUIRED);
    node       = xml_attribute(move, "node",  ATTR_REQUIRED);

    if (box_name.empty())
    {   if (table_name.empty())
        {   Throw(AuroraException, "Move requires a Box or Table.");
        }
        else if ((!_node.empty())  &&  (_node.find(node) == _node.end()))
        {   // remove_table(Name(table_name));
            ERROR << "NYI:  Delete a table moved out of a region:  "
                  << table_name;
        }
        else
        {   modify_table_node(Name(table_name), node);
            close_prior();
        }
    }
    else if (table_name.empty())
    {   if ((!_node.empty())  &&  (_node.find(node) == _node.end()))
        {   remove_box(Name(box_name));
            DEBUG << "Delete a box moved out of a region:  " << box_name;
        }
        else
        {   modify_box_node(Name(box_name), node);
            close_prior();
        }
    }
    else
    {   Throw(AuroraBadEntityException, "Can not move both a Box and Table.");
    }

    return;
}




////////////////////////////////////////////////////////////////////////////////
//
//  Reconnect a box port to a different stream when adding a drop box.
//
void RegionalCatalog::connect_element(const DOMElement *connect)
                                throw(AuroraException)
{
    string    message;
    string    add_box;         // Box being inserted.
    string    new_stream;      // New stream from insert to destination box.
    string    dest_box;        // Destination box.
    string    dest_port;       // Index to the In port on the destination box.
    string    connect_xml;
    string    modify_xml;
    string    node;            // Endpoint for the downstream node.

    uint16    port = 0;
    Name      old_stream;      // Existing stream from source to insert box.

    CatalogStream   *catalog_stream;   // Entry for both streams.
    CatalogBox      *catalog_box;      // Entry for the destination box.
    CatalogBox      *catalog_add;      // Entry for the new box.
    BoxPort         *box_port;

    vector<DOMElement *>  box;
    vector<DOMElement *>  out;
    vector<DOMElement *>  in;

    vector<BoxPort>           *sink_port;
    vector<BoxPort>::iterator  sink;

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
//    Note that the upstream node may be outside the region.
//    Add inherited _update_queue and _key???
//..............................................................................


    xml_expect_tag(connect, "connect");

    // The local node needs the new stream and the destination box and port.
    // We could potentially connect multiple boxes at once here???
    //
    xml_child_elements(box, connect, "box");

    if (box.size() != 1)
    {   message = "Can only connect one box.";
        DEBUG << message;
        Throw(AuroraException, message);
    }

    add_box = xml_attribute(*box.begin(), "name", ATTR_REQUIRED);
    xml_child_elements(out, *box.begin(), "out");

    new_stream = xml_attribute(*out.begin(), "stream", ATTR_REQUIRED);
    dest_box   = xml_attribute(connect,      "box",    ATTR_NOT_REQUIRED);

    if (dest_box.empty())
    {   // Upstream portion of an upstream  connect:
        //     <connect  node=<downstream node> ...
        //
        // A modify will follow from the downstream node if it is also
        // in the region.  It will connect the new stream to the destination.

        // This is the data address for the downstream node.
        //
        node = xml_attribute(connect, "node", ATTR_REQUIRED);
        DEBUG << "Upstream connect; downstream node=" << node;

        // Add the filter box and the new stream.
        //
        catalog_add = Diagram::add_box_element(*box.begin(), true);

        // The old streams sink to the old box should be changed to the new box.
        // We don't know the destination box so we can't locate the arc.
        // If the destination box is in the region (both in) then use:
        //    find_sink_box_port(CatalogBox  *box, uint16  port)
        //
        // If it is not in this region (up only) then just use the first empty sink.
        //
        catalog_stream = catalog_add->get_in_stream(0);

        DEBUG << "old stream=" << catalog_stream->get_stream_name()
                 << " sinks: " << catalog_stream->get_sink_port()->size();

        // Find the sink connected to a box with the matching node.
        // There is only one arc to the downstream node so this is it.
        //
        sink_port = catalog_stream->get_sink_port();

        for (sink = sink_port->begin(); sink != sink_port->end(); sink++)
        {
            catalog_box = sink->get_port_box();
            InetAddress box_address(catalog_box->get_box_node());

            string box_node = to_string(
                InetAddress(box_address.get_host(),
                            box_address.get_port() + DATA_PORT_OFFSET));

            DEBUG << "destination box=" << catalog_box->get_box_name()
                           << "  node=" << box_node;

            if (box_node == node)
            {   DEBUG << "hit destination box=" << catalog_box->get_box_name();

                // Set the sink to the inserted box at port 0.
                //
                sink->set_box_port(catalog_add, 0);
                node = "";   // Signal the arc to the destination box was found.
                break;
            }
        }

        // If the downstream node is not in the region, there will be no sink
        // for the stream, so we add one to the old stream.
        //
        if (!node.empty())
        {  BoxPort  bp;
           bp.set_box_port(catalog_add, 0);
           DEBUG << "Downstream node not in region; Add a sink to the old stream.";
           catalog_stream->add_sink_port(bp);
        }
    }
    else
    {   // Local connect.
        //    <connect  box=<destination box>  [port=<destination port>] ...
        //
        dest_port = xml_attribute(connect, "port", ATTR_NOT_REQUIRED);

        if (!dest_port.empty())
        {   port = atoi(dest_port.c_str());
        }

        // Reference the destination box and the existing stream.
        //
        catalog_box = get_box(Name(dest_box));

        if (!catalog_box)
        {   message = "Box \"" + dest_box
                    + "\" to be connected is not in the region.";

            DEBUG << message;
            Throw(AuroraException, message);
        }

        // Get the existing stream via the In on the destination box.
        //
        old_stream     = catalog_box->get_in_stream(port)->get_stream_name();
        catalog_stream = get_stream(old_stream);

        if (!catalog_stream)
        {   message = "Can not find old stream (" + to_string(old_stream) + ").";
            DEBUG << message;
            Throw(AuroraException, message);
        }

        DEBUG << "new_stream=" << new_stream
              << "  dest_box=" << dest_box   <<  "  dest_port=" << port;

        DEBUG << "sink size=" << catalog_stream->get_sink_port()->size();


        ////////////////////////////////////////////////////////////////////
        // Everything has been parsed and validated; now connect the box.
        //..................................................................

        // First add the new box and the new stream.
        //
        catalog_add = Diagram::add_box_element(*box.begin(), true);
        DEBUG << "old stream sinks=" << catalog_stream->get_sink_port()->size();


        // Look for the existing stream`s sink connected to the destination.
        //
        box_port = catalog_stream->find_sink_box_port(catalog_box, port);

        if (!box_port)
        {   message = "old stream=" + to_string(old_stream)
                    + " has no sink to box " + dest_box
                    + " at port=" + port;

            DEBUG << message;
            Throw(AuroraException, message);
        }

        // Reset the old stream's sink to the inserted box at In port 0.
        box_port->set_box_port(catalog_add, 0);

        // Add the sink port for the new stream.
        //
        BoxPort  bp;
        bp.set_box_port(catalog_box, port);

        catalog_stream = get_stream(new_stream);
        catalog_stream->add_sink_port(bp);

        DEBUG << "new stream sinks=" << catalog_stream->get_sink_port()->size();

        // Reset the destination box's In port to the new stream.
        //
        //(*(catalog_box->get_box_in()))[port] = catalog_stream;
        catalog_box->reset_in_stream(port, catalog_stream);
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  The downstream node reflects a modify In on a connect and disconnect.
//  Box and table parameters may be modified.
//
void RegionalCatalog::modify_element(const DOMElement *modify)
                               throw(AuroraBadEntityException)
{
    string   box;
    string   message;
    string   stream;
    string   schema;

    Name     old_stream;
    string   port;

    uint16   index;
    BoxPort  box_port;

    CatalogBox        *destination;
    CatalogStream     *catalog_stream;

    vector<DOMElement *>            in;
    vector<DOMElement *>::iterator  item;
//
//    <modify  (box={box name} | table={table name}) >
//         <parameter name={parameter name}  value={parameter value} /> ...
//    </modify>
//
//    <modify  box={box name} >
//         <in stream={new stream} port={port index} />
//    </modify>
//..............................................................................


    xml_expect_tag(modify, "modify");
    xml_child_elements(in, modify, "in");

    if (in.begin() == in.end())
    {   add_modify_element(_root);        // Parse parameter changes in Diagram.
    }
    else
    {   box = xml_attribute(modify, "box", ATTR_NOT_REQUIRED);

        if (box.empty())
        {   Throw(AuroraBadEntityException, "Modify In requires a Box.");
        }

        destination = get_box(Name(box));

        if (!destination)
        {   Throw(AuroraBadEntityException,
                  "Modify target box is not cataloged:  " + box);
        }

        // Modify any given In streams.
        //
        for (item = in.begin(); item != in.end(); item++)
        {
            stream = xml_attribute(*item, "stream", ATTR_REQUIRED);
            port   = xml_attribute(*item, "port",   ATTR_NOT_REQUIRED);

            if (port.empty())
            {   port = "0";
            }

            index = atoi(port.c_str());
            DEBUG << "box=" << box << " new stream=" << stream << " port=" << index;

            // Modify the Box In stream for a given port.
            //
            catalog_stream = get_stream(Name(stream));

            if (!catalog_stream)
            {   // Only the downstream node is in the region.
                //
                DEBUG << "add new stream=" << stream;

                catalog_stream = destination->get_in_stream(index);

                if (!catalog_stream)
                {   Throw(AuroraBadEntityException,
                          "No existing In stream on the destination box.");
                }

                old_stream = catalog_stream->get_stream_name();
                schema     = catalog_stream->get_schema_name();

                if (schema.empty())
                {   Throw(AuroraBadEntityException,
                          "No schema for the existing In stream="
                             + to_string(old_stream));
                }

                catalog_stream = add_stream(stream, schema);

                // Remove the old stream.
                //
                old_stream = destination->get_in_stream(index)->get_stream_name();
                DEBUG << "Remove old stream:  " << old_stream;

                get_stream_map()->erase(old_stream);
            }

            DEBUG << "reset new stream=" << catalog_stream->get_stream_name()
                            << "  port=" << index;

            destination->reset_in_stream(index, catalog_stream);

            // Record the sink connection on the new stream.
            //
            box_port.set_box_port(destination, index);
            catalog_stream->add_sink_port(box_port);
        }
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Reconnect a box port to it's original stream when removing an inserted box.
//
void RegionalCatalog::disconnect_element(const DOMElement *disconnect)
                                   throw(AuroraException)
{
    string    message;
    string    add_box;         // Box being inserted.
    string    new_stream;      // New stream from insert to destination box.
    string    dest_box;        // Destination box.
    string    dest_port;       // Index to the In port on the destination box.
    string    connect_xml;
    string    modify_xml;

    uint16    port = 0;

    CatalogStream   *catalog_stream;   // Entry for both streams.
    CatalogBox      *catalog_dest;     // Entry for the destination box.
    CatalogBox      *catalog_add;      // Entry for the new box.
    BoxPort         *box_port;

//
//  <disconnect  stream={insert stream name} />
//
//  Hmmm If the inserted box was up and out then we have:
//
//     [Source]--old-->[Insert]-->||---new-->[Destination]
//                                ||
//     [Source]--old------------->||---old-->[Destination]
//
//  We'd have to go to the upstream node and ask or extend the XML???
//  Do local disconnect and remote disconnect differ???
//     In the local case modify is not done.
//..............................................................................


    xml_expect_tag(disconnect, "disconnect");

    new_stream = xml_attribute(disconnect, "stream", ATTR_REQUIRED);
    DEBUG << "<disconnect  stream=\"" << new_stream << "\" />";

    catalog_stream = get_stream(Name(new_stream));

    if (!catalog_stream)
    {   message = "Can't disconnect stream (" + new_stream
                + ") as it is not in the regional catalog.";

        DEBUG << message;
        Throw(AuroraException, message);
    }

    // Inserted filter box to be removed.
    //
    catalog_add = catalog_stream->get_stream_source_box();

    if (!catalog_add)
    {   message = "No box found in the regional catalog to disconnect.";

        DEBUG << message;
        Throw(AuroraException, message);
    }

    add_box = to_string(catalog_add->get_box_name());

    DEBUG << "Disconnect box (" << add_box
          << ") and stream ("    << new_stream << ").";


    ////////////////////////////////////////////////////////////////////
    // Everything has been parsed and validated; now disconnect the box.
    //..................................................................

    if (catalog_stream->is_output_stream())
    {   
        // A remote disconnect where only the upstream node is in the region.
        // The old stream may have other sinks.
        //
        catalog_stream = catalog_add->get_in_stream(0);
        DEBUG << "Upstream only:  Erase the last sink on the old stream.";

        // Validate that the added sink is at the end and 
        // connected to the insert box at port 0.
        //
        vector<BoxPort>  *sink_port;
        sink_port = catalog_stream->get_sink_port();

        if (catalog_stream->get_sink_box_port(sink_port->size() - 1)
                          ->get_port_box() != catalog_add)
        {
            message = "The last sink is not connected to the inserted box.";
            DEBUG << message;
            Throw(AuroraException, message);
        }

        sink_port->pop_back();
    }
    else if (xml_attribute(disconnect, "upstream", ATTR_NOT_REQUIRED).empty())
    {
        // A local disconnect if no upstream attribute.
        // For remote disconnects this will be done by the modify.

        // Destination box and port number.
        //
        box_port = catalog_stream->get_sink_box_port(0);

        if (box_port)
        {   catalog_dest = box_port->get_port_box();

            if (!catalog_dest)
            {   message = "Missing destination box on a local disconnect.";
                DEBUG << message;
                Throw(AuroraException, message);
            }

            port = box_port->get_port();

            DEBUG << "destination box=" << catalog_dest->get_box_name()
                  <<          "  port=" << port;
        }
 
        // Look for the old stream`s sink connected to the insert box.
        //
        catalog_stream = catalog_add->get_in_stream(0);
        box_port = catalog_stream->find_sink_box_port(catalog_add, 0);

        if (!box_port)
        {   message = "old stream=" + catalog_stream->get_stream_name_string()
                    + " has no sink to box " + add_box
                    + " at port=0";

            DEBUG << message;
            Throw(AuroraException, message);
        }

        // Then reset the old stream`s sink to the destination box at the port.
        //
        box_port->set_box_port(catalog_dest, port);

        // Reset the destination box's In port to the old stream.
        //
        catalog_dest->reset_in_stream(port, catalog_stream);
    }
         
    // Remove the inserted box and the added stream.
    // Regional components wrap each box in a query (for now).
    //
    get_query_map()->erase(Name(add_box));
    get_box_map()->erase(Name(add_box));

    // Delete the added stream.
    //
    DEBUG << "Remove added stream:  " << new_stream;
    get_stream_map()->erase(Name(new_stream));

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Virtual override to add only new boxes; ignore existing boxes.
//
CatalogBox  *RegionalCatalog::add_box_element(const DOMElement  *box,
                                                       Boolean   dynamic)
                                        throw(AuroraBadEntityException)
{
    string  name;
    string  type;

    BoxMap::iterator  in_box;
    CatalogBox       *add_box = NULL;
//..............................................................................


    xml_expect_tag(box, "box");
    name = xml_attribute(box, "name", ATTR_REQUIRED);
    type = xml_attribute(box, "type", ATTR_REQUIRED);

    in_box = _topology._box_map.find(name);

    if (in_box == _topology._box_map.end())
    {   add_box = Diagram::add_box_element(box, dynamic);
    }

    return  add_box;
}



////////////////////////////////////////////////////////////////////////////////
//
// Clear the entire catalog.
//
void  RegionalCatalog::clear_catalog()
{
//..............................................................................


    _topology._schema_map.clear();
    _topology._query_map.clear();
    _topology._stream_map.clear();
    _topology._box_map.clear();
    _topology._table_map.clear();

    _view_map.clear();

    _overlay._publisher_map.clear();
    _overlay._subscription_map.clear();
    _overlay._subscriber_map.clear();
    _overlay._direct_subscription_map.clear();
    _overlay._stream_states_map.clear();
    _overlay._local_stream_states_map.clear();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
string RegionalCatalog::as_string()
{
    ostringstream oss;
//..............................................................................


    oss << "================== Regional Catalog =================" << endl;
    oss << "------------- nodes in region -------------" << endl;

    for (set<string>::const_iterator it = _node.begin();
         it != _node.end();
         ++it)
    {
        oss << *it << endl;
    }

    oss << Catalog::as_string();
    oss << "=====================================================" << endl;

    return  oss.str();
}



////////////////////////////////////////////////////////////////////////////////
//
//  Called by a regional component to insert a filter box on a stream.
//
Status  RegionalCatalog::connect_box(/// XML for a box with one in and one out stream.
                                     string   box_xml,

                                     /// Box to receive the output.
                                     Name     target_box,

                                     /// In port on the target box (default 0).
                                     uint16   target_port,

                                     /// True to always deploy the box on the downstream node.
                                     Boolean  downstream)
{
    string    target_node;
    string    source_node = "";

    string    old_stream;
    string    new_stream;

    string    xml;
    uint16    target_sink = 0;

    RPC<void>             rpc;

    const DOMElement     *box;
    vector<DOMElement *>  stream;

    CatalogStream        *source_stream;
    CatalogBox           *source_box;
    CatalogBox           *sink_box;
    vector<BoxPort>      *sink;

    vector<BoxPort>::iterator  port;
//
//  Determines if the filter should be inserted locally or upstream.
//  The filter is inserted upstream if the source box is on an upstream node
//  and there is only one sink for the selected arc to the destination box.
//..............................................................................


    Status  status = parse_element(box_xml);
    DEBUG << "parse_element status=" << status;

    if (status)
    {   box = get_element();

        try
        {   xml_expect_tag(box, "box");

            // bad if no out???
            xml_child_elements(stream, box, "out");
            new_stream = xml_attribute( *stream.begin(), "stream", ATTR_REQUIRED );
        }
        catch(AuroraException  &e)
        {   status = "Failed parsing a box from XML:\n   " + e.as_string();
            DEBUG << status;
        }
    }

    if (status)
    {   sink_box = get_box(target_box);

        if (!sink_box)
        {   status = "Could not access the target box:  "
                   + to_string(target_box);
        }
    }

    if (status)
    {   // validate target_port???
        target_node = sink_box->get_box_node();
        old_stream  = sink_box->get_in_stream(target_port)->get_stream_name_string();

        // if target.empty() -> assume local???

        DEBUG << "old=" << old_stream << "  add=" << new_stream
                                   << "  target=" << target_node;

        source_stream = get_stream(Name(old_stream));

        if (!source_stream)
        {   status = "Could not find the source stream (" + old_stream + ").";
        }
    }

    if (status  &&  !downstream)
    {   source_box = source_stream->get_stream_source()->get_port_box();

        if (source_box)
        {   source_node = source_box->get_box_node();
            DEBUG << "source=" << source_node;
        }

        if (source_node != target_node)
        {   // Count the number of source stream sinks on the target node.
            //
            sink = source_stream->get_sink_port();

            for (port = sink->begin(); port != sink->end(); port++)
            {   source_box = (*port).get_port_box();

                if (source_box)
                {   if (source_box->get_box_node() == target_node)
                    {   target_sink++;

                        if (target_sink == 2)
                        {   break;
                        }
                    }
                }
            }
        }
    }

    if (status)
    {   DEBUG << "source=" << source_node << "  target=" << target_node << "  sink=" << target_sink;

        // May want to avoid reconnecting the client each time???
        ptr<MedusaClient> client = (ptr<MedusaClient>) new MedusaClient(target_node);

        Remote<QueryProcessor> query_processor =
                                    client->qp_for(MedusaID(target_node));

        xml = "<connect  box=\"" + to_string(target_box)
            +      "\"  port=\"" + target_port + "\"";

        if ((source_node != target_node)  &&  (target_sink == 1))
        {  xml += " upstream=\"1\"";
        }

        xml += " >\n" + box_xml + "</connect>\n";

        DEBUG << "connect xml=" << xml;
        rpc = query_processor.add_xml_string(xml);

        DEBUG << "add_xml_string status=" << rpc;
        status = rpc.stat();
    }
    
    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Called by a regional component to remove an inserted filter box.
//
Status  RegionalCatalog::disconnect_stream(// Added stream to disconnect.
                                           Name     stream,

                                           // Original stream if it is upstream.
                                           // Empty if a local connect.
                                           Name     upstream)
{
    Status          status = true;
    string          target_node;
    string          xml;
    RPC<void>       rpc;

    CatalogStream  *catalog_stream;
    CatalogBox     *box;
    BoxPort        *box_port;
//
// When using the connect_box wrapper the caller may not know if the
// filter has been placed on the upstream node.  In this case the
// upstream parameter must be provided.   If an upstream parameter is given
// and the filter was placed downstream it will be ignored.
//..............................................................................


    catalog_stream = get_stream(stream);

    if (!catalog_stream)
    {   status = "Could not find the source stream ("
               + to_string(stream) + ").";
    }

    if (status)
    {   box_port = catalog_stream->get_sink_box_port(0);

        if (!box_port)
        {   status = "No box port";
        }
    }

    if (status)
    {   box = box_port->get_port_box();

        if (!box)
        {   status = "No box";
        }
    }

    if (status)
    {   target_node = box->get_box_node();

        DEBUG << "stream=" << stream << "  target=" << target_node
              <<  "  box=" << box->get_box_name();

        // May want to avoid reconnecting the client each time???
        ptr<MedusaClient> client = (ptr<MedusaClient>) new MedusaClient(target_node);

        Remote<QueryProcessor> query_processor =
                                client->qp_for(MedusaID(target_node));

        // Does the input stream need to be suspended first???
        //
        xml = "<disconnect  stream=\"" + to_string(stream);

        if (upstream)
        {   xml += "\"  upstream=\"" + to_string(upstream);
        }

        xml += "\" />\n";
        DEBUG << "disconnect xml=" << xml;

        rpc = query_processor.add_xml_string(xml);

        DEBUG << "add_xml_string " << xml << ": " << rpc;
        status = rpc.stat();
    }

    DEBUG << "status=" << status;
    
    return  status;
}


BOREALIS_NAMESPACE_END
