#include "SQLUpdate.h"

#include "ExprLexer.hpp"
#include "ExprParser.hpp"
#include "ExprWalker.hpp"
#include "antlr/AST.hpp"
#include "antlr/CommonAST.hpp"
#include "antlr/MismatchedTokenException.hpp"

BOREALIS_NAMESPACE_BEGIN

SQLUpdate::SetExpr::SetExpr(string field_name, ptr<Expression> expr,
                            string table_name, const ExprContext& ctxt) :
    _expr(expr)
    {
    //TupleDescription schema;
    const CatalogSchema *schema;
    unsigned int tuple_index;
    //ctxt.getNamedTupleInfo(table_name, schema, tuple_index);
    ctxt.get_named_schema_info(table_name, schema, tuple_index);
    int field_index = schema->index_of_field(field_name);
    if (field_index == -1)
        Throw(aurora_typing_exception,
              "Field " + field_name + " does not exist.");
    //SchemaField *f = schema.getSchemaField(field_index);
    // TODO: Check type/size.
    _offset = schema->get_field_offset( field_index );
}

ptr<SQLUpdate> SQLUpdate::parse(string expr, const ExprContext& ctxt,
                                string table_name)
    throw (ExprException)
    {
    try
    {
        DEBUG << "Parsing SQL update expression {" << expr << "}";

        istringstream iss(expr);
        ExprLexer lexer(iss);
        ExprParser parser(lexer);
        antlr::ASTFactory ast_factory;

        parser.initializeASTFactory(ast_factory);
        parser.setASTFactory(&ast_factory);

        parser.sql_update_expr();
        ASSERT(parser.getAST());

        DEBUG << "AST(list): " << parser.getAST()->toStringList();

        ExprWalker walker;
        SQLContext sc(ctxt, table_name);
        ptr<SQLUpdate> expr = walker.sql_update_expr(parser.getAST(), sc);
        ASSERT(expr);
        return expr;
    } catch (antlr::ANTLRException& e)
    {
        Throw(ExprException, e.getMessage());
    }
}

BOREALIS_NAMESPACE_END
