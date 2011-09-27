#include <NMSTL/tcprpc>
#include <NMSTL/text>

#include  "HeadServer.h"

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
//  Parse a Borealis Network Diagram represented as an XML string into the
//  global catalog and deploy it.
//
RPC<void>  HeadServer::deploy_xml_string(// A string containing an XML network diagram.
                                         string   xml_string)
{
// This adds the item and then deploys it.  THis might not be desireable.
// We usually want to make several modifications then deploy them.
// This works by parsing into the catalog, scanning it and the deploying
// what has not been deployed.  This is too complex.
// Instead we could just pass items through to the in the order given.
// Maybe there should be an update list of items to transact.
//..............................................................................


    DEBUG << "Parsing update:  " << xml_string;
    Status  status = _global_catalog.parse_string( xml_string );

    if  (!status)
    {    WARN << "Could not parse: " << xml_string;
    }
    else if (!_global_catalog.is_borealis_diagram())
    {   status = _global_catalog.deploy();     // Deploy deployment strings only.

        if (!status)
        {   WARN << "Could not deploy: " << xml_string;
        }
    }

    if (status) return  true;
    return  RPCFault(status);
}



////////////////////////////////////////////////////////////////////////////////
//
//  Parse a Borealis Network Diagram represented as an XML file into the
//  global catalog and deploy it.
//
RPC<void>  HeadServer::deploy_xml_file(// File containing an XML Borealis network diagram.
                                       string    xml_file)
{
//..............................................................................


    DEBUG << "Parsing file:  " << xml_file;
    Status  status = _global_catalog.parse_file(xml_file);

    if  (!status)
    {    WARN << "Could not parse: " << xml_file;
    }
    else if (!_global_catalog.is_borealis_diagram())
    {   status = _global_catalog.deploy();     // Deploy deployment files only.

        if (!status)
        {   WARN << "Could not deploy: " << xml_file;
        }
    }

    if (status)  return  true;
    return  RPCFault(status);
}



////////////////////////////////////////////////////////////////////////////////
//
//  Process update Xml received from a Borealis node or a Global Component.
//
RPC<void>  HeadServer::add_xml_string(// An update XML element.
                                      string   update_xml)
{
    Status  status = true;

    string  tag;
    string  endpoint;

    ptr<DOMDocument>   doc;              // Parsed document.
    const DOMElement  *root;             // Parsing root.
//
//  This is used to reflect changes initiated by Borealis nodes.
//  It needs to be modified to add stuff to the set of deployed items.
//
//     <modify  (box={box name} | table={table name}) >
//             <parameter name={parameter name}  value={parameter value} />
//     </modify>
//
//     <move  (box={box name} | table={table name})  node={destination} />
//
//     <global  endpoint={Global Component endpoint} />
//
//     <delete  global={Global Component endpoint} />   NYI
//..............................................................................


    DEBUG << "Parsing update:  " << update_xml;

    try
    {   doc  = parseXmlString(update_xml);
        root = doc->getDocumentElement();
        tag  = to_string(root->getTagName());

        if (tag == "modify")
        {   send_catalog_changed();
            _global_catalog.add_modify_element(root);
        }
        else if (tag == "move")
        {   send_catalog_changed();
            status = add_move_element(root);

            if (status)
            {   status = reflect_global_update(update_xml);
            }
        }
        else if (tag == "global")
        {   status = add_global_element(root);
        }
        else if (tag == "delete")
        {   xml_expect_tag(root, "delete");
            endpoint = xml_attribute(root, "global",  ATTR_REQUIRED);
            endpoint = Util::form_endpoint( endpoint, DEFAULT_GLOBAL_PORT );

            status = "NYI:  Disconnect a Global Component:  " + endpoint;
        }
        else
        {   status = "Unknown update element:  " + tag;
        }
    }
    catch(AuroraBadEntityException  &e)
    {   DEBUG << "Could not digest update xml:" << e;
        status = e.as_string();
    }
    catch(AuroraException  &e)
    {   DEBUG << "Failed parsing update xml:" << e;
        status = e.as_string();
    }

    if (status)  return  true;
    WARN << status;
    return  RPCFault(status);
}



////////////////////////////////////////////////////////////////////////////////
//
// Change the location of a box in the global catalog.
//
Status  HeadServer::add_move_element(const DOMElement  *move)
{
    Status  status = true;
    string  box_name;
    string  table_name;
    string  node;
//..............................................................................


    try
    {   xml_expect_tag(move, "move");

        box_name   = xml_attribute(move, "box",   ATTR_NOT_REQUIRED);
        table_name = xml_attribute(move, "table", ATTR_NOT_REQUIRED);
        node       = xml_attribute(move, "node",  ATTR_REQUIRED);

        if (box_name.empty())
        {   if (table_name.empty())
            {   status = "Move requires a Box or Table.";
            }
            else
            {   _global_catalog.modify_table_node(Name(table_name), node);
            }
        }
        else if (table_name.empty())
        {   _global_catalog.modify_box_node(Name(box_name), node);
        }
        else
        {   status = "Can not move both a Box and Table.";
        }

        if (status)
        {   _global_catalog.close_prior();
        }
    }
    catch(AuroraBadEntityException  &e)
    {   DEBUG << "Could not digest move xml:" << e;
        status = e.as_string();
    }
    catch(AuroraException  &e)
    {   DEBUG << "Failed parsing move xml:" << e;
        status = e.as_string();
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
// Accept a request to feed the global catalog to a global component.
//
Status  HeadServer::add_global_element(const DOMElement  *global)
{
    int         code;
    Status      status = true;
    string      endpoint;
    pthread_t   thread;
    
//..............................................................................


    try
    {   xml_expect_tag(global, "global");
        endpoint = xml_attribute(global, "endpoint", ATTR_REQUIRED);
        endpoint = Util::form_endpoint( endpoint,  DEFAULT_GLOBAL_PORT );

        // If already in use, tell the thread to wait.
        //
        if (_send_catalog)
        {   status = "NYI:  Only one Global Component can load a catalog at a time.";
            DEBUG << status;
        }
        else
        {   DEBUG << "endpoint = " << endpoint;

            // Connect to the global component.
            //
            _global_client = new UpdateClient(InetAddress(endpoint),
                                              "GlobalServer");

            // Mark the catalog in use by a copy.
            //
            _send_catalog = SEND_RUNNING;


            // Launch a thread to copy the catalog.
            //
            code = pthread_create(&thread, 0,
                                  (void*(*)(void*))send_global_catalog,
                                  (void*)this);

            if (code)
            {   _send_catalog = SEND_CLEAR;
                status = "Can not start the send_global_catalog thread.";
                DEBUG << "<" << code << ">  " << status;
            }
        }
    }
    catch(AuroraBadEntityException  &e)
    {   DEBUG << "Could not digest global xml:" << e;
        status = e.as_string();
    }
    catch(AuroraException  &e)
    {   DEBUG << "Failed parsing global xml:" << e;
        status = e.as_string();
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
// Thread to send an initial global catalog to a Global Component.
//
void  HeadServer::send_global_catalog(// This function is static so it can be
                                      // run as a thread.  The server object
                                      // is passed in so it may be accessed.
                                      //
                                      HeadServer  *server)
{
    Status   status = true;

    CatalogSchema::SchemaMap           *schema_map;
    CatalogSchema::SchemaMap::iterator  schema;

    Catalog::StreamMap                 *stream_map;
    Catalog::StreamMap::iterator        stream;

    Catalog::TableMap                  *table_map;
    Catalog::TableMap::iterator         table;

    Catalog::BoxMap                    *box_map;
    Catalog::BoxMap::iterator           box;

    Catalog::ViewMap                   *view_map;
    Catalog::ViewMap::iterator          view;

    vector<CatalogSubscription*>           *sub_out;
    vector<CatalogSubscription*>::iterator  subscription;
//
//  Entry:  _send_catalog = SEND_RUNNING
//
//   Exit:  _send_catalog = SEND_CLEAR
//..............................................................................


    DeployDiagram   &global_catalog = server->get_global_catalog();
    DEBUG << "send the global catalog...";

    while (server->_send_catalog != SEND_CLEAR)
    {   status = true;                         // Get a fresh start.

        // Send all the schemas first.
        //
        schema_map = global_catalog.get_schema_map();
        DEBUG << "send " << schema_map->size() << " schemas ...";

        for (schema = schema_map->begin(); schema != schema_map->end(); schema++)
        {
            if (!status) break;
            status = send_update_xml(server, schema->second.form_schema_xml());
        }

        // Send inputs before tables and boxes as they may reference inputs.
        //
        stream_map = global_catalog.get_stream_map();
        DEBUG << "send " << stream_map->size() << " streams ...";

        for (stream = stream_map->begin(); stream != stream_map->end(); stream++)
        {
            if (!status) break;

            if (stream->second.is_input_stream())
            {
                status = send_update_xml(server, stream->second.form_input_xml());
            }
        }

        table_map  = global_catalog.get_table_map();
        DEBUG << "send " << table_map->size() << " tables ...";

        for (table = table_map->begin(); table != table_map->end(); table++)
        {
            if (!status) break;
            status = send_update_xml(server, table->second.form_table_xml());
        }

        // Send boxes after tables as they may access a table.
        //
        box_map  = global_catalog.get_box_map();
        DEBUG << "send " << box_map->size() << " boxes ...";

        for (box = box_map->begin(); box != box_map->end(); box++)
        {
            if (!status) break;
            status = send_update_xml(server, box->second.form_box_xml());
        }

        // Subscribe to outputs after tables and boxes as the schema needs infering.
        //
        stream_map = global_catalog.get_stream_map();
        DEBUG << "send " << stream_map->size() << " streams ...";

        for (stream = stream_map->begin(); stream != stream_map->end(); stream++)
        {
            if (!status) break;

            if (stream->second.is_output_stream())
            {
                // Outputs are sent as subscribe's for update XML.
                //
                sub_out = stream->second.get_subscribers();

                for (subscription  = sub_out->begin();
                     subscription != sub_out->end(); subscription++)
                {
                    status = send_update_xml(server,
                                             (*subscription)->form_subscribe_xml());

                    if (!status) break;
                }
            }
        }

        // Send views after all streams as they may reference any stream.
        //
        view_map = global_catalog.get_view_map();
        DEBUG << "send " << view_map->size() << " views ...";

        for (view = view_map->begin(); view != view_map->end(); view++)
        {
            if (!status) break;
            status = send_update_xml(server, view->second.form_view_xml());
        }

        if (status)
        {   status = send_update_xml(server, "<loaded />");

            if (status)
            {   // Success; now exit.
                //
                server->_send_catalog = SEND_CLEAR;
            }
        }

        if (!status  &&  server->_send_catalog != SEND_CLEAR)
        {   server->_send_catalog = SEND_RUNNING;
            status = send_update_xml(server, "<reset />");

            DEBUG << "The catalog send was interrupted:  " << status;

            if (!status)
            {   ERROR << "The global component could not be reset.";

                // Give up and exit.
                //
                server->_send_catalog = SEND_CLEAR;
            }
        }
    }

    // Finished sending the catalog.
    //
    NOTICE << "... done sending the global catalog:  " << status; 

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
Status  HeadServer::send_update_xml(// The server object is passed in so it
                                    // may be accessed statically.
                                    //
                                    HeadServer  *server,

                                    // A single Update XML element.
                                    //
                                    string       update_xml)
{
    Status     status = true;
    RPC<void>  rpc;
//
// This function is static so it can be called from within the
// send_global_catalog thread.  It is also used to reflect updates
// to global components by reflect_global_update.
//..............................................................................


    DEBUG << "send update_xml=" << update_xml;

    if (server->_send_catalog == SEND_CHANGED)
    {   status = "The catalog changed while being sent.";
        NOTICE << status;
    }
    else
    {   rpc = server->get_global_client()->add_xml_string(update_xml);
        NOTICE << "add_xml_string " << update_xml << ": " << rpc;
        status = rpc.stat();

        if (!status)
        {   if (server->get_global_client()->is_connected())
            {   DEBUG << "still connected." << status;
            }
            else
            {   DEBUG << "disconnected." << status;
                server->disconnect_global_client();
            }
        }
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
//  A catalog update is imminent, stop sending.
//
void  HeadServer::send_catalog_changed()
{
//..............................................................................


    if (_send_catalog == SEND_RUNNING)
    {
        // Mark it changed only while sending; otherwize leave it clear.
        //
        _send_catalog = SEND_CHANGED;
    }

    return;
}




////////////////////////////////////////////////////////////////////////////////
//
// Reflect a catalog update to any global components.
//
Status  HeadServer::reflect_global_update(string  update_xml)
{
    Status     status = true;
//..............................................................................


    if (_global_client)
    {   if (_send_catalog == SEND_CLEAR)
        {   status = send_update_xml(this, update_xml);
        }
        else if (_send_catalog == SEND_RUNNING)
        {   // If sending out the catalog, tell the thread to restart.
            //
            FATAL << "Need to call send_catalog_changed before the update.";
        }
    }

    return  status;
}



BOREALIS_NAMESPACE_END

//--- Added by nmstl-rpcgen; do not modify.  Leave at end of file!
#define NMSTL_RPC_DEFINE
#include "rpc_HeadServer.h"
#undef NMSTL_RPC_DEFINE
//---
