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
    void parse(string cql);

private:
    pANTLR3_BASE_TREE get_root(ParseContext context);
    void parse_node(ParseContext context, pANTLR3_BASE_TREE node);
    bool is_terminal_token(ParseContext context, pANTLR3_BASE_TREE node);

    void handle_parse_select(ParseContext, pANTLR3_BASE_TREE select_node);
    void handle_parse_from(ParseContext, pANTLR3_BASE_TREE from_node);
    void handle_parse_where(ParseContext, pANTLR3_BASE_TREE where_node);
    void handle_parse_group_by(ParseContext, pANTLR3_BASE_TREE group_by_node);
    void handle_parse_having(ParseContext, pANTLR3_BASE_TREE having_node);

    void handle_parse_proj_term(ParseContext context, pANTLR3_BASE_TREE proj_term_node);
    void handle_parse_proj_term_in_order(ParseContext context, pANTLR3_BASE_TREE proj_term_node);
};

BOREALIS_NAMESPACE_END
#endif /* PARSEDRIVER_H_ */
