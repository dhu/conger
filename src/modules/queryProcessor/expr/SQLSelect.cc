#include "SQLSelect.h"

#include "ExprLexer.hpp"
#include "ExprParser.hpp"
#include "ExprWalker.hpp"
#include "antlr/AST.hpp"
#include "antlr/CommonAST.hpp"
#include "antlr/MismatchedTokenException.hpp"

void SQLSelect::setup( const ExprContext  &ctxt )
                throw( AuroraException )
{
    if ( _select_star )
    {
        // Use the table schema.
        unsigned int trash;
        const CatalogSchema  *schema;  // = &_result_schema;
        //ctxt.getNamedTupleInfo(_table_name, _result_schema, trash);
        ctxt.get_named_schema_info(_table_name, schema, trash);
        _result_schema = *schema;      // Need to copy the result.
        DEBUG << "Select * found schema " << _result_schema
              << " copied from " << schema->as_string();
    }
    else
    {
        vector<SchemaField>      fields;
        vector<SelectEntry>::const_iterator  i;
        int   unnamed_field_counter = 0;

        for ( i = _select_list.begin(); i != _select_list.end(); ++i )
        {
            string name = i->first;
            if ( name == "" )
            {   name = "out" + to_string( unnamed_field_counter );
                unnamed_field_counter++;
            }

            Expression &expr = *(i->second);
            int size = expr.getLength();

            if ( size == -1 )
            {    Throw( aurora_typing_exception,
                        "Illegal variable sized argument to SQL Select: " +
                        to_string( expr ));
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
                       "Unsupported type " + to_string( type )
                                           + " in SQL Select." );
            }

            fields.push_back( SchemaField( name, type, size ));
        }

        copy( fields.begin(), fields.end(),
              back_inserter( _result_schema._field ) );

    DEBUG << "SQLSelect schema " << _result_schema;

        //_result_schema = TupleDescription( fields.begin(), fields.end(),
        //                                   TupleDescription::NO_HEADER );
    }
}


ptr<SQLSelect> SQLSelect::parse( string   expr,
                      const ExprContext  &ctxt,
                                 string   table_name
                               )
                          throw( ExprException )
{
    try
    {   DEBUG << "Parsing SQL select expression {" << expr << "}";

        istringstream iss( expr );
        ExprLexer lexer( iss );
        ExprParser parser( lexer );
        antlr::ASTFactory ast_factory;

        parser.initializeASTFactory( ast_factory );
        parser.setASTFactory( &ast_factory );

        parser.sql_select_expr();
        ASSERT( parser.getAST() );

        DEBUG << "AST(list): " << parser.getAST()->toStringList();

        ExprWalker walker;
        SQLContext sc(ctxt, table_name);

    DEBUG << "SQLContext table " << sc.table_name;

        ptr<SQLSelect> expr = walker.sql_select_expr( parser.getAST(), sc );
        ASSERT(expr);
        return expr;
    }
    catch ( antlr::ANTLRException  &e )
    {
    DEBUG << "About to throw exception for table " << table_name;
    Throw( ExprException, e.getMessage() );
    }
}
