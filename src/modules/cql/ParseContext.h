/**
 * ParseContext.h
 *
 *  Created on: Jun 21, 2012
 *      Author: hellojinjie
 */

#ifndef PARSECONTEXT_H_
#define PARSECONTEXT_H_

#include "common.h"
#include "antlr3.h"
#include <string.h>

BOREALIS_NAMESPACE_BEGIN

namespace CQL
{

enum WindowType
{
    TUPLES, ///< 对应 row
    VALUES  ///< 对应 range
};

enum TimeUnit
{
    NANOSECOND,
    MICROSECOND,
    MILLISECOND,
    SECOND,
    MINUTE,
    HOUR,
    DAY
};

}
/**
 * [range 2 hours slide 4 minutes timeout 8 minutes]
 * [row 100 slide 80 timeout 30]
 */
struct WindowDefinition
{
    CQL::WindowType type;
    int range;
    CQL::TimeUnit range_time_unit;
    int row;
    int slide;
    CQL::TimeUnit slide_time_unit;
    int time_out;
    CQL::TimeUnit time_out_time_unit;

    WindowDefinition(): range(0), row(0), slide(0), time_out(0)
    {
    }
};

/**
 * 举个栗子：
 * expression = SUM(price) + 23
 * aggregate_expression = SUM(price)
 * alias = SUM_price_23
 * map_expression = SUM_price_ + 23
 * aggregate_output_field_name = SUM_price_
 *
 */
struct ProjectionTerm
{
    string expression;
    string alias;

    string aggregate_expression;
    string map_expression;
    string aggregate_output_field_name;
    string join_expression;
    string join_output_field_name;

    /* 表明此时正在解析 join 这个只在解析的时候用到 */
    bool parsing_join;

    /* 表明此时正在解析 aggregate 这个只在解析的时候用到 */
    bool parsing_aggregate;

    /* 是不是聚集函数 */
    bool is_aggregate;

    /* 是不是投影函数 */
    bool is_map;

    /* 是不是 join */
    bool is_join;

    ProjectionTerm(): parsing_join(false), parsing_aggregate(false), is_aggregate(false),
            is_map(false), is_join(false)
    {
    }
};

struct StreamVariable
{
    string stream_name;
    WindowDefinition window;
    StreamVariable()
    {
    }
};

struct StreamJoin
{
    StreamVariable stream;
    string condition;
    int time_out;
    CQL::TimeUnit time_unit;

    StreamJoin(): time_out(0)
    {
    }
};

struct WhereConditoin
{
    string condition;
};

struct StreamGroupBy
{
    WindowDefinition window;
    list<string> attribute_list;
};

struct HavingCondition
{
    string condition;
};

/**
 * 这个类不是线程安全的，每解析一个语句都应该 new 一个实例
 */
class ParseContext
{
public:
    string query_name;
    string origin_cql;
    list<ProjectionTerm> select_list;
    StreamVariable from_stream;
    StreamJoin stream_join;
    WhereConditoin where;
    StreamGroupBy group_by;
    HavingCondition having;
    string output_stream;

    bool has_join;
    bool has_aggregate;
    bool has_group_by;
    bool has_where;
    bool has_map;
    bool has_having;
    bool is_select_all; ///< 是不是选择所有，就是说是不是 select *
    bool is_aggregate_and_map; ///< 先 aggregate 再 map 的情况

public:
    ParseContext(): has_join(false), has_aggregate(false), has_group_by(false),
            has_where(false), has_map(false), has_having(false), is_select_all(false),
            is_aggregate_and_map(false)
    {
    }
    virtual ~ParseContext()
    {

    }
};


BOREALIS_NAMESPACE_END

#endif /* PARSECONTEXT_H_ */
