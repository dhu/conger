#include "SQLUtil.h"
#include "ExprUtil.h"

ptr<Expression>
SQLWhereClause::FieldEqSubClause::asExpression(const ExprContext &ctxt) const
{
    return relPromote<EQExpressions>
        (Expression::makeFieldExpression(_table_name, _field_name, ctxt),
         _value);
}

void SQLWhereClause::ConjunctionSubClause::addClause(ptr<SubClause> c)
{
    if (c->getType() == CONJUNCTION)
    {
        // If it is another conjunction, add its subclauses.
        const vector<ptr<SubClause> > &cs =
            c.dynamic_cast_to<ConjunctionSubClause>()->get_sub_clauses();
        DEBUG << "Copying in clauses from other conjunction: " << cs;
        copy(cs.begin(), cs.end(), back_inserter(_clauses));
    } else
    {
        _clauses.push_back(c);
    }
}

ptr<Expression>
SQLWhereClause::ConjunctionSubClause::asExpression(const ExprContext &ctxt) const
{
    vector<ptr<SubClause> >::const_iterator i;
    ptr<Expression> ret(new ConstantExpression<bool>(true));
    for (i = _clauses.begin(); i != _clauses.end(); ++i)
    {
        ret = ptr<Expression>(new AndExpression<bool>(NArgs((*i)->asExpression(ctxt), ret)));
    }
    return ret;
}
