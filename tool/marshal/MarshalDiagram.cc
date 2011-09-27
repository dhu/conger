#include  "MarshalDiagram.h"
#include  "parseutil.h"

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
// Reparse Schemas, Inputs and Outputs to add extra diagram items.
//
Status  MarshalDiagram::parse_marshal()
{
    Status       status = true;

    string       prefix;
    string       endpoint;
    string       name;
    string       lower;
    string       stream;

    vector<DOMElement *>            element;
    vector<DOMElement *>::iterator  iterator;
//
//  Entry:  The XML has been parsed into the catalog.
//          _prefix contains the default prefix for the client name.
//..............................................................................


    try
    {   // Reparse schemas and retain the mixed case name.
        //
        xml_child_elements(element, _root, "schema");

        for (iterator = element.begin(); iterator != element.end(); ++iterator)
        {
            xml_expect_tag(*iterator, "schema");
            name  = xml_attribute( *iterator, "name", ATTR_REQUIRED );
            lower = name;
            lowercase( lower );

            DEBUG  << "<schema  name=\"" << name << "\">";

            _schema_mixed[lower] = name;    // Map lowercase schema to mixed case.
        }

        // If any subscription elements not within a client element,
        // the default is in use.  Create an empty default client.
        //
        xml_child_elements(element, _root, "subscribe");

        if (!element.empty())
        {   // The default client always uses the default monitor port.
            // This may already be set and is redundant.
            //
            _generate_client[_prefix] =
                form_endpoint("", DEFAULT_MONITOR_PORT);

            DEBUG <<  "prefix="   << prefix
                  << " client="   << _generate_client[prefix];
        }

        // Reparse publish elements for the default client.
        //
        status = reparse_publish(_root, _prefix);

        if (status)
        {   // Reparse client elements and retain the endpoints.
            //
            xml_child_elements(element, _root, "client");

            for (iterator  = element.begin();
                 iterator != element.end(); iterator++)
            {
                // Parse the client name and endpoint.
                //
                xml_expect_tag(*iterator, "client");
                prefix   = xml_attribute(*iterator, "prefix",   ATTR_NOT_REQUIRED);
                endpoint = xml_attribute(*iterator, "endpoint", ATTR_NOT_REQUIRED);

                if (prefix.empty())
                {   prefix = _prefix;
                }

                endpoint = Util::form_endpoint(endpoint, DEFAULT_MONITOR_PORT);

                if (_generate_client.find(prefix) == _generate_client.end())
                {   _generate_client[prefix] = endpoint;
                    DEBUG << "client prefix=" << prefix << " endpoint=" << endpoint;
                }
                else if (!endpoint.empty())
                {   if (_generate_client[prefix] != endpoint)
                    {   status = "Missmatched endpoints for a client.";

                        DEBUG <<  "prefix="   << prefix
                              << " client="   << _generate_client[prefix]
                              << " endpoint=" << endpoint;
                    }
                }

                if (status)
                {   status = reparse_publish(*iterator, prefix);
                }

                if (!status)
                {   DEBUG << status;
                    break;
                }
            }
        }
    }
    catch( AuroraException  &e )
    {   DEBUG << "Failed reparsing Schemas, Inputs and Outputs:  " << e;
        status = "Failed reparsing Schemas, Inputs and Outputs:  " + e.as_string();
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
// Reparse Publish elements to add extra diagram items.
//
Status  MarshalDiagram::reparse_publish(const DOMElement  *root,
                                                  string   prefix)
{
    string       name;
    string       lower;
    string       stream;
    string       node;

    vector<BoxPort>                *sink_port;
    vector<BoxPort>::iterator       port;

    vector<DOMElement *>            element;
    vector<DOMElement *>::iterator  iterator;
//..............................................................................


    try
    {   // Reparse publish elements and retain the endpoints.
        //
        xml_child_elements( element, root, "publish" );

        for (iterator = element.begin(); iterator != element.end(); ++iterator)
        {
            // Parse the stream name and endpoint.
            //
            xml_expect_tag( *iterator, "publish" );
            stream = xml_attribute(*iterator, "stream", ATTR_REQUIRED);
            node   = xml_attribute(*iterator, "node",   ATTR_NOT_REQUIRED);

            lowercase( stream );

            // map(pub stream->client)
            //               ->[0 to n nodes]
            //
            // create if a new stream; else reference old.
            GeneratePublish  &publish = _generate_publish[stream];

            if (publish._prefix.empty())
            {   publish._prefix = prefix;            // empty for default???
            }
            else
            {   DEBUG << "NYI:  prefix must match.";
            }

            if (node.empty())
            {   sink_port = get_stream(stream)->get_sink_port();

                for (port = sink_port->begin(); port != sink_port->end(); port++)
                {
                    node = port->get_port_box()->get_box_node();
                    publish._node.push_back(node);
                    DEBUG << "stream=" << stream << " node=" << node;
                }
            }
            else
            {   node = Util::form_endpoint(node, DEFAULT_BOREALIS_PORT);
                publish._node.push_back(node);
                DEBUG << "stream=" << stream << " node=" << node;
            }
        }
    }
    catch( AuroraException  &e )
    {   DEBUG << "Failed reparsing a publish or subscribe:  " << e;
        return( "Failed reparsing a publish or subscribe:  " + e.as_string() );
    }

    return  true;
}



////////////////////////////////////////////////////////////////////////////////
//
// Establish the default prefix for the client name.
//
void  MarshalDiagram::setDefaultPrefix(/// The first relative XML file path.
                                       /// 
                                       string  xml_file)
{
    string::size_type    index;
//..............................................................................


    _prefix = xml_file;
    lowercase(_prefix);

    if (_prefix.substr(_prefix.length() - 4, 4) == ".xml")
    {   _prefix.erase( _prefix.length() - 4, 4);
    }

    index = _prefix.rfind("/");

    if (index != string::npos)
    {   _prefix.erase(0, index + 1);
    }

    proper_case(_prefix);
    DEBUG << "prefix=" << _prefix;

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Automatically fill in deployment for a network.
//
Status  MarshalDiagram::generate_deployment()
{
    Status   status = true;
    string   endpoint;
    string   name;

    GeneratePublish           *publish;
    StreamMap::const_iterator  stream;
//..............................................................................


    status = Diagram::generate_deployment();

    if (status)
    {   for ( stream  = get_stream_map()->begin();
              stream != get_stream_map()->end(); stream++ )
        {
            name = to_string( (*stream).first );
            DEBUG << "name=" << name;

            if ( (*stream).second.is_input_stream() )
            {   endpoint = form_endpoint("", DEFAULT_BOREALIS_PORT);
                publish  = &_generate_publish[name];

                publish->_prefix = _prefix;
                publish->_node.push_back(endpoint);

                DEBUG << "generate publish for " << name;
            }
            else
            {   // done by Diagram::generate_deployment???
                //endpoint = form_endpoint( "", DEFAULT_MONITOR_PORT );
                //add_subscribe( name, endpoint, 0xFFFF );

                DEBUG << "no need to generate subscribe for " << name;
            }
        }

        // what if no ins or outs???
        endpoint = form_endpoint("", DEFAULT_MONITOR_PORT);
        _generate_client[_prefix] = endpoint;

        DEBUG <<  "prefix="   << _prefix
              << " client="   << _generate_client[_prefix]
              << " endpoint=" << endpoint;
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Capitalize the first letter in a sequence of alphabetic characters.
//
void MarshalDiagram::proper_case( // Symbolic name to format.
                                  string  &name )
{
    Boolean   lower = False;
//..............................................................................


    for ( uint32 i = 0; i < name.length(); i++ )
    {   if ( !isalpha( name[i] ))            // IF not alphabetic,
        {   lower = False;                   //     Uppercase the next alphabetic.
        }
        else if ( lower )                    // ELSE IF after first alphabetic,
        {   name[i] = tolower( name[i] );    //     Lowercase the output.
        }
        else                                 // ELSE first alphabetic in subsection,    
        {   name[i] = toupper( name[i] );    //     Uppercase the output.
            lower = True;                    //     Rest of this subsection is low.
        }                                    // END IF
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Determine the schemas for pending intermediate streams.
//  Check for cyclic deployment.
//
Status  MarshalDiagram::infer_schema()
{
    Status       status = true;
    Boolean      done   = False;
    Boolean      cyclic;
    Boolean      infer;
    BoxMap      *box_map;

    set<CatalogBox *>    box_infer;

            set<CatalogBox *>::iterator  box;
                       BoxMap::iterator  map;
    CatalogStream::StreamStar::iterator  in;
    CatalogStream::StreamStar           *box_in;
//
//  If there are any boxes the network must be complete and acyclic.
//  If there are no boxes no inferencing is done and no errors are issued.
//  If any box has an undefined external type a warning is issues and
//  furthur inferencing is suspended.
//..............................................................................


    // List all the boxes yet to be inferred.
    //
    box_map = get_box_map();

    for (map = box_map->begin(); map != box_map->end(); map++)
    {
        box_infer.insert(&(map->second));
    }


    // do until all streams have schemas,
    //
    while (( status )  &&  ( !done ))
    {   cyclic = False;
        done   = True;

        //  Do over boxes pending inferencing on each node,
        //
        DEBUG << "-------- Infer " << box_map->size() << " boxes ...";

        for (box = box_infer.begin(); box != box_infer.end(); box++)
        {
            DEBUG  << "box_name=" << (*box)->get_box_name();
            
            cyclic = True;
            infer  = True;
            done   = False;
            box_in = (*box)->get_box_in();

            // See if all Ins have schemas.
            //
            for (in = box_in->begin(); in != box_in->end(); in++)
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
                DEBUG  << "Infer box " << (*box)->get_box_name();
                status = (*box)->infer_box_out(get_schema_map());

                if (status.is_false())
                {   WARN << "WARNING:  The box (" << (*box)->get_box_name()
                         << ") has an unknown external type ("
                         << (*box)->get_box_type() << ").";

                    status = true;
                    done   = True;      // Give up on any more inferencing.
                }

                // Remove the box from the to-do list;  Rescan boxes.
                //
                box_infer.erase(&*(*box));
                break;
            }

            DEBUG << "Skip box (" << (*box)->get_box_name() << ")";
        }

        // Assert at least one box was resolved each round.
        //
        if (cyclic)
        {   status = "Cyclic network.";
        }
    }

    return  status;
}


BOREALIS_NAMESPACE_END
