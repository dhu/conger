#include "Wrapper.h"
#include <stdlib.h>
#include "xercesDomUtil.h"

#define CMDPORT 8888    /* the port where the client will be connecting to */
#define CMDIP "127.0.0.1"    /* the ip where the client will be connecting to */

TinyDBWrapper::Wrapper::Wrapper()
{
    _ws_state = ws_invalid;
}

/*
Open connection to java part
get static information (schema, boxes, metrics)
*/
bool TinyDBWrapper::Wrapper::Initialize()
{
    if (_ws_state != ws_invalid)
    {
        return false;
    }
    try
    {
        // open command socket
        if (!_cmd_socket.create())
        {
            cerr << "Error Create" << endl;
            return false;
        }

        if (!_cmd_socket.connect(CMDIP, CMDPORT))
        {
            cerr << "Error Connect" << endl;
            _cmd_socket.close();
            return false;
        }

        // get TinyDB Catalog
        if (!_catalog.retrieve_catalog(_cmd_socket))
        {
            cerr << "Error get catalog" << endl;
            _cmd_socket.close();
            return false;
        }
  // TODO
    }
    catch(exception& e)
    {
        cerr << "Exception: Wrapper::Initialize(): " << e.what() << endl;
        _catalog.clear();
        _cmd_socket.close();
        return false;
    }
    _ws_state = ws_initialized;
    return true;
}

void TinyDBWrapper::Wrapper::deinitialize()
{
    if (_ws_state == ws_invalid)
    {
        return;
    }
    _ws_state = ws_invalid;
    try
    {
        // close all data sockets

        // close command socket
        _cmd_socket.write_byte(CMD_EXIT);
        _cmd_socket.flush();
        // clear catalog
        _catalog.clear();
        _cmd_socket.close();
    }
    catch(exception& e)
    {
        cerr << "Exception: Wrapper::Deinitialize()" << e.what() << endl;
        return;
    }
    return;
}

TinyDBWrapper::Catalog* TinyDBWrapper::Wrapper::get_catalog()
{
    return &_catalog;
}

string TinyDBWrapper::Wrapper::get_catalog_xml()
{
    string str_xml = "<modify>\n\t<add>\n\t\t";
    string str_temp = _catalog.get_schemata_xml();
    unsigned int i_pos = 0;
    while ((i_pos = str_temp.find("\n", i_pos)) != string::npos)
    {
        i_pos++;
        if (i_pos < str_temp.length())
        {
            str_temp.insert(i_pos, "\t\t");
        }
    }
    str_xml.append(str_temp);
    str_xml.append("\t</add>\n</modify>\n");
    return str_xml;
}

TinyDBWrapper::TQuery* TinyDBWrapper::Wrapper::prepare_query(string str_borealis_query_xml, int i_epoch_duration)
{
    // basic assumptions: no operators with multiple input streams allowed
    //                    fields are named: schema.field,
    //                    predicates use field names or aggregate expressions: schema.field < avg(schema.field)
    // boxes must be listed in order within query from the first input to
    // the last output stream
    // currently supported operators: filter, map,
    //                                aggregate (only one box per query)
    //                                static join (only one box per query)
    TQuery* p_tquery = NULL;
    try
    {
        TQuery new_tquery;
        map<string, string> mstr_input_streams;
        map<string, string> mstr_output_streams;

        ptr<DOMDocument> doc = parseXmlString(str_borealis_query_xml);
        const DOMElement *root = doc->getDocumentElement();

        xml_expect_tag(root, "query");

        new_tquery.set_name(xml_attribute(root, "name", ATTR_REQUIRED));

        vector<DOMElement*> v_boxes;
        xml_child_elements(v_boxes, root, "box");
        QueryComponents qc_query_components;
        // at the beginning all attributs are selected
        for (unsigned int a=0; a<get_catalog()->get_schema_count(); a++)
        {
            for (unsigned int i=0; i<get_catalog()->get_schema(a)->get_field_count(); i++)
            {
                qc_query_components.vstr_select_expressions.insert(qc_query_components.vstr_select_expressions.end(),
                    get_catalog()->get_schema(a)->get_name() + "." + get_catalog()->get_schema(a)->get_field(i)->get_name());
            }
        }
        // parse all boxes of the query
        for (unsigned int i=0; i<v_boxes.size(); i++)
        {
            Box new_box;
            // returns with a list of boxes, list of the used input and output streams
            // and a struct that contains the basic information to build TinySQL query
            if (!box_from_dom(*(v_boxes.at(i)), new_box, mstr_input_streams, mstr_output_streams, qc_query_components))
            {
                return NULL;
            }
            else
            {
                new_tquery.add_box(new_box);
            }
        }
        // check query
        // asumption: query is in correct borealis notation
        // first check if inputs of the subnetwork are all within TinyDB and only one output exists

        // remove all streams of TinyDB from list of inputs
        for (unsigned int i=0; i<get_catalog()->get_schema_count(); i++)
        {
            mstr_input_streams.erase(get_catalog()->get_schema(i)->get_stream_name());
        }
        // remove used from list of input streams, since it is a table
        if (qc_query_components.str_table.length() > 0)
        {
            mstr_input_streams.erase(qc_query_components.str_table);
        }
        // remove all streams that are input of a box and output of another box within tinydb
        unsigned int i_key_no = mstr_output_streams.size();
        for (unsigned int i=0; i<i_key_no; i++)
        {
            // remove input streams that are output of another box
            if (mstr_input_streams.erase(mstr_output_streams.begin()->first) > 0)
            {
                // remove the output stream (only one, if borealis query
                // is correct) if it was used as input stream
                if (mstr_output_streams.erase(mstr_output_streams.begin()->first) != 1)
                {
                    cerr << "Error: syntax error: two output streams with same name" << endl;
                    return NULL;
                }
            }
        }
        // now mstrInputStreams should be empty and mstrOutputStreams should contain exact one element
        if (mstr_input_streams.size() != 0 || mstr_output_streams.size() != 1)
        {
            cerr << "Error: syntax error: input streams are allowed only from data source and only one output stream is allowed" << endl;
            return NULL;
        }
        // add output stream to query
        new_tquery.set_query_stream(mstr_output_streams.begin()->second);
        // generate TinySQL query from struct qcQueryComponents
        string str_query = generate_tiny_sqlquery(qc_query_components);
        if (str_query.length() == 0)
        {
            cerr << "Error: could not generate TinySQL statement" << endl;
            return NULL;
        }
        // add epoch duration
        str_query.append("EPOCH DURATION ");
        char ac_buffer[256];
        sprintf(ac_buffer, "%d", i_epoch_duration);
        str_query.append(ac_buffer);
        cout << "New query: " << str_query << endl;
        int i_query_id;
        if ((i_query_id = check_query_syntax(str_query)) == -1)
        {
            return NULL;
        }
        if (!new_tquery.prepare(str_query, i_query_id))
        {
            return NULL;
        }
        // set schema of output stream
  // TODO borealis interface mut be queried, eventually for all boxes
        // add query to list of queries
        p_tquery = _tqueries.add_query(new_tquery);
        p_tquery->set_cmd_socket(&_cmd_socket);
    }
    catch (AuroraException& e)
    {
        cerr << "Aurora exception: " << e.what() << endl;
        return NULL;
    }
    return p_tquery;
}

TinyDBWrapper::TQuery* TinyDBWrapper::Wrapper::re_prep_query(TinyDBWrapper::TQuery* t)
{
  int i_query_id;
  if ((i_query_id = check_query_syntax(t->get_tiny_dbquery())) == -1)
    {
      return NULL;
    }
  if (!t->re_prepare(i_query_id))
    {
      return NULL;
    }
  return t;
}

string TinyDBWrapper::Wrapper::generate_tiny_sqlquery(QueryComponents& rqc_query_components)
{
// generate TinySQL query from struct rqcQueryComponents
    string str_query = "SELECT ";
    if (rqc_query_components.vstr_select_expressions.size() == 0)
    {
        cerr << "Error: syntax error: now selected fields" << endl;
        return "";
    }
    else
    {
        for (unsigned int i=0; i<rqc_query_components.vstr_select_expressions.size()-1; i++)
        {
            if (i == 0)
            {
                // check if first attribute is epoch
                if (rqc_query_components.vstr_select_expressions.at(i).compare("sensors.epoch") != 0)
                {
                    cerr << "attribute epoch must be first attribute" << endl;
                    return "";
                }
            }
            else
            {
                str_query.append(rqc_query_components.vstr_select_expressions.at(i) + ", ");
            }
        }
        // last attribute without ','
        str_query.append(rqc_query_components.vstr_select_expressions.at(rqc_query_components.vstr_select_expressions.size()-1) + " ");
    }
    // epoch not allowed within TinySQL statement
    if (str_query.find("sensors.epoch", 0) != string::npos)
    {
        cerr << "attribute epoch is only allowed once" << endl;
        return "";
    }
    // hack: assuming sensors
    str_query.append("FROM sensors");
    // add optional join table
    if (!rqc_query_components.str_table.empty())
    {
        str_query.append(", " + rqc_query_components.str_table + " ");
    }
    else
    {
        str_query.append(" ");
    }
    if (rqc_query_components.vstr_where_predicates.size() > 0)
    {
        str_query.append("WHERE ");
        for (unsigned int i=0; i<rqc_query_components.vstr_where_predicates.size()-1; i++)
        {
            str_query.append(rqc_query_components.vstr_where_predicates.at(i) + " AND ");
        }
        // last predicate without 'AND'
        str_query.append(rqc_query_components.vstr_where_predicates.at(rqc_query_components.vstr_where_predicates.size()-1) + " ");
    }
    if (!rqc_query_components.str_group_by_expression.empty())
    {
        str_query.append("GROUP BY " + rqc_query_components.str_group_by_expression);
        str_query.append(" ");
    }
    if (rqc_query_components.vstr_having_predicates.size() > 0)
    {
        str_query.append("HAVING ");
        for (unsigned int i=0; i<rqc_query_components.vstr_having_predicates.size()-1; i++)
        {
            str_query.append(rqc_query_components.vstr_having_predicates.at(i) + " AND ");
        }
        // last predicate without 'AND'
        str_query.append(rqc_query_components.vstr_having_predicates.at(rqc_query_components.vstr_having_predicates.size()-1) + " ");
    }
    return str_query;
}

bool TinyDBWrapper::Wrapper::box_from_dom(DOMElement& r_domelement,
                                        Box& r_box,
                                        map<string, string>& rmstr_input_streams,
                                        map<string, string>& rmstr_output_streams,
                                        QueryComponents& rqc_query_components)
{
    try
    {
        r_box.set_name(xml_attribute(&r_domelement, "name", ATTR_REQUIRED));
        r_box.set_type(xml_attribute(&r_domelement, "type", ATTR_REQUIRED));

        map<string, string> mstr_supported_boxes;
        // get actual supported boxes, one or all out of [filter, map, staticjoin, aggregate]
        for (unsigned int i=0; i<get_catalog()->get_box_count(); i++)
        {
            mstr_supported_boxes.insert(pair<string, string>(get_catalog()->get_box(i)->get_type(),
                                                           get_catalog()->get_box(i)->get_type()));
        }
        // only the four types are allowed and only if they actual supported by tinydb
        if ((r_box.get_type() != "filter"
             && r_box.get_type() != "map"
             && r_box.get_type() != "staticjoin"
             && r_box.get_type() != "aggregate")
            || (mstr_supported_boxes.find(r_box.get_type()) == mstr_supported_boxes.end()))
        {
            cerr << "Error: unsupported box type: " << r_box.get_type() << endl;
            return false;
        }

        vector<DOMElement*> v_streams;

        // Input streams
        xml_child_elements(v_streams, &r_domelement, "input");
        if (v_streams.size() == 0)
        {
            cerr << "Error: no input streams for box : " << r_box.get_name() << endl;
            return false;
        }
        for (unsigned int i=0; i<v_streams.size(); i++)
        {
            int i_port;
            pair<string, string> pstr_stream;
            get_dom_attribute(*(v_streams.at(i)), "port", i_port);
            string str_name;
            get_dom_attribute(*(v_streams.at(i)), "stream", str_name);
            Stream new_stream(i_port, str_name);
            r_box.add_input(new_stream);
            pstr_stream.first    = str_name;
            pstr_stream.second   = str_name;
            rmstr_input_streams.insert(pstr_stream);
        }
        v_streams.clear();

        // Output streams
        xml_child_elements(v_streams, &r_domelement, "output");
        if (v_streams.size() == 0)
        {
            cerr << "Error: no output streams for box : " << r_box.get_name() << endl;
            return false;
        }
        for (unsigned int i=0; i<v_streams.size(); i++)
        {
            int i_port;
            pair<string, string> pstr_stream;
            get_dom_attribute(*(v_streams.at(i)), "port", i_port);
            string str_name;
            get_dom_attribute(*(v_streams.at(i)), "stream", str_name);
            Stream new_stream(i_port, str_name);
            r_box.add_output(new_stream);
            pstr_stream.first    = str_name;
            pstr_stream.second   = str_name;
            rmstr_output_streams.insert(pstr_stream);
        }
        v_streams.clear();

        // Parameters
        map<string, string> mstr_parameters;
        xml_child_elements(v_streams, &r_domelement, "parameter");
        for (unsigned int i=0; i<v_streams.size(); i++)
        {
            pair<string, string> pstr_parameter;
            string str_name;
            get_dom_attribute(*(v_streams.at(i)), "name", str_name);
            string str_value;
            get_dom_attribute(*(v_streams.at(i)), "value", str_value);
            Parameter new_parameter(str_name, str_value);
            r_box.add_parameter(new_parameter);
            pstr_parameter.first     = str_name;
            pstr_parameter.second    = str_value;
            mstr_parameters.insert(pstr_parameter);
        }
        v_streams.clear();
        // check parameters
        if (r_box.get_type() == "filter")
        {
            if (!check_filter_box(r_box, mstr_parameters, rqc_query_components))
            {
                return false;
            }
        }
        else if (r_box.get_type() == "map")
        {
            if (!check_map_box(r_box, mstr_parameters, rqc_query_components))
            {
                return false;
            }
        }
        else if (r_box.get_type() == "staticjoin")
        {
            if (!check_static_join_box(r_box, mstr_parameters, rqc_query_components))
            {
                return false;
            }
        }
        else if (r_box.get_type() == "aggregate")
        {
            if (!check_aggregate_box(r_box, mstr_parameters, rqc_query_components))
            {
                return false;
            }
        }
        else
        {
            // should not happen
            cerr << "unknown parse error" << endl;
            return false;
        }

        // add properties

    }
    catch (AuroraException& e)
    {
        cerr << "Aurora exception: Wrapper::BoxFromDom()" << e.what() << endl;
        return false;
    }
    return true;
}

bool TinyDBWrapper::Wrapper::check_filter_box(Box& r_box, map<string, string>& rmstr_parameters, QueryComponents& rqc_query_components)
{
    // a filter box adds a predicate to the list of predicates
    // predicates of different boxes combined with AND
    if (rmstr_parameters.size() != 2)
    {
        cerr << "Error: wrong number of parameters for filter: " << r_box.get_name() << endl;
        return false;
    }
    // exact one predicate
    map<string, string>::iterator iter;
    if ((iter = rmstr_parameters.find("predicate.0")) != rmstr_parameters.end())
    {
        // after an aggregate box predicates are added to the having list
        if (!rqc_query_components.str_group_by_expression.empty())
        {
            if (!check_having_predicate(iter->second))
            {
                cerr << "Error: wrong format of predicate.0 for filter: " << r_box.get_name() << endl;
                return false;
            }
            rqc_query_components.vstr_having_predicates.insert(rqc_query_components.vstr_having_predicates.end(), iter->second);
            }
        else
        {
            if (!check_filter_predicate(iter->second))
            {
                cerr << "Error: wrong format of predicate.0 for filter: " << r_box.get_name() << endl;
                return false;
            }
            rqc_query_components.vstr_where_predicates.insert(rqc_query_components.vstr_where_predicates.end(), iter->second);
        }
    }
    else
    {
        cerr << "Error: missing predicate.0 for filter: " << r_box.get_name() << endl;
        return false;
    }
    // pass-on-false-port must be 0
    if ((iter = rmstr_parameters.find("pass-on-false-port")) != rmstr_parameters.end())
    {
        if (iter->second.compare("0") != 0)
        {
            cerr << "Error: pass-on-false-port must be set to 0 for filter: " << r_box.get_name() << endl;
            return false;
        }
    }
    else
    {
        cerr << "Error: missing pass-on-false-port for filter: " << r_box.get_name() << endl;
        return false;
    }
    return true;
}

bool TinyDBWrapper::Wrapper::check_map_box(Box& r_box, map<string, string>& rmstr_parameters, QueryComponents& rqc_query_components)
{
    // a map "reduces" the list of selected attributes
    // any upstream map is more restrictive -> replaces the original list
    // no map after an aggregate box
    if (!rqc_query_components.str_group_by_expression.empty())
    {
        cerr << "Error: after an aggregate box no map allowed: " << r_box.get_name() << endl;
        return false;
    }           if ((rmstr_parameters.size() % 2) != 0)
    {
        cerr << "Error: parameter(s) missing for map: " << r_box.get_name() << endl;
        return false;
    }
    unsigned int i_no_of_fields = rmstr_parameters.size() / 2;
    // delete old list
    rqc_query_components.vstr_select_expressions.erase(rqc_query_components.vstr_select_expressions.begin(), rqc_query_components.vstr_select_expressions.end());
    // expression and output-field-name must be identical and an attribute of sensors
    map<string, string>::iterator iter1;
    map<string, string>::iterator iter2;
    char ac_buffer[256];
    for (unsigned int i=0; i<i_no_of_fields; i++)
    {
        sprintf(ac_buffer, "output-field-name.%d", i);
        if ((iter1 = rmstr_parameters.find(ac_buffer)) == rmstr_parameters.end())
        {
            cerr << "Error: missing " << ac_buffer << " for map: " << r_box.get_name() << endl;
            return false;
        }
        sprintf(ac_buffer, "expression.%d", i);
        if ((iter2 = rmstr_parameters.find(ac_buffer)) == rmstr_parameters.end())
        {
            cerr << "Error: missing " << ac_buffer << " for map: " << r_box.get_name() << endl;
            return false;
        }
        if (iter1->second.compare(iter2->second) != 0)
        {
            cerr << "Error: value of " << ac_buffer << " must be identicall to the corresponding value of its output field name for map: " << r_box.get_name() << endl;
            return false;
        }
        rqc_query_components.vstr_select_expressions.insert(rqc_query_components.vstr_select_expressions.end(), iter2->second);
    }
    return true;
}

bool TinyDBWrapper::Wrapper::check_static_join_box(Box& r_box, map<string, string>& rmstr_parameters, QueryComponents& rqc_query_components)
{
    // joins a stream with a table and performs a map on the result
    // attributes from the input stream might be removed and new attributes of the table added
    // the join predicate is added to the list of predicates
    // currently only one join box allowed per query
    if (!rqc_query_components.str_table.empty())
    {
        cerr << "Error: only one staticjoin box allowed, but at least one other box present: " << r_box.get_name() << endl;
        return false;
    }
    // no join after an aggregate box
    if (!rqc_query_components.str_group_by_expression.empty())
    {
        cerr << "Error: after an aggregate box no staticjoin allowed: " << r_box.get_name() << endl;
        return false;
    }
    rqc_query_components.str_table = r_box.get_input(1)->get_name();
    // minimum of 2 parameters
    if (rmstr_parameters.size() < 2)
    {
        cerr << "Error: parameter(s) missing for staticjoin: " << r_box.get_name() << endl;
        return false;
    }
    map<string, string>::iterator iter;
    // check predicate
    if ((iter = rmstr_parameters.find("predicate")) != rmstr_parameters.end())
    {
        if (!check_filter_predicate(iter->second))
        {
            cerr << "Error: wrong format of predicate for staticjoin: " << r_box.get_name() << endl;
            return false;
        }
        rqc_query_components.vstr_where_predicates.insert(rqc_query_components.vstr_where_predicates.end(), iter->second);
    }
    else
    {
        cerr << "Error: missing predicate for staticjoin: " << r_box.get_name() << endl;
        return false;
    }
    // check out-fields
    unsigned int i_no_of_fields = rmstr_parameters.size() - 1;
    // delete old list
    rqc_query_components.vstr_select_expressions.erase(rqc_query_components.vstr_select_expressions.begin(), rqc_query_components.vstr_select_expressions.end());
    char ac_buffer[256];
    string str_temp;
    for (unsigned int i=0; i<i_no_of_fields; i++)
    {
        sprintf(ac_buffer, "out-field.%d", i);
        if ((iter = rmstr_parameters.find(ac_buffer)) == rmstr_parameters.end())
        {
            cerr << "Error: missing " << ac_buffer << " for staticjoin: " << r_box.get_name() << endl;
            return false;
        }
        // "right" is the name of the table
        // hack: assuming that "left" is "sensors"
        if (iter->second.find("left", 0) != 0)
        {
            if (iter->second.find("right", 0) != 0)
            {
                cerr << "Error: wrong format of " << ac_buffer << " for staticjoin: " << r_box.get_name() << endl;
                return false;
            }
            else
            {
                str_temp = r_box.get_input(1)->get_name() + "." + iter->second.substr(6);
            }
        }
        else
        {
            str_temp = "sensors." + iter->second.substr(5);
        }
        rqc_query_components.vstr_select_expressions.insert(rqc_query_components.vstr_select_expressions.end(), str_temp);
    }
    return true;
}

bool TinyDBWrapper::Wrapper::check_aggregate_box(Box& r_box, map<string, string>& rmstr_parameters, QueryComponents& rqc_query_components)
{
    // applies aggregate functions, currently COUNT, MIN, MAX, SUM, AVG supported
    // new "attributes" are added to the selection list
    // the group by expressions are added
    // currently only one aggregate box allowed per query
    // only filters are allowed to follow the aggregate box
    // filter predicates then are attached to the having list
    if (!rqc_query_components.str_group_by_expression.empty())
    {
        cerr << "Error: only one aggregate box allowed, but at least one other box present: " << r_box.get_name() << endl;
        return false;
    }
    // minimum of 9 parameters
    if (rmstr_parameters.size() < 9)
    {
        cerr << "Error: parameter(s) missing for aggregate: " << r_box.get_name() << endl;
        return false;
    }
    map<string, string>::iterator iter;
    // check window-size-by
    if ((iter = rmstr_parameters.find("window-size-by")) != rmstr_parameters.end())
    {
        if (iter->second.compare("VALUES") != 0)
        {
            cerr << "Error: window-size-by must be set to \"VALUES\" for aggregate: " << r_box.get_name() << endl;
            return false;
        }
    }
    else
    {
        cerr << "Error: missing window-size-by for aggregate: " << r_box.get_name() << endl;
        return false;
    }
    // check window-size
    if ((iter = rmstr_parameters.find("window-size")) != rmstr_parameters.end())
    {
        if (iter->second.compare("1") != 0)
        {
            cerr << "Error: window-size-by must be set to 1 for aggregate: " << r_box.get_name() << endl;
            return false;
        }
    }
    else
    {
        cerr << "Error: missing window-size for aggregate: " << r_box.get_name() << endl;
        return false;
    }
    // check advance
    if ((iter = rmstr_parameters.find("advance")) != rmstr_parameters.end())
    {
        if (iter->second.compare("1") != 0)
        {
            cerr << "Error: advance must be set to 1 for aggregate: " << r_box.get_name() << endl;
            return false;
        }
    }
    else
    {
        cerr << "Error: missing advance for aggregate: " << r_box.get_name() << endl;
        return false;
    }
    // check timeout
    if ((iter = rmstr_parameters.find("timeout")) != rmstr_parameters.end())
    {
        if (iter->second.compare("-1") != 0)
        {
            cerr << "Error: timeout must be set to -1 for aggregate: " << r_box.get_name() << endl;
            return false;
        }
    }
    else
    {
        cerr << "Error: missing timeout for aggregate: " << r_box.get_name() << endl;
        return false;
    }
    // check order-by
    if ((iter = rmstr_parameters.find("order-by")) != rmstr_parameters.end())
    {
        if (iter->second.compare("FIELD") != 0)
        {
            cerr << "Error: order-by must be set to \"FIELD\" for aggregate: " << r_box.get_name() << endl;
            return false;
        }
    }
    else
    {
        cerr << "Error: missing order-by for aggregate: " << r_box.get_name() << endl;
        return false;
    }
    // check order-on-field
    if ((iter = rmstr_parameters.find("order-on-field")) != rmstr_parameters.end())
    {
        if (iter->second.compare("sensors.epoch") != 0)
        {
            cerr << "Error: order-on-field must be set to \"sensors.epoch\" for aggregate: " << r_box.get_name() << endl;
            return false;
        }
    }
    else
    {
        cerr << "Error: missing order-on-field for aggregate: " << r_box.get_name() << endl;
        return false;
    }
    // check slack
    if ((iter = rmstr_parameters.find("slack")) != rmstr_parameters.end())
    {
        if (iter->second.compare("0") != 0)
        {
            cerr << "Error: slack must be set to 0 for aggregate: " << r_box.get_name() << endl;
            return false;
        }
    }
    else
    {
        cerr << "Error: missing slack for aggregate: " << r_box.get_name() << endl;
        return false;
    }
    // check group-by
    if ((iter = rmstr_parameters.find("group-by")) != rmstr_parameters.end())
    {
        if (!check_group_by_expression(iter->second))
        {
            cerr << "Error: wrong format of group-by of aggregate: " << r_box.get_name() << endl;
            return false;
        }
        // remove first attribute sensors.epoch
        rqc_query_components.str_group_by_expression = iter->second.substr(13);
        // remove comma and spaces before next attribute
        while ((rqc_query_components.str_group_by_expression[0] == ' '
                || rqc_query_components.str_group_by_expression[0] == ',')
               && rqc_query_components.str_group_by_expression.length() > 0)
        {
            rqc_query_components.str_group_by_expression = rqc_query_components.str_group_by_expression.substr(1);
        }
        // if string is empty epoch was the only attribute
        if (rqc_query_components.str_group_by_expression.length() == 0)
        {
            cerr << "Error: wrong format of group-by of aggregate: " << r_box.get_name() << endl;
            return false;
        }
    }
    else
    {
        cerr << "Error: missing group-by for aggregate: " << r_box.get_name() << endl;
        return false;
    }
    // check aggregates
    unsigned int i_no_of_fields = rmstr_parameters.size() - 8;
    char ac_buffer[256];
    for (unsigned int i=0; i<i_no_of_fields; i++)
    {
        sprintf(ac_buffer, "aggregate-function.%d", i);
        if ((iter = rmstr_parameters.find(ac_buffer)) == rmstr_parameters.end())
        {
            cerr << "Error: missing " << ac_buffer << " for aggregate: " << r_box.get_name() << endl;
            return false;
        }
        // check function
        if (//iter->second.find("count", 0) != 0 && not yet supported
            iter->second.find("min", 0) != 0 &&
            iter->second.find("max", 0) != 0 &&
            iter->second.find("sum", 0) != 0 &&
            iter->second.find("avg", 0) != 0)
        {
            cerr << "Error: unsupported function in or wrong format of " << ac_buffer << " of aggregate: " << r_box.get_name() << endl;
            return false;
        }
        if (!check_aggregate_expression(iter->second))
        {
            cerr << "Error: wrong format of " << ac_buffer << " for aggregate: " << r_box.get_name() << endl;
            return false;
        }
        // check aggregate format
        string str_temp;
        size_t s_field_pos = iter->second.find("(", 0);
        if (s_field_pos == string::npos || s_field_pos == iter->second.length())
        {
            cerr << "Error: wrong format of function in " << ac_buffer << " of aggregate: " << r_box.get_name() << endl;
            return false;
        }
        // remove aggregated attribute from list
        str_temp = iter->second.substr(s_field_pos+1, iter->second.length()-s_field_pos-2);
        vector<string> vstr_temp;
        for (unsigned int i=0; i<rqc_query_components.vstr_select_expressions.size(); i++)
        {
            if (rqc_query_components.vstr_select_expressions.at(i).compare(str_temp) != 0)
            {
                vstr_temp.insert(vstr_temp.end(), rqc_query_components.vstr_select_expressions.at(i));
            }
        }
        rqc_query_components.vstr_select_expressions.erase(rqc_query_components.vstr_select_expressions.begin(), rqc_query_components.vstr_select_expressions.end());
        rqc_query_components.vstr_select_expressions.assign(vstr_temp.begin(), vstr_temp.end());
        // add aggregate to select list
        rqc_query_components.vstr_select_expressions.insert(rqc_query_components.vstr_select_expressions.end(), iter->second);
    }
    return true;
}

bool TinyDBWrapper::Wrapper::check_filter_predicate(string str_predicate)
{
  // TODO mayby more checks
    // epoch is not allowed within a TinySQL query
    if (str_predicate.find("epoch", 0) != string::npos)
    {
        cerr << "attribute epoch not allowed" << endl;
        return false;
    }

    return true;
}

bool TinyDBWrapper::Wrapper::check_having_predicate(string str_predicate)
{
  // TODO mayby more checks
    // epoch is not allowed within a TinySQL query
    if (str_predicate.find("epoch", 0) != string::npos)
    {
        cerr << "attribute epoch not allowed" << endl;
        return false;
    }

    return true;
}

bool TinyDBWrapper::Wrapper::check_aggregate_expression(string str_expression)
{
  // TODO mayby more checks
    // epoch is not allowed within a TinySQL query
    if (str_expression.find("epoch", 0) != string::npos)
    {
        cerr << "attribute epoch not allowed" << endl;
        return false;
    }

    return true;
}

bool TinyDBWrapper::Wrapper::check_group_by_expression(string str_expression)
{
  // TODO mayby more checks
    // epoch must be first attribute
    if (str_expression.find("sensors.epoch", 0) != 0)
    {
        cerr << "attribute epoch must be first attribute" << endl;
        return false;
    }
    // epoch is only allowed once
    if (str_expression.find("sensors.epoch", 1) != string::npos)
    {
        cerr << "attribute epoch is only allowed once" << endl;
        return false;
    }

    return true;
}

int TinyDBWrapper::Wrapper::check_query_syntax(string str_query)
{
    _cmd_socket.write_byte(CMD_PREPQUERY);
    _cmd_socket.write_int(str_query.length());
    _cmd_socket.write_byte_array((BYTE*)str_query.c_str(), str_query.length());
    BYTE b_ret;
    if ((b_ret = _cmd_socket.read_byte()) == 0)
    {
        int i_query_id = _cmd_socket.read_int();
        return i_query_id;
    }
    else if (b_ret == 1)
    {
        cerr << "Error: processing request CheckQuerySyntax" << endl;
        return -1;
    }
    else
    {
        int i_len = _cmd_socket.read_int();
        char* pc_buffer = new char[i_len+1];
        _cmd_socket.read_byte_array((BYTE*)pc_buffer, i_len);
        pc_buffer[i_len] = '\0';
        cerr << "Error: syntax error: " << pc_buffer << endl;
        return -1;
    }
}

TinyDBWrapper::TQueries* TinyDBWrapper::Wrapper::get_queries()
{
    return &_tqueries;
}

string TinyDBWrapper::Wrapper::get_queries_xml()
{
    return _tqueries.get_xml();
}
