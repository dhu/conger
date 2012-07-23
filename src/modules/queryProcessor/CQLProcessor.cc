/**
 * CongerProcessor.cc
 *
 *  Created on: Apr 17, 2012
 *      Author: hellojinjie
 */
#include "QueryProcessor.h"
#include "CongerDiagram.h"      // 继承自  Diagram
#include "DeployParser.h"       // 用来解析 conger 的 xml 部署文件
#include "DeployDescript.h"     // 用来描述部署信息的一个结构体， 该结构体可以由 DeployParser 生成
#include "ParseDriver.h"
#include "ParseContext.h"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/case_conv.hpp>

BOREALIS_NAMESPACE_BEGIN

void QueryProcessor::transform_cql_to_boxes(ParseContext& context)
{
    if (context.has_join)
    {
        this->transform_cql_join(context);
    }
    else if (context.has_aggregate)
    {
        this->transform_cql_aggregate(context);
    }
    else if (context.has_where)
    {
        this->transform_cql_filter(context);
    }
}

void QueryProcessor::transform_cql_map(ParseContext& context)
{
    /*
    map<string, string> box_parameters;
    box_parameters["expression.0"] = "time";
    box_parameters["output-field-name.0"] = "time";
    box_parameters["expression.1"] = "price / 6.3";
    box_parameters["output-field-name.1"] = "usd";

    add_conger_box("try_map", "map", "inputstream",
            "outputstream", box_parameters);
    */
}

void QueryProcessor::transform_cql_filter(ParseContext& context)
{
    map<string, string> box_parameters;
    box_parameters["expression.0"] = context.where.condition;
    box_parameters["pass-on-false-port"] = "0";

    add_conger_box(context.query_name, "filter", context.from_stream.stream_name,
            "outputstream", box_parameters);
}

void QueryProcessor::transform_cql_union(ParseContext& context)
{

}

void QueryProcessor::transform_cql_join(ParseContext& context)
{
    using boost::lexical_cast;
    using boost::algorithm::to_lower;

    map<string, string> box_parameters;
    list<ProjectionTerm> select_list = context.select_list;

    list<ProjectionTerm>::iterator select_iter = select_list.begin();
    int index(0);
    for (; select_iter != select_list.end(); select_iter++)
    {
        string output_key = "out-field." + lexical_cast<string>(index);
        string output_name_key = "out-field-name." + lexical_cast<string>(index);
        to_lower(select_iter->expression);
        box_parameters[output_key] = select_iter->expression;
        if (select_iter->alias.empty())
        {
            box_parameters[output_name_key] = "join-outpt-" + lexical_cast<string>(index);
        }
        else
        {
            box_parameters[output_name_key] = select_iter->alias;
        }
        index++;
    }

    /* window, join buffer size */
    WindowDefinition left_window = context.from_stream.window;
    if (left_window.type == CQL::VALUES)
    {
        box_parameters["left-order-by"] = "VALUES";
        box_parameters["left-buffer-size"] = lexical_cast<string>(left_window.range);
        box_parameters["left-order-on-field"] = "time";
    }
    else
    {
        box_parameters["left-order-by"] = "TUPLES";
        box_parameters["left-buffer-size"] = lexical_cast<string>(left_window.range);
    }

    WindowDefinition right_window = context.stream_join.stream.window;
    if (left_window.type == CQL::VALUES)
    {
        box_parameters["right-order-by"] = "VALUES";
        box_parameters["right-buffer-size"] = lexical_cast<string>(right_window.range);
        box_parameters["right-order-on-field"] = "time";
    }
    else
    {
        box_parameters["right-order-by"] = "TUPLES";
        box_parameters["right-buffer-size"] = lexical_cast<string>(right_window.range);
    }

    /* on, predicate */
    to_lower(context.stream_join.condition);
    box_parameters["predicate"] = context.stream_join.condition;

    string inputstreams = context.from_stream.stream_name + ":"
            + context.stream_join.stream.stream_name;
    add_conger_box(context.query_name, "join", inputstreams, "outputstream", box_parameters);
}

void QueryProcessor::transform_cql_aggregate(ParseContext& context)
{
    using boost::lexical_cast;

    map<string, string> box_parameters;
    list<ProjectionTerm> select_list = context.select_list;

    list<ProjectionTerm>::iterator select_iter = select_list.begin();
    int index(0);
    for (; select_iter != select_list.end(); select_iter++)
    {
        string function_index = "aggregate-function." + lexical_cast<string>(index);
        string function_output = "aggregate-function-output-name." + lexical_cast<string>(index);
        using boost::algorithm::to_lower;
        to_lower(select_iter->expression);
        box_parameters[function_index] = select_iter->expression;
        if (select_iter->alias.empty())
        {
            box_parameters[function_output] = "aggregate-outpt-" + lexical_cast<string>(index);
        }
        else
        {
            box_parameters[function_output] = select_iter->alias;
        }
        index++;
    }

    /* 判断一下有没有 group by 语句
     * XXX 如何判断一个结构体有没有初始化? */
    if (context.has_group_by)
    {
        WindowDefinition window = context.group_by.window;
        if (window.type == CQL::VALUES)
        {
            box_parameters["window-size-by"] = "VALUES";
            /* XXX 时间单位的换算有问题 */
            box_parameters["window-size"] = lexical_cast<string>(window.range);
            box_parameters["advance"] = lexical_cast<string>(window.slide);
            box_parameters["order-by"] = "FIELD";
        }
        else
        {
            box_parameters["window-size-by"] = "TUPLES";
            box_parameters["window-size"] = lexical_cast<string>(window.range);
            box_parameters["advance"] = lexical_cast<string>(window.slide);
            box_parameters["order-by"] = "TUPLENUM";
        }
        if (!context.group_by.attribute_list.empty())
        {
            list<string> attr_list = context.group_by.attribute_list;
            for (list<string>::iterator iter = attr_list.begin();
                    iter != attr_list.end(); iter++)
            {
                box_parameters["order-on-field"] += *iter;
                box_parameters["order-on-field"] += ",";
            }
            box_parameters["order-on-field"] = box_parameters["order-on-field"].
            substr(0, box_parameters["order-on-field"].size() - 1);
        }
        else
        {
            box_parameters["order-on-field"] = "time";
        }
    }
    else
    {
        WindowDefinition window = context.from_stream.window;
        if (window.type == CQL::VALUES)
        {
            box_parameters["window-size-by"] = "VALUES";
            /* XXX 时间单位的换算有问题 */
            box_parameters["window-size"] = lexical_cast<string>(window.range);
            box_parameters["advance"] = lexical_cast<string>(window.slide);
            box_parameters["order-by"] = "FIELD";
        }
        else
        {
            box_parameters["window-size-by"] = "TUPLES";
            box_parameters["window-size"] = lexical_cast<string>(window.range);
            box_parameters["advance"] = lexical_cast<string>(window.slide);
            box_parameters["order-by"] = "TUPLENUM";
        }
        if (!context.group_by.attribute_list.empty())
        {
            list<string> attr_list = context.group_by.attribute_list;
            for (list<string>::iterator iter = attr_list.begin();
                    iter != attr_list.end(); iter++)
            {
                box_parameters["order-on-field"] += *iter;
                box_parameters["order-on-field"] += ",";
            }
            box_parameters["order-on-field"] = box_parameters["order-on-field"].
            substr(0, box_parameters["order-on-field"].size() - 1);
        }
        else
        {
            box_parameters["order-on-field"] = "time";
        }
    }

    add_conger_box(context.query_name, "aggregate", context.from_stream.stream_name,
            "outputstream", box_parameters);
}

BOREALIS_NAMESPACE_END
