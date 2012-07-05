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

/**
 * [range 2 hours slide 4 minutes timeout 8 minutes]
 * [row 100 slide 80 timeout 30]
 */
struct WindowDefinition
{
    WindowType type;
    int range;
    TimeUnit range_time_unit;
    int row;
    int slide;
    TimeUnit slide_time_unit;
    int time_out;
    TimeUnit time_out_time_unit;

    WindowDefinition(): range(0), row(0), slide(0), time_out(0)
    {
    }
};

struct ProjectionTerm
{
    string expression;
    string alias;

    /* 是不是聚集函数 */
    bool is_aggregate;

    /* 是不是投影函数 */
    bool is_map;

    ProjectionTerm(): is_aggregate(false), is_map(false)
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
    TimeUnit time_unit;

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
    string origin_cql;
    list<ProjectionTerm> select_list;
    StreamVariable from_stream;
    StreamJoin stream_join;
    WhereConditoin where;
    StreamGroupBy group_by;
    HavingCondition having;

    typedef int ProjTerm;
    typedef int ArithExpr;


public:
    ParseContext();
    virtual ~ParseContext();
};


BOREALIS_NAMESPACE_END

#endif /* PARSECONTEXT_H_ */
