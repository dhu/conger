#include  <fstream>

#include  "GenerateMarshal.h"


BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
// Create methods for the marshaling code.
//
Status  GenerateMarshal::generate_body(// Relative path of the target file.
                                       string  file)
{
    ofstream  body;
    Status    status = true;
//..............................................................................


    NOTICE << "generate: " << file;
    body.open(file.c_str());

    if (body)
    {   status = body_top(body);

        if (status)
        {   body_publish_client(body);
            body_subscribe(body);
            body_bottom(body);
        }

        body.close();
    }
    else
    {   status = "Couldn't open the body file:  " + file;
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
// Determine the ip and port for an Input or Output stream.
//
void  GenerateMarshal::stream_address( // Stream endpoint:  [<ip address>][:<port>]
                                      string   endpoint,

                                      // Node to which the stream is deployed.
                                      string  &ip,

                                      // Port used to access the stream.
                                      string  &port
                                    )
{
    string   default_ip("localhost");
    string::size_type    index;
//..............................................................................


    ip    = default_ip;                     // Assume the default IP.
    port  = endpoint;
    index = endpoint.find( ":", 0 );

    if ( index == 0 )                             // IF :<port>
    {   port  = endpoint.substr( 1, endpoint.size() - 1 );
    }
    else if ( index == string::npos )             // ELSE IF <ip>
    {   ip   = endpoint;
        port = to_string(DEFAULT_BOREALIS_PORT);
    }
    else                                          // ELSE <ip>:<port>
    {
        ip   = endpoint.substr(0, index);
        port = endpoint.substr(index + 1, endpoint.size() - index - 1);
    }

    DEBUG << "IP of endpoint is:  " << ip << ":" << port;

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Write the top common portion of the marshaling method file.
//
Status  GenerateMarshal::body_top(// An open method file output stream.
                                  ofstream  &body)
{
    string   client_ip;
    string   client_port;
    string   name;
    string   upper;
    Status   status = true;

    

    set<CatalogSubscription *>::iterator    output;
    map<string, GeneratePublish>::iterator  publish;

    CatalogStream        *stream;
//..............................................................................


    body << "#include  \"" << _prefix << "Marshal.h\"\n";
    body << "#include  \"util.h\"\n\n";

    name = _prefix;
    uppercase(name);

    body << "#define " << name << "_XML     \"" << _xml_file << "\"\n";
    body << "#define MAX_BUFFER      640000000\n\n";

    client_ip = _monitor;

    if (_deploy_local)
    {
        stream_address(_monitor, client_ip, client_port);
        client_ip = "localhost:" + client_port;
    }

    body << "#define " << name << "_ENDPOINT     \"" << client_ip << "\"\n\n";


    // Only Publish streams need an associated node.
    // Subcribe uses the client listener node???
    //
    for (publish  = _generate_publish.begin();
         publish != _generate_publish.end(); publish++)
    {
        if (publish->second._prefix == _prefix)
        {   name = to_string(publish->first);
            upper  = name;
            uppercase(upper);

            body << "#define " << upper << "  \"" << name << "\"\n";
            //publish = &(stream->second);
            // If empty infer nodes from boxes???
        }
    }


    for (output  = _subscribe.begin();
         output != _subscribe.end(); output++)
    {
        stream = (*output)->get_subscription_stream();
        name   = to_string(stream->get_stream_name());
        upper  = name;
        uppercase(upper);

        body << "#define " << upper << "  \"" << name << "\"\n";
    }

    body << "\n";
    body << BOX_TOP;
    body << "// Generated marshaling code for the " << _prefix << " program.\n";
    body << BOX_BOTTOM;

    body << BOX_TOP;
    body << "//  Subscribe to input and output streams.\n";
    body << "//\n";
    body << "int32  " << _prefix << "Marshal::open()\n";
    body << "{\n";
    body << "    int32   status;\n";
    body << BOX_BOTTOM;

    open_client(body);
    listen_stream(body);

    body << "    //  Launch the front-end with the xml file.\n";
    body << "    //  Creates a client that communicates with a borealis node at ip\n";
    body << "    //  though the given port.\n";
    body << "    //  Then it sends the network diagram as XML strings\n";
    body << "    //  and runs start_query.\n";
    body << "    //  It waits until the front-end terminates and the port is now free.\n";
    body << "    //\n";

    name = _prefix;
    uppercase(name);
    body << "    status = launchDiagram(" << name << "_XML);\n\n";

    body << "    if (status)\n";
    body << "    {   ERROR << \"launchDiagram failed ( \" << status << \" ).\";\n";
    body << "    }\n";

    connect_stream(body, True);

    body << "\n";
    body << "    return  status;\n";
    body << "}\n\n\n\n";


    body << BOX_TOP;
    body << "//  Connect to inputs, subscribe to outputs.\n";
    body << "//\n";
    body << "void  " << _prefix << "Marshal::openInputOutput()\n";
    body << "{\n";
    body << BOX_BOTTOM;

    open_client(body);
    listen_stream(body);
    connect_stream(body);

    body << "    return;\n";
    body << "}\n\n\n\n";


    body << BOX_TOP;
    body << "//  Activate the client I/O event loop.\n";
    body << "void  " << _prefix << "Marshal::runClient()\n";
    body << "{\n";
    body << BOX_BOTTOM;

    body << "    //  This does not return unless terminated or on an exception.\n";
    body << "    _client->run();\n\n";

    body << "    return;\n";
    body << "}\n\n\n\n";


    body << BOX_TOP;
    body << "//  Terminate the client I/O event loop.\n";
    body << "void  " << _prefix << "Marshal::terminateClient()\n";
    body << "{\n";
    body << BOX_BOTTOM;

    body << "    _client->terminate();\n\n";

    body << "    return;\n";
    body << "}\n\n\n\n";


    body << BOX_TOP;
    body << "//  Copy a string value to a fixed length array and zero fill.\n";
    body << "//\n";
    body << "void  " << _prefix << "Marshal::setStringField(string  value,\n";
    body << "                                       char  field[],\n";
    body << "                                     uint32  length)\n";
    body << "                              throw(AuroraException)\n";
    body << "{\n";
    body << BOX_BOTTOM;

    body << "    if (value.length() > length)\n";
    body << "    {   Throw(AuroraException,\n";
    body << "              \"Protocol string over \" + to_string(length) + \".\");\n";
    body << "    }\n\n";

    body << "    strncpy(field, value.c_str(), length);\n\n";

    body << "    return;\n";
    body << "}\n\n\n\n";

    return  status;
}


////////////////////////////////////////////////////////////////////////////////
//
//  Write the method body to create a client and connect the first node.
//
void GenerateMarshal::open_client(ofstream   &body)
{
    string   endpoint = "";
    CatalogBox  *box;
//..............................................................................


    if (_subscribe.empty())
    {   endpoint = *(_generate_publish.begin()->second._node.begin());
    }
    else
    {   box = (*_subscribe.begin())->get_subscription_stream()
                                   ->get_stream_source()->get_port_box();
        if (box)
        {   endpoint = box->get_box_node();
        }
        else
        {   // If no box then use localhost default.
            //
            endpoint = Util::form_endpoint("", DEFAULT_MONITOR_PORT);
        }
    }

    // Note that the MedusaClient no longer needs to be connected to any node.
    //
    body << "    // Open a client to send and receive data.\n";
    body << "    //\n";
    body << "    _client = new MedusaClient(InetAddress());\n\n";;

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Write the method body to listen for outputs.
//
void GenerateMarshal::listen_stream(ofstream   &body)
{
    string   name;

    CatalogStream  *stream;
    set<CatalogSubscription *>::iterator   output;
//..............................................................................


    body << "    // Subscribe to outputs.\n";
    body << "    //\n";

    for (output  = _subscribe.begin();
         output != _subscribe.end(); output++ )
    {
        stream = (*output)->get_subscription_stream();
        name   = to_string(stream->get_stream_name());

        DEBUG << "name=" << name;
        proper_case(name);
        body << "    subscribe" << name << "();\n";
    }

    body << "\n"; 

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Write the method body to connect inputs and subscribe to outputs.
//
void GenerateMarshal::connect_stream(// An open method file output stream.
                                     ofstream  &body,

                                     // If True, generate an else statement.
                                     Boolean    else_clause)
{
    string   name;
    Boolean  hit = False;
    map<string, GeneratePublish>::iterator  publish;
//..............................................................................


    for (publish  = _generate_publish.begin();
         publish != _generate_publish.end(); publish++)
    {
        if (publish->second._prefix == _prefix)
        {
            if (!hit)
            {   if (else_clause)
                {   body << "    else\n";
                }

                body << "    {   // Establish input data path connections.\n";
                hit = True;
            }

            name = to_string(publish->first);
            proper_case( name );

            body << "        connect" << name << "();\n";
        }
    }

    if (hit)
    {   body << "    }\n";
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Write marshaling methods to connect each input on a client.
//
void GenerateMarshal::body_publish_client(// An open method file output stream.
                                          ofstream  &body )
{
    string   name;

    map<string, GeneratePublish>::iterator  publish;
//..............................................................................


    for (publish  = _generate_publish.begin();
         publish != _generate_publish.end(); publish++ )
    {
        if (publish->second._prefix == _prefix)
        {   name   = publish->first;
            body_publish(body, name, &publish->second._node);
        }
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Write marshaling methods to connect a single published input stream.
//
void GenerateMarshal::body_publish(// An open method file output stream.
                                   ofstream  &body,

                                   // Stream name (proper case).
                                   string     name,

                                   // Publish declaration.
                                   vector<string>   *node)
{
    string   upper;
    string   tuple;
    string   node_ip;
    string   node_port;
    Boolean  hit = False;

    CatalogStream  *stream;
    SchemaArray    *array;

    vector<string>::iterator       endpoint;
    vector<SchemaField>::iterator  f;
//..............................................................................


    stream = get_stream(name);  // Must not be null???
    proper_case(name);

    tuple = to_string(stream->get_schema_name());
    tuple = _schema_mixed[tuple];
    DEBUG  << "publish " << name << " for " << tuple;

    upper = name;
    uppercase(upper);

    body << BOX_TOP;
    body << "void  " << _prefix << "Marshal::connect" << name << "()\n";
    body << "{\n";
    body << BOX_BOTTOM;

    // Do over each node receiving the input stream.
    //
    for (endpoint = node->begin(); endpoint != node->end(); endpoint++)
    {   stream_address(*endpoint, node_ip, node_port);

        if (_deploy_local)
        {   node_ip = "localhost";
        }
  
        body << "    // Starting to produce events on input stream.\n";
        body << "    //\n";
        body << "    if (!_client->set_data_path(MAX_BUFFER, Util::get_host_address(\"" << node_ip << "\"), " << node_port << "))\n";
        body << "    {   ERROR << \"Failed setting data path\";\n";
        body << "    }\n";
        body << "    else\n";
        body << "    {   DEBUG << \"Set data path\";\n\n";

        // When there are multiple nodes per input, should PACKET be unique???
        body << "        _event" << name << " = ptr<StreamEvent>(new StreamEvent(" << upper << "));\n";
        body << "        _event" << name << "->_inject = true;\n";
        body << "    }\n\n";
    }

    body << "    return;\n";
    body << "}\n\n\n\n";

    body << BOX_TOP;
    body << "void  " << _prefix << "Marshal::batch" << name << "( " << name << "  *tuple";

    // For each variable sized array field, add a max parameter.
    //
    vector<SchemaField>  &field = stream->get_stream_schema()->get_schema_field();

    for (f = field.begin(); f != field.end(); f++)
    {
        array = f->get_array();

        if (array)
        {   hit = True;

            if (array->is_variable())
            {
                DEBUG << "variable array name=" << f->get_name()
                       << " size=" << array->array_size();

                body << ", int32  max_" << f->get_name();
            }
        }
    }

    body << " )\n";
    body << "{\n";

    if (hit)
    {   body << "    int32  size;\n";
    }

    body << BOX_BOTTOM;


    // For each array field, copy the array data into the event.
    //
    for (f = field.begin(); f != field.end(); f++)
    {
        array = f->get_array();

        if (array)
        {   DEBUG << "array name=" << f->get_name()
                       << " size=" << array->array_size();

            body << "    // Inject the array data into the event.\n";
            body << "    //\n";
            body << "    size = ";

            if (array->is_variable())
            {   vector<SchemaArray::DimensionInformation>  &dimension = array->get_all_dimensions();

                body << "(max_" << f->get_name() << " + "
                     << 1 - dimension[0]._lower  << ") * ";
            }

            body << array->array_size() << ";\n";

            body << "    _event"
                 << name << "->insert_array(string((const char *)tuple->_data."
                         << f->get_name() << ", size));\n\n";

            body << "    // Replace the array pointer with the array size in bytes.\n";
            body << "    //\n";

            body << "    tuple->_data." << f->get_name()
                 << " = (uint8  *)size;\n\n";
        }
    }


    body << "    // Tuples are buffered in a string.\n";
    body << "    //\n";
    body << "    _event" << name << "->insert_bin(string((const char *)tuple,\n";
    body << "                                  sizeof(" << name << ")));\n\n";

    body << "    return;\n";
    body << "}\n\n\n\n";

    body << BOX_TOP;
    body << "void  " << _prefix << "Marshal::send" << name << "( uint32  sleep )\n";
    body << "{\n";
    body << BOX_BOTTOM;

    body << "    // Transmit data to the node.\n";
    body << "    Status  status = _client->fast_post_event(_event" << name << ");\n\n";

    body << "    while (!status)\n";
    body << "    {   if (status.as_string() == DataHandler::NO_SPACE)\n";
    body << "        {   // Wait if no more space in buffer.\n";
    body << "            // At this point the data was never put in the buffer.\n";
    body << "            // It needs to be requeued unless we want to drop it.\n";
    body << "            WARN << \"We dropped a tuple.\";\n";
    body << "            Thread::sleep(Time::msecs(sleep));\n\n";

    body << "            // retry (make this conditional).\n";
    body << "            status = _client->fast_post_event(_event" << name << ");\n";
    body << "        }\n";
    body << "        else\n";
    body << "        {   ERROR << \"Connection closed... stopping event stream\";\n";
    body << "            return;\n";
    body << "        }\n";
    body << "    }\n\n";

    body << "    if (sleep)\n";
    body << "    {    // The event loop is activated so that the queue can be processed.\n";
    body << "         // The callback is enqueued with a timer.\n";
    body << "         // We only callback with a timer because this is looping.\n";
    body << "         // We also need a delayed callback so the queue can be processed.\n";
    body << "         // If we just go to sleep the event loop will not be run.\n";
    body << "         //\n";
    body << "         (new CallbackTimer(_client->get_loop(),\n";
    body << "                             wrap(this, &" << _prefix << "Marshal::delay" << name << ")))\n";
    body << "                  ->arm(Time::now() + Time::msecs(sleep));\n\n";
    body << "    }\n\n";

    body << "    return;\n";
    body << "}\n\n\n\n";

    body << BOX_TOP;
    body << "//  Resume here after sending a tuple.\n";
    body << "//\n";
    body << "void  " << _prefix << "Marshal::delay" << name << "()\n";
    body << "{\n";
    body << BOX_BOTTOM;

    body << "    // Release the previous event.\n";
    body << "    //\n";
    body << "    _event" << name << ".reset();\n\n";

    body << "    // Construct a new " << name << " input event.\n";
    body << "    //\n";
    body << "    _event" << name << " = ptr<StreamEvent>(new StreamEvent(" << upper << "));\n";
    body << "    _event" << name << "->_inject = true;\n\n";

    body << "    // Return to the application code.\n";
    body << "    //\n";
    body << "    sent" << name << "();\n\n";

    body << "    return;\n";
    body << "}\n\n\n\n";

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Write marshaling methods to subscribe to each output.
//
void GenerateMarshal::body_subscribe(// An open method file output stream.
                                     //
                                     ofstream  &body )
{
    string   name;
    string   upper;
    string   tuple;
    Boolean  hit;

    CatalogStream  *stream;
    SchemaArray    *array;

    set<CatalogSubscription *>::iterator   output;

    vector<SchemaField>::iterator  f;
//..............................................................................



    // Define the global output handler's body.
    //
    body << BOX_TOP;
    body << "// Dispatch output on our fast datapath to a handler.\n";
    body << "//\n";
    body << "Status  " << _prefix << "Marshal::outputHandler(ptr<StreamEvent>  event)\n";
    body << "{\n";
    body << BOX_BOTTOM;

    for (output  = _subscribe.begin();
         output != _subscribe.end(); output++ )
    {
        stream = (*output)->get_subscription_stream();
        name   = to_string(stream->get_stream_name());

        proper_case(name);

        upper = name;
        uppercase(upper);

        body << "    if (event->_stream == Name(" << upper << "))\n";
        body << "    {   return " << _prefix << "Marshal::" << name << "Handler(event);\n";
        body << "    }\n\n";
    }

    body << "    return  string(\"Unknown output stream \") + to_string(event->_stream);\n";
    body << "}\n\n\n\n";

    for (output  = _subscribe.begin();
         output != _subscribe.end(); output++)
    {
        stream = (*output)->get_subscription_stream();
        name   = to_string(stream->get_stream_name());

        // This gives the listening endpoint of the client.  We want the node listener???
        //endpoint = to_string((*output)->get_subscription_endpoint());

        // This code assumes there is a box for every subscription.
        // If no box is defined, use the default node???
        //
        //endpoint = (*output)->get_subscription_stream()
        //                 ->get_stream_source()->get_port_box()
        //                 ->get_box_node();

        //name = to_string((*output).first);
        proper_case(name);

        upper = _prefix;
        uppercase(upper);

        tuple = to_string(stream->get_schema_name());

        tuple = _schema_mixed[tuple];
        DEBUG  << "subscribe " << name << " for " << tuple;

        body << BOX_TOP;
        body << "// Subscribing to receive output on a fast datapath.\n";
        body << "//\n";
        body << "void  " << _prefix << "Marshal::subscribe" << name << "()\n";
        body << "{\n";
        body << BOX_BOTTOM;


        // Subcribe uses the client listener node; fix this???
        //
        body << "    DEBUG << \"Subscribing to receive output.\";\n\n";

        body << "    // Setup the subscription request.\n";
        body << "    Status  status = _client->set_data_handler(\n";
        body << "                                  InetAddress(Util::form_endpoint(" << upper << "_ENDPOINT,\n";
        body << "                                                                  DEFAULT_MONITOR_PORT)),\n";
        body << "                                  wrap(&outputHandler));\n\n";

        body << "    if (status)\n";
        body << "    {   DEBUG << \"Done subscribing to output.\";\n";
        body << "    }\n";
        body << "    else\n";
        body << "    {   ERROR << \"Could not subscribe to output.\";\n";
        body << "    }\n\n";

        body << "    return;\n";
        body << "}\n\n\n\n";

        body << BOX_TOP;
        body << "// Receive output on a fast datapath.\n";
        body << "//\n";
        body << "Status  " << _prefix << "Marshal::" << name << "Handler(ptr<StreamEvent>  event)\n";
        body << "{\n";
        body << "    " << tuple << "  *tuple;\n";
        body << "    int32         index;\n";
        body << "    uint32        offset = 0;\n";


        // If any array fields; generate an offset and referrence variable.
        //
        vector<SchemaField>  &field = stream->get_stream_schema()->get_schema_field();
        hit = False;

        for (f = field.begin(); f != field.end(); f++)
        {
            array = f->get_array();
            DEBUG << "name=" << f->get_name();

            if (array)
            {   DEBUG << "name=" << f->get_name() << " hit=" << (char)hit;

                if (!hit)
                {   body << "    uint32        size   = 0;\n";
                    body << "    uint8        *array;\n";

                    hit = True;
                }

                body << "    uint32        size_" << f->get_name() << ";\n";
            }
        }

        body << BOX_BOTTOM;

        body << "    // For each tuple that was received,\n";
        body << "    //\n";
        body << "    for (index = 0; index < event->_inserted_count; index++)\n";
        body << "    {\n";
        body << "        offset += HEADER_SIZE;\n\n";

        body << "        // At the tuple data past the header.\n";
        body << "        //\n";
        body << "        tuple = (" << tuple << " *)&event->_bin_tuples[offset];\n";
        body << "        DEBUG << \"DATA:  \" << to_hex_string(tuple, sizeof(" << tuple << "));\n\n";

        // Convert the array size to a reference for each array field.
        //
        for (f = field.begin(); f != field.end(); f++)
        {
            array = f->get_array();

            if (array)
            {   body << "        // At the array data in the event.\n";
                body << "        //\n";
                body << "        array = (uint8 *)&(event->_bin_arrays[size]);\n\n";

                body << "        // Offset past the array data.\n";
                body << "        //\n";

                body << "        size_" << f->get_name()
                     << " = *(uint32 *)&tuple->" << f->get_name() << ";\n";

                body << "        size += size_" << f->get_name() << ";\n";

                body << "        DEBUG << \"size=\" << size << \"  size_" << f->get_name()
                     << "=\" << size_" << f->get_name() << ";\n\n";

                body << "        // Modify the array field to reference the array data.\n";
                body << "        //\n";
                body << "        tuple->" << f->get_name() << " = array;\n\n";
            }
        }

        body << "        received" << name << "(tuple";

        for (f = field.begin(); f != field.end(); f++)
        {
            array = f->get_array();

            if (array)
            {   if (array->is_variable())
                {   vector<SchemaArray::DimensionInformation>  &dimension = array->get_all_dimensions();

                    body << ", (int32)(size_" << f->get_name() << " / "
                         << array->array_size() << ") - "
                         << 1 - dimension[0]._lower;
                }
            }
        }

        body << ");\n";

        body << "        offset += sizeof(" << tuple << ");\n";
        body << "    }\n\n";

        body << "    // Signal done with this batch.\n";
        body << "    //\n";
        body << "    return  true;\n";
        body << "}\n\n\n\n";
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Write the bottom common portion of the marshaling method file.
//
void GenerateMarshal::body_bottom(// An open method file output stream.
                                  ofstream  &body)
{
//..............................................................................


    body << BOX_TOP;
    body << "// Launch the Borealis front-end.\n";
    body << "//\n";
    body << "int32  " << _prefix << "Marshal::launchDiagram(string  xml)  // Path of an initial xml file.\n";
    body << "{\n";
    body << "    int32   status;\n";
    body << "    string  command;\n";
    body << BOX_BOTTOM;

    body << "    INFO << \"Connect with: \" << xml;\n\n";

    body << "    command = string() + \"BigGiantHead  \" + xml;\n";
    body << "    status  = system(command.c_str());\n\n";

    body << "    DEBUG << \"Ran the Borealis front end:  \" << status;\n\n";

    body << "    return  status;\n";
    body << "}\n\n";

    body << "////////////////////////////  end  " << _prefix
         << "Marshal.cc  //////////////////////////\n";

    return;
}

BOREALIS_NAMESPACE_END
