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
#include <boost/foreach.hpp>
#include <fstream>

BOREALIS_NAMESPACE_BEGIN

void QueryProcessor::transform_cql_to_boxes(ParseContext& context)
{
    // 前面处理单个的情况，最后一个处理多个的情况
    if (context.has_join and not context.has_aggregate and not context.has_where and not context.has_map)
    {
        this->transform_cql_join(context);
    }
    else if (context.has_aggregate and not context.has_join
            and not context.has_where and not context.is_aggregate_and_map and not context.has_map)
    {
        this->transform_cql_aggregate(context);
    }
    else if (context.has_where and context.is_select_all and not context.has_aggregate
            and not context.has_join and not context.has_map)
    {
        this->transform_cql_filter(context);
    }
    else if (not context.has_where and not context.has_aggregate
            and not context.has_join and not context.is_select_all)
    {
        this->transform_cql_map(context);
    }
    else
    {
        this->transform_cql_multi_boxes(context);
    }
}

void QueryProcessor::transform_cql_map(ParseContext& context)
{
    using boost::lexical_cast;
    map<string, string> box_parameters;
    int index = 0;
    BOOST_FOREACH(const ProjectionTerm& term, context.select_list)
    {
        box_parameters["expression." + lexical_cast<string>(index)] = term.expression;
        /* 前面解析的时候，已经保证了 alias 都是有值的 */
        box_parameters["output-field-name." + lexical_cast<string>(index)] = term.alias;
        index = index + 1;
    }
    this->dump_box_parameters(context.query_name, box_parameters);
    add_conger_box(context.query_name, "map", context.from_stream.stream_name,
           context.output_stream, box_parameters);
}

void QueryProcessor::transform_cql_filter(ParseContext& context)
{
    map<string, string> box_parameters;
    box_parameters["expression.0"] = context.where.condition;
    box_parameters["pass-on-false-port"] = "0";

    this->dump_box_parameters(context.query_name, box_parameters);
    add_conger_box(context.query_name, "filter", context.from_stream.stream_name,
            context.output_stream, box_parameters);
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
        to_lower(select_iter->join_expression);
        box_parameters[output_key] = select_iter->join_expression;
        box_parameters[output_name_key] = select_iter->alias;
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
        box_parameters["left-buffer-size"] = lexical_cast<string>(left_window.row);
    }

    WindowDefinition right_window = context.stream_join.stream.window;
    if (right_window.type == CQL::VALUES)
    {
        box_parameters["right-order-by"] = "VALUES";
        box_parameters["right-buffer-size"] = lexical_cast<string>(right_window.range);
        box_parameters["right-order-on-field"] = "time";
    }
    else
    {
        box_parameters["right-order-by"] = "TUPLES";
        box_parameters["right-buffer-size"] = lexical_cast<string>(right_window.row);
    }

    /* on, predicate */
    to_lower(context.stream_join.condition);
    box_parameters["predicate"] = context.stream_join.condition;

    this->dump_box_parameters(context.query_name, box_parameters);
    string inputstreams = context.from_stream.stream_name + ":"
            + context.stream_join.stream.stream_name;
    add_conger_box(context.query_name, "join", inputstreams, context.output_stream, box_parameters);
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
        if (select_iter->is_aggregate)
        {
            string function_index = "aggregate-function." + lexical_cast<string>(index);
            string function_output = "aggregate-function-output-name." + lexical_cast<string>(index);
            using boost::algorithm::to_lower;
            to_lower(select_iter->aggregate_expression);
            box_parameters[function_index] = select_iter->aggregate_expression;
            if (select_iter->alias.empty())
            {
                box_parameters[function_output] = select_iter->aggregate_output_field_name;
            }
            else
            {
                box_parameters[function_output] = select_iter->alias;
            }
            index++;
        }
    }

    /* 判断一下有没有 group by 语句
     * XXX 如何判断一个结构体有没有初始化? */
    if (context.has_group_by)
    {
        WindowDefinition window = context.group_by.window;
        if (window.type == CQL::VALUES)
        {
            box_parameters["window-size-by"] = "VALUES";
            box_parameters["timeout"] = lexical_cast<string>(window.range);
            /* XXX 时间单位的换算有问题 */
            box_parameters["window-size"] = lexical_cast<string>(window.range);
            box_parameters["advance"] = lexical_cast<string>(window.slide);
            box_parameters["order-by"] = "FIELD";
            box_parameters["order-on-field"] = "time";
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
                box_parameters["group-by"] += *iter;
                box_parameters["group-by"] += ",";
            }
            box_parameters["group-by"] = box_parameters["group-by"].
            substr(0, box_parameters["group-by"].size() - 1);
        }
    }
    else
    {
        WindowDefinition window = context.from_stream.window;
        if (window.type == CQL::VALUES)
        {
            box_parameters["window-size-by"] = "VALUES";
            box_parameters["timeout"] = lexical_cast<string>(window.range);
            /* XXX 时间单位的换算有问题 */
            box_parameters["window-size"] = lexical_cast<string>(window.range);
            box_parameters["advance"] = lexical_cast<string>(window.slide);
            box_parameters["order-by"] = "FIELD";
            box_parameters["order-on-field"] = "time";
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
                box_parameters["group-by"] += *iter;
                box_parameters["group-by"] += ",";
            }
            box_parameters["group-by"] = box_parameters["group-by"].
            substr(0, box_parameters["group-by"].size() - 1);
        }
    }

    this->dump_box_parameters(context.query_name, box_parameters);
    add_conger_box(context.query_name, "aggregate", context.from_stream.stream_name,
            context.output_stream, box_parameters);
}

void QueryProcessor::transform_cql_multi_boxes(ParseContext& context)
{
    if (context.has_where and not context.is_select_all and not context.has_aggregate
            and not context.has_group_by and not context.has_join)
    {
        /* 这是先 filter 再 map 的情况 */
        /* 第一步， filter */
        map<string, string> box_parameters;
        box_parameters["expression.0"] = context.where.condition;
        box_parameters["pass-on-false-port"] = "0";

        this->dump_box_parameters(context.query_name + "_filter", box_parameters);
        add_conger_box(context.query_name + "_filter", "filter", context.from_stream.stream_name,
                context.query_name + "intermediate_outputstream", box_parameters);

        /* 第二步， map */
        box_parameters.clear();
        int index = 0;
        using boost::lexical_cast;
        BOOST_FOREACH(const ProjectionTerm& term, context.select_list)
        {
            box_parameters["expression." + lexical_cast<string>(index)] = term.expression;
            /* 前面解析的时候，已经保证了 alias 都是有值的 */
            box_parameters["output-field-name." + lexical_cast<string>(index)] = term.alias;
            index = index + 1;
        }

        this->dump_box_parameters(context.query_name + "_map", box_parameters);
        add_conger_box(context.query_name + "_map", "map", context.query_name + "intermediate_outputstream",
               context.output_stream, box_parameters);
    }
    else if (context.has_aggregate and context.is_aggregate_and_map)
    {
        /* 这里是先 aggregate 再 map 的情况，
         * 注意这里如何区分单独的 aggregate 和 先 aggregate 再 map 的情况 */

        /* 第一步，aggregate */
        using boost::lexical_cast;

        map<string, string> box_parameters;
        list<ProjectionTerm> select_list = context.select_list;

        list<ProjectionTerm>::iterator select_iter = select_list.begin();
        int index(0);
        for (; select_iter != select_list.end(); select_iter++)
        {
            if (select_iter->is_aggregate)
            {
                string function_index = "aggregate-function." + lexical_cast<string>(index);
                string function_output = "aggregate-function-output-name." + lexical_cast<string>(index);
                using boost::algorithm::to_lower;
                to_lower(select_iter->aggregate_expression);
                box_parameters[function_index] = select_iter->aggregate_expression;
                if (select_iter->alias.empty())
                {
                    box_parameters[function_output] = "aggregate-output-" + lexical_cast<string>(index);
                }
                else
                {
                    box_parameters[function_output] = select_iter->aggregate_output_field_name;
                }
                index++;
            }
        }

        /* 判断一下有没有 group by 语句 */
        if (context.has_group_by)
        {
            WindowDefinition window = context.group_by.window;
            if (window.type == CQL::VALUES)
            {
                box_parameters["window-size-by"] = "VALUES";
                box_parameters["timeout"] = lexical_cast<string>(window.range);
                /* XXX 时间单位的换算有问题 */
                box_parameters["window-size"] = lexical_cast<string>(window.range);
                box_parameters["advance"] = lexical_cast<string>(window.slide);
                box_parameters["order-by"] = "FIELD";
                box_parameters["order-on-field"] = "time";
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
                    box_parameters["group-by"] += *iter;
                    box_parameters["group-by"] += ",";
                }
                box_parameters["group-by"] = box_parameters["group-by"].
                substr(0, box_parameters["group-by"].size() - 1);
            }
        }
        else
        {
            WindowDefinition window = context.from_stream.window;
            if (window.type == CQL::VALUES)
            {
                box_parameters["window-size-by"] = "VALUES";
                box_parameters["timeout"] = lexical_cast<string>(window.range);
                /* XXX 时间单位的换算有问题 */
                box_parameters["window-size"] = lexical_cast<string>(window.range);
                box_parameters["advance"] = lexical_cast<string>(window.slide);
                box_parameters["order-by"] = "FIELD";
                box_parameters["order-on-field"] = "time";
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
                    box_parameters["group-by"] += *iter;
                    box_parameters["group-by"] += ",";
                }
                box_parameters["group-by"] = box_parameters["group-by"].
                substr(0, box_parameters["group-by"].size() - 1);
            }
        }

        this->dump_box_parameters(context.query_name + "_aggregate", box_parameters);
        add_conger_box(context.query_name + "_aggregate", "aggregate", context.from_stream.stream_name,
                context.query_name + "intermediate_outputstream", box_parameters);

        /* 第二步，map */
        box_parameters.clear();
        index = 0;
        BOOST_FOREACH(const ProjectionTerm& term, context.select_list)
        {
            box_parameters["expression." + lexical_cast<string>(index)] = term.map_expression;
            /* 前面解析的时候，已经保证了 alias 都是有值的 */
            box_parameters["output-field-name." + lexical_cast<string>(index)] = term.alias;
            index = index + 1;
        }

        this->dump_box_parameters(context.query_name + "_map", box_parameters);
        add_conger_box(context.query_name + "_map", "map", context.query_name + "intermediate_outputstream",
                context.output_stream, box_parameters);
    }
    else if (context.has_aggregate and context.has_having and not context.has_join and not context.has_map)
    {
        /* 第一步，aggregate */
        using boost::lexical_cast;

        map<string, string> box_parameters;
        list<ProjectionTerm> select_list = context.select_list;

        list<ProjectionTerm>::iterator select_iter = select_list.begin();
        int index(0);
        for (; select_iter != select_list.end(); select_iter++)
        {
            if (select_iter->is_aggregate)
            {
                string function_index = "aggregate-function." + lexical_cast<string>(index);
                string function_output = "aggregate-function-output-name." + lexical_cast<string>(index);
                using boost::algorithm::to_lower;
                to_lower(select_iter->aggregate_expression);
                box_parameters[function_index] = select_iter->aggregate_expression;
                if (select_iter->alias.empty())
                {
                    box_parameters[function_output] = "aggregate-output-" + lexical_cast<string>(index);
                }
                else
                {
                    box_parameters[function_output] = select_iter->aggregate_output_field_name;
                }
                index++;
            }
        }



        /* 判断一下有没有 group by 语句 */
        if (context.has_group_by)
        {
            WindowDefinition window = context.group_by.window;
            if (window.type == CQL::VALUES)
            {
                box_parameters["window-size-by"] = "VALUES";
                box_parameters["timeout"] = lexical_cast<string>(window.range);
                /* XXX 时间单位的换算有问题 */
                box_parameters["window-size"] = lexical_cast<string>(window.range);
                box_parameters["advance"] = lexical_cast<string>(window.slide);
                box_parameters["order-by"] = "FIELD";
                box_parameters["order-on-field"] = "time";
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
                    box_parameters["group-by"] += *iter;
                    box_parameters["group-by"] += ",";
                }
                box_parameters["group-by"] = box_parameters["group-by"].
                substr(0, box_parameters["group-by"].size() - 1);
            }
        }
        else
        {
            WindowDefinition window = context.from_stream.window;
            if (window.type == CQL::VALUES)
            {
                box_parameters["window-size-by"] = "VALUES";
                box_parameters["timeout"] = lexical_cast<string>(window.range);
                /* XXX 时间单位的换算有问题 */
                box_parameters["window-size"] = lexical_cast<string>(window.range);
                box_parameters["advance"] = lexical_cast<string>(window.slide);
                box_parameters["order-by"] = "FIELD";
                box_parameters["order-on-field"] = "time";
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
                    box_parameters["group-by"] += *iter;
                    box_parameters["group-by"] += ",";
                }
                box_parameters["group-by"] = box_parameters["group-by"].
                substr(0, box_parameters["group-by"].size() - 1);
            }
        }

        this->dump_box_parameters(context.query_name + "_aggregate", box_parameters);
        add_conger_box(context.query_name + "_aggregate", "aggregate", context.from_stream.stream_name,
                context.query_name + "intermediate_outputstream", box_parameters);

        /* 第二步，filter */
        box_parameters.clear();
        box_parameters["expression.0"] = context.having.condition;
        box_parameters["pass-on-false-port"] = "0";

        this->dump_box_parameters(context.query_name + "_filter", box_parameters);
        add_conger_box(context.query_name + "_filter", "filter",
                context.query_name + "intermediate_outputstream", context.output_stream, box_parameters);

    }
    else if (context.has_join and context.has_where and not context.has_map)
    {
        /* 先 join 再 filter 的情况 */
        /* 第一步，先 join */
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
            to_lower(select_iter->join_expression);
            box_parameters[output_key] = select_iter->join_expression;
            if (select_iter->alias.empty())
            {
                box_parameters[output_name_key] = "join-output-" + lexical_cast<string>(index);
            }
            else
            {
                // box_parameters[output_name_key] = select_iter->join_output_field_name;
                // 对于先 join 再 filter 来说，join 的输出的 field 的名称应该直接用 alias
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

        this->dump_box_parameters(context.query_name + "_join", box_parameters);
        string inputstreams = context.from_stream.stream_name + ":"
                + context.stream_join.stream.stream_name;
        add_conger_box(context.query_name + "_join", "join", inputstreams,
                context.query_name + "intermediate_outputstream", box_parameters);

        /* 第二步，filter */
        box_parameters.clear();
        box_parameters["expression.0"] = context.where.condition;
        box_parameters["pass-on-false-port"] = "0";

        add_conger_box(context.query_name + "_filter", "filter",
                context.query_name + "intermediate_outputstream", context.output_stream, box_parameters);

    }
    else if (context.has_join and context.has_map and not context.has_where and not context.has_aggregate)
    {
        /* 先 join 再 map 的情况 */
        /* 第一步，先 join */
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
            to_lower(select_iter->join_expression);
            box_parameters[output_key] = select_iter->join_expression;
            box_parameters[output_name_key] = select_iter->join_output_field_name;
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
            box_parameters["left-buffer-size"] = lexical_cast<string>(left_window.row);
        }

        WindowDefinition right_window = context.stream_join.stream.window;
        if (right_window.type == CQL::VALUES)
        {
            box_parameters["right-order-by"] = "VALUES";
            box_parameters["right-buffer-size"] = lexical_cast<string>(right_window.range);
            box_parameters["right-order-on-field"] = "time";
        }
        else
        {
            box_parameters["right-order-by"] = "TUPLES";
            box_parameters["right-buffer-size"] = lexical_cast<string>(right_window.row);
        }

        /* on, predicate */
        to_lower(context.stream_join.condition);
        box_parameters["predicate"] = context.stream_join.condition;

        this->dump_box_parameters(context.query_name + "_join", box_parameters);
        string inputstreams = context.from_stream.stream_name + ":"
                + context.stream_join.stream.stream_name;
        add_conger_box(context.query_name + "_join", "join", inputstreams,
                context.query_name + "_outputstream", box_parameters);

        /* 第二步，再 map */
        box_parameters.clear();
        index = 0;
        BOOST_FOREACH(const ProjectionTerm& term, context.select_list)
        {
            box_parameters["expression." + lexical_cast<string>(index)] = term.map_expression;
            /* 前面解析的时候，已经保证了 alias 都是有值的 */
            box_parameters["output-field-name." + lexical_cast<string>(index)] = term.alias;
            index = index + 1;
        }

        this->dump_box_parameters(context.query_name + "_map", box_parameters);
        add_conger_box(context.query_name + "_map", "map", context.query_name + "intermediate_outputstream",
                context.output_stream, box_parameters);
    }
    else if (context.has_join and context.has_aggregate and not context.has_map and not context.has_where)
    {
        /* 先 join 后 aggregate 的情况 */
        /* 第一步，先 join */
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
            to_lower(select_iter->join_expression);
            box_parameters[output_key] = select_iter->join_expression;
            box_parameters[output_name_key] = select_iter->join_output_field_name;
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
        if (right_window.type == CQL::VALUES)
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

        this->dump_box_parameters(context.query_name + "_join", box_parameters);
        string inputstreams = context.from_stream.stream_name + ":"
                + context.stream_join.stream.stream_name;
        add_conger_box(context.query_name + "_join", "join", inputstreams,
                context.query_name + "_outputstream", box_parameters);

        /* 第二步，aggregate */
        box_parameters.clear();

        select_list = context.select_list;
        select_iter = select_list.begin();
        index = 0;
        for (; select_iter != select_list.end(); select_iter++)
        {
            if (select_iter->is_aggregate)
            {
                string function_index = "aggregate-function." + lexical_cast<string>(index);
                string function_output = "aggregate-function-output-name." + lexical_cast<string>(index);
                using boost::algorithm::to_lower;
                to_lower(select_iter->aggregate_expression);
                box_parameters[function_index] = select_iter->aggregate_expression;
                if (select_iter->alias.empty())
                {
                    box_parameters[function_output] = "aggregate-output-" + lexical_cast<string>(index);
                }
                else
                {
                    box_parameters[function_output] = select_iter->aggregate_output_field_name;
                }
                index++;
            }
        }

        /* 判断一下有没有 group by 语句 */
        if (context.has_group_by)
        {
            WindowDefinition window = context.group_by.window;
            if (window.type == CQL::VALUES)
            {
                box_parameters["window-size-by"] = "VALUES";
                box_parameters["timeout"] = lexical_cast<string>(window.range);
                /* XXX 时间单位的换算有问题 */
                box_parameters["window-size"] = lexical_cast<string>(window.range);
                box_parameters["advance"] = lexical_cast<string>(window.slide);
                box_parameters["order-by"] = "FIELD";
                box_parameters["order-on-field"] = "time";
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
                    box_parameters["group-by"] += *iter;
                    box_parameters["group-by"] += ",";
                }
                box_parameters["group-by"] = box_parameters["group-by"].
                substr(0, box_parameters["group-by"].size() - 1);
            }
        }
        else
        {
            WindowDefinition window = context.from_stream.window;
            if (window.type == CQL::VALUES)
            {
                box_parameters["window-size-by"] = "VALUES";
                box_parameters["timeout"] = lexical_cast<string>(window.range);
                /* XXX 时间单位的换算有问题 */
                box_parameters["window-size"] = lexical_cast<string>(window.range);
                box_parameters["advance"] = lexical_cast<string>(window.slide);
                box_parameters["order-by"] = "FIELD";
                box_parameters["order-on-field"] = "time";
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
                    box_parameters["group-by"] += *iter;
                    box_parameters["group-by"] += ",";
                }
                box_parameters["group-by"] = box_parameters["group-by"].
                substr(0, box_parameters["group-by"].size() - 1);
            }
        }

        this->dump_box_parameters(context.query_name + "_aggregate", box_parameters);
        add_conger_box(context.query_name + "_aggregate", "aggregate",
                context.query_name + "intermediate_outputstream", context.output_stream, box_parameters);

    }
    else if (context.has_aggregate and context.has_where and not context.has_map and not context.has_having)
    {
        /* 先 filter 再 aggregate 的情况 */
        /* 第一步， filter */
        map<string, string> box_parameters;
        box_parameters["expression.0"] = context.where.condition;
        box_parameters["pass-on-false-port"] = "0";

        this->dump_box_parameters(context.query_name + "_filter", box_parameters);
        add_conger_box(context.query_name + "_filter", "filter", context.from_stream.stream_name,
                context.query_name + "intermediate_outputstream", box_parameters);

        /* 第二步，aggregate */
        using boost::lexical_cast;

        box_parameters.clear();
        list<ProjectionTerm> select_list = context.select_list;

        list<ProjectionTerm>::iterator select_iter = select_list.begin();
        int index(0);
        for (; select_iter != select_list.end(); select_iter++)
        {
            if (select_iter->is_aggregate)
            {
                string function_index = "aggregate-function." + lexical_cast<string>(index);
                string function_output = "aggregate-function-output-name." + lexical_cast<string>(index);
                using boost::algorithm::to_lower;
                to_lower(select_iter->aggregate_expression);
                box_parameters[function_index] = select_iter->aggregate_expression;
                if (select_iter->alias.empty())
                {
                    box_parameters[function_output] = "aggregate-output-" + lexical_cast<string>(index);
                }
                else
                {
                    box_parameters[function_output] = select_iter->aggregate_output_field_name;
                }
                index++;
            }
        }

        /* 判断一下有没有 group by 语句 */
        if (context.has_group_by)
        {
            WindowDefinition window = context.group_by.window;
            if (window.type == CQL::VALUES)
            {
                box_parameters["window-size-by"] = "VALUES";
                box_parameters["timeout"] = lexical_cast<string>(window.range);
                /* XXX 时间单位的换算有问题 */
                box_parameters["window-size"] = lexical_cast<string>(window.range);
                box_parameters["advance"] = lexical_cast<string>(window.slide);
                box_parameters["order-by"] = "FIELD";
                box_parameters["order-on-field"] = "time";
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
                    box_parameters["group-by"] += *iter;
                    box_parameters["group-by"] += ",";
                }
                box_parameters["group-by"] = box_parameters["group-by"].
                substr(0, box_parameters["group-by"].size() - 1);
            }
        }
        else
        {
            WindowDefinition window = context.from_stream.window;
            if (window.type == CQL::VALUES)
            {
                box_parameters["window-size-by"] = "VALUES";
                box_parameters["timeout"] = lexical_cast<string>(window.range);
                /* XXX 时间单位的换算有问题 */
                box_parameters["window-size"] = lexical_cast<string>(window.range);
                box_parameters["advance"] = lexical_cast<string>(window.slide);
                box_parameters["order-by"] = "FIELD";
                box_parameters["order-on-field"] = "time";
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
                    box_parameters["group-by"] += *iter;
                    box_parameters["group-by"] += ",";
                }
                box_parameters["group-by"] = box_parameters["group-by"].
                substr(0, box_parameters["group-by"].size() - 1);
            }
        }

        this->dump_box_parameters(context.query_name + "_aggregate", box_parameters);
        add_conger_box(context.query_name + "_aggregate", "aggregate",
                context.query_name + "intermediate_outputstream", context.output_stream, box_parameters);
    }

}

void QueryProcessor::dump_box_parameters(string box_name, map<string, string> box_parameters)
{
    ofstream filestr;

    INFO << "I am in dump box parameters, can you see me";
    filestr.open("/home/jj/box_parameters.txt", fstream::out | fstream::app);

    if (!filestr.good())
    {
        INFO << "can not open file";
    }
    filestr << box_name << endl;
    map<string, string>::iterator iter = box_parameters.begin();
    for ( ; iter != box_parameters.end(); iter++)
    {
        filestr << iter->first << "  " << iter->second << endl;
    }

    filestr << endl << endl << endl;
    filestr.flush();
    filestr.close();
}

BOREALIS_NAMESPACE_END
