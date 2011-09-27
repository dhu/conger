#include "SQLDelete.h"

#include "ExprLexer.hpp"
#include "ExprParser.hpp"
#include "ExprWalker.hpp"
#include "antlr/AST.hpp"
#include "antlr/CommonAST.hpp"
#include "antlr/MismatchedTokenException.hpp"

BOREALIS_NAMESPACE_BEGIN;

ptr<SQLDelete> SQLDelete::parse(string expr, const ExprContext& ctxt, string table_name)
    throw( ExprException )
{
    try
    {   DEBUG << "Parsing SQL delete expression {" << expr << "}";

        istringstream  iss( expr );
        ExprLexer      lexer( iss );
        ExprParser     parser( lexer );
        antlr::ASTFactory  ast_factory;

        parser.initializeASTFactory( ast_factory );
        parser.setASTFactory( &ast_factory );

        parser.sql_delete_expr();
        ASSERT(parser.getAST());

        DEBUG << "AST(list): " << parser.getAST()->toStringList();

        ExprWalker walker;
        SQLContext sc( ctxt, table_name );
        ptr<SQLDelete> expr = walker.sql_delete_expr(parser.getAST(), sc);
        ASSERT( expr );
        return( expr );
    }
    catch ( antlr::MismatchedTokenException  &e )
    {
        Throw( ExprException, e.getMessage() + " tokenText=\"" + e.tokenText +
               "\" mismatchType=" + e.mismatchType + " token=" +
               (e.token ? ("\"" + e.token->toString() + "\"") : "NULL") +
               " node=" +
               (e.node ? ("\"" + e.node->toStringList() + "\"") : "NULL" ));
    }
    catch ( antlr::ANTLRException  &e )
    {
        Throw( ExprException, e.getMessage() );
    }
}

BOREALIS_NAMESPACE_END;
