#include  "CatalogView.h"

BOREALIS_NAMESPACE_BEGIN



////////////////////////////////////////////////////////////////////////////////
//
//  Create an XML string for a connection point view.
//
string  CatalogView::form_view_xml()
{
    string     element;

    BoxParameter::ParameterMap           *map;
    BoxParameter::ParameterMap::iterator  parameter;
//
//..............................................................................


    element = "<connection_point_view  name=\"" + Util::full_name(get_view_name())
                              + "\"  stream=\"" + Util::full_name(get_view_stream_name())
                              + "\" >\n";

    if (get_view_field().empty())
    {   element += "<range  start=\"" + to_string(get_view_start())
                       + "\"  end=\"" + to_string(get_view_end()) + "\" />\n";
    }
    else
    {   element += "<order  field=\"" + to_string(get_view_field())  + "\" />\n";
        element +=  "<size  value=\"" + to_string(get_view_start())  + "\" />\n";
        element += "<radius value=\"" + to_string(get_view_radius()) + "\" />\n";
    }

    // View parameters.
    //
    map = get_view_parameter_map();

    for (parameter  = map->begin();
         parameter != map->end(); parameter++)
    {
        // Escape the parameter value for the engine's parser.
        //
        element += " <parameter  name=\"" + parameter->first
                          + "\" value=\"" + xml_escape(parameter->second)
                          + "\"/>\n";
    }

    element += "</connection_point_view>\n";
    DEBUG << element;

    return  element;
}


BOREALIS_NAMESPACE_END
