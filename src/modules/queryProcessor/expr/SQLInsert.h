#ifndef SQLINSERT_H
#define SQLINSERT_H

#include "Expression.h"
#include "TupleDescription.h"

using namespace Borealis;

class SQLInsert
{
  public:
    SQLInsert(string table, vector<ptr<Expression> > args) :
        _table(table), _args(args) {}
    ~SQLInsert() {}

    // Figure out the insert TupleDescription based on args.
    void setup(const ExprContext& ctxt) throw (AuroraException);

    TupleDescription get_insert_tuple_description() const
    {
        return _insert_schema;
    }

    string get_table_name() const { return _table; }

    void eval_into(char *buf, EvalContext& ctxt);

    const string as_string() const
    {
        return "SQLInsert(schema:" + to_string(_insert_schema)
            + " args:" + to_string(_args) + ")";
    }

  public:
    static ptr<SQLInsert> parse(string expr, const ExprContext& ctxt,
                                string table_name)
        throw (ExprException);

  private:
    string _table;
    TupleDescription _insert_schema;

    vector<ptr<Expression> > _args;
};

#endif                         // SQLINSERT_H
