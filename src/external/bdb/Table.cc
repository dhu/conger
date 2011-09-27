#include "Table.h"
#include "TableCursors.h"
#include "AuroraNode.h"
#include "XmlTempString.h"
#include "SQLDelete.h"
#include "SQLSelect.h"
#include "SQLUpdate.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

BOREALIS_NAMESPACE_BEGIN

TableEnvironment::TableEnvironment(AuroraNode &node) :
    _node(node), _open(false)
{
    string table_dir;
    //ptr<ConfFile> props = _node.get_properties();
    //if (props)
    //    table_dir = props->get_parameter("server", "table-directory", "tables/");
    //else
    table_dir = "tables/";

    // Each Table object will have its own db environment, in its own directory.
    int ret = mkdir(table_dir.c_str(), 0777);
    if (ret == -1)
    {
        if (errno == EEXIST)
        {
            DEBUG << "Error making " << table_dir << ": " << strerror(errno);
        } else
        {
            FATAL << "Error making " << table_dir << ": " << strerror(errno);
        }
    }

    try
    {
        _db_env = new DbEnv(0);
        
        DEBUG << "Created BerkeleyDB environment version " <<
            DbEnv::version(NULL, NULL, NULL);

        // TODO: This all isn't compatible with a threaded scheduler.
        // No locking for now.
        // Default to 10 meg cache.
        u_int32_t cache_size = 10;
        //props->get_typed_parameter("server", "table-cache-size", cache_size);
        DEBUG << "Cache size: " << cache_size << " megabytes";
        _db_env->set_cachesize(0, (cache_size*1024*1024), 1);
        _db_env->set_errcall(&TableEnvironment::db_err_callback);
        _db_env->open(table_dir.c_str(),
                      DB_CREATE | DB_INIT_MPOOL, 0);
        _open = true;
    } catch (DbException &e)
    {
        Throw(AuroraBadEntityException, string() +
              "DbException while creating table environment: " +
              e.what());
    }
}

//void TableEnvironment::db_err_callback(const char *errpfx, char *msg)  // bdb 42
void TableEnvironment::db_err_callback(const DbEnv  *env,         // bdb 44
                                       const char   *errpfx,
                                       const char   *msg)
{
    if (errpfx)
        ERROR << "Db Error: " << errpfx << msg;
    else
        ERROR << "Db Error: " << msg;
}


TableIndex::TableIndex(ptr<TableEnvironment> tenv,
                       Table &table, string field_name) :
    _tenv(tenv), _table(table), _db(_tenv->get_db_env(), 0)
{
    TupleDescription schema(table.get_schema(), TupleDescription::NO_HEADER);
    _field_index = schema.index_of_field(field_name);
    if (_field_index == -1)
        Throw(AuroraBadEntityException,
              "Index field " + field_name +
              " doesn't exist in the schema of table " +
              to_string(table));
    _field = schema.get_schema_field(_field_index);
    // Open the database.
    try
    {
        string name(table.get_name() + "_index_" + field_name);
        DEBUG << "Opening database "  << name;
        _db->set_flags(DB_DUP);
        _db.mark_open();
        /// was: _db->open(name.c_str(), NULL, DB_BTREE, DB_CREATE, 0);
        _db->open(NULL, name.c_str(), NULL, DB_BTREE, DB_CREATE, 0);
    } catch (DbException &e)
    {
        Throw(AuroraBadEntityException,
              "DbException while creating index " +
              to_string(*this) + ": " + e.what());
    }
}

// TODO: Transactions.
void
TableIndex::insert_row(const Dbt &pkey, const Dbt &value)
{
    // As long as we are doing a single field, we can just fill skey
    // with a pointer into the value.

    // fix up
    CatalogSchema* schema = _table.get_schema();

    char *skey_data = (char *)value.get_data()
                    + schema->get_field_offset( _field_index );

    Dbt skey(skey_data, _field->get_size());
    DEBUG << "Inserting secondary key " << to_string(skey)
          << " for primary key " << to_string(pkey);
    try
    {
        _db->put(NULL, &skey, const_cast<Dbt *>(&pkey), 0);
    } catch (DbException &e)
    {
        Throw(AuroraBadEntityException,
              "DbException while inserting into index " +
              to_string(*this) + ": " + e.what());
    }
}

void
TableIndex::delete_row(const Dbt &pkey, const Dbt &value)
{
    Dbc *cursor = get_db_cursor();

    // fix up
    CatalogSchema* schema = _table.get_schema();

    char *skey_data = (char *)value.get_data()
                    + schema->get_field_offset( _field_index );

    Dbt skey( skey_data, _field->get_size() );

    int ret = cursor->get(&skey, const_cast<Dbt*>(&pkey), DB_GET_BOTH);
    if (ret == 0)
    {
        // Found the right row. Kill it.
        cursor->del(0);
    } else if (ret == DB_NOTFOUND)
    {
        ERROR << "Tried to delete secondary index pair ("
              << skey << ", " << pkey
              << ") but didn't find it.";
    } else
    {
        ASSERT(false);
    }

    cursor->close();
}

Dbc *TableIndex::get_db_cursor()
{
    Dbc *cursorp;
    try
    {
        _db->cursor(NULL, &cursorp, 0);
    } catch (DbException &e)
    {
        Throw(AuroraBadEntityException,
              "DbException while creating select cursor on index " +
              to_string(*this) + ": " + e.what());
    }
    return cursorp;
}

Table::Table( string                  name,
              const TupleDescription  schema,
              const DOMElement       *element,
              ptr<TableEnvironment>   tenv
            )
    throw( AuroraException )
    : _name(name),
      _params(element),
      _tenv(tenv),
      _db(_tenv->get_db_env(), 0),
      _db_open_flags(0)
{
    xml_expect_tag( element, "table" );

    // Create our own schema, without the header.
    const vector<SchemaField>* fields = schema.get_schema_fields();


    // fix -- replace with clone() and removeHeader()
    _schema = new CatalogSchema();
    _schema->set_schema_name( schema.get_name() + "-no-header" );

    copy( fields->begin(), fields->end(),
          back_inserter( _schema->get_schema_field() ));

    if (( fields->size() > 0 )  &&  ( (*fields)[0].get_offset() > 0 ))
    {   for ( uint32  i = 0; i < _schema->get_schema_field().size(); ++i )
        {  _schema->set_field_offset( i,
                        _schema->get_field_offset( i ) - HEADER_SIZE );

           //_schema->_field[i]._offset -= HEADER_SIZE;
        }
    }

    DEBUG << "Table " << _name << " schema = " << _schema->as_string();

    string key_param = _params.parameter("key", BoxParameter::REQUIRED);

    // TODO: More forgiving parsing.
    split(key_param, ',', _key_field_names);
    
    DEBUG << "Table keys " << key_param << " found "
        << _key_field_names.size() << " keys.";

    vector<string>::const_iterator i;
    _key_length = 0;
    for (i = _key_field_names.begin(); i != _key_field_names.end(); ++i)
    {
        int index = _schema->index_of_field(*i);
        if (index == -1)
            Throw(AuroraBadEntityException,
                  "Table " + _name + " missing field " + *i +
                  " referenced in key.");

        const SchemaField* f = _schema->get_schema_field(index);
        _key_fields.push_back(f);
        int size = f->get_size();
        if (size < 0)
            Throw(AuroraBadEntityException,
                  "Table " + _name + ": primary key field " + *i +
                  " has negative length.");
        _key_length += size;
    }

    if (_key_fields.empty())
        Throw(AuroraBadEntityException,
              "Table " + _name + " has a no primary key.");

    // Buffer for keys.
    _key_buffer = new char[_key_length];

    string type_param = _params.parameter("type", "btree");
    if (type_param == "btree")
        _db_type = DB_BTREE;
    else if (type_param == "hash")
        _db_type = DB_HASH;
    else
        Throw(AuroraBadEntityException,
              "Table " + _name + " has invalid type \"" + type_param + "\".");

    bool truncate(false), create(false);

    _params.typed_parameter("truncate", truncate);
    if (truncate)
    {
    DEBUG << "Setting truncate flag";
        _db_open_flags |= DB_TRUNCATE;
    }

    _params.typed_parameter("create", create);
    if (create)
    {
    DEBUG << "Setting create flag";
        _db_open_flags |= DB_CREATE;
    }

    init();

    DEBUG << "Table init complete.";

    // Create indexes.
    XmlTempString xindex("index");
    DOMNodeList *nodes = element->getElementsByTagName(xindex.to_xml());

    for (unsigned int i = 0; i < nodes->getLength(); ++i)
    {
        const DOMElement* index_element =
            static_cast<const DOMElement*>(nodes->item(i));

        string index_field =
            xml_attribute(index_element, "field", ATTR_NON_EMPTY);

        add_index(index_field);
    }

    DEBUG << "Secondary indexes: " << _indexes;
}



Table::Table(CatalogTable* table, ptr<TableEnvironment> tenv) 
    throw( AuroraException )
    : _name(table->get_table_name().as_string()),
      _tenv(tenv),
      _db(_tenv->get_db_env(), 0),
      _db_open_flags(0)
{
    _params = *(table->get_table_parameters());

    // Create our own schema, without the header.
    vector<SchemaField>& fields = table->get_table_schema()->get_schema_field();

    // fix -- replace with clone() and removeHeader()
    _schema = new CatalogSchema();
    _schema->set_schema_name( table->get_table_schema()->get_schema_name() + "-no-header" );

    copy( fields.begin(), fields.end(),
          back_inserter( _schema->get_schema_field() ));

    if (( fields.size() > 0 )  &&  ( fields[0].get_offset() > 0 ))
    {   for ( uint32  i = 0; i < _schema->get_schema_field().size(); ++i )
        {  _schema->set_field_offset( i,
                        _schema->get_field_offset( i ) - HEADER_SIZE );
        }
    }

    DEBUG << "Table " << _name << " schema = " << _schema->as_string();


    vector<Name>::iterator key_it = table->get_table_key()->begin();
    vector<Name>::iterator key_end = table->get_table_key()->end();

    _key_length = 0;
    for (; key_it != key_end; ++key_it)
    {
        int index = _schema->index_of_field(key_it->as_string());
        if (index == -1)
            Throw(AuroraBadEntityException,
                  "Table " + _name + " missing field " + key_it->as_string() +
                  " referenced in key.");

        const SchemaField* f = _schema->get_schema_field(index);
        _key_fields.push_back(f);
        int size = f->get_size();
        if (size < 0)
            Throw(AuroraBadEntityException,
                  "Table " + _name + ": primary key field " + key_it->as_string() +
                  " has negative length.");
        _key_length += size;
    }

    if (_key_fields.empty())
        Throw(AuroraBadEntityException,
              "Table " + _name + " has a no primary key.");
    // Buffer for keys.
    _key_buffer = new char[_key_length];

    string type_param = _params.parameter("type", "btree");
    if (type_param == "btree")
        _db_type = DB_BTREE;
    else if (type_param == "hash")
        _db_type = DB_HASH;
    else
        Throw(AuroraBadEntityException,
              "Table " + _name + " has invalid type \"" + type_param + "\".");


    bool truncate(false), create(false);

    _params.typed_parameter("truncate", truncate);
    if (truncate)
    {
    DEBUG << "Setting truncate flag";
        _db_open_flags |= DB_TRUNCATE;
    }

    _params.typed_parameter("create", create);
    if (create)
    {
    DEBUG << "Setting create flag";
        _db_open_flags |= DB_CREATE;
    }


    init();

    DEBUG << "Table init complete.";


    vector<Name>::iterator index_it = table->get_table_index()->begin();
    vector<Name>::iterator index_end = table->get_table_index()->end();

    for (; index_it != index_end; ++index_it)
        add_index(index_it->as_string());

}



Table::Table(string name, TupleDescription schema, DBTYPE db_type,
             int db_open_flags, ptr<TableEnvironment> tenv) :
    _name(name), _tenv(tenv),
    _db(_tenv->get_db_env(), 0), _db_type(db_type),
    _db_open_flags(db_open_flags),
    _key_length(0), _key_buffer(0)
{
    const vector<SchemaField>* fields = schema.get_schema_fields();

    _schema = new CatalogSchema();
    _schema->_name =  schema.get_name() + "-no-header";

    copy( fields->begin(), fields->end(),
          back_inserter( _schema->get_schema_field() ));

    if ( (fields->size() > 0) && ((*fields)[0].get_offset() > 0) )
    {   for (unsigned int i = 0; i < _schema->get_schema_field().size(); ++i)
        {  _schema->set_field_offset( i,
                        _schema->get_field_offset( i ) - HEADER_SIZE );

           //_schema->_field[i]._offset -= HEADER_SIZE;
        }
    }

    DEBUG << "Custom table " << _name << " schema = " << _schema;

    init();
}

void Table::add_index(const string& index_field)
{
    DEBUG << "Found index over field " << index_field;
    ptr<TableIndex> ti(new TableIndex(_tenv, *this, index_field));
    _indexes.push_back(ti);
}

void Table::init()
{
    // Buffer for rows during doUpdate;
    _row_buffer = new char[_schema->get_size()];

    try
    {
        // No name for now.
        if (_db_type == DB_QUEUE)
        {
            DEBUG << "Setting queue table record length to "
          << _schema->get_size();
            _db->set_re_len(_schema->get_size());
        }

        DEBUG << "Opening database "  << _name;
        _db.mark_open();
        //// was:  _db->open(_name.c_str(), NULL, _db_type, _db_open_flags, 0);
        _db->open( NULL, _name.c_str(), NULL, _db_type, _db_open_flags, 0);
        DBTYPE real_type;
        _db->get_type(&real_type);
        if (real_type != _db_type)
        {
        DEBUG << "Table type mismatch!";
            Throw(AuroraBadEntityException,
                  "Specified (or default) and actual database types do not match.");
    }
    } catch (DbException &e)
    {
    DEBUG << "DbException while creating table " + _name + ": " + e.what();
        Throw(AuroraBadEntityException,
              "DbException while creating table " + _name +
              ": " + e.what());
    }

    // TODO: Create some kind of metadata table, and check the schema
    // against it.
}

Table::~Table()
{
    DEBUG << "Closing " << *this;
    delete _schema;
    delete[] _row_buffer;
    delete[] _key_buffer;
}

Dbc *Table::get_db_cursor()
{
    Dbc *cursorp;
    try
    {
        _db->cursor(NULL, &cursorp, 0);
    } catch (DbException &e)
    {
        Throw(AuroraBadEntityException,
              "DbException while creating select cursor on table " + _name +
              ": " + e.what());
    }
    return cursorp;
}

// TODO: Make this more efficient for contiguous keys.
void Table::eval_key(char *buf, const char *row)
{
    char *pos = buf;

    vector<const SchemaField*>::const_iterator f;
    for ( f = _key_fields.begin(); f != _key_fields.end(); ++f)
    {   memcpy( pos, (row + (*f)->get_offset()), (*f)->get_size() );
        pos += (*f)->get_size();
    }
}


void Table::insert_into_indexes(const Dbt &key, const Dbt &value)
{
    vector<ptr<TableIndex> >::iterator i;
    for (i = _indexes.begin(); i != _indexes.end(); ++i)
        (*i)->insert_row(key, value);
}


void Table::insert(const void *row)
{
    eval_key(_key_buffer, (const char *)row);

    Dbt key(_key_buffer, _key_length);
    Dbt value(const_cast<void *>(row), _schema->get_size());

    TupleDescription td(_schema, TupleDescription::NO_HEADER);
    DEBUG << "Inserting " << td.tuple_data_to_string(row)
          << " with key " << key;
    try
    {
        _db->put(NULL, &key, &value, 0);
    } catch (DbException &e)
    {
        Throw(AuroraBadEntityException,
              "DbException while inserting into table " + _name +
              ": " + e.what());
    }
    insert_into_indexes(key, value);
}

void Table::insert_into_queue(const void *row) throw (AuroraException)
{
    TupleDescription td(_schema, TupleDescription::NO_HEADER);
    DEBUG << "Insert into queue: " << td.tuple_to_string(row);

    u_int32_t recno;
    Dbt key(&recno, 0);
    key.set_ulen(sizeof recno);
    key.set_flags(DB_DBT_USERMEM);

    Dbt value(const_cast<void*>(row), _schema->get_size());

    try
    {
        _db->put(NULL, &key, &value, DB_APPEND);
        DEBUG << "Recno returned: " << recno;
        insert_into_indexes(key, value);
    } catch (DbException &e)
    {
        DEBUG << "Doh!";
        Throw(AuroraBadEntityException,
              "DbException while inserting into table " + _name +
              ": " + e.what());
    }
}

unsigned int Table::get_size() throw (AuroraException)
{
//    if (_db_type != DB_QUEUE)
//        Throw(AuroraException, "Table::getSize() currently supported only for queue tables");

    if ( _db_type == DB_QUEUE )
    {
        DB_QUEUE_STAT *stat;

        try
        {
            //_db->stat(&stat, 0);       // bdb 42
            _db->stat(NULL, &stat, 0);   // bdb 44

            DEBUG << "Current table size: " << stat->qs_ndata;
            return stat->qs_ndata;
        }
        catch (DbException& e)
        {
            Throw(AuroraException, "Unable to determine table size: " + string(e.what()));
        }
    }

    else if ( _db_type == DB_BTREE )
    {
        DB_BTREE_STAT *stat;

        try
        {
            //_db->stat(&stat, 0);       // bdb 42
            _db->stat(NULL, &stat, 0);   // bdb 44

            DEBUG << "Current table size: " << stat->bt_ndata;
            return stat->bt_ndata;
        }
        catch (DbException& e)
        {
            Throw(AuroraException, "Unable to determine table size: " + string(e.what()));
        }
    }

    else if ( _db_type == DB_HASH )
    { 
        DB_HASH_STAT *stat;

        try
        {
            //_db->stat(&stat, 0);       // bdb 42
            _db->stat(NULL, &stat, 0);   // bdb 44

            DEBUG << "Current table size: " << stat->hash_ndata;
            return stat->hash_ndata;
        }
        catch (DbException& e)
        {
            Throw(AuroraException, "Unable to determine table size: " + string(e.what()));
        }
    }
    else 
        Throw(AuroraException, "Table::getSize() unknown table type.");

}

unsigned int Table::delete_from_head(unsigned int count)
{
    DEBUG << "Consuming " << count << " records from database";

    unsigned int deleted = 0;

    TupleDescription td(_schema, TupleDescription::NO_HEADER);

    try
    {
        Dbt key, value;
        while (deleted != count)
        {
            _db->get(0, &key, &value, DB_CONSUME);
            delete_from_indexes(key, value);
            DEBUG << " - consumed " << td.tuple_to_string(value.get_data());
            ++deleted;
        }
    } catch (DbException& e)
    {
        WARN << "DB_CONSUME failed: " << e.what();
    }
    DEBUG << "Consumed " << deleted;
    return deleted;
}

void Table::delete_from_indexes(const Dbt &key, const Dbt &value)
{
    DEBUG << "Clearing row " << value << " from indexes.";
    vector<ptr<TableIndex> >::iterator i;
    for (i = _indexes.begin(); i != _indexes.end(); ++i)
    {
        (*i)->delete_row(key, value);
    }
}

// Just scan the table using a cursor.
class cursor_scan : public ScanQueryHandle
{
  public:
    cursor_scan(ptr<Table::Cursor> cursor) :
        _cursor(cursor)
    {}

    virtual void do_scan()
    {
        _cursor->reset();
    }

    /// Returns true iff there are more results.
    virtual bool avail() const
    {
        return _cursor->avail();
    }

    virtual const char *data() const
    {
        ASSERT(avail());
        return _cursor->row_data();
    }

    /// Consumes a tuple.
    virtual cursor_scan& operator ++ ()
    {
        ++(*_cursor);
        return *this;
    }

    virtual ~cursor_scan() {}

  private:
    ptr<Table::Cursor> _cursor;
};

// Do a basic select using a cursor.
class cursor_select : public SelectQueryHandle
{
  public:
    // The eval context passed in must be the same one that the cursor
    // is using.
    // TODO: Consider if this is a good interface, redesign.
    cursor_select(ptr<Table::Cursor> cursor, ptr<SQLSelect> select) :
        _cursor(cursor),  _select(select), _ctxt(2)
    {}

    virtual void do_query(const char *tuple)
    {
        // Input tuple always goes in position 0.
        _ctxt.set_tuple(0, tuple);
        _cursor->set_tuple(tuple);
        _cursor->reset();
    }

    /// Returns true iff there are more results.
    virtual bool avail() const
    {
        return _cursor->avail();
    }

    /// Evalutate the select list against the current row, into
    /// the provided buffer, which must be large enough.
    virtual void eval_into(char *buf)
    {
        ASSERT(avail());
        /// Row should already be in the EvalContext in the
        /// appropriate position.
        _ctxt.reset();
        _ctxt.set_tuple(1, _cursor->row_data());
        _select->eval_into(buf, _ctxt);
    }

    /// Consumes a tuple.
    virtual cursor_select& operator ++ ()
    {
        ++(*_cursor);
        return *this;
    }

    virtual ~cursor_select() {}


  private:
    ptr<Table::Cursor> _cursor;
    ptr<SQLSelect> _select;
    EvalContext _ctxt;
};

ptr<Table::Cursor> Table::cursor_factory(ptr<SQLWhereClause> wc)
{
    ptr<Table::Cursor> ret;
    if (!wc)
    {
        // Empty where clause
        ret = TableScanCursor::factory(this, wc);
    }
    if (!ret) ret = KeyEqualCursor::factory(this, wc);
    if (!ret) ret = IndexKeyEqualCursor::factory(this, wc);
    if (!ret) ret = TableScanCursor::factory(this, wc);

    DEBUG << "cursorFactory for " << wc
          << " returns " << ret;
    return ret;
}

ptr<SelectQueryHandle> Table::compile_select(ptr<SQLSelect> select)
{
    ptr<Cursor> cursor = cursor_factory(select->get_where_clause());
    DEBUG << "compileSelect got cursor " << cursor;
    return ptr<SelectQueryHandle>(new cursor_select(cursor, select));
}

void UpdateQueryHandle::do_whole_update(const char *tuple)
{
    do_update(tuple);
    while (avail())
    {
        update_row();
        ++(*this);
    }
}

// Do a basic update using a cursor.
class cursor_update : public UpdateQueryHandle
{
  public:
    // The eval context passed in must be the same one that the cursor
    // is using.
    // TODO: Consider if this is a good interface, redesign.
    cursor_update(Table *table, ptr<Table::Cursor> cursor,
                 ptr<SQLUpdate> update) :
        _table(table), _cursor(cursor),  _update(update), _ctxt(2)
    {
        _row_buffer = new char[_table->get_schema()->get_size()];
    }

    virtual void do_update(const char *tuple)
    {
        // Input tuple always goes in position 0.
        _ctxt.set_tuple(0, tuple);
        _cursor->set_tuple(tuple);
        _cursor->reset();
        _updated_row = false;
    }

    virtual bool avail() const
    {
        return _cursor->avail();
    }

    virtual void update_row()
    {
        _ctxt.set_tuple(1, _cursor->row_data());
        _ctxt.reset();

        _update->eval_new_row(_row_buffer, _ctxt);
        DEBUG << "Delete old row.";
        _cursor->delete_row();
        DEBUG << "Adding new row.";
        _table->insert(_row_buffer);
        _updated_row = true;
    }

    // Only valid before updateRow is called.
    virtual const char *old_row() const
    {
        ASSERT(!_updated_row);
        return _cursor->row_data();
    }

    // Only valid after updateRow is called.
    virtual const char *new_row() const
    {
        ASSERT(_updated_row);
        return _row_buffer;
    }

    virtual cursor_update& operator ++ ()
    {
        ++(*_cursor);
        _updated_row = false;
        return *this;
    }

    virtual ~cursor_update() { delete[] _row_buffer; }
  private:
    Table *_table;
    ptr<Table::Cursor> _cursor;
    ptr<SQLUpdate> _update;
    EvalContext _ctxt;
    char *_row_buffer;
    bool _updated_row;
};

ptr<UpdateQueryHandle> Table::compile_update(ptr<SQLUpdate> update)
{
    ptr<Cursor> cursor = cursor_factory(update->get_where_clause());
    DEBUG << "compile_update got cursor " << cursor;
    return ptr<UpdateQueryHandle>(new cursor_update(this, cursor, update));
}


// Do a basic delete using a cursor.
class cursor_delete : public DeleteQueryHandle
{
 public:
    cursor_delete(Table* table, ptr<Table::Cursor> cursor, ptr<SQLDelete> del)
        :  _table(table), _cursor(cursor), _del(del)
    {
        _row_size = _table->get_schema()->get_size();
        _row_buffer = new char[_row_size];
    }

    virtual void do_delete(const char* tuple)
    {
        _cursor->set_tuple(tuple);
        _cursor->reset();
        _deleted_row = false;
    }

    virtual bool avail() const { return _cursor->avail(); }

    virtual void delete_row()
    {
        DEBUG << "Delete old row.";
        memcpy(_row_buffer, _cursor->row_data(), _row_size);
        _cursor->delete_row();
        _deleted_row = true;
    }

    // Only valid after delete_row is called.
    virtual const char* deleted_row()
    {
        assert(_deleted_row);
        return _row_buffer;
    }

    virtual cursor_delete& operator ++ ()
    {
        ++(*_cursor);
        _deleted_row = false;
        return *this;
    }

    virtual ~cursor_delete() { delete[] _row_buffer; }

 private:
    Table* _table;
    ptr<Table::Cursor> _cursor;
    ptr<SQLDelete> _del;
    bool _deleted_row;
    char* _row_buffer;
    size_t _row_size;
};

ptr<DeleteQueryHandle> Table::compile_delete(ptr<SQLDelete> delete_from)
{
    ptr<Cursor> cursor = cursor_factory(delete_from->get_where_clause());
    DEBUG << "compile_delete got cursor " << cursor;
    return ptr<DeleteQueryHandle>(new cursor_delete(this, cursor, delete_from));
}


ptr<ScanQueryHandle> Table::get_scan_query_handle()
{
    ptr<Cursor> cursor = cursor_factory(ptr<SQLWhereClause>());
    return ptr<ScanQueryHandle>(new cursor_scan(cursor));
}


void Table::sync()
{
    DEBUG << "Syncing " << *this << " to disk.";
    try
    {
        _db->sync(0);
    } catch (DbException &e)
    {
        Throw(AuroraBadEntityException,
              "DbException while syncing table " + _name +
              ": " + e.what());
    }
}


BOREALIS_NAMESPACE_END
