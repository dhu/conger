#include "CatalogStream.h"

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
//  Match a stream to the reference stream (unchanged).
//  If the stream argument has no schema it is assigned the schema.
//
Boolean  CatalogStream::match_stream_schema( // Stream to match up.
                                             CatalogStream  *Match
                                            )
{
    Boolean         status = True;
    CatalogSchema  *in_schema;
    CatalogSchema  *out_schema;
//
//..............................................................................


    in_schema  = get_stream_schema();
    out_schema = Match->get_stream_schema();

    if (!in_schema)
    {   DEBUG << "Undefined In schema for stream (" + get_stream_name_string() + ")";
        status = False;
    }
    else if (!out_schema)
    {   DEBUG << "Set schema (" << to_string(in_schema->get_schema_name())
              << ") to stream (" << Match->get_stream_name_string() << ").";

        Match->set_stream_schema(in_schema);
    }
    else if (in_schema != out_schema)
    {   status = in_schema->equal_schema(out_schema);
    }

    return( status );
}



////////////////////////////////////////////////////////////////////////////////
//
void CatalogStream::subscribe_source( CatalogSubscription  *remote_source )
{
//..............................................................................


    _remote_source = remote_source;
    _remote_source->set_subscription_stream( this );

    _source.set_box_port( 0, 0 );

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void CatalogStream::subscribe_sink( CatalogSubscription  *remote_sink )
{
//..............................................................................


    _remote_sink.push_back( remote_sink );
    remote_sink->set_subscription_stream( this );

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  CatalogStream::unsubscribe_sink( CatalogSubscription  *remote_sink )
{
      vector<CatalogSubscription*>::iterator   sink_found;
//..............................................................................


    sink_found = find( _remote_sink.begin(),
                       _remote_sink.end(),
                        remote_sink );

    if ( sink_found != _remote_sink.end() )
    {    _remote_sink.erase( sink_found );
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void CatalogStream::shallow_copy(const CatalogStream &stream)
{
//..............................................................................


    _name = stream.get_stream_name();
    _schema = NULL;
    _sink.clear();
    _remote_source = NULL;
    _remote_sink.clear();

    return;
}




////////////////////////////////////////////////////////////////////////////////
//
// Get stream source box.
//
CatalogBox *CatalogStream::get_stream_source_box()
{
//..............................................................................


    return(_source.get_port_box());
}



////////////////////////////////////////////////////////////////////////////////
//
vector<CatalogBox*> CatalogStream::get_stream_sink_boxes()
{
    vector<CatalogBox*> boxes;
//..............................................................................


    for (register unsigned int i = 0; i < _sink.size(); i++)
    {
        boxes.push_back(_sink[i].get_port_box());
    }

    return boxes;
}



////////////////////////////////////////////////////////////////////////////////
//
// Find a sink connected to a box and port.
//
BoxPort  *CatalogStream::find_sink_box_port(CatalogBox  *box, uint16  port)
{
    vector<BoxPort>::iterator   box_port;
//..............................................................................


    for (box_port = _sink.begin(); box_port != _sink.end(); box_port++)
    {
        //DEBUG <<    "box=" << box_port->get_port_box()
        //      << "  port=" << box_port->get_port();

        if ((box_port->get_port_box() == box)  &&
            (box_port->get_port() == port))
        {
            return(&(*box_port));
        }
    }

    return(NULL);
}



////////////////////////////////////////////////////////////////////////////////
//
// Set stream source box.
//
void  CatalogStream::set_stream_source_box(CatalogBox *box)
{
//..............................................................................

    _source.set_box(box);

    if (box)
    {   _remote_source = NULL;
    }
}



////////////////////////////////////////////////////////////////////////////////
//
string  CatalogStream::as_string() const
{
    string s;
//..............................................................................


    s += string() + "CatalogStream{";
    s += ( "name=" + _name.as_string() + ",schema=" );

    if ( _schema )
    {   s += _schema->as_string();
    }
    else
    {   s += "NULL";
    }

    s += ",source=";
    s += _source;
    s += "}";

    return(s);
}


BOREALIS_NAMESPACE_END
