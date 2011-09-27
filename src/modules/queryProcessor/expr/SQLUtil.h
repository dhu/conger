#ifndef SQLUTIL_H
#define SQLUTIL_H

#include "Expression.h"

using namespace Borealis;

struct SQLContext
{
    SQLContext(const ExprContext &ec, string tname) :
        ctxt(ec), table_name(tname) {}
    bool isTableField(string field) const
    {
        unsigned int index;

        //TupleDescription td;
        //ctxt.getNamedTupleInfo(table_name, td, index);
        //return (td.indexOfField(field) != -1);

        const CatalogSchema  *td;
        ctxt.get_named_schema_info( table_name, td, index );
        return( td->index_of_field( field ) != -1 );
    }

    const ExprContext &ctxt;
    string table_name;
};

class SQLWhereClause
{
  public:
    enum Type
    {
        EXPRESSION,
        FIELD_EQ_EXPRESSION,
        CONJUNCTION
    };

    class SubClause
    {
      public:
        virtual Type getType() const = 0;
        virtual ptr<Expression> asExpression(const ExprContext &ctxt) const = 0;
        virtual ~SubClause() {}
        virtual string as_string() const { return "SubClause(UNKNOWN)"; }
    };

    class ExprSubClause : public SubClause
    {
      public:
        ExprSubClause(ptr<Expression> expr) :
            _type(EXPRESSION), _expr(expr)
        {
            // Make sure the clause is boolean.
            if (_expr && _expr->getType() != DataType::BOOL)
                Throw(aurora_typing_exception,
                      "Non-boolean where sub clause: " + to_string(_expr));
        }

        virtual ~ExprSubClause() {}
        virtual Type getType() const { return _type; }
        virtual ptr<Expression> asExpression(const ExprContext &ctxt) const
            { return _expr; }
        virtual string as_string() const { return ("ExprSubClause(" +
                                                   to_string(_expr) +
                                                   ")"); }
      private:
        Type _type;
        ptr<Expression> _expr;
    };


    class FieldEqSubClause : public SubClause
    {
      public:
        FieldEqSubClause(string table_name, string field_name,
                         ptr<Expression> value) :
            _type(FIELD_EQ_EXPRESSION), _table_name(table_name),
            _field_name(field_name), _value(value) {}
        virtual ~FieldEqSubClause() {}
        virtual Type getType() const { return _type; }
        virtual ptr<Expression> asExpression(const ExprContext &ctxt) const;
        string get_field_name() const { return _field_name; }
        ptr<Expression> get_field_value() const { return _value; }
        string as_string() const { return ("FieldEqSubClause(" + _field_name +
                                           "==" + to_string(_value) + ")");}

      private:
        Type _type;
        string _table_name, _field_name;
        ptr<Expression> _value;
    };

    class ConjunctionSubClause : public SubClause
    {
      public:
        ConjunctionSubClause(ptr<SubClause> l, ptr<SubClause> r) : _type(CONJUNCTION)
        {
            addClause(l);
            addClause(r);
        }

        virtual ~ConjunctionSubClause() {}
        virtual Type getType() const { return _type; }
        virtual ptr<Expression> asExpression(const ExprContext &ctxt) const;
        const vector<ptr<SubClause> > &get_sub_clauses() const { return _clauses; }
        virtual string as_string() const { return "ConjunctionSubClause(" +
                                               to_string(_clauses) + ")"; }
      private:
        void addClause(ptr<SubClause> c);
        Type _type;
        vector<ptr<SubClause> > _clauses;
    };

    SQLWhereClause(const ExprContext &ctxt, ptr<SubClause> sub_clause) :
        _ctxt(ctxt), _sub_clause(sub_clause) {}

    ptr<Expression> asExpression() const
    {
        return _sub_clause->asExpression(_ctxt);
    }

    ptr<SubClause> get_sub_clause() const { return _sub_clause; }

    // TODO: Better as_string method.
    string as_string() const { return ("WhereClause(" +
                                       to_string(_sub_clause) +
                                       ")"); }
  private:
    const ExprContext _ctxt; // For when we need to reexamine structures.
    ptr<SubClause> _sub_clause;
};

#endif                         // SQLUTIL_H
