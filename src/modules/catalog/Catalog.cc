#include <string>

#include  "Catalog.h"
#include  "parseutil.h"

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
Catalog::Catalog()
    : _add_schema( NULL ),
      _add_field(  0 ),
      _add_box(    NULL ),
      _add_table(  NULL ),
      _add_view(   NULL ),
      _add_in(     0 ),
      _add_out(    0 ),
      _add_query(  NULL )
{
//..............................................................................


    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Add a subscription
// -- does not modify stream queues.
//
void Catalog::add_subscription(CatalogSubscription  &subscription)
                         throw(AuroraBadEntityException)
{
//..............................................................................


    InetAddress sub_dest = subscription.get_subscription_endpoint();
    Name stream_name = subscription.get_subscription_stream_name();

    SubscriptionMap &site_in = _overlay._subscription_map[sub_dest];

    if ( site_in.find(stream_name) == site_in.end() )
    {
        site_in[stream_name].shallow_copy(subscription);

        DEBUG << "sink flag = " << site_in[stream_name].get_sink_flag();

        // Also adding the subscription to the direct map
        _overlay._direct_subscription_map[stream_name] = site_in[stream_name];

        CatalogStream  *stream = subscription.get_subscription_stream();

        if (!stream)
        {
            stream = get_stream(stream_name);
        }

        if (stream)
        {
            if (_topology._stream_map.find(stream_name) == _topology._stream_map.end())
            {
                CatalogStream &new_stream = _topology._stream_map[stream_name];
                new_stream.shallow_copy(*stream);

                // set its schema.
                //
                if (_topology._schema_map.find(stream_name) == _topology._schema_map.end())
                {
                    // create new stream schema.
                    //
                    Name schema_name = stream->get_schema_name();
                    _topology._schema_map[schema_name] = *(stream->get_stream_schema());
                }

                new_stream.set_stream_schema(&(_topology._schema_map[stream->get_schema_name()]));
            }

            // we don't set the stream's subscribed ports here.
            //
            CatalogSubscription& csub = site_in[stream_name];

            DEBUG << "sink flag = " << csub.get_sink_flag();

            csub.set_subscription_stream( &(_topology._stream_map[stream_name]) );
        }
        else
        {   Throw(AuroraBadEntityException,
                  "Stream \"" + stream_name.as_string()
                  + "\" does not exist in the catalog.");
        }
    }
    else
    {
        Throw(AuroraBadEntityException,
              "Subscription of stream \"" + stream_name.as_string() + "\""
              + " from " + to_string(sub_dest) + " already exists in the catalog.");
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Remove a subscription
// -- does not modify stream queues.
//
void Catalog::remove_subscription(CatalogSubscription  *subscription)
    throw(AuroraBadEntityException)
{
//..............................................................................


    InetAddress sub_dest = subscription->get_subscription_endpoint();
    SubscriptionMap& site_in = _overlay._subscription_map[sub_dest];

    SubscriptionMap::iterator sub_iter =
        site_in.find(subscription->get_subscription_stream_name());

    if ( sub_iter != site_in.end() )
    {
        site_in.erase(sub_iter);
        // Also erasing from the direct map
        _overlay._direct_subscription_map.erase(subscription->get_subscription_stream_name());
    }
    else
    {   Throw(AuroraBadEntityException,
              "Subscription of "
              + subscription->get_subscription_stream_name().as_string()
              + " to " + to_string(sub_dest)
              + " does not exist in the catalog.");
    }

    if (site_in.empty())
    {   _overlay._subscription_map.erase(sub_dest);
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Add a subscriber to whom we'll send tuples.
//
void Catalog::add_subscriber(CatalogSubscription  &subscription)
                       throw(AuroraBadEntityException)
{
    CatalogStream    *stream;
    Name              stream_name;
    Name              schema_name;
    InetAddress       sub_src;
//
// Does not modify stream queues.
//..............................................................................


    sub_src     = subscription.get_subscription_endpoint();
    stream_name = subscription.get_subscription_stream_name();
    //DEBUG << "name=" << stream_name << "  endpoint=" << sub_src;

    SubscriptionMap  &site_out = _overlay._subscriber_map[sub_src];

    if (site_out.find(stream_name) == site_out.end())
    {
        site_out[stream_name].shallow_copy(subscription);
        stream = subscription.get_subscription_stream();

        if (!stream)
        {   DEBUG << "Stream from topology";
            stream = get_stream(stream_name);
        }

        if (stream)
        {   if (_topology._stream_map.find(stream_name) ==
                _topology._stream_map.end())
            {
                DEBUG << "Stream name is " << stream_name;
                DEBUG << "stream name is " << stream->get_stream_name();

                // Add a new stream to the stream map.
                //
                CatalogStream  &new_stream = _topology._stream_map[stream_name];
                new_stream.shallow_copy( *stream );

                DEBUG << "====== xxx";

                // set its schema.
                //
                schema_name = stream->get_schema_name();
                DEBUG << "====== yyy";

                if ( _topology._schema_map.find( schema_name ) ==
                     _topology._schema_map.end())
                {
                    // create new stream schema.
                    //
                    _topology._schema_map[ schema_name ] =
                        *(stream->get_stream_schema());
                }

                new_stream.set_stream_schema(
                                             &( _topology._schema_map[ schema_name ]));
            }

            // we don't set the stream's subscribed ports here.
            //
            CatalogSubscription& csub = site_out[stream_name];
            csub.set_subscription_stream( &(_topology._stream_map[stream_name]) );
        }
        else
        {   Throw( AuroraBadEntityException,
                   "Stream \"" + stream_name.as_string()
                   + "\" does not exist in the catalog." );
        }
    }
    else
    {   Throw( AuroraBadEntityException,
               " Subscriber of " + stream_name.as_string()
               + " already exists in the catalog." );
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Remove a subscriber so we no longer send them tuples.
//
Status  Catalog::remove_subscriber(CatalogSubscription *subscription)
{
    Status            status = true;
//
// Does not modify stream queues.
//..............................................................................


    InetAddress  sub_src = subscription->get_subscription_endpoint();
    DEBUG << "sub_src = " << subscription->get_subscription_endpoint();

    SubscriptionMap  &site_out = _overlay._subscriber_map[sub_src];

    SubscriptionMap::iterator sub_iter = site_out.find(
                                  subscription->get_subscription_stream_name());

    if (sub_iter != site_out.end())
    {
        site_out.erase(sub_iter);
    }
    else
    {   status = "Subscriber of "
               //+ subscription->get_subscription_stream_name().as_string()
               //+ " from "
               + to_string(sub_src)
               + " does not exist in the catalog.";

        DEBUG << status;
    }

    return  status;
}

/**
 *  The catalog holds configuration data for each node.
 *  The Catalog class defines the data held in the catalog.
 *  State data resides in node objects and network statistics are managed by
 *  the StatsManager.
 */



////////////////////////////////////////////////////////////////////////////////
//
// Begin a sequence of calls to create a new schema.
//
void  Catalog::begin_schema(// Schema to create.
                            Name  name)
    throw(AuroraBadEntityException)
{
//..............................................................................


    Status  status = validate_schema(name);

    if (status)
    {   close_prior();

        _add_schema = &_topology._schema_map[ name ];    // Insert a new schema.

        _add_schema->_name = name;
        _add_field = 0;
    }
    else
    {   Throw( AuroraBadEntityException, status.as_string() );
    }

    // DEBUG << "add schema: " << name;

    return;
}




////////////////////////////////////////////////////////////////////////////////
//
// Add a field to a schema in a catalog structure.
//
void  Catalog::add_field(// Name of the field.
                         string     name,

                         // The type of the field.
                         DataType  type,

                         // Size is required for string types.
                         uint16    size)
                   throw(AuroraBadEntityException)
{
    SchemaField   field;
//..............................................................................


    // DEBUG << "add field: " << _add_field << " to " << _add_schema->_name;

    Status  status = validate_field( name, type, size );

    if (status)
    {   field.set_name(name);
        field.set_type_size(type, size);

        _add_schema->_field.push_back(field);
        _add_field++;
    }
    else
    {   Throw(AuroraBadEntityException, status.as_string());
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Add a an array definition to a field to a schema in a catalog structure.
//
void  Catalog::add_array(// Array declaration (mixed case).
                         string     type,

                         // Maximum size if a string element; else "0".
                         string     size)
                   throw(AuroraBadEntityException)
{
    DataType   data_type;
    uint8      data_size;

    string     element;
    string     message;

    string     lower;
    string     upper;

    CatalogSchema     *schema = NULL;

    string::size_type  index;
    string::size_type  start;

    stack<SchemaArray::DimensionInformation>    dimension;
//
//  type := <element> '(' ([ <lower> '...' ] {<upper> | '*' }), ... ')'
//..............................................................................


    // Split off the element type.  Can there be leading spaces???
    //
    index   = type.find("(", 0);
    element = type.substr(0, index);

    while (element[element.size() - 1] == ' ')
    {   element.erase(element.size() - 1, 1);
    }

    lowercase(element);
    DEBUG << "element=" << element << " type=" << type;

    if (element == "string")
    {   data_size = string_to_uint(size);
    }
    else if (size == "0")
    {   try
        {   data_size = data_type.from_string(element);
        }
        catch (AuroraBadEntityException &e)
        {
            DEBUG << "Array of tuples ...";
            schema = get_schema(element);

            if (schema == NULL)
            {   message = "Undefined array element type:  " + type;

                DEBUG << message;
                Throw(AuroraBadEntityException, message);
            }

            data_type = DataType::NONE;
            data_size = schema->get_size();
        }
    }
    else
    {   message = "In the array field (" + element
                + ") the size attribute can only be used with a string type.";

        DEBUG << message;
        Throw(AuroraBadEntityException, message);
    }

    DEBUG  << "data_size=" << to_string((uint32)data_size);

    // The 'new' should be changed to a heap allocating container???
    //
    SchemaArray *array = new SchemaArray(data_type, data_size, schema);

    if (array == NULL)
    {   message = "Out of memory when allocating schema::  " + type;

        DEBUG << message;
        Throw(AuroraBadEntityException, message);
    }

    // Add each array dimension.
    //
    index++;

    while (1)
    {   while (type[index] == ' ') { index++; }
        start = index;                  // Beginning of a bound.

        if (isalpha(type[index]))
        {   index++;
            while (isalpha(type[index])) { index++; }
            upper = type.substr(start, index - start);
            lowercase(upper);

            while (type[index] == ' ')   { index++; }

            if (type[index] == ':')
            {   start = index + 1;
                while (type[start] == ' ') { start++; }

                index = start;
                while (isdigit(type[index])) { index++; }

                if (start == index)
                {   message = "Missing string size in a sparse dimension:  "
                            + type;

                    DEBUG << message;
                    Throw(AuroraBadEntityException, message);
                }

                upper = upper + ":" + type.substr(start, index - start);
            }
        }
        else if (type[index] == '*')
        {   index++;
            upper = "*";
        }
        else
        {   if (type[index] == '+'  ||  type[index] == '-') { index++; }

            while (isdigit(type[index])) { index++; }
            DEBUG << "index=" << index << " start=" << start;

            if (start == index)
            {   message = "Missing bound in an array declaration:  " + type;

                DEBUG << message;
                Throw(AuroraBadEntityException, message);
            }

            upper = type.substr(start, index - start);
        }

        lower = "0";

        while (type[index] == ' ') { index++; }

        if (type.substr(index, 3) == "...")
        {   lower = upper;
            
            index += 3;
            while (type[index] == ' ') { index++; }

            start = index;

            if (type[index] == '*')
            {   index++;
            }
            else
            {   if (type[index] == '+'  or  type[index] == '-') { index++; }
                while (isdigit(type[index])) { index++; }

                if (start == index)
                {   message = "Missing bound in an array declaration:  " + type;

                    DEBUG << message;
                    Throw(AuroraBadEntityException, message);
                }
            }

            upper = type.substr(start, index - start);
            while (type[index] == ' ') { index++; }
        }

        DEBUG << " lower=" << lower << " upper=" << upper;

        dimension.push(array->parse_dimension(lower, upper));

        if (type[index] != ',')
        {   break;
        }

        index++;
    }

    // Add array dimensions so the rightmost is elemnt 0.
    //
    while (!dimension.empty())
    {
        array->add_dimension(dimension.top());
        dimension.pop();
    }


    // Verify the dimensions were parsed to the right parenthesis.
    //
    if (type[index] != ')')
    {   message = "Missing right bracket in an array declaration:  " + type;

        DEBUG << message;
        Throw(AuroraBadEntityException, message);
    }

    index++;

    while (index < type.size())   // if trailing spaces are trimmed we can skip this???
    {   if (type[index] != ' ')
        {   message = "Junk after an array declaration:  " + type;

            DEBUG << message;
            Throw(AuroraBadEntityException, message);
        }

        index++;
    }

    // The field references the array definition.
    //
    DEBUG << "size=" << array->array_size() << "_add_field" << _add_field;

    _add_schema->_field[_add_field - 1].set_array(array);

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Add a dataflow stream to the catalog.
//
CatalogStream  *Catalog::add_stream( // Stream to create.
                                    Name    name,

                                    // Data types in the stream.
                                    Name    schema
                                    )
    throw( AuroraBadEntityException )
{
    CatalogStream   *stream;
//..............................................................................


    Status  status = validate_stream( name, schema );

    if ( status )
    {   close_prior();
        stream = &_topology._stream_map[ name ];             // Insert a new stream.

        stream->set_stream_name( name );
        stream->set_stream_schema( &_topology._schema_map[ schema ]);  // At existing schema.
        stream->set_stream_source( NULL, 0 ); // The source is initially disconnected.
    }
    else
    {   Throw( AuroraBadEntityException, status.as_string() );
    }

    DEBUG  << "added stream " << name << " with schema: " << schema;

    return  stream;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Add an input stream to the catalog.
//
void  Catalog::add_input(// Stream to connect to input.
                         //
                         string  stream)
                   throw(AuroraBadEntityException)
{
    CatalogStream  *connect;
//
//  Should the stream be flaged as an input???
//..............................................................................


    Status  status = validate_input_output(stream);

    if (status)
    {   close_prior();

        // Add it to the set of input streams.
        //
        connect = &_topology._stream_map[stream];
        //_input.push_back(connect);
    }
    else
    {   Throw(AuroraBadEntityException, status.as_string());
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Add publication of an input stream to the catalog.
//
void  Catalog::add_publish(// Name of the input stream.
                           //
                           string  stream,

                           // Node receiving the input.
                           //
                           string  node)
                     throw(AuroraBadEntityException)
{
    InetAddress       address;
    SubscriptionMap  *map;
//..............................................................................


    address = InetAddress(node);
    map     = &_overlay._publisher_map[address];           // new or prior map

    (*map)[stream] = CatalogSubscription(address, stream);
    DEBUG << "publisher size=" << map->size();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Add a subscription to an output stream to the catalog.
//
CatalogSubscription  Catalog::add_subscribe(// Stream to connect to output.
                                            //
                                            string  stream,

                                            // Endpoint of the application monitor.
                                            //
                                            string  endpoint,

                                            // Number of consecutive tuples that can be lost.
                                            // Use 0xFFFF for unlimited tuple loss.
                                            //
                                            uint16  gap)
        throw(AuroraBadEntityException)
{
    CatalogSubscription  subscription;
//..............................................................................


    Status  status = validate_input_output(stream);

    if (status)
    {   close_prior();

        if (endpoint.empty())
        {   FATAL << "NYI:  subscription endpoint is required for now.";
        }

        subscription = CatalogSubscription(InetAddress(endpoint), stream);
        subscription.set_gap(gap);

        add_subscriber(subscription);
    }
    else
    {   Throw(AuroraBadEntityException, status.as_string());
    }

    return  subscription;
}


unsigned int NADA = 99999;

////////////////////////////////////////////////////////////////////////////////
//
// Add a connection point view to a catalog structure.
// Only sets the BoxPort if view is incomplete and BoxPort is still NULL
// view is "incomplete" until the BoxPort is also set
//
CatalogView  *Catalog::add_view(// Name of the connection point view.
                                Name    name,

                                // Name of the stream to connect to.
                                Name    stream,

                                // Name of a file to order by.
                                // An empty string if ordering by tuple.
                                string   field,

                                // Field order value or start value of a range.
                                uint32  start,

                                // End value of a range.
                                // Ignored if ordering by field.
                                uint32  end,

                                uint32 radius)
                          throw(AuroraBadEntityException)
{
    StreamMap::iterator  in_stream;
//..............................................................................


    Status  status = validate_view(name, stream, field, start, end);

    if (status)
    {   close_prior();
        _add_view = &_view_map[name];                       // Add a new view.
        _add_view->set_view_name(name);

        in_stream = _topology._stream_map.find( stream );

        if (in_stream == _topology._stream_map.end())
        {   _add_view->set_view_stream(stream,             // Add a new stream.
                                       &_topology._stream_map[stream]);

            DEBUG << "Added stream " << stream << " for view " << name;
        }

        // set the stream anyway
        //
        _add_view->set_view_stream(stream, &_topology._stream_map[stream]);
        _add_view->set_view_window(field, start, end);
        _add_view->set_view_radius(radius);
        _add_view->not_done();                  // BoxPort left not set yet

        DEBUG << "Added incomplete view";
    }
    else if (status.as_string() == "incomplete")  
    {
        close_prior();
        _add_view = &_view_map[name];                       // Add a new view.

        in_stream = _topology._stream_map.find(stream);

        if (in_stream == _topology._stream_map.end())
        {   _add_view->set_view_stream(stream,           // Add a new stream.
                                       &_topology._stream_map[stream]);

            DEBUG << "Added stream " << stream << " for view " << name;
        }

        // set the stream anyway
        //
        _add_view->set_view_stream(stream, &_topology._stream_map[stream]);
        _add_view->set_view_window(field, start, end );
        _add_view->set_view_radius(radius);

        // BoxPort must be set ... why not? ... fix it by setting the BoxPort here
        //
        if (_add_view->get_box_port()->get_port() == NADA)
        {
            vector<BoxPort> box_ports = *(_add_view->get_view_stream()->get_sink_port());

            for (vector<BoxPort>::iterator iter = box_ports.begin(); 
                 iter != box_ports.end(); 
                 ++iter)
            {
                // iter through the BoxPort's and find the box that says the cpview is the same as this
                //
                if ((*iter).get_port_box()->get_cpview(stream.as_string()) == name.as_string())
                {
                    _add_view->set_box_port(*iter);
                    break;  // no need to keep looking
                }
            }
        }

        _add_view->done();
        DEBUG << "Added complete view";
    }
    else
    {   Throw(AuroraBadEntityException, status.as_string());
    }

    return  _add_view;
}



////////////////////////////////////////////////////////////////////////////////
//
// Add a connection point view to a catalog structure.
// Sets the BoxPort for the view
// view is "incomplete" until the other fields have been set
//
CatalogView *Catalog::add_view( /// Name of the connection  point view.
              Name name,
              /// Name of the box that this view supplies tuples to.
              Name box,
              // Name of the stream to connect to.
              Name stream
              )
    throw(AuroraBadEntityException)
{
    CatalogView *view;
    StreamMap::iterator  in_stream;
    BoxMap::iterator   in_box;
//..............................................................................


    Status status = validate_view(name);

    if (status)
    {   view = &_view_map[name];
        view->set_view_name(name);

        // figure out the box port to set for the view
        BoxPort port = find_box_port_for_view(&_topology._stream_map[stream], box);
        view->set_box_port(port);
        view->not_done();
        DEBUG << "Added incomplete view";
    }
    else if (status.as_string() == "incomplete")
    {
        view = &_view_map[name];

        BoxPort port = find_box_port_for_view(&_topology._stream_map[stream], box);
        view->set_box_port(port);
        view->done();
        DEBUG << "Added complete view";
    }
    else
    {   Throw(AuroraBadEntityException, status.as_string());
    }

    return  view;
}



////////////////////////////////////////////////////////////////////////////////
//
// What is the BoxPort in stream that feeds into box
// Used to figure out the BoxPort for view
// 
BoxPort Catalog::find_box_port_for_view(CatalogStream *stream, Name box)
{
    vector<BoxPort> box_ports = *(stream->get_sink_port());

    for (vector<BoxPort>::iterator iter = box_ports.begin(); 
         iter != box_ports.end(); ++iter)
    {
        // iter through the BoxPort's and find the box with the same name as the one this view goest to
        //
        if ((*iter).get_port_box()->get_box_name() == box)
        {   return  *iter;
        }
    }

    BoxPort ret;
    ret.set_box_port((new CatalogBox()), NADA);

    return  ret;
}



////////////////////////////////////////////////////////////////////////////////
//
// Begin a sequence of calls to add a box.
// To add a box call:  addBox, (addIn | addOut | addParameter | addTable )+
//
void  Catalog::begin_box(// Name of the box.
                         Name    name,

                         // Box operation.
                         string  type)
                   throw(AuroraBadEntityException)
{
//..............................................................................


    Status  status = validate_box(name, type);

    if (status)
    {   close_prior();
        _add_box = &_topology._box_map[name];      // Add a new box.
        _add_box->set_box_name_type(name, type);

        //_add_box->_node = "";     // Empty for now.  Needs to be set???
        //_add_box->_query = NULL;
        //_add_box->_table = NULL;

        if (_add_query)
        {   // Link this box to the query set.
            _add_box->set_box_query(_add_query);

            // Include this box in the query set.
            _add_query->get_query_box()->push_back(_add_box);

            DEBUG << "Link box " << name
                  << " to query " << _add_query->get_query_name();
        }

        _add_in  = 0;
        _add_out = 0;
    }
    else
    {   Throw( AuroraBadEntityException, status.as_string() );
    }

    // DEBUG  << "add box " << name << " of type " << type;

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Connect a stream to the next box input.
// Streams must be connect in the order that input ports are assigned.
//
void  Catalog::add_in(// Name of the stream to connect.
                      Name    stream,

                      // Queue options.
                      CatalogBox::InQueue  in_queue,

                      Boolean dynamic)
{
    CatalogStream  *connect;
    BoxPort         port;

    StreamMap::iterator  in_stream;
//..............................................................................


    // anjali- if there are more than one input to attach.
    //
    Status  status = validate_more_in(stream);

    if(!status)
    {   Throw(AuroraBadEntityException, status.as_string());
    }

    // Add the connection to the box.
    //
    in_stream =  _topology._stream_map.find(stream);
    connect   = &_topology._stream_map[stream];      // Stream or add new stream.

    if (in_stream == _topology._stream_map.end())
    {   connect->set_stream_name(stream);
        connect->set_stream_schema(NULL);       // Assume an intermedite stream.
        connect->set_stream_source(NULL, 0);    // Flag a disconnected stream.
    }

    //_add_box->get_box_in()->push_back(connect);
    _add_box->add_in_stream(connect, in_queue);

    if (!dynamic)
    {
        // Add the connection to the stream.
        //
        port.set_box_port(_add_box, _add_in);

        //DEBUG  << "connect stream " << connect->_name << " to input port " << _add_in
        //              << " on box " << _add_box->_name;

        connect->add_sink_port(port);
    }

    _add_in++;

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Connect a stream to the next box output.
// Streams must be connect in the order that output ports are assigned.
//
void  Catalog::add_out(// Name of the stream to connect.
                       Name   stream)
{
    CatalogStream  *connect;

    StreamMap::iterator  in_stream;
//..............................................................................


    DEBUG  << "connect " << stream;

    // Add the connection to the box.
    //
    in_stream =  _topology._stream_map.find( stream );
    connect   = &_topology._stream_map[ stream ];      // New or existing stream.

    if ( in_stream == _topology._stream_map.end() )
    {   connect->set_stream_name( stream );
        connect->set_stream_schema( NULL );         // Assume an intermedite stream.
    }

    _add_box->get_box_out()->push_back( connect );

    // Add the connection to the stream.
    //
    connect->set_stream_source( _add_box, _add_out );
    _add_out++;

    // DEBUG  << "connect stream " << connect->_name << " to output port " << _add_out
    //              << " on box " << _add_box->_name;

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Assign a value to a box, table or view parameter.
//
void  Catalog::add_parameter(// Name of the parameter.
                             string  parameter,

                             // Value to assign the parameter.
                             string  value)
                       throw(AuroraBadEntityException)
{
    BoxParameter  add;
//
// no duplicate parameter???
//..............................................................................


    // anjali - begin
    Status  status = validate_parameter(parameter, value);

    if(!status)
    {    Throw( AuroraBadEntityException, status.as_string() );
    }
    // end

    //add._parameter = parameter;
    //add._value     = value;

    if (_add_view)
    {   BoxParameter::ParameterMap *p = _add_view->get_view_parameter_map();
        (*p)[parameter] = value;
    }
    else if (_add_box)
    {   BoxParameter::ParameterMap *p = _add_box->get_box_parameter_map();
        (*p)[parameter] = value;
    }
    else
    {   BoxParameter::ParameterMap *p = _add_table->get_table_parameter_map();
        (*p)[parameter] = value;
    }

    // DEBUG  << "add " << _add_box->_name << " parameter " << parameter
    //                                             << " = " << value;

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Assign a table to a box
//
void  Catalog::add_table(// Name of the table.
                         Name table )
                   throw(AuroraBadEntityException)
{
    TableMap::iterator in_table;
//..............................................................................


    if (_add_box)
    {   in_table = _topology._table_map.find( table );

        if ( in_table == _topology._table_map.end() )
        {    Throw( AuroraBadEntityException,
                    "Box " + to_string( _add_box->get_box_name() ) + " uses table "
                    + to_string( table ) + " before declaration." );
        }

        _add_box->set_box_table( &( _topology._table_map[ table ]));
    }
    else
    {   Throw( AuroraBadEntityException,
               "Table used in something other than a box." );
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Access a box and optionally change its location.
//
Status  Catalog::modify_box_node(/// Name of a box in the catalog.
                                 Name    name,

                                 /// Destination node.
                                 /// Empty to just access the box.
                                 string  node)
{
    Status  status = true;
    BoxMap::iterator  box;
//..............................................................................


    _add_table = NULL;                        // Close of any Table changes.
    _add_view  = NULL;                        // Close of any Table changes.
    box = _topology._box_map.find(name);

    if ( box == _topology._box_map.end() )
    {   status = "Box to modify is not in the catalog:  " + to_string(name);
    }
    else
    {   _add_box = &box->second;

        if (!node.empty())
        {   _add_box->set_box_node(node);
        }
    }

    return  status;
}
 


////////////////////////////////////////////////////////////////////////////////
//
// Begin a sequence of calls to add a table.
// To add a table call:  beginTable, (addKey | addIndex | addParameter )+
// Note that a table must be defined within a query.
//
void  Catalog::begin_table(// Name of the table.
                           Name    name,

                           // Schema of the table contents.
                           Name    schema)
                     throw(AuroraBadEntityException)
{
//..............................................................................


    Status  status = validate_table( name, schema );

    if (status)
    {   close_prior();
        _add_table = &_topology._table_map[ name ];       // Add a new table.

        _add_table->set_table_name_schema( name,
                                           &_topology._schema_map[ schema ]);  // existing schema.
        //_add_table->_query  = NULL;
        //_add_table->_node   = "";             // Empty for now.  Needs to be set???

        if ( _add_query )
        {   // Link this table to the query set.
            _add_table->set_table_query( _add_query );

            // Include this table in the query set.
            _add_query->add_query_table( _add_table );
        }

        //_add_table->_schema = &_topology._schema_map[ schema ];  // Add existing schema.

        // DEBUG  << "add table " << name << " of type " << type;
    }
    else
    {   DEBUG << "Failed to add table " << name << " : " << status;
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Add a key field to a table.
//
void  Catalog::add_key(// Name of the field to add to the key.
                       Name   field)
{
//..............................................................................


    _add_table->add_table_key(field);

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Index a field in a table.
//
void  Catalog::add_index(// Name of the field to index.
                         Name   field)
{
//..............................................................................


    _add_table->add_table_index(field);

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Access a table and optionally change its location.
//
Status  Catalog::modify_table_node(// Name of a table in the catalog.
                                   Name    name,

                                   // Destination node.
                                   // Empty to just access the table.
                                   string  node)
{
    TableMap::iterator  table;
    Status  status = true;
//..............................................................................


    _add_box  = NULL;                // Close of any prior Box changes.
    _add_view = NULL;                // Close of any prior View changes.
    table = _topology._table_map.find(name);

    if ( table == _topology._table_map.end() )
    {   status = "Table to modify is not in the catalog:  " + to_string(name);
    }
    else
    {   _add_table = &table->second;

        if (!node.empty())
        {   _add_table->set_table_node(node);
        }
    }
    
    return(status);
}
 



////////////////////////////////////////////////////////////////////////////////
//
// Begin a sequence of calls to create a query.
// To add a query call:  beginQuery, <add query elements>, endQuery
//
void  Catalog::begin_query(// Name of the query.
                           Name  name)
                     throw(AuroraBadEntityException)
{
//..............................................................................


    Status  status = validate_query(name);

    if (status)
    {   close_prior();

        // Create a new set of boxes.  Subsequent boxes are added to this set.
        //
        _add_query = &_topology._query_map[ name ];
        _add_query->set_query_name( name );
    }
    else
    {   Throw(AuroraBadEntityException, status.as_string());
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Last in a sequence of calls to create a query.
//
void  Catalog::end_query()
    throw( AuroraBadEntityException )
{
//..............................................................................


    Status  status = prior_element();

    if ( status )
    {   close_prior();

        _add_query = NULL;
    }
    else
    {   Throw( AuroraBadEntityException, status.as_string() );
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
string Catalog::as_string() const
{
//..............................................................................


    ostringstream oss;
    oss << "------------------ Catalog ----------------" << endl;
    oss << "Boxes: {";
    bool first = true;
    for (BoxMap::const_iterator it = _topology._box_map.begin();
         it != _topology._box_map.end();
         ++it)
    {
        if (!first) oss << " "; else first = false;
        oss << it->first;
    }
    oss << "}" << endl;
    oss << "streams: {";
    first = true;
    for (StreamMap::const_iterator it = _topology._stream_map.begin();
         it != _topology._stream_map.end();
         ++it)
    {
        if (!first) oss << " "; else first = false;
        oss << it->first;
    }
    oss << "}" << endl;
    oss << "-------------------------------------------" << endl;
    return oss.str();

}


////////////////////////////////////////////////////////////////////////////////
/// Magda added --
/// Methods below manage the information about the consistency states of streams
/// .............................................................................
void
Catalog::add_remote_input_stream(Name stream_name, InetAddress src_node)
{

    ConsistencyState default_stream_state;
    _overlay._stream_states_map[stream_name][src_node] = default_stream_state;
    //_overlay._rstream_states_map[src_node][stream_name] = default_stream_state;
    DEBUG << "Input stream is " << stream_name;
    DEBUG << "Upstream neighbor is " << src_node;
    DEBUG << "Initializing state to " << default_stream_state;

}

void
Catalog::monitor_local_stream(Name stream_name)
{

    if ( _overlay._local_stream_states_map.find( stream_name ) == _overlay._local_stream_states_map.end() )
    {
        ConsistencyState default_stream_state;
        _overlay._local_stream_states_map[stream_name] = default_stream_state;
        DEBUG << "New output stream to monitor " << stream_name;
    }

}



/// Update the consistency of all locally running streams.
void 
Catalog::update_local_stream_states(ConsistencyState new_state)
{

    PerOutputStreamStates::iterator i;
    for ( i = _overlay._local_stream_states_map.begin(); 
          i != _overlay._local_stream_states_map.end(); ++i)
    {
        i->second = new_state;
    }

}

bool
Catalog::update_input_stream_state(Name stream_name, InetAddress src_node, ConsistencyState new_state)
{
    bool performed_update = false;

    StreamStatesIndex::iterator i = _overlay._stream_states_map.find(stream_name);
    if ( i != _overlay._stream_states_map.end())
    {
        _overlay._stream_states_map[stream_name][src_node] = new_state;
        //_overlay._rstream_states_map[src_node][stream_name] = new_state;
        performed_update = true;
    }
    else 
    { WARN << "Trying to update the state of an unknown stream";
    }

    return performed_update;

}



////////////////////////////////////////////////////////////////////////////////
//
// Remove a box from the catalog. (except in/out)
//
void Catalog::remove_box(Name box_name, set<Name> streams_to_preserve)
    throw(AuroraBadEntityException)
{
//..............................................................................


    BoxMap::iterator biter = _topology._box_map.find(box_name);

    if (biter == _topology._box_map.end())
    {
        Throw(AuroraBadEntityException,
              "Box \"" + to_string(box_name) +
              "\" does not exist in the catalog!");
    }

    CatalogBox *box = &(_topology._box_map[box_name]);

    if (box->get_box_query())
    {
        Name query_name = box->get_box_query()->get_query_name();
        CatalogQuery *query = &(_topology._query_map[query_name]);
        QueryMap::iterator qiter = _topology._query_map.find(query_name);

        if ((query->get_query_box()->size() == 1)) 
        {   // it must be me.
            // remove this query object from the catalog.
            //
            if (qiter != _topology._query_map.end())
            {
                _topology._query_map.erase(qiter);
            }
        }
        else
        {   // just remove this box from query's list.
            //
            query->remove_query_box(box_name);
        }
    }

    if (box->get_box_table())
    {
        Name table_name = box->get_box_table()->get_table_name();

        // should we delete the schema as well?
        // we must check if it is being used by other streams or
        // tables...

        //TableMap::iterator titer = _topology._table_map.find(table_name);
        //if (titer != _topology._table_map.end())
        //{
        //    _topology._table_map.erase(titer);
        //}
    }

    // Remove output stream...
    if (box->get_box_out())
    {
        CatalogStream::StreamStar *outputs = box->get_box_out();
        CatalogStream::StreamStar::iterator siter = outputs->begin();
        CatalogStream::StreamStar::iterator send = outputs->end();

        for (; siter != send; ++siter)
        {
            Name stream_name = (*siter)->get_stream_name();

            set<Name>::iterator preserve_found =
                streams_to_preserve.find(stream_name);

            if (preserve_found == streams_to_preserve.end())
            {
                StreamMap::iterator stream =
                    _topology._stream_map.find(stream_name);
                if (stream != _topology._stream_map.end())
                {
                    _topology._stream_map.erase(stream);
                }
            }
        }
    }

    _topology._box_map.erase(biter);

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Add a box to the catalog.
//
// Note: All fields of CatalogBox are properly set, 
// except sink and source ports of its in/out streams.
//
CatalogBox *Catalog::add_box(CatalogBox &box) throw(AuroraBadEntityException)
{
    CatalogBox  *cbox;
    Name         box_name;
    Name         table_name;
    Name         query_name;
//..............................................................................


    box_name = box.get_box_name();

    if (_topology._box_map.find(box_name) != _topology._box_map.end())
    {
        Throw(AuroraBadEntityException,
              "Box \"" + to_string(box.get_box_name()) +
              "\" already exists in the catalog!");
    }

    // name, type, node, parameters: directly copied.
    //
    cbox = &(_topology._box_map[box_name]);
    cbox->shallow_copy(box);

    // query
    //
    if (box.get_box_query())
    {
        DEBUG << "query";
        query_name = box.get_box_query()->get_query_name();

        if (_topology._query_map.find(query_name) == _topology._query_map.end())
        {
            // create new query.
            //
            _topology._query_map[query_name].set_query_name(query_name);
        }

        // Add box to query if necessary.
        CatalogBox::BoxStar *query_boxes =
            _topology._query_map[query_name].get_query_box();

        if (find(query_boxes->begin(), query_boxes->end(), cbox)
            == query_boxes->end())
        {
            query_boxes->push_back(cbox);
        }

        // set (new or existing) query for the added box.
        //
        cbox->set_box_query(&(_topology._query_map[query_name]));
    }
    /*
    else
    {
        // every box must belong to a query.
        // create single box query with same name as the box.
        //
        assert(_topology._query_map.find(box_name) ==
               _topology._query_map.end());
        _topology._query_map[box_name].set_query_name(box_name);
        _topology._query_map[box_name].get_query_box()->push_back(cbox);

        // set new query for the added box.
        //
        cbox->set_box_query(&(_topology._query_map[box_name]));
    }
    */

    // table
    //
    if (box.get_box_table())
    {
        DEBUG << "table";
        table_name = box.get_box_table()->get_table_name();
        query_name = box.get_box_query()->get_query_name();

        if (_topology._table_map.find(table_name) == _topology._table_map.end())
        {
            // create new table.
            //
            CatalogTable &new_table = _topology._table_map[table_name];
            new_table.shallow_copy(*(box.get_box_table()));

            // set its query as above.
            //
            CatalogQuery &table_query = _topology._query_map[query_name];
            new_table.set_table_query( &table_query );
            table_query.get_query_table()->push_back( &new_table );

            // set its schema.
            //
            Name schema_name = box.get_box_table()->get_table_schema_name();
            CatalogSchema &table_schema =
                *(box.get_box_table()->get_table_schema());

            if (_topology._schema_map.find(schema_name) ==
                _topology._schema_map.end())
            {
                // create new table schema.
                //
                _topology._schema_map[schema_name] = table_schema;
            }
            new_table.set_table_schema(&(_topology._schema_map[schema_name]));
        }

        // set (new or existing) table for the added box.
        //
        cbox->set_box_table(&(_topology._table_map[table_name]));
    }
    // else: no table specified, nothing to do..

    // in streams
    //
    if (box.get_box_in())
    {
        DEBUG << "in streams";
        CatalogStream::StreamStar *ins = box.get_box_in();

        for (unsigned int i = 0; i < ins->size(); i++)
        {
            CatalogStream &in = *(ins->at(i));
            Name in_name = in.get_stream_name();

            if (_topology._stream_map.find(in_name) ==
                _topology._stream_map.end())
            {
                // create new stream.
                //
                CatalogStream &new_in = _topology._stream_map[in_name];
                new_in.shallow_copy(in);

                // set its schema.
                //
                Name in_schema_name = in.get_schema_name();

                if (_topology._schema_map.find(in_schema_name) ==
                    _topology._schema_map.end())
                {
                    // create new stream schema.
                    //
                    _topology._schema_map[in_schema_name] =
                        *(in.get_stream_schema());
                }
                new_in.set_stream_schema(
                                &(_topology._schema_map[in_schema_name]));

                // we can't set stream's source and sink here !!!
                //
            }

            // NYI:  Added default InQueue???
            // add (new or existing) input stream for the added box.
            //
            cbox->add_in_stream(&(_topology._stream_map[in_name]), _in_queue);
        }
    }
    // else: no input stream specified, nothing to do..

    // out streams
    //
    if (box.get_box_out())
    {
        DEBUG << "out streams";
        CatalogStream::StreamStar *outs = box.get_box_out();

        for (unsigned int i = 0; i < outs->size(); i++)
        {
            CatalogStream &out = *(outs->at(i));
            Name out_name = out.get_stream_name();

            if (_topology._stream_map.find(out_name) ==
                _topology._stream_map.end())
            {
                // create new stream.
                //
                CatalogStream &new_out = _topology._stream_map[out_name];
                new_out.shallow_copy(out);

                // set its schema.
                //
                if (_topology._schema_map.find(out.get_schema_name()) ==
                    _topology._schema_map.end())
                {
                    // create new stream schema.
                    //
                    _topology._schema_map[out.get_schema_name()] =
                        *(out.get_stream_schema());
                }
                new_out.set_stream_schema(
                        &(_topology._schema_map[out.get_schema_name()]));
                // we can't set stream's source and sink here !!!
                //
                                       
            }

            // add (new or existing) output stream for the added box.
            //
            cbox->add_out_stream(&(_topology._stream_map[out_name]));
        }
    }
    // else: no output stream specified, nothing to do..

    return cbox;
}




////////////////////////////////////////////////////////////////////////////////
//
// Remove a filter box from the catalog and it's Out stream.
//
void Catalog::remove_filter(Name   box_name)
                     throw(AuroraBadEntityException)
{
    CatalogBox    *box;
    CatalogQuery  *query;
    string         message;
    Name           query_name;
//..............................................................................


    box = &(_topology._box_map[box_name]);

    if (!box)
    {   message = "Box \"" + to_string(box_name)
                           + "\" does not exist in the catalog.";

        DEBUG << message;
        Throw(AuroraBadEntityException, message);
    }
    else if (box->get_box_out()->size() != 1)
    {
        message = "There must be one Out stream to remove.";
 
        DEBUG << message;
        Throw(AuroraBadEntityException, message);
    }
    else if (box->get_box_table())
    {
        message = "NYI:  Can't remove boxes with tables.";
 
        DEBUG << message;
        Throw(AuroraBadEntityException, message);
    }
    else if (!box->get_box_query())
    {   message = "Box \"" + to_string(box_name)
                           + "\" is not contained in a query.";

        DEBUG << message;
        Throw(AuroraBadEntityException, message);
    }

    // ??? Rewrite using:  remove_box(Name);

    query_name = box->get_box_query()->get_query_name();

    query = &(_topology._query_map[query_name]);

    QueryMap::iterator qiter = _topology._query_map.find(query_name);

    if (query->get_query_box()->size() == 1)
    {
        // Remove the query object from the catalog as it has only this box.
        //
        if (qiter != _topology._query_map.end())
        {
            _topology._query_map.erase(qiter);
        }
    }
    else
    {   //  Remove this box from query's list of boxes.
        //
        query->remove_query_box(box_name);
    }

    // Remove the Out stream.
    //
    StreamMap::iterator stream = _topology._stream_map.find(
                                     box->get_out_stream(0)->get_stream_name());

    if (stream == _topology._stream_map.end())
    {
        message = "The Out stream was not cataloged.";
 
        DEBUG << message;
        Throw(AuroraBadEntityException, message);
    }
    else
    {   _topology._stream_map.erase(stream);
    }
 

    // Remove the filter from the catalog.
    //
    BoxMap::iterator biter = _topology._box_map.find(box_name);

    if (biter == _topology._box_map.end())
    {
        message = "The Box stream was not cataloged.";
 
        DEBUG << message;
        Throw(AuroraBadEntityException, message);
    }
    else
    {   _topology._box_map.erase(biter);
    }

    return;
}

BOREALIS_NAMESPACE_END
