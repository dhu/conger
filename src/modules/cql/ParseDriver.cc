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

pANTLR3_BASE_TREE ParseDriver::get_root(ParseContext& context)
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

void ParseDriver::parse_node(ParseContext& context, pANTLR3_BASE_TREE node)
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

void ParseDriver::handle_parse_select(ParseContext& context, pANTLR3_BASE_TREE select_node)
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

void ParseDriver::handle_parse_proj_term(ParseContext& context, pANTLR3_BASE_TREE proj_term_node)
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
        pANTLR3_BASE_TREE child_node = (pANTLR3_BASE_TREE) proj_term_node->getChild(proj_term_node, 1);
        if (child_node->getType(child_node) == KW_AS)
        {
            pANTLR3_BASE_TREE alias_node = (pANTLR3_BASE_TREE) child_node->getChild(child_node, 0);
            string alias = string((char*)alias_node->getText(alias_node)->chars);
            context.select_list.back().alias = alias;
        }
    }
}

/**
 * 对select 中的每个结点进行中序遍历
 */
void ParseDriver::handle_parse_proj_term_in_order(ParseContext& context, pANTLR3_BASE_TREE node)
{
    ANTLR3_UINT32 node_type = node->getType(node);
    /* 定义 switch case 中要用到的变量 */
    pANTLR3_BASE_TREE s_node;
    pANTLR3_BASE_TREE ss_node;
    string s_text;
    int s_count;
    switch (node_type)
    {
    case PLUS:
    case MINUS:
    case STAR:
    case DIV:
        context.select_list.back().expression.append("(");
        s_node = (pANTLR3_BASE_TREE) node->getChild(node, 0);
        this->handle_parse_proj_term_in_order(context, s_node);

        s_text = string((char*)node->getText(node)->chars);
        context.select_list.back().expression.append(s_text);

        s_node = (pANTLR3_BASE_TREE) node->getChild(node, 1);
        this->handle_parse_proj_term_in_order(context, s_node);
        context.select_list.back().expression.append(")");
        break;
    case DOT:
        /* 处理 stream.field 的形式 */
        ss_node = (pANTLR3_BASE_TREE) node->getChild(node, 0);
        s_text = string((char*)ss_node->getText(ss_node)->chars);
        /* 把流名加到 streams 中去，如果已经有了就不要再加了 */
        if (find(context.select_list.back().streams.begin(),
                context.select_list.back().streams.end(), s_text)
                == context.select_list.back().streams.end())
        {
            context.select_list.back().streams.push_back(s_text);
        }

        s_text = string((char*)node->getText(node)->chars);
        context.select_list.back().expression.append(s_text);

        ss_node = (pANTLR3_BASE_TREE) node->getChild(node, 1);
        this->handle_parse_proj_term_in_order(context, ss_node);
        break;
    case Integer:
    case Number:
    case Identifier:
        s_text = string((char*)node->getText(node)->chars);
        context.select_list.back().expression.append(s_text);
        break;
    case TOK_AGGR:
        context.select_list.back().is_aggregate = true;
        s_node = (pANTLR3_BASE_TREE) node->getChild(node, 0);
        this->handle_parse_proj_term_in_order(context, s_node);
        context.select_list.back().expression.append("(");

        s_node = (pANTLR3_BASE_TREE) node->getChild(node, 1);
        this->handle_parse_proj_term_in_order(context, s_node);
        context.select_list.back().expression.append(")");
        break;
    case TOK_FUNC:
        context.select_list.back().is_map = true;
        s_node = (pANTLR3_BASE_TREE) node->getChild(node, 0);
        this->handle_parse_proj_term_in_order(context, s_node);
        context.select_list.back().expression.append("(");
        s_count = node->getChildCount(node);
        for (int i = 1; i < s_count; i++)
        {
            s_node = (pANTLR3_BASE_TREE) node->getChild(node, i);
            this->handle_parse_proj_term_in_order(context, s_node);
            if (i != s_count - 1)
            {
                context.select_list.back().expression.append(",");
            }
        }
        context.select_list.back().expression.append(")");

        break;
    case KW_MAX:
    case KW_COUNT:
    case KW_MIN:
    case KW_AVG:
    case KW_SUM:
        s_text = string((char*)node->getText(node)->chars);
        context.select_list.back().expression.append(s_text);
        break;
    default:
        DEBUG << "unexpected operator, node type: " << node_type;
        break;
    }
}

void ParseDriver::handle_parse_from(ParseContext& context, pANTLR3_BASE_TREE from_node)
{
    ANTLR3_UINT32 from_node_type = from_node->getType(from_node);
    pANTLR3_BASE_TREE s_node;
    ANTLR3_UINT32 s_child_count;
    switch (from_node_type)
    {
    case TOK_FROM:
        DEBUG << "start parse from clause";
        break;
    case TOK_RELATION_LIST:
        /* 空，什么也不做 */
        break;
    case TOK_RELATION_VARIABLE:
        s_node = (pANTLR3_BASE_TREE) from_node->getChild(from_node, 0);
        context.from_stream.stream_name = string((char*)s_node->getText(s_node)->chars);
        s_child_count = from_node->getChildCount(from_node);
        if (s_child_count > 1)
        {
            s_node = (pANTLR3_BASE_TREE) from_node->getChild(from_node, 1);
            this->handle_parse_window(context, s_node);
        }
        return;
        break;
    case TOK_COND_LIST:
        break;
    case KW_TIMEOUT:
        break;
    default:
        DEBUG << "unknown node type: " << from_node_type;
        break;
    }
    int child_count = from_node->getChildCount(from_node);
    for (int i = 0; i < child_count; i++)
    {
        pANTLR3_BASE_TREE child_node = (pANTLR3_BASE_TREE) from_node->getChild(from_node, i);
        this->handle_parse_from(context, child_node);
    }
}

void ParseDriver::handle_parse_window(ParseContext& context, pANTLR3_BASE_TREE window_node)
{
    ANTLR3_UINT32 window_node_type = window_node->getType(window_node);
    ANTLR3_UINT32 s_child_count;
    pANTLR3_BASE_TREE s_child_node;
    switch (window_node_type)
    {
    case TOK_WINDOW:
        break;
    case KW_RANGE:
        s_child_count = window_node->getChildCount(window_node);
        s_child_node = (pANTLR3_BASE_TREE) window_node->getChild(window_node, 0);
        context.from_stream.window.range = atoi((char*) s_child_node->getText(s_child_node)->chars);
        if (s_child_count > 1)
        {
            s_child_node = (pANTLR3_BASE_TREE) window_node->getChild(window_node, 1);
            context.from_stream.window.range_time_unit =
                    this->infer_time_unit(s_child_node->getType(s_child_node));
        }
        break;
    case KW_SLIDE:
        s_child_count = window_node->getChildCount(window_node);
        s_child_node = (pANTLR3_BASE_TREE) window_node->getChild(window_node, 0);
        context.from_stream.window.slide = atoi((char*) s_child_node->getText(s_child_node)->chars);
        if (s_child_count > 1)
        {
            s_child_node = (pANTLR3_BASE_TREE) window_node->getChild(window_node, 1);
            context.from_stream.window.slide_time_unit =
                    this->infer_time_unit(s_child_node->getType(s_child_node));
        }
        break;
    case KW_ROW:
        s_child_node = (pANTLR3_BASE_TREE) window_node->getChild(window_node, 0);
        context.from_stream.window.row = atoi((char*) s_child_node->getText(s_child_node)->chars);
        break;
    case KW_TIMEOUT:
        s_child_count = window_node->getChildCount(window_node);
        s_child_node = (pANTLR3_BASE_TREE) window_node->getChild(window_node, 0);
        context.from_stream.window.time_out = atoi((char*) s_child_node->getText(s_child_node)->chars);
        if (s_child_count > 1)
        {
            s_child_node = (pANTLR3_BASE_TREE) window_node->getChild(window_node, 1);
            context.from_stream.window.time_out_time_unit =
                    this->infer_time_unit(s_child_node->getType(s_child_node));
        }
        break;
    default:
        DEBUG << "unhandle node type: " << window_node_type;
        break;
    }
}

/**
 * 根据节点的类型判断时间单位
 *
 * @param node_type 节点类型
 * @return time_unit 时间单位，枚举类型
 */
TimeUnit ParseDriver::infer_time_unit(ANTLR3_UINT32 node_type)
{
    TimeUnit time_unit;
    switch (node_type)
    {
    case KW_DAY:
    case KW_DAYS:
        time_unit = TimeUnit::DAY;
        break;
    case KW_HOUR:
    case KW_HOURS:
        time_unit = TimeUnit::HOUR;
        break;
    case KW_MINUTE:
    case KW_MINUTES:
        time_unit = TimeUnit::MINUTE;
        break;
    case KW_SECOND:
    case KW_SECONDS:
        time_unit = TimeUnit::SECOND;
        break;
    case KW_MILLISECOND:
    case KW_MILLISECONDS:
        time_unit = TimeUnit::MILLISECOND;
        break;
    case KW_MICROSECOND:
    case KW_MICROSECONDS:
        time_unit = TimeUnit::MICROSECOND;
        break;
    default:
        DEBUG << "unknown time unit, node type: " << node_type;
        break;
    }

    return time_unit;
}

void ParseDriver::handle_parse_where(ParseContext& context, pANTLR3_BASE_TREE where_node)
{
    ANTLR3_UINT32 where_node_type = where_node->getType(where_node);
    switch (where_node_type)
    {
    case TOK_WHERE:
        DEBUG << "start parse where clause";
        break;
    default:
        DEBUG << "unknown node type: " << where_node_type;
        break;
    }

}

void ParseDriver::handle_parse_group_by(ParseContext& context, pANTLR3_BASE_TREE group_by_node)
{
    ANTLR3_UINT32 group_by_node_type = group_by_node->getType(group_by_node);
    if (group_by_node_type != TOK_GROUP_BY)
    {
        return;
    }
    DEBUG << "start parse group by clause";

}

void ParseDriver::handle_parse_having(ParseContext& context, pANTLR3_BASE_TREE having_node)
{
    ANTLR3_UINT32 having_node_type = having_node->getType(having_node);
    if (having_node_type != TOK_HAVING)
    {
        return;
    }
    DEBUG << "start parse having clause";

}

void ParseDriver::handle_parse_contidtion(string& condition, pANTLR3_BASE_TREE node)
{
    ANTLR3_UINT32 node_type = node->getType(nodeo);
    switch (node_type)
    {
    case KW_OR:
        break;
    case KW_AND:
        break;
    case EQUAL:
    case NOTEQUAL:
    case LESSTHANOREQUALTO:
    case LESSTHAN:
    case GREATERTHANOREQUALTO:
    case GREATERTHAN:
        break;
    case DOT:
        break;
    case Number:
    case Integer:
    case Identifier:
        break;
    case PLUS:
    case MINUS:
    case STAR:
    case DIV:
        break;
    default:
        break;
    }
}

BOREALIS_NAMESPACE_END

