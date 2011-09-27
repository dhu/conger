#include "Box.h"

BOREALIS_NAMESPACE_BEGIN


// --------------------------------------------------
void Box::add_input(int port, StreamID stream_id)
{

    StreamBindings::iterator i = _inputs.begin();
    bool found = false;
    while (!found && i != _inputs.end())
    {
        if ( i->first == port)
        {
            i->second = stream_id;
            found = true;
        }
        i++;
    }
    if ( !found )
        _inputs.push_back( pair<int,StreamID>(port,stream_id));
}

// --------------------------------------------------
void Box::add_output(int port, StreamID stream_id)
{

    StreamBindings::iterator i = _outputs.begin();
    bool found = false;
    while (!found && i != _outputs.end())
    {
        if ( i->first == port)
        {
            i->second = stream_id;
            found = true;
        }
        i++;
    }
    if ( !found )
        _outputs.push_back( pair<int,StreamID>(port,stream_id));

}

// --------------------------------------------------
void Box::add_param(string name, string value)
{
    _params.push_back( pair<string,string>(name,value));
}

// --------------------------------------------------
string Box::get_param(string name) const
{
    BoxParams::const_iterator i;
    for ( i = _params.begin(); i != _params.end(); ++i )
    {
        if ( i->first == name )
        { return i->second;
        }
    }
    return string();
}

// --------------------------------------------------
string Box::repr() const
{

    string out;
    out << "\nBox{name=" << _name << " type=" << _type;

    out << "\n  Inputs=" << _inputs;
    out << "\n  Outputs=" << _outputs;
    out << "\n  Params=" << _params;
    out << "\n  Table=" << _table;
    out << "\n}";
    return out;
}


// --------------------------------------------------
string Box::as_xml() const
{

    string out = string ("<box name=\"" + to_string(_name)
                         + "\" type=\"" + _type + "\">\n" );
    StreamBindings::const_iterator i;
    for ( i = _inputs.begin(); i != _inputs.end(); ++i)
        out << "  <in port=\"" << i->first << "\" stream=\""
            << xml_escape(to_string(i->second.get_name())) << "\"/>\n";
    for ( i = _outputs.begin(); i != _outputs.end(); ++i)
        out << "  <out port=\"" << i->first << "\" stream=\""
            << xml_escape(to_string(i->second.get_name())) << "\"/>\n";
    BoxParams::const_iterator j;
    for ( j = _params.begin(); j != _params.end(); ++j)
        out << "  <parameter name=\"" << xml_escape(j->first) << "\" value=\""
            << xml_escape(j->second) << "\"/>\n";

    if (_table)
    out << "  <table name=\"" + xml_escape(to_string(_table)) + "\"/>\n";

    out << "</box>\n";
    return out;
}


////////////////////////////////////////////////////////////////////////////////
//
//  Create a Box object from a box definition in the catalog.
//
void  Box::from_catalog( CatalogBox  *box )
{
    uint32        port;
    vector<Name>  cp_ids;

     BoxParameter::ParameterMap           *parameter_map;
     BoxParameter::ParameterMap::iterator  parameter;
     CatalogStream::StreamStar::iterator  stream;
//..............................................................................


    _name = box->get_box_name();
    _type = box->get_box_type();
    port   = 0;
    DEBUG <<  "Box name=" << box->get_box_name()
          << " Box type=" << box->get_box_type();

    for ( stream  = box->get_box_in()->begin();
          stream != box->get_box_in()->end(); ++stream )
    {
        port++;
        DEBUG << "Input stream " << (*stream)->get_stream_name();
        Name stream_name( (*stream)->get_stream_name() );

        add_input( (int)port, StreamID( stream_name, cp_ids ));
    }

    // Output streams
    port = 0;

    for ( stream  = box->get_box_out()->begin();
          stream != box->get_box_out()->end(); ++stream )
    {
         port++;
         Name stream_name( (*stream)->get_stream_name() );

         add_output( (int)port, StreamID( stream_name ));
    }


    // Box parameters.
    //
    parameter_map = box->get_box_parameter_map();

    for (  parameter  = parameter_map->begin();
           parameter != parameter_map->end(); ++parameter )
    {
         _params.push_back( pair<string,string>( (*parameter).first,
                                                  (*parameter).second ));
    }

    // A Box may reference a single table.
    //
    if ( box->get_box_table() )
    {   _table = box->get_box_table()->get_table_name();
    }

    return;
}

BOREALIS_NAMESPACE_END
