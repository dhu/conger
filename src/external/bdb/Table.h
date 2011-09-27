#ifndef TABLE_H
#define TABLE_H

#include <db_cxx.h>
#include <string>

#include "Expression.h"
#include "SQLSelect.h"
#include "TupleDescription.h"
#include "xercesDomUtil.h"
#include "BoxParameter.h"
#include "Catalog.h"

BOREALIS_NAMESPACE_BEGIN;

class AuroraNode;
class SQLDelete;
class SQLSelect;
class SQLUpdate;
class Table;

/**
 * Things to think about in Tables:
 * - Do we need to use transactions to make sure updating multiple rows
 *   does the right thing?
 *
 * Things left to implement in tables, in rough priority of how they
 * might be done:
 * - Better handling for inconsistent on disk state in persistent
 *   tables.
 * - Noticing schema changes in persistent tables.
 * - Allowing schema changes without data loss, maybe.
 * - Replacing Sleepycat DB with Foldgers Crystals.
 *
 * And things to implement in queries and optimization, again in rough
 * priority:
 * - Where clauses that have additional clauses beyond those which
 *   match the query plans.
 * - Using prefixes of concatenated indexes.
 * - Updates that don't change keys don't need to delete/insert.
 * - Where clauses with ORs.
 * - Optimizing range operations (where key > m and key < n).
 * - Query optimization that chooses between indexes.
 */

class SelectQueryHandle
{
  public:
    virtual void do_query(const char *tuple) = 0;

    /// Returns true iff there is a row available.
    virtual bool avail() const = 0;

    /// Evalutate the select list against the current row, into
    /// the provided buffer, which must be large enough.
    virtual void eval_into(char *buf) = 0;

    /// Move to the next row.
    virtual SelectQueryHandle& operator ++ () = 0;

    virtual ~SelectQueryHandle() {}
};

class UpdateQueryHandle
{
  public:
    virtual void do_update(const char *tuple) = 0;
    virtual bool avail() const = 0;
    virtual void update_row() = 0;

    // TODO: Check to make sure this specification is acceptable.
    // Only valid before updateRow is called.
    virtual const char *old_row() const = 0;
    // Only valid after updateRow is called.
    virtual const char *new_row() const = 0;

    /// Update the next row.
    virtual UpdateQueryHandle& operator ++ () = 0;

    virtual void do_whole_update(const char *tuple);
    virtual ~UpdateQueryHandle() {}
};

class DeleteQueryHandle
{
 public:
    virtual void do_delete(const char* tuple) = 0;

    /// Returns true iff there is a row avaialble to delete.
    virtual bool avail() const = 0;

    /// Delete the row currently pointed to by the handle.
    virtual void delete_row() = 0;

    /// Returns a pointer to the last deleted row.
    /// Only valid after delete_row is called.
    virtual const char* deleted_row() = 0;

    /// Move on to the next row.
    virtual DeleteQueryHandle& operator ++ () = 0;

    virtual ~DeleteQueryHandle() {}
};

class ScanQueryHandle
{
  public:
    virtual void do_scan() = 0;

    /// Returns true iff there is a row available.
    virtual bool avail() const = 0;

    /// Returns a pointer to the row's data.
    virtual const char *data() const = 0;

    /// Move to the next row.
    virtual ScanQueryHandle& operator ++ () = 0;

    virtual ~ScanQueryHandle() {}
};

class DbHandle
{
  public:
    DbHandle(DbEnv *dbe, int flags) : _open(false), _db(new Db(dbe, flags)) {}
    ~DbHandle()
    {
        if (_open)
        {
            try
            {
                DEBUG << "Closing DbHandle";
                _db->close(0);
            } catch (DbException &e)
            {
                NOTICE << "Caught DbException: " << e.what();
            }
        }
        delete _db;
    }

    // TODO: This is lame. Get it right.
    void mark_open() { _open = true; }
    Db *operator->() const { return _db; }
    Db *get() const { return _db; }


  private:
    DbHandle(const DbHandle &);
    DbHandle &operator=(const DbHandle &);
    bool _open;
    Db *_db;
};

class TableEnvironment
{
  public:
    TableEnvironment(AuroraNode &node);
    ~TableEnvironment()
    {
        if (_open)
        {
            try
            {
                _db_env->close(0);
            } catch (DbException &e)
            {
                NOTICE << "Caught DbException: " << e.what();
            }
        }
        delete _db_env;
    }
    DbEnv *get_db_env() { return _db_env; }
    const DbEnv *get_db_env() const { return _db_env; }


  private:

    //static void db_err_callback(const char *errpfx, char *msg);  // bdb 42
    static void db_err_callback(const DbEnv  *env,                 // bdb 44
                                const char   *errpfx,
                                const char   *msg);

    TableEnvironment(const TableEnvironment &);
    TableEnvironment &operator=(const TableEnvironment &);
    AuroraNode &_node;
    bool _open; // If true, table needs to be closed.
    DbEnv *_db_env;
};

class TableIndex
{
  public:
    TableIndex(ptr<TableEnvironment> tenv, Table &table,
               string field_name);
    ~TableIndex() {}

    void insert_row(const Dbt &pkey, const Dbt &value);

    void delete_row(const Dbt &pkey, const Dbt &value);

    string as_string() const
    {
        return "TableIndex(field=" + to_string(*_field) + ")";
    }

    string get_field_name() const
    {
        return _field->get_name();
    }

    size_t get_key_length() const { return _field->get_size(); }

    Dbc *get_db_cursor();

  private:
    TableIndex(TableIndex &);
    TableIndex();
    TableIndex &operator=(TableIndex &);

    ptr<TableEnvironment> _tenv;
    Table &_table;
    DbHandle _db;
    // fix up
    //const TupleDescription::Field *_field;
    const SchemaField *_field;
    int _field_index;
};


class Table
{
  public:
    /// Creates or opens the table with the given name and schema.
    Table(string name, const TupleDescription schema,
          const DOMElement *element, ptr<TableEnvironment> tenv)
        throw( AuroraException );

    Table(CatalogTable* table, ptr<TableEnvironment> tenv)
        throw( AuroraException );

    Table(string name, TupleDescription schema, DBTYPE db_type,
          int db_open_flags, ptr<TableEnvironment> tenv);

    void  add_index(const string& index_field);

    /// Close the table.
    ~Table();

    string get_name() const { return _name; }

    //const TupleDescription getSchema() const { return _schema; }
    CatalogSchema* get_schema() const { return _schema; }

    void insert(const void *tuple);

    // TODO: This might want an ExprContext.
    // Query handle construction.
    ptr<SelectQueryHandle> compile_select(ptr<SQLSelect> select);
    ptr<UpdateQueryHandle> compile_update(ptr<SQLUpdate> update);
    ptr<DeleteQueryHandle> compile_delete(ptr<SQLDelete> delete_from);
    ptr<ScanQueryHandle> get_scan_query_handle();

    /// Write any cached data out to disk.
    void sync();

    // For queue tables
    unsigned int delete_from_head(unsigned int head);
    void insert_into_queue(const void *tuple) throw (AuroraException);

    unsigned int get_size() throw (AuroraException);

    string as_string() const
    {
        //return "Table{name=" + _name + "; schema=" + _schema.getName() + "}";
        return "Table{name=" + _name + "; schema=" + _schema->as_string() + "}";
    }

    // The following methods are used by the cursors. They
    // should probably be protected in some way or another.
    Dbc *get_db_cursor();
    const vector<string> &get_key_field_names() const
       { return _key_field_names; }
    vector<ptr<TableIndex> > &get_indexes() { return _indexes; }
    void insert_into_indexes(const Dbt &_key, const Dbt &_value);
    void delete_from_indexes(const Dbt &_key, const Dbt &_value);

  protected:
    void eval_key(char *buf, const char *row);

  private:
    string _name;
    // fix up -- rename to generic Parameter class not BoxParameter!
    //Params _params;
    BoxParameter _params;
    //TupleDescription _schema;
    CatalogSchema* _schema;
    ptr<TableEnvironment> _tenv;
    DbHandle _db;
    DBTYPE _db_type;
    int _db_open_flags;

    vector<string> _key_field_names;
    // fix up
    //vector<TupleDescription::Field> _key_fields;
    vector<const SchemaField*> _key_fields;
    int _key_length;
    char *_key_buffer;

    vector<ptr<TableIndex> > _indexes;

    // Working buffer for new rows, used by doUpdate.
    // TODO: This might be vestigial.
    char *_row_buffer;

    void init();

  public:
    // Abstract base class for cursors.
    // TODO: Make cursors not share evalcontexts.
    class Cursor
    {
      public:
        /// Set the input tuple in this cursors internal context. Must
        /// be called before reset, if the cursor is going to use data
        /// from the input tuple.
        virtual void set_tuple(const char *tuple) = 0;

        /// Point the cursor at the first valid row. This will need to
        /// be called before the cursor is valid after construction.
        virtual void reset() = 0;

        /// Return true iff row_data will return data for a valid row.
        virtual bool avail() const = 0;

        /// Returns a pointer to the data of the current row.
        /// Valid only if avail().
        virtual const char *row_data() const = 0;

        /// Delete the current row from the database.
        /// Valid only if avail().
        virtual void delete_row() const = 0;

        /// String representation of this cursor.
        virtual string as_string() const
        {
            return to_string(typeid(*this));
        }

        /// Move forward to the next row.
        virtual Cursor& operator ++ () = 0;

        virtual ~Cursor() {}
      private:
        /// Prohibit postfix increment.
        // org ??? Cursor operator ++ (int);
    };

  private:
    ptr<Cursor> cursor_factory(ptr<SQLWhereClause> wc);
};

BOREALIS_NAMESPACE_END;

static inline string to_string(const Dbt &dbt)
{
    return "Dbt(" + to_escaped_string(dbt.get_data(),
                                      dbt.get_size()) + ")";
}

#endif
