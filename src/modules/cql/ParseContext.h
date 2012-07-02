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

BOREALIS_NAMESPACE_BEGIN

struct ProjectionTerm
{
    string expression;
    string alias;

    /* 是不是聚集函数 */
    bool is_aggregate;

    /* 是不是投影函数 */
    bool is_map;
};


/**
 * 这个类不是线程安全的，每解析一个语句都应该 new 一个实例
 */
class ParseContext
{
public:
    string origin_cql;
    list<ProjectionTerm> select_list;
    typedef int FromClause;
    typedef int WhereClause;
    typedef int GroupByClause;
    typedef int HavingClause;

    typedef int ProjTerm;
    typedef int ArithExpr;

    /** 没用？
     * 当前解析的子句的类型，
     * 可以是 TOK_SELECT, TOK_FROM, TOK_WHERE, TOK_GROUP_BY, TOK_HAVING
     */
    ANTLR3_UINT32 clause_type;

    /** 前一次访问的 结点 */
    stack<pANTLR3_BASE_TREE> previous_node;

public:
    ParseContext();
    virtual ~ParseContext();
};


BOREALIS_NAMESPACE_END

#endif /* PARSECONTEXT_H_ */
