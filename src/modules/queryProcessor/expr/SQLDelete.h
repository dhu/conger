#ifndef SQL_DELETE_H_
#define SQL_DELETE_H_

#include "Expression.h"
#include "SQLUtil.h"

BOREALIS_NAMESPACE_BEGIN;

class SQLDelete
{
  public:
    SQLDelete(string table_name, ptr<SQLWhereClause> where_clause) :
        _table_name(table_name), _where_clause(where_clause)
    {}

    static ptr<SQLDelete>
    parse(string expr, const ExprContext& ctxt, string table_name)
        throw( ExprException );
    
    string get_table_name() const { return _table_name; }
    
    ptr<SQLWhereClause> get_where_clause() { return _where_clause; }
    
  private:
    string _table_name;
    ptr<SQLWhereClause> _where_clause;
};

BOREALIS_NAMESPACE_END;

#endif /*SQL_DELETE_H_*/
