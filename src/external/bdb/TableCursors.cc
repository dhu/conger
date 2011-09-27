#include "TableCursors.h"


ptr<TableScanCursor>
TableScanCursor::factory(Table *table,
                         ptr<SQLWhereClause> wc)
{
    DEBUG << "Creating TableScanCursor";
    TableScanCursor *tcs;
    if (wc)
        tcs = new TableScanCursor(table, wc->asExpression());
    else
        // Empty where clause.
        tcs = new TableScanCursor(table, ptr<Expression>());
    return ptr<TableScanCursor>(tcs);
}


ptr<KeyEqualCursor>
KeyEqualCursor::factory(Table *table,
                        ptr<SQLWhereClause> wc)
{
    typedef SQLWhereClause::SubClause SubClause;
    typedef SQLWhereClause::FieldEqSubClause FieldEqSubClause;
    typedef SQLWhereClause::ConjunctionSubClause ConjunctionSubClause;

    KeyEqualCursor *ret = NULL;

    const vector<string> &key_field_names = table->get_key_field_names();

    ptr <SubClause> s = wc->get_sub_clause();
    if (s->getType() == SQLWhereClause::FIELD_EQ_EXPRESSION)
    {
        ptr<FieldEqSubClause> fes = s.dynamic_cast_to<FieldEqSubClause>();
        if (key_field_names.size() == 1 &&
            fes->get_field_name() == key_field_names[0])
            {
            DEBUG << "Found an opportunity for a KeyEqual cursor on single field.";
            ret = new KeyEqualCursor(table,
                                     fes->get_field_value()->getLength(),
                                     fes->get_field_value());
        }
    } else if (ptr<ConjunctionSubClause> cs =
               s.dynamic_cast_to<ConjunctionSubClause>())
               {
        DEBUG << "Handling Conjuction: " << to_string(s);

        // If this conjunction is all eq expressions, and matches the
        // size of the primary key, then we can use an EQ cursor.
        const vector<ptr<SubClause> > &clauses = cs->get_sub_clauses();
        if (key_field_names.size() == clauses.size())
        {
            vector<ptr<Expression> > values(key_field_names.size());
            size_t key_length(0);
            bool success = true;
            vector<ptr<SubClause> >::const_iterator c;
            for (c = clauses.begin(); c != clauses.end(); ++c)
            {
                ptr<SubClause> clause = *c;
                DEBUG << "Dealing with subclause " << clause;
                if (clause->getType() == SQLWhereClause::FIELD_EQ_EXPRESSION)
                {
                    ptr<FieldEqSubClause> fes = clause.dynamic_cast_to<FieldEqSubClause>();
                    key_length += fes->get_field_value()->getLength();
                    DEBUG << "Looking for field " << fes->get_field_name();
                    for (unsigned int i = 0; i < key_field_names.size(); ++i)
                    {
                        DEBUG << "Looking at field name(" << i
                              << ") " << key_field_names[i];
                        if (fes->get_field_name() == key_field_names[i])
                        {
                            DEBUG << "Field " << fes->get_field_name()
                                  << " found at key postition " << i;
                            if (values[i])
                            {
                                DEBUG << "Attempt to double set value for field "
                                      << key_field_names[i] << " already set to "
                                      << values[i];
                                success = false;
                            }
                            values[i] = fes->get_field_value();
                        }
                    }
                } else
                {
                    DEBUG << "Not FieldEqSubclause, can't use a KeyEqualCursor.";
                    success = false;
                }
            }
            // Make sure we filled in all of values.
            for (unsigned int i = 0; i < values.size(); ++i)
            {
                if (!values[i])
                {
                    DEBUG << "Missing value for field "
                          << key_field_names[i];
                    success = false;
                }
            }

            if (success)
            {
                DEBUG << "Successfully got concat key. "
                      << "Key names: " << key_field_names
                      << "Key Values: " << values;
                ret = new KeyEqualCursor(table, key_length, values);
            }
        } else
        {
            DEBUG << "Key expression sizes don't match.";
        }
    }

    if (ret)
    {
        DEBUG << "Returning " << ret;
        return ptr<KeyEqualCursor>(ret);
    } else
    {
        DEBUG << "No KeyEqualCursor.";
        return ptr<KeyEqualCursor>();
    }
}

ptr<IndexKeyEqualCursor>
IndexKeyEqualCursor::factory(Table *table,
                             ptr<SQLWhereClause> wc)
{
    typedef SQLWhereClause::SubClause SubClause;
    typedef SQLWhereClause::FieldEqSubClause FieldEqSubClause;

    DEBUG << "Trying IndexKeyEqualCursor::factory";

    ptr <SubClause> s = wc->get_sub_clause();
    if (ptr<FieldEqSubClause> fes =
        s.dynamic_cast_to<FieldEqSubClause>())
        {
        DEBUG << "Checking TableIndexes.";
        vector<ptr<TableIndex> > &indexes = table->get_indexes();
        vector<ptr<TableIndex> >::iterator i;
        for (i = indexes.begin(); i != indexes.end(); ++i)
        {
            TableIndex &ti = **i;
            if (ti.get_field_name() == fes->get_field_name())
            {
                DEBUG << "Found opportunity for a IndexKeyEqual cursor.";
                ASSERT((int)ti.get_key_length() == fes->get_field_value()->getLength());
                return ptr<IndexKeyEqualCursor>
                    (new IndexKeyEqualCursor(table, &ti,
                                             ti.get_key_length(),
                                             fes->get_field_value()));
            }
        }
        DEBUG << "Doesn't match any TableIndexes.";
    } else
    {
        DEBUG << s << " is not a FieldEqSubClause, no IndexKeyEqualCursor.";
    }
    return ptr<IndexKeyEqualCursor>();
}
