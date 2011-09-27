#include "Aggregate.h"
#include "ExprLexer.hpp"
#include "ExprParser.hpp"
#include "ExprWalker.hpp"
#include "antlr/AST.hpp"
#include "antlr/CommonAST.hpp"

#include "StandardAggregates.h"

NMSTL_SERIAL_DEFINE( Aggregate, 1010 );
NMSTL_SERIAL_DEFINE( AggregateWindow, 1011 );


////////////////////////////////////////////////////////////////////////////////
//
Aggregate  *Aggregate::instantiate(const string&  name)
                             throw(AuroraBadEntityException)
{
    Aggregate  *aggregate;
//..............................................................................


    aggregate = instantiate_udb(name);
    
    if (aggregate == NULL)
    {   if (name == "count")          aggregate = new CountAggregate;
        else if (name == "min")       aggregate = new MinAggregate;
        else if (name == "max")       aggregate = new max_aggregate;
        else if (name == "sum")       aggregate = new sum_aggregate;
        else if (name == "avg")       aggregate = new avg_aggregate;
        else if (name == "firstval")  aggregate = new FirstvalAggregate;
        else if (name == "lastval")   aggregate = new LastvalAggregate;
        else
        {   Throw(AuroraBadEntityException,
                  "Unknown aggregate function:  " + name);
        }
    }

    return(aggregate);
}



AggregateWindow* Aggregate::open_window()
{
    return new AggregateWindow();
}


ptr<Aggregate> Aggregate::parse( string expr, const ExprContext  &ctxt )
                          throw( ExprException )
{
    try
    {
        DEBUG << "Parsing aggregate expression {" << expr << "}";

        istringstream      iss( expr );
        ExprLexer          lexer( iss );
        ExprParser         parser( lexer );
        antlr::ASTFactory  ast_factory;

        parser.initializeASTFactory( ast_factory );
        parser.setASTFactory( &ast_factory );

        parser.aggregate_expr();
        ASSERT( parser.getAST() );

        DEBUG << "AST: " << parser.getAST()->toStringList();

        ExprWalker walker;
        ptr<Aggregate> expr = walker.aggregate_expr( parser.getAST(), ctxt );
        ASSERT( expr );

        DEBUG << "Done!";
        return( expr );
    }
    catch ( antlr::ANTLRException  &e )
    {
        Throw( ExprException, e.getMessage() );
    }
}

const string Aggregate::as_string() const
{
    string s = "Aggregate(out:" + to_string(_out_fields) + " args:" + to_string(_args) + ")";
    return s;
}

const string AggregateWindow::as_string() const
{
    string s = _agg->as_string();
    return s;
}

