#ifndef SQL_SELECT_H
#define SQL_SELECT_H

#include "Expression.h"
#include "TupleDescription.h"
#include "SQLUtil.h"

BOREALIS_NAMESPACE_BEGIN


class SQLSelect
{
  public:
    typedef pair<string, ptr<Expression> > SelectEntry;

    /// Empty select list means select all.
    /// NULL where clause means always true.
    SQLSelect(string table,
              const vector<SelectEntry>& select_list,
              ptr<SQLWhereClause> where_clause) :
        _table_name(table),
        _select_star(select_list.empty()),
        _select_list(select_list),
        _where_clause(where_clause)
    {
        vector<SelectEntry>::const_iterator i;
        for (i = _select_list.begin(); i != _select_list.end(); ++i)
            _select_expressions.push_back(i->second);
    }

    ~SQLSelect() {}

    // Figure out the insert TupleDescription based on args. ctxt
    // should be loaded with the table and the input descriptions.
    void setup(const ExprContext& ctxt) throw (AuroraException);

    string get_table_name() const { return _table_name; }

    CatalogSchema*  get_result_catalog_schema()
    {  return( &_result_schema );
    }

    /****************************
    TupleDescription getResultTupleDescription() const
    {
        return _result_schema;
    }
    ********************************/

    /// Evaluate the where clause, return true if it is true or nonexistant.
    ptr<SQLWhereClause> get_where_clause()
    {
        return _where_clause;
    }

    /// Evaluates into a buffer.  The buffer must be large enough.
    void eval_into(char *buf, const EvalContext& ctxt) throw (EvalException)
    {
        if (_select_star)
            // Select the whole table row, which will be tuple id 1.
            memcpy(buf, ctxt.getTuple(1), _result_schema.get_size());
        else
            Expression::eval_vector_into(_select_expressions, buf, ctxt);
    }

    const string as_string() const
    {
        string selection;
        if (_select_star)
            selection = "*";
        else
            selection = to_string(_select_list);

        return "SQLSelect(" + selection + " from " + _table_name +
            " where " + _where_clause + ")";
    }

  public:
    static ptr<SQLSelect> parse(string expr, const ExprContext& ctxt,
                                string table_name)
        throw (ExprException);

  private:
    string _table_name;
    bool _select_star;
    vector<SelectEntry> _select_list;
    vector<ptr<Expression> > _select_expressions;
    ptr<SQLWhereClause> _where_clause;

    CatalogSchema    _result_schema;
    //TupleDescription _result_schema;
};

BOREALIS_NAMESPACE_END

#endif // SQL_SELECT_H
