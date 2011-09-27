#include "SQLInsert.h"
#include "ExprLexer.hpp"
#include "ExprParser.hpp"
#include "ExprWalker.hpp"
#include "antlr/AST.hpp"
#include "antlr/CommonAST.hpp"
#include "antlr/MismatchedTokenException.hpp"

// Figure out the insert TupleDescription based on args.
//
void SQLInsert::setup( const ExprContext  &ctxt )
                throw( AuroraException )
{
    vector<ptr<Expression> >::const_iterator i;
    vector<SchemaField> fields;
    int unnamed_field_counter = 0;

    for ( i = _args.begin(); i != _args.end(); ++i )
    {
        Expression &expr = **i;
        int size = expr.getLength();
        string name = "out" + to_string( unnamed_field_counter );
        unnamed_field_counter++;

        if ( size == -1 )
        {   Throw(aurora_typing_exception,
                  "Illegal variable sized argument to SQL Insert: " +
                  to_string(expr));
        }

        DataType type;
        DataType etype = expr.getType();

        if ( etype == DataType::INT )
        {   type = DataType::INT;
        }
        else if ( etype == DataType::LONG )
        {   type = DataType::LONG;
        }
        else if ( etype == DataType::SHORT )
        {   type = DataType::SHORT;
        }
        else if ( etype == DataType::BYTE )
        {   type = DataType::BYTE;
        }
        else if ( etype == DataType::SINGLE )
        {   type = DataType::SINGLE;
        }
        else if ( etype == DataType::DOUBLE )
        {   type = DataType::DOUBLE;
        }
        else if ( etype == DataType::STRING )
        {   type = DataType::STRING;
        }
        else
        {   Throw( aurora_typing_exception,
                   "Unsupported type " + to_string( type ) + " in SQL Insert." );
        }

        fields.push_back( SchemaField( name, type, size ));
    }
    _insert_schema = TupleDescription(fields.begin(), fields.end(),
                                      TupleDescription::NO_HEADER);
    DEBUG << "Insert schema: " << _insert_schema;
}


void SQLInsert::eval_into( char  *buf, EvalContext  &ctxt )
{
    vector<ptr<Expression> >::const_iterator i;

    for (i = _args.begin(); i != _args.end(); ++i)
    {
        Expression& expr = **i;
        expr.eval_into( buf, ctxt );
        buf += expr.getLength();
    }
}

ptr<SQLInsert> SQLInsert::parse( string   expr,
                      const ExprContext  &ctxt,
                                 string   table_name
                               )
                          throw( ExprException )
{
    try
    {   DEBUG << "Parsing SQL insert expression {" << expr << "}";

        istringstream  iss( expr );
        ExprLexer      lexer( iss );
        ExprParser     parser( lexer );
        antlr::ASTFactory  ast_factory;

        parser.initializeASTFactory( ast_factory );
        parser.setASTFactory( &ast_factory );

        parser.sql_insert_expr();
        ASSERT(parser.getAST());

        DEBUG << "AST(list): " << parser.getAST()->toStringList();

        ExprWalker walker;
        SQLContext sc( ctxt, table_name );
        ptr<SQLInsert> expr = walker.sql_insert_expr(parser.getAST(), sc);
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
