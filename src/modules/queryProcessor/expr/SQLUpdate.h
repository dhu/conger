#ifndef SQL_UPDATE_H
#define SQL_UPDATE_H

#include "Expression.h"
#include "TupleDescription.h"
#include "SQLUtil.h"

BOREALIS_NAMESPACE_BEGIN

class SQLUpdate
{
  public:
    class SetExpr
    {
      public:
        SetExpr(string field_name, ptr<Expression> expr,
                string table_name, const ExprContext& ctxt);
        ~SetExpr() {}

        // Evaluate into the tuple, given the context.
        void eval_into(char *tuple, const EvalContext& ctxt)
        {
            _expr->eval_into(tuple+_offset, ctxt);
        }

        string as_string() const
        {
            return ("SetExpr(offset(" + to_string(_offset) +
                    "=" + _expr + ")");
        }
      private:
        unsigned int _offset;
        ptr<Expression> _expr;
    };
  public:
    SQLUpdate(string table,
              const vector<ptr<SetExpr> >& set_list,
              ptr<SQLWhereClause> where_clause,
              const ExprContext &ctxt) :
        _table_name(table),
        _set_list(set_list),
        _where_clause(where_clause)
    {
        // TODO: Use named context or something.
        //TupleDescription td;
        //ctxt.getNamedTupleInfo(_table_name, td, _table_tuple_num);
        const CatalogSchema  *td;
        ctxt.get_named_schema_info(_table_name, td, _table_tuple_num);

        _row_size = td->get_size();
    }
    ~SQLUpdate() {}

    string get_table_name() const { return _table_name; }

    /// Return the where clause, or NULL if there isn't one.
    ptr<SQLWhereClause> get_where_clause() { return _where_clause; }

    /// Given eval context with the input tuple at index 1, and the
    /// current table row at 0. new_row should have enough space.
    void eval_new_row(char *new_row, const EvalContext& ctxt)
        throw (EvalException)
        {
        // Copy existing row
        memcpy(new_row, ctxt.getTuple(_table_tuple_num), _row_size);
        // Run set expressions.
        vector<ptr<SetExpr> >::const_iterator i;
        for (i = _set_list.begin(); i != _set_list.end(); ++i)
        {
            SetExpr &se = **i;
            se.eval_into(new_row, ctxt);
        }
    }

    const string as_string() const
    {
        return ("SQLUpdate(table=" + _table_name + " setlist=" +
                _set_list + " where=" + _where_clause + ")");
    }

  public:
    static ptr<SQLUpdate> parse(string expr, const ExprContext& ctxt,
                                string table_name)
        throw (ExprException);

  private:
    string _table_name;
    unsigned int _table_tuple_num; // The number of the table row in eval contexts.
    size_t _row_size;
    vector<ptr<SetExpr> > _set_list;
    ptr<SQLWhereClause> _where_clause;
};

BOREALIS_NAMESPACE_END

#endif // SQL_UPDATE_H
