header
{
    #include <iostream>
    #include "Expression.h"
    #include "Aggregate.h"
    #include "SQLDelete.h"
    #include "SQLInsert.h"
    #include "SQLSelect.h"
    #include "SQLUpdate.h"

    using namespace std;
}

// Added to undefine TRUE and FALSE so they do not conflict with the
// enumeration in ExprParserTokenTypes.hpp; which is generated from the
// expr.g grammar file.
//
header "pre_include_cpp"
{
    #include "Expression.h"
}

header "post_include_cpp"
{
    #include "ExprUtil.h"

    inline static string unescape(string input) { return input.substr(1, input.length() - 2); }
}



options
{
    language = "Cpp";
}



class ExprParser extends Parser;

options
{
    k = 2;
    buildAST = true;
    defaultErrorHandler = false;
}

tokens
{
    UNARY;
    FUNCTIONCALL;
    SELECT_LIST;
}




aggregate_expr
    :   function_call EOF
    ;

expr
    :   top_level_expr EOF
    ;

top_level_expr
    :   logical_or_expr
    ;

// All these greedy options are necessary to support if/then/else.
//
// "if a then b else c + d" could be parsed as "(if a then b else c) + d" or
// "if a then b else (c+d)".  greedy=true assures the parser that the second
// one is correct.

logical_or_expr
    :   logical_and_expr ( options { greedy=true; } : LOGICAL_OR^ logical_and_expr )*
    ;

logical_and_expr
    :   not_equal_expr ( options { greedy=true; } : LOGICAL_AND^ not_equal_expr )*
    ;

not_equal_expr
    :   equal_expr ( options { greedy=true; } : NE^ equal_expr )*
    ;

equal_expr
    :   rel_op_expr ( options { greedy=true; } : EQ^ rel_op_expr )*
    ;

rel_op_expr
    :   add_expr ( options { greedy=true; } : (LANGLE^|LE^|RANGLE^|GE^) add_expr )*
    ;

add_expr 
    :   mult_expr
        ( options { greedy=true; } : (PLUS^|MINUS^) mult_expr )*
    ;

mult_expr
    :   unary_expr
        ( options { greedy=true; } : (TIMES^|DIV^|MOD^) unary_expr )*
    ;


unary_expr
    :   (   primary_expr
        |   unary_op unary_expr
            { ## = #( #[UNARY,"UNARY"], ## ); }
        )
    ;

unary_op : PLUS | MINUS | NOT;

primary_expr
    :   INTEGER
    |   FLOAT
    |   STRING_LITERAL
    |   TRUE | FALSE
    |   possibly_qualified_identifier
    |   array_internal_reference
    |   explicit_field_ref
    |   function_call
    |   LPAREN! top_level_expr RPAREN!
    |   if_then_expr
    ;

if_then_expr
    :    IF^ top_level_expr THEN! top_level_expr ELSE! top_level_expr
    ;

field_expr
    :   possibly_qualified_identifier
    |   array_internal_reference
    |   explicit_field_ref
    ;

function_call
    :   id:IDENTIFIER LPAREN!
        (   top_level_expr
            (   COMMA! top_level_expr  )*
        )?
        RPAREN!
        { ## = #( #[FUNCTIONCALL,"FUNCTIONCALL"], ## ); }
    ;

possibly_qualified_identifier
    :   id1:IDENTIFIER
        (   DOT^
            (   id2:IDENTIFIER
            |   explicit_field_ref
            )
        )?
    ;

array_internal_reference
    :   id1:IDENTIFIER
        (   ARROW^
            (   id2:IDENTIFIER
            |   explicit_field_ref
            )
        )?
    ;
    
explicit_field_ref
    :   FIELD! LPAREN! sl:STRING_LITERAL RPAREN!
        { ## = #[IDENTIFIER, unescape(sl->getText())]; }
    ;

sql_insert_expr
    :   INSERT^ INTO table:IDENTIFIER VALUES LPAREN!
        ( top_level_expr
            (   COMMA! top_level_expr)*
        )
        RPAREN!
    ;

sql_select_expr
    :   SELECT^ sql_select_list
        FROM table:IDENTIFIER
        sql_where_clause
    ;

sql_select_list
    :   TIMES
        { ## = #(#[SELECT_LIST,"SELECT_LIST"]); }
    |   ( sql_select_entry ( COMMA! sql_select_entry) * )
        { ## = #(#[SELECT_LIST,"SELECT_LIST"], ##); }
    ;

sql_select_entry
    : ( (IDENTIFIER) => (field:IDENTIFIER
            { ## = #(#[AS,"as"], field, #[IDENTIFIER, field->getText()]); } )
        |
        (top_level_expr (AS^ IDENTIFIER)?)
        // TODO: Automatically name other simple expressions.
    )
    ;

sql_where_clause
    :   WHERE^ top_level_expr
    |   /* No tokens means empty where clause. */
        { ## = #(#[WHERE,"where"]); }
    ;

sql_update_expr
    :   UPDATE^ table:IDENTIFIER
        sql_set_list
        sql_where_clause
    ;

sql_set_list
    :   SET^ sql_set_expr (COMMA! sql_set_expr)*
    ;

sql_set_expr
    :   field_expr EQ^ top_level_expr
    ;

sql_delete_expr
    :   DELETE^ FROM table:IDENTIFIER
        sql_where_clause
    ;
        


class ExprLexer extends Lexer;

options {
    k = 3;
    charVocabulary = '\0'..'\377';
}

tokens {
    TRUE = "true";
    FALSE = "false";
    IF = "if";
    THEN = "then";
    ELSE = "else";
    FIELD = "field";
    DELETE = "delete";
    INSERT = "insert";
    INTO = "into";
    VALUES = "values";
    SELECT = "select";
    AS = "as";
    FROM = "from";
    WHERE = "where";
    UPDATE = "update";
    SET = "set";
    LOGICAL_AND = "and";
    LOGICAL_OR = "or";
    NOT = "not";    
}

WS
    :   (   ' '
        |   '\t'
        |   ( "\r\n" | '\n' | '\r' ) { newline(); }
        )
        { $setType(antlr::Token::SKIP); }
    ;

TIMES  : '*';
DIV    : '/';
MOD    : '%';

PLUS   : '+';
MINUS  : '-';

LANGLE : '<';
RANGLE : '>';
LE     : "<=";
GE     : ">=";

EQ     : "=" | "==";
NE     : "!=";

C_LOGICAL_AND : "&&" { $setType(LOGICAL_AND); };
C_LOGICAL_OR  : "||" { $setType(LOGICAL_OR); };
C_NOT    : '!' { $setType(NOT); };

LPAREN : '(' ;
RPAREN : ')' ;
DOT    : '.' ;
ARROW  : "->";
COMMA  : ',' ;

protected DIGIT : '0'..'9';
protected INTEGER : ( DIGIT ) +;
protected FLOAT : ( DIGIT )+ DOT ( DIGIT )* ( 'f' | 'd' )?;

NUMBER
    :   ( ( DIGIT )+ DOT ) => FLOAT { $setType(FLOAT); }
    |   INTEGER { $setType(INTEGER); }
    ;

STRING_LITERAL
    :   '\"' (~'\"')* '\"'
    |   '\'' (~'\'')* '\''
    ;

protected ALPHA : 'A'..'Z' | 'a'..'z' | '_';

IDENTIFIER
    :   ALPHA ( ALPHA | DIGIT )*
    ;


class ExprWalker extends TreeParser;

options {
    defaultErrorHandler = false;
}

aggregate_expr [const ExprContext &ctxt] returns [ptr<Aggregate> ret]
    {
        string what = aggregate_expr_AST_in ? aggregate_expr_AST_in->getText() : "aggregate expression";
        ptr<Expression> l;
        vector<ptr<Expression> > args;
    }
    :   #(FUNCTIONCALL idx:IDENTIFIER
            ( l=expr[ctxt] { args.push_back(l); } )*
            {
                what = idx->getText();

                ret.set(Aggregate::instantiate(idx->getText()));

                ret->setArgs( args );
                ret->setup( ctxt ); // may throw AuroraException if setup fails

                if ( ret->numOutputFields() == 0 )
                {   Throw( ExprException,
                           "aggregate did not generate tuple description" );
                }
            }
        )
    ;
    exception 
    catch [ AuroraException  t ]
    {   throw antlr::SemanticException( what + ": " + t.getMessage() );
    }

expr [const ExprContext &ctxt] returns [ptr<Expression> ret]
    {
        string what = expr_AST_in ? expr_AST_in->getText() : "expression";
        vector<ptr<Expression> > args;
        ptr<Expression> l, r, e;
    }
    :   #(PLUS l=expr[ctxt] r=expr[ctxt])
        {
            if (( l->getType() == DataType::STRING )  ||
                ( r->getType() == DataType::STRING ))
            {
                ret.set( new ConcatExpression<EString>( NArgs( l, r )));
            }
            else
            {   ret = arithPromote<PlusExpressions>( l, r );
            }
        }
    |   #(MINUS l=expr[ctxt] r=expr[ctxt])
        { ret = arithPromote<MinusExpressions>(l, r); }
    |   #(TIMES l=expr[ctxt] r=expr[ctxt])
        { ret = arithPromote<TimesExpressions>(l, r); }
    |   #(DIV l=expr[ctxt] r=expr[ctxt])
        { ret = arithPromote<DivExpressions>(l, r); }
    |   #(MOD l=expr[ctxt] r=expr[ctxt])
        { ret = arithPromote<ModExpressions>(l, r); }
    |   #(EQ l=expr[ctxt] r=expr[ctxt])
        { ret = relPromote<EQExpressions>(l, r); }
    |   #(NE l=expr[ctxt] r=expr[ctxt])
        { ret = relPromote<NEExpressions>(l, r); }
    |   #(LANGLE l=expr[ctxt] r=expr[ctxt])
        { ret = relPromote<LTExpressions>(l, r); }
    |   #(RANGLE l=expr[ctxt] r=expr[ctxt])
        { ret = relPromote<GTExpressions>(l, r); }
    |   #(LE l=expr[ctxt] r=expr[ctxt])
        { ret = relPromote<LEExpressions>(l, r); }
    |   #(GE l=expr[ctxt] r=expr[ctxt])
        { ret = relPromote<GEExpressions>(l, r); }
    |   #(LOGICAL_AND l=expr[ctxt] r=expr[ctxt])
        {
            if (( l->getType() != DataType::BOOL )  ||
                ( r->getType() != DataType::BOOL ))
            {
                Throw( ExprException, "invalid types" );
            }

            ret.set( new AndExpression<bool>( NArgs( l, r )));
        }
    |   #(LOGICAL_OR l=expr[ctxt] r=expr[ctxt])
        {
            if (( l->getType() != DataType::BOOL )  ||
                ( r->getType() != DataType::BOOL ))
            {
                Throw( ExprException, "invalid types" );
            }

            ret.set( new OrExpression<bool>( NArgs( l, r )));
        }
    |   
        #(FUNCTIONCALL idx:IDENTIFIER
            ( l=expr[ctxt] { args.push_back(l); } )*
        )
        {
            what = idx->getText();


            DEBUG << "+++++++++++++ Get function:  " << idx->getText();

            // Access the function class.  May throw ExprException.
            Function *func = Function::instantiate(idx->getText());

            // Virtual call to evaluate the function.
            ret = func->makeExpression(args);
        }
    |   #(IF l=expr[ctxt] r=expr[ctxt] e=expr[ctxt] )
        {
            if ( l->getType() != DataType::BOOL )
            {    Throw(ExprException, "invalid predicate type");
            }

            if ( r->getType() != e->getType() )
            {    Throw(ExprException, "different types in then and else clauses");
            }

            if ( r->getType() == DataType::SINGLE )
            {    ret.set(new IfThenElseExpression<single>(NArgs(l,r,e)));
            }
            else if (r->getType() == DataType::DOUBLE)
            {    ret.set(new IfThenElseExpression<double>(NArgs(l,r,e)));
            }
            else if (r->getType() == DataType::INT)
            {    ret.set(new IfThenElseExpression<int32>(NArgs(l,r,e)));
            }
            else if (r->getType() == DataType::LONG)
            {   ret.set(new IfThenElseExpression<int64>(NArgs(l,r,e)));
            }
            else if (r->getType() == DataType::BYTE)
            {   ret.set(new IfThenElseExpression<int8>(NArgs(l,r,e)));
            }
            else if (r->getType() == DataType::SHORT)
            {   ret.set(new IfThenElseExpression<int16>(NArgs(l,r,e)));
            }
            else if (r->getType() == DataType::BOOL)
            {    ret.set(new IfThenElseExpression<bool>(NArgs(l,r,e)));
            }
            else
            {    ret.set(new IfThenElseExpression<EString>(NArgs(l,r,e)));
            }
        }
    |   ival:INTEGER
        {
            uint32  intval;
            if (!to_scalar(ival->getText(), intval)) {
                Throw(ExprException, "invalid integer");
            }
            ret.set(new ConstantExpression<int32>(intval));
        }
    |   fval:FLOAT
        {
            // yna -- support specifying constants as singles or doubles.
            string float_spec;
            string float_text;
            
            size_t text_back = fval->getText().length() - 1;

            if ( fval->getText().at(text_back) == 'f' ) {
                float_spec = "f";
                float_text = fval->getText().substr(0, text_back);
            }
            else if ( fval->getText().at(text_back) == 'd' ) {
                float_spec = "d";
                float_text = fval->getText().substr(0, text_back);
            }
            else
                float_text = fval->getText();

            if ( float_spec == "" || float_spec == "f" ) {
                float floatval;
                if (!to_scalar(float_text, floatval)) {
                    Throw(ExprException, "invalid floating-point number");
                }
                ret.set(new ConstantExpression<single>(floatval));
            } else {
                double doubleval;
                if (!to_scalar(float_text, doubleval)) {
                    Throw(ExprException, "invalid floating-point number");
                }
                ret.set(new ConstantExpression<double>(doubleval));
            }
        }
    |   sval:STRING_LITERAL
        {
            string stringval = sval->getText();
            ret.set(new ConstantExpression<EString>(unescape(stringval)));
        }
    |   TRUE  { ret.set(new ConstantExpression<bool>(true)); }
    |   FALSE { ret.set(new ConstantExpression<bool>(false)); }
    |   #(UNARY
            (   PLUS r=expr[ctxt] { ret = r; }
            |   MINUS r=expr[ctxt]
                {
                    if (r->getType() == DataType::SINGLE)
                    {   ret.set(new UMinusExpression<single>(r));
                    }
                    else if (r->getType() == DataType::DOUBLE)
                    {   ret.set(new UMinusExpression<double>(r));
                    }
                    else if (r->getType() == DataType::INT)
                    {   ret.set(new UMinusExpression<int32>(r));
                    }
                    else if (r->getType() == DataType::LONG)
                    {   ret.set(new UMinusExpression<int64>(r));
                    }
                    else if (r->getType() == DataType::SHORT)
                    {   ret.set(new UMinusExpression<int16>(r));
                    }
                    else if (r->getType() == DataType::BYTE)
                    {   ret.set(new UMinusExpression<int8>(r));
                    }
                    else
                    {   Throw(ExprException, "invalid type");
                    }
                }
            |   NOT r=expr[ctxt]
                {
                    if (r->getType() != DataType::BOOL)
                        Throw(ExprException, "invalid type");
                    ret.set(new NotExpression<bool>(r));
                }
            )
        )
    |   #(DOT id1:IDENTIFIER id2:IDENTIFIER)
        {
            ret = Expression::makeFieldExpression(id1->getText(),
                                                   id2->getText(),
                                                   ctxt);
        }
      |   #(ARROW lhid:IDENTIFIER rhid:IDENTIFIER)
        {
            ret = Expression::makeArrayReference(lhid->getText(),
                                                   rhid->getText(),
                                                   ctxt);
  
        }
    |   id:IDENTIFIER
        {
            ret = Expression::makeFieldExpression("", id->getText(),
                                                   ctxt);
        }
    ;
    exception 
    catch [AuroraException t] {
        throw antlr::SemanticException(what + ": " + t.getMessage());
    }

sql_insert_expr [const SQLContext &sc] returns [ptr<SQLInsert> ret]
    {
        string what = sql_insert_expr_AST_in ? sql_insert_expr_AST_in->getText() : "SQL insert expression";
        ptr<Expression> l;
        vector<ptr<Expression> > values;
    }
    :   #(INSERT INTO
            table:IDENTIFIER {table->getText() == sc.table_name}?
            VALUES
            ( l=expr[sc.ctxt] { values.push_back(l); } )*
            {
                ret.set(new SQLInsert(table->getText(), values));
                ret->setup(sc.ctxt); // may throw AuroraException if setup fails
                // TODO: Something to check it matched the table?
            }
        )
    ;
    exception 
    catch [AuroraException t] {
        throw antlr::SemanticException(what + ": " + t.getMessage());
    }

sql_select_expr [const SQLContext &sc] returns [ptr<SQLSelect> ret]
    {
        string what = sql_select_expr_AST_in ? sql_select_expr_AST_in->getText() : "SQL select expression";
        string name;
        SQLSelect::SelectEntry se;
        vector<SQLSelect::SelectEntry> select_list;
        ptr<Expression> value;
        ptr<SQLWhereClause> where_clause;
        DEBUG << "Parsing " << what;
    }
    :   #(SELECT #(SELECT_LIST ( se=sql_select_entry[sc] { select_list.push_back(se); } )* )
            FROM table:IDENTIFIER { table->getText() == sc.table_name }?
            where_clause=sql_where_clause[sc]
            {
                ret.set(new SQLSelect(table->getText(),
                                      select_list, where_clause));
                    // TODO: Something to check it matched the table?
                ret->setup(sc.ctxt); // may throw AuroraException if setup fails
            }
        )
    ;
    exception 
    catch [AuroraException t] {
        throw antlr::SemanticException(what + ": " + t.getMessage());
    }

sql_select_entry [const SQLContext &sc] returns [SQLSelect::SelectEntry ret]
    {
        string what = sql_select_entry_AST_in ? sql_select_entry_AST_in->getText() : "SQL select entry";
        ptr<Expression> e;
    }
    : ( (e=expr[sc.ctxt] { ret.first = ""; ret.second = e; })
       | 
      #(AS e=expr[sc.ctxt] name:IDENTIFIER
         { ret.first = name->getText(); ret.second = e; } )
      )
    ;
    exception 
    catch [AuroraException t] {
        throw antlr::SemanticException(what + ": " + t.getMessage());
    }


sql_set_expr [const SQLContext &sc] returns [ptr<SQLUpdate::SetExpr> ret]
    {
        ptr<Expression> value;
    }
    :   #(EQ (#(DOT table:IDENTIFIER {table->getText() == sc.table_name}?
                    field:IDENTIFIER) value=expr[sc.ctxt]
                {
                    // TODO: Support table aliases and stuff.
                    ret.set(new SQLUpdate::SetExpr(field->getText(), value,
                            sc.table_name, sc.ctxt));
                }
            |   // TODO: This branch doesn't seem to work.
                lvalue:IDENTIFIER value=expr[sc.ctxt]
                {
                    ret.set(new SQLUpdate::SetExpr(lvalue->getText(), value,
                            sc.table_name, sc.ctxt));
                }
            )
        )
    ;

sql_update_expr [const SQLContext &sc] returns [ptr<SQLUpdate> ret]
    {
        string what = sql_update_expr_AST_in ? sql_update_expr_AST_in->getText() : "SQL update expression";
        ptr<SQLUpdate::SetExpr> se;
        vector<ptr<SQLUpdate::SetExpr> > set_list;
        ptr<SQLWhereClause> where_clause;
    }
    :   #(UPDATE table:IDENTIFIER {table->getText() == sc.table_name}?
            #(SET ( se=sql_set_expr[sc] { set_list.push_back(se); } )* )
            where_clause=sql_where_clause[sc]
            {
                ret.set(new SQLUpdate(table->getText(), set_list,
                                      where_clause, sc.ctxt));
            }
        )
    ;
    exception 
    catch [AuroraException t] {
        throw antlr::SemanticException(what + ": " + t.getMessage());
    }


sql_delete_expr [const SQLContext &sc] returns [ptr<SQLDelete> ret]
    {
        string what = sql_delete_expr_AST_in ? sql_delete_expr_AST_in->getText() : "SQL delete expression";
        ptr<SQLWhereClause> where_clause;
    }
    :   #(DELETE FROM table:IDENTIFIER {table->getText() == sc.table_name}?
            where_clause=sql_where_clause[sc]
            {
                ret.set(new SQLDelete(table->getText(), where_clause));
            }
        )
    ;
    exception 
    catch [AuroraException t] {
        throw antlr::SemanticException(what + ": " + t.getMessage());
    }


sql_where_clause [const SQLContext &sc] returns [ptr<SQLWhereClause> ret]
    {
        string what = sql_where_clause_AST_in ? sql_where_clause_AST_in->getText() : "SQL where clause";
        ptr<SQLWhereClause::SubClause> sub_clause;
    }
    // TODO: Make sure this works properly for empty WHERE clause.
    : #(WHERE ((sub_clause=sql_where_conjunction[sc])?)
        {
            if (sub_clause) {
                ret.set(new SQLWhereClause(sc.ctxt, sub_clause));
                DEBUG << "Got a where clause: " << ret;
            }
            // Otherwise return null, which is the default value of ret.
        }
        )   
    ;
    exception 
    catch [AuroraException t] {
        throw antlr::SemanticException(what + ": " + t.getMessage());
    }

sql_where_conjunction [const SQLContext &sc] returns [ptr<SQLWhereClause::SubClause> ret]
    {
        string what = sql_where_conjunction_AST_in ?
                      sql_where_conjunction_AST_in->getText() :
                      "SQL where conjuction sub clause";
        ptr<SQLWhereClause::SubClause> l,r;
    }
    :
        ((#(LOGICAL_AND sql_where_conjunction[sc] sql_where_conjunction[sc])) =>
          #(LOGICAL_AND l=sql_where_conjunction[sc] r=sql_where_conjunction[sc]
                { ret.set(new SQLWhereClause::ConjunctionSubClause(l,r)); })
    | 
        (ret=sql_where_sub_clause[sc])
        )
    ;
    exception 
    catch [AuroraException t] {
        throw antlr::SemanticException(what + ": " + t.getMessage());
    }

sql_where_sub_clause [const SQLContext &sc] returns [ptr<SQLWhereClause::SubClause> ret]
    {
        string what = sql_where_sub_clause_AST_in ?
                      sql_where_sub_clause_AST_in->getText() :
                      "SQL where sub clause";
        ptr<Expression> e;
    }
    : ((#(EQ #(DOT IDENTIFIER IDENTIFIER))) =>
            // TODO: This Guard clause makes parsing fail when it is
            // violated, rather than just take the other branch. This
            // is a bug.
        #(EQ #(DOT table:IDENTIFIER {table->getText() == sc.table_name}?
                field:IDENTIFIER) e=expr[sc.ctxt]
        // TODO: Make sure e doesn't reference table rows.
            {
                DEBUG << "Equivalence with a field in the table ("
                      << field->getText() << ")";
                ret.set(new SQLWhereClause::FieldEqSubClause(
                        table->getText(), field->getText(), e));
            }
        )
        // TODO: Consider if e might be polluted. 
    | (#(EQ IDENTIFIER)) =>
       #(EQ sfield:IDENTIFIER {sc.isTableField(sfield->getText())}?
            e=expr[sc.ctxt]
        // TODO: Make sure e doesn't reference table rows.
            {
                DEBUG << "Equivalence with a field in the table ("
                      << sfield->getText() << ")";
                ret.set(new SQLWhereClause::FieldEqSubClause(
                        sc.table_name, sfield->getText(), e));
            }
        )
        // TODO: Consider if e might be polluted. 
    | (e=expr[sc.ctxt]
            {
                ret.set(new SQLWhereClause::ExprSubClause(e));
            }
        ))
    ;
    exception 
    catch [AuroraException t] {
        throw antlr::SemanticException(what + ": " + t.getMessage());
    }
