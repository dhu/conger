/**
 * ParseDriver.h
 *
 *  Created on: Jun 23, 2012
 *      Author: hellojinjie
 */

#ifndef PARSEDRIVER_H_
#define PARSEDRIVER_H_

#include "common.h"
#include "ParseContext.h"
#include <antlr3.h>

BOREALIS_NAMESPACE_BEGIN
class ParseDriver
{
private:

public:
    ParseDriver();
    virtual ~ParseDriver();
    ParseContext parse(string query_name, string cql);

private:
    pANTLR3_BASE_TREE get_root(ParseContext& context);
    void parse_node(ParseContext& context, pANTLR3_BASE_TREE node);

    void handle_parse_select(ParseContext& context, pANTLR3_BASE_TREE select_node);
    void handle_parse_from(ParseContext& context, pANTLR3_BASE_TREE from_node);
    void handle_parse_where(ParseContext& context, pANTLR3_BASE_TREE where_node);
    void handle_parse_group_by(ParseContext& context, pANTLR3_BASE_TREE group_by_node);
    void handle_parse_having(ParseContext& context, pANTLR3_BASE_TREE having_node);
    void handle_parse_join(ParseContext& context, pANTLR3_BASE_TREE join_node);

    void handle_parse_proj_term(ParseContext& context, pANTLR3_BASE_TREE proj_term_node);
    void handle_parse_proj_term_in_order(ParseContext& context, pANTLR3_BASE_TREE proj_term_node);

    CQL::TimeUnit infer_time_unit(ANTLR3_UINT32 node_type);

    void handle_parse_contidtion(ParseContext& context, string& condition, pANTLR3_BASE_TREE node);
    void handle_parse_window(WindowDefinition& window, pANTLR3_BASE_TREE window_node);
};

BOREALIS_NAMESPACE_END
#endif /* PARSEDRIVER_H_ */
