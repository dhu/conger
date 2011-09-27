#ifndef TABLE_CURSORS_H
#define TABLE_CURSORS_H

#include "Table.h"

/// Cursor that takes a where clause, and scans a table for
/// matching tuples.
class TableScanCursor : public Table::Cursor
{
  public:
    static ptr<TableScanCursor> factory(Table *table,
                                        ptr<SQLWhereClause> wc);

    /// Returns true iff there are more rows..
    bool avail() const
    {
        return _avail;
    }

    /// Returns a pointer to the data of the current row.
    /// Valid only if avail().
    const char *row_data() const
    {
        ASSERT(avail());
        return (char *)_row.get_data();
    }

    /// Delete the current row from the database.
    /// Valid only if avail().
    virtual void delete_row() const
    {
        try
        {
            _table->delete_from_indexes(_key, _row);
            _dbc->del(0);
        } catch (DbException &e)
        {
            Throw(AuroraBadEntityException, string() +
                  "DbException while deleting row: "
                  + e.what());
        }
    }

    virtual void set_tuple(const char *tuple)
    {
        _ctxt.set_tuple(0, tuple);
    }

    virtual void reset()
    {
        try
        {
            int ret = _dbc->get(&_key, &_row, DB_FIRST);
            if (ret == DB_NOTFOUND)
            {
                // No rows.
                _avail = false;
            } else if (ret == 0)
            {
                _ctxt.set_tuple(1, (const char *)_row.get_data());
                if (!_where_clause ||
                    _where_clause->eval<bool>(_ctxt))
                    {
                    DEBUG << "Satisfied the where clause";
                    _avail = true;
                } else
                {
                    // Find the next key.
                    ++(*this);
                }
            } else
            {
                // Shouldn't happen.
                ASSERT(false);
            }
        } catch (DbException &e)
        {
            Throw(AuroraBadEntityException, string() +
                  "DbException while resetting cursor: "
                  + e.what());
        }
    }

    /// Move forward to the next valid row
    /// Set that row data as the index 1 duple in the eval context.
    virtual TableScanCursor& operator ++ ()
    {
        try
        {
            int ret;
            while ((ret = _dbc->get(&_key, &_row, DB_NEXT)) == 0)
            {
                // TODO: Check the length of the returned row.
                _ctxt.reset();
                _ctxt.set_tuple(1, (const char *)_row.get_data());
                // NULL _where_clause is true.
                if (!_where_clause ||
                    _where_clause->eval<bool>(_ctxt))
                    {
                    DEBUG << "Satisfied the where clause";
                    _avail = true;
                    return *this;
                }
            }
            if (ret == DB_NOTFOUND)
            {
                DEBUG << "No more results available in table";
                _avail = false;
            }
            return *this;
        } catch (DbException &e)
        {
            Throw(AuroraBadEntityException, string() +
                  "DbException while retrieving next result from table: "
                  + e.what());
        }
    }

    virtual string as_string() const
    {
        return "TableScanCursor(where=" +
            to_string(_where_clause) + ")" ;
    }

    // Reset will need to be called before this can be used.
    TableScanCursor(Table *table,
                    ptr<Expression> where_clause) :
        _table(table), _dbc(_table->get_db_cursor()),
        _where_clause(where_clause), _ctxt(2)
    {
    }

    /// Close the database cursor on destruction.
    /// TODO: Some kind of pool of these, maybe.
    virtual ~TableScanCursor()
    {
        _dbc->close();
    }

  private:
    Table *_table;
    Dbc *_dbc;
    ptr<Expression> _where_clause;
    EvalContext _ctxt;
    Dbt _key;
    Dbt _row;
    bool _avail;
};

/// Cursor that takes an expression and returns the value for the
/// matching key.
class KeyEqualCursor : public Table::Cursor
{
  public:
    static ptr<KeyEqualCursor> factory(Table *table,
                                       ptr<SQLWhereClause> wc);

    /// Returns true iff there are more rows..
    bool avail() const
    {
        return _avail;
    }

    /// Returns a pointer to the data of the current row.
    /// Valid only if avail().
    const char *row_data() const
    {
        ASSERT(avail());
        return (char *)_row.get_data();
    }

    /// Delete the current row from the database.
    /// Valid only if avail().
    virtual void delete_row() const
    {
        try
        {
            _table->delete_from_indexes(_key, _row);
            _dbc->del(0);
        } catch (DbException &e)
        {
            Throw(AuroraBadEntityException, string() +
                  "DbException while deleting row: "
                  + e.what());
        }
    }

    virtual void set_tuple(const char *tuple)
    {
        _ctxt.set_tuple(0, tuple);
    }

    virtual void reset()
    {
        try
        {
            Expression::eval_vector_into(_values, _key_buf, _ctxt);
            _key.set_data(_key_buf);
            _key.set_size(_key_size);

            int ret = _dbc->get(&_key, &_row, DB_SET);
            if (ret == DB_NOTFOUND)
            {
                // No rows.
                DEBUG << "No rows on reset for key "
                      << to_hex_string(_key_buf, _key_size);
                _avail = false;
            } else if (ret == 0)
            {
                // Set the tuple because broken things expect that.
                // TODO: Get rid of broken thigns.
                _ctxt.set_tuple(1, (const char *)_row.get_data());
                // TODO: Eval the rest of the where clause, if any.
                DEBUG << "Satisfied the where clause";
                _avail = true;
            } else
            {
                // Shouldn't happen.
                ASSERT(false);
            }
        } catch (DbException &e)
        {
            Throw(AuroraBadEntityException, string() +
                  "DbException while resetting cursor: "
                  + e.what());
        }
    }

    /// Move forward to the next valid row
    /// Set that row data as the index 1 duple in the eval context.
    virtual KeyEqualCursor& operator ++ ()
    {
        // Only ever one valid row.
        _avail = false;
        return *this;
    }

    virtual string as_string() const
    {
        return "KeyEqualCursor(values=" + to_string(_values) + ")";
    }

    // Reset will need to be called before this can be used.
    KeyEqualCursor(Table *table, size_t key_size,
                   ptr<Expression> value) :
        _table(table),
        _dbc(_table->get_db_cursor()), _values(1),
        _ctxt(2), _key_size(key_size)
    {
        _values[0] = value;
        _key_buf = new char[_key_size];
    }

    // Reset will need to be called before this can be used.
    KeyEqualCursor(Table *table, size_t key_size,
                   vector<ptr<Expression> > values) :
        _table(table), _dbc(_table->get_db_cursor()), _values(values),
        _ctxt(2), _key_size(key_size)
    {
        _key_buf = new char[_key_size];
    }

    /// Close the database cursor on destruction.
    /// TODO: Some kind of pool of these, maybe.
    virtual ~KeyEqualCursor()
    {
        _dbc->close();
        delete[] _key_buf;
    }

  private:
    Table *_table;
    Dbc *_dbc;
    vector<ptr<Expression> > _values;
    EvalContext _ctxt;
    size_t _key_size;
    char *_key_buf;
    Dbt _key;
    Dbt _row;
    bool _avail;
};

/// Cursor that takes an expression and returns the value for the
/// matching key in the index.
class IndexKeyEqualCursor : public Table::Cursor
{
  public:
    static ptr<IndexKeyEqualCursor> factory(Table *table,
                                            ptr<SQLWhereClause> wc);

    /// Returns true iff there are more rows..
    virtual bool avail() const
    {
        return _avail;
    }

    /// Returns a pointer to the data of the current row.
    /// Valid only if avail().
    const char *row_data() const
    {
        ASSERT(avail());
        return (char *)_row.get_data();
    }

    /// Delete the current row from the database.
    /// Valid only if avail().
    virtual void delete_row() const
    {
        try
        {
            _table->delete_from_indexes(_pkey, _row);
            _pdbc->del(0);
        } catch (DbException &e)
        {
            Throw(AuroraBadEntityException, string() +
                  "DbException while deleting row: "
                  + e.what());
        }
    }

    virtual void set_tuple(const char *tuple)
    {
        _ctxt.set_tuple(0, tuple);
    }

    virtual void reset()
    {
        try
        {
            _value->eval_into(_key_buf, _ctxt);
            _skey.set_data(_key_buf);
            _skey.set_size(_key_size);

            int ret = _sdbc->get(&_skey, &_pkey, DB_SET);
            if (ret == DB_NOTFOUND)
            {
                // No rows.
                DEBUG << "No rows on reset for secondary key "
                      << _skey;
                _avail = false;
            } else if (ret == 0)
            {
                get_primary_row();
                return;
            } else
            {
                // Shouldn't happen.
                ASSERT(false);
            }
        } catch (DbException &e)
        {
            Throw(AuroraBadEntityException, string() +
                  "DbException while resetting cursor: "
                  + e.what());
        }
    }

    /// Move forward to the next valid row
    /// Set that row data as the index 1 duple in the eval context.
    virtual IndexKeyEqualCursor& operator ++ ()
    {
        // TODO: There may be more rows, duplicate secondary keys should be allowed.
        // Only ever one valid row.
        int ret = _sdbc->get(&_skey, &_pkey, DB_NEXT_DUP);
        if (ret == DB_NOTFOUND)
        {
            // No more rows.
            _avail = false;
        } else if (ret == 0)
        {
            get_primary_row();
        } else
        {
            // Shouldn't happen.
            ASSERT(false);
        }
        return *this;
    }

    virtual string as_string() const
    {
        return "IndexKeyEqualCursor(value=" + to_string(_value) + ")";
    }

    // Reset will need to be called before this can be used.
    IndexKeyEqualCursor(Table *table, TableIndex *table_index,
                        size_t key_size, ptr<Expression> value) :
        _table(table), _table_index(table_index),
        _pdbc(_table->get_db_cursor()),
        _sdbc(_table_index->get_db_cursor()),
        _value(value), _ctxt(2), _key_size(key_size),
        _key_buf(new char[_key_size]), _avail(false)
    {}

    /// Close the database cursor on destruction.
    /// TODO: Some kind of pool of these, maybe.
    virtual ~IndexKeyEqualCursor()
    {
        _pdbc->close();
        _sdbc->close();
        delete[] _key_buf;
    }

  private:
    /// Once _pkey has been set, go fetch the actual row.
    void get_primary_row()
    {
        int ret = _pdbc->get(&_pkey, &_row, DB_SET);
        if (ret == DB_NOTFOUND)
        {
            // No rows. This should never happen.
            FATAL << "No rows for primary key " << _pkey;
        } else if (ret == 0)
        {
            // Set the tuple because broken things expect that.
            // TODO: Get rid of broken thigns.
            _ctxt.set_tuple(1, (const char *)_row.get_data());
            // TODO: Eval the rest of the where clause, if any.
            DEBUG << "Satisfied the where clause";
            _avail = true;
        } else
        {
            // Shouldn't happen.
            ASSERT(false);
        }
    }

    Table *_table;
    TableIndex *_table_index;
    Dbc *_pdbc;
    Dbc *_sdbc;
    ptr<Expression> _value;
    EvalContext _ctxt;
    size_t _key_size;
    char *_key_buf;
    Dbt _pkey;
    Dbt _skey;
    Dbt _row;
    bool _avail;
};

#endif
