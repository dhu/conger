/**
 * CongerDiagram.cc
 *
 *  Created on: Apr 17, 2012
 *      Author: hellojinjie
 */

#include "CongerDiagram.h"
#include "parseutil.h"

BOREALIS_NAMESPACE_BEGIN

CongerDiagram::CongerDiagram()
{

}

CongerDiagram::~CongerDiagram()
{
}

/**
 * 简化的 add schema，没有考虑设定 type size 的情况，也没有考虑 array 的情况
 */
CatalogSchema* CongerDiagram::add_conger_schema(string schema_name,
        map<string, string> schema_fields)
{
    TypeArray::iterator             in_array;

    string    message;
    string    name;
    string    type;
    string    size;

    DataType  data;
    uint8     data_size;

    try
    {
        name = schema_name;
        lowercase(name);

        begin_schema(name);

        map<string, string>::iterator field_iterator = schema_fields.begin();
        for ( ; field_iterator != schema_fields.end(); field_iterator++)
        {
            name = field_iterator->first;
            type = field_iterator->second;

            lowercase(name);
            data_size = data.from_string(type);
            add_field(name, data, data_size);
        }
    }
    catch(AuroraException  &e)
    {
        message = "Failed parsing a schema from XML:\n   " + e.as_string();
        DEBUG << message;
        Throw(AuroraBadEntityException, message);
    }
    return  _add_schema;
}

/**
 * 这里我们只解析 input stream， output stream 是不解析的
 */
CatalogStream* CongerDiagram::add_conger_stream(string stream_name, string schema_name)
{
    CatalogStream   *result;

    string    message;
    string    stream;
    string    schema;

    // Parse the stream and schema names.
    //
    try
    {
        stream = stream_name;
        lowercase(stream);

        schema = schema_name;
        result = add_stream(stream, schema);

    }
    catch(AuroraException  &e)
    {
        message = "Failed parsing an input from XML:\n   " + e.as_string();
        DEBUG << message;
        Throw(AuroraBadEntityException, message);
    }

    return  result;
}

void CongerDiagram::add_conger_query(string query_name,
        map<string, string> query_parameters)
{

}

CatalogBox* CongerDiagram::add_conger_box(string box_name,
        map<string, string> box_parameters)
{
    /*
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
*/
   // return  add_box;
    return NULL;
}

BOREALIS_NAMESPACE_END
