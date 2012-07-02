/**
 * ParseDriver.cc
 *
 *  Created on: Jun 23, 2012
 *      Author: hellojinjie
 */

#include "ParseDriver.h"
#include "antlr3.h"             // antlr3 的 runtime 的头文件 /opt/borealis-tools/antlrs/include
#include "CongerCQLLexer.h"     // antlr3 自动生成的文件
#include "CongerCQLParser.h"    // antlr3 自动生成的文件
#include "ParseContext.h"

BOREALIS_NAMESPACE_BEGIN
ParseDriver::ParseDriver()
{

}

ParseDriver::~ParseDriver()
{
    /* TODO 这里应该做些 ANTLR3 内存释放操作 */
}

pANTLR3_BASE_TREE ParseDriver::get_root(ParseContext context)
{
    pANTLR3_INPUT_STREAM input;
    pCongerCQLLexer lxr;
    pANTLR3_COMMON_TOKEN_STREAM tstream;
    pCongerCQLParser psr;

    const char * inputString = context.origin_cql.c_str();

    input = antlr3StringStreamNew((uint8_t *) inputString, ANTLR3_ENC_UTF8,
            strlen(inputString), (uint8_t *) "cql_statement");
    lxr = CongerCQLLexerNew(input);
    tstream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT,
            TOKENSOURCE(lxr));
    psr = CongerCQLParserNew(tstream);

    CongerCQLParser_statement_return statementAST = psr->statement(psr);

    pANTLR3_BASE_TREE root = statementAST.tree;
    return root;
}

/**
 * 解析 CQL 语句，这个方法应该是线程安全的
 *
 * @param cql 要解析的 CQL 语句
 * @return 返回解析结果 ParseContext
 */
void ParseDriver::parse(string cql)
{
    ParseContext context;
    context.origin_cql = cql;

    pANTLR3_BASE_TREE root = this->get_root(context);
    this->parse_node(context, root);
}

void ParseDriver::parse_node(ParseContext context, pANTLR3_BASE_TREE node)
{
    if (!node)
    {
        return;
    }
    string node_text = string((char*)node->getText(node)->chars);
    DEBUG << node_text;
    ANTLR3_UINT32 node_type = node->getType(node);
    switch (node_type)
    {
    case EOF:
        DEBUG << "end parsing cql ";
        return;
    case TOK_SFW:
        DEBUG << "start parsing select from where clause ";
        break;
    case TOK_SELECT:
        this->handle_parse_select(context, node);
        return;
    case TOK_FROM:
        this->handle_parse_from(context, node);
        return;
        break;
    case TOK_WHERE:
        this->handle_parse_where(context, node);
        return;
        break;
    case TOK_GROUP_BY:
        this->handle_parse_group_by(context, node);
        return;
        break;
    case TOK_HAVING:
        this->handle_parse_having(context, node);
        return;
        break;
    }
    
    int child_count = node->getChildCount(node);
    for (int i = 0; i < child_count; i++)
    {
        pANTLR3_BASE_TREE child_node = (pANTLR3_BASE_TREE) node->getChild(node, i);
        this->parse_node(context, child_node);
    }
}

void ParseDriver::handle_parse_select(ParseContext context, pANTLR3_BASE_TREE select_node)
{
    ANTLR3_UINT32 select_node_type = select_node->getType(select_node);
    switch (select_node_type)
    {
    case TOK_SELECT:
        context.clause_type = TOK_SELECT; ///< 这句是不是多余的？
        DEBUG << "start parse select clause";
        break;
    case TOK_PROJTERM_LIST:
        DEBUG << "start parse select proj list clause";
        break;
    case TOK_PROJTERM:
        this->handle_parse_proj_term(context, select_node);
        return;
        break;
    }
    int child_count = select_node->getChildCount(select_node);
    for (int i = 0; i < child_count; i++)
    {
        pANTLR3_BASE_TREE child_node = (pANTLR3_BASE_TREE) select_node->getChild(select_node, i);
        this->handle_parse_select(context, child_node);
    }
}

void ParseDriver::handle_parse_proj_term(ParseContext context, pANTLR3_BASE_TREE proj_term_node)
{
    ANTLR3_UINT32 node_type = proj_term_node->getType(proj_term_node);
    string node_text = string((char*)proj_term_node->getText(proj_term_node)->chars);
    DEBUG << "node " << node_text << " has type " << node_type;
    DEBUG << "start parse proj_term";
    ProjectionTerm term;
    context.select_list.push_back(term);
    int child_count = proj_term_node->getChildCount(proj_term_node);
    pANTLR3_BASE_TREE child_node = (pANTLR3_BASE_TREE) proj_term_node->getChild(proj_term_node, 0);
    this->handle_parse_proj_term_in_order(context, child_node);
    if (child_count > 1)
    {

    }
}

/**
 * 对select 中的每个结点进行中序遍历
 */
void ParseDriver::handle_parse_proj_term_in_order(ParseContext context, pANTLR3_BASE_TREE node)
{

    int child_count = node->getChildCount(node);

    if (child_count > 0)
    {
        pANTLR3_BASE_TREE child_node = (pANTLR3_BASE_TREE) node->getChild(node, 0);
        this->handle_parse_proj_term_in_order(context, child_node);
    }

    ANTLR3_UINT32 node_type = node->getType(node);
    string node_text = string((char*)node->getText(node)->chars);
    DEBUG << "node " << node_text << " has type " << node_type;

    for (int i = 1; i < child_count; i++)
    {
        pANTLR3_BASE_TREE child_node = (pANTLR3_BASE_TREE) node->getChild(node, i);
        this->handle_parse_proj_term_in_order(context, child_node);
    }

}

void ParseDriver::handle_parse_from(ParseContext context, pANTLR3_BASE_TREE from_node)
{
    ANTLR3_UINT32 from_node_type = from_node->getType(from_node);
    if (from_node_type != TOK_FROM)
    {
        return;
    }
    DEBUG << "start parse from clause";

}
void ParseDriver::handle_parse_where(ParseContext context, pANTLR3_BASE_TREE where_node)
{
    ANTLR3_UINT32 where_node_type = where_node->getType(where_node);
    if (where_node_type != TOK_WHERE)
    {
        return;
    }
    DEBUG << "start parse where clause";

}

void ParseDriver::handle_parse_group_by(ParseContext context, pANTLR3_BASE_TREE group_by_node)
{
    ANTLR3_UINT32 group_by_node_type = group_by_node->getType(group_by_node);
    if (group_by_node_type != TOK_GROUP_BY)
    {
        return;
    }
    DEBUG << "start parse group by clause";

}

void ParseDriver::handle_parse_having(ParseContext context, pANTLR3_BASE_TREE having_node)
{
    ANTLR3_UINT32 having_node_type = having_node->getType(having_node);
    if (having_node_type != TOK_HAVING)
    {
        return;
    }
    DEBUG << "start parse having clause";

}

bool ParseDriver::is_terminal_token(ParseContext context, pANTLR3_BASE_TREE node)
{
    
    return false;
}

BOREALIS_NAMESPACE_END

