/**
 * DeployParser.cpp
 *
 *  Created on: Mar 30, 2012
 *      Author: hellojinjie
 */

#include "DeployParser.h"
#include <NMSTL/debug>
#include <NMSTL/ptr>
#include <iostream>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

BOREALIS_NAMESPACE_BEGIN

DeployParser::DeployParser()
{
}

DeployParser::~DeployParser()
{
}

DeployDescript DeployParser::parse(string conger_string)
{
    using boost::property_tree::ptree;
    using boost::property_tree::ptree_bad_path;
    using std::string;
    using std::list;
    using std::map;

    ptree pt;
    istringstream input_string_stream(conger_string);

    read_xml(input_string_stream, pt);

    this->deploy.deployName = pt.get<string>("conger.deploy.deploy-name");
    /* 读取 schemas */
    BOOST_FOREACH(const ptree::value_type &v, pt.get_child("conger.schemas"))
    {
        if (v.first == "schema")
        {
            string schema_name = v.second.get<string>("<xmlattr>.name");
            list<SchemaFieldType> fields;
            BOOST_FOREACH(const ptree::value_type &vv, v.second.get_child(""))
            {
                if (vv.first == "field")
                {
                    SchemaFieldType field;
                    field.fieldName = vv.second.get<string>("<xmlattr>.name");
                    field.typeName = vv.second.get<string>("<xmlattr>.type");
                    try
                    {
                        field.size = vv.second.get<int>("<xmlattr>.size");
                    }
                    catch (ptree_bad_path& e)
                    {}
                    fields.push_back(field);
                }
            }
            this->deploy.schemas[schema_name] = fields;
        }
    }

    /* 读取 streams */
    BOOST_FOREACH(const ptree::value_type &v, pt.get_child("conger.streams"))
    {
        if (v.first == "stream")
        {
            string stream_name = v.second.get<string>("<xmlattr>.name");
            string stream_type = v.second.get<string>("<xmlattr>.type");
            string stream_schema = v.second.get<string>("<xmlattr>.schema");
            if (stream_type == "input")
            {
                this->deploy.inputStreams[stream_name] = stream_schema;
            }
            else if (stream_type == "output")
            {
                this->deploy.outputStreams[stream_name] = stream_schema;
            }
        }
    }

    /* 读取 querys */
    BOOST_FOREACH(const ptree::value_type &v, pt.get_child("conger.querys"))
    {
        if (v.first == "query")
        {
            string query_name;
            map<string, string> parameters;
            BOOST_FOREACH(const ptree::value_type& vv, v.second.get_child(""))
            {
                string key = vv.first;
                if (key == "<xmlattr>")
                {
                    query_name = vv.second.get<string>("name");
                }
                else if (key == "in")
                {
                    if (parameters["in"] == "")
                    {
                        parameters["in"] = vv.second.get_value<string>();
                    }
                    else
                    {
                        parameters["in"] += ":";
                        parameters["in"] += vv.second.get_value<string>();
                    }
                }
                else if (key == "out")
                {
                    parameters["out"] = vv.second.get_value<string>();
                }
                else if (key == "cql")
                {
                    parameters["cql"] = vv.second.get_value<string>();
                }
                DEBUG << vv.second.get_value<string>();
            }
            this->deploy.querys[query_name] = parameters;
        }
    }

    DEBUG << this->deploy.deployName;

    return this->deploy;
}

BOREALIS_NAMESPACE_END
