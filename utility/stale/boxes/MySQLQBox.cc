#include "QBox.h"
#include "Timestamp.h"
#include "Expression.h"

#include <mysql.h>

inline std::string to_string(enum_field_types t)
{
    switch (t)
    {
      case MYSQL_TYPE_DECIMAL: return "MYSQL_TYPE_DECIMAL";
      case MYSQL_TYPE_TINY: return "MYSQL_TYPE_TINY";
      case MYSQL_TYPE_SHORT: return "MYSQL_TYPE_SHORT";
      case MYSQL_TYPE_LONG: return "MYSQL_TYPE_LONG";
      case MYSQL_TYPE_FLOAT: return "MYSQL_TYPE_FLOAT";
      case MYSQL_TYPE_DOUBLE: return "MYSQL_TYPE_DOUBLE";
      case MYSQL_TYPE_NULL: return "MYSQL_TYPE_NULL";
      case MYSQL_TYPE_TIMESTAMP: return "MYSQL_TYPE_TIMESTAMP";
      case MYSQL_TYPE_LONGLONG: return "MYSQL_TYPE_LONGLONG";
      case MYSQL_TYPE_INT24: return "MYSQL_TYPE_INT24";
      case MYSQL_TYPE_DATE: return "MYSQL_TYPE_DATE";
      case MYSQL_TYPE_TIME: return "MYSQL_TYPE_TIME";
      case MYSQL_TYPE_DATETIME: return "MYSQL_TYPE_DATETIME";
      case MYSQL_TYPE_YEAR: return "MYSQL_TYPE_YEAR";
      case MYSQL_TYPE_NEWDATE: return "MYSQL_TYPE_NEWDATE";
      case MYSQL_TYPE_ENUM: return "MYSQL_TYPE_ENUM";
      case MYSQL_TYPE_SET: return "MYSQL_TYPE_SET";
      case MYSQL_TYPE_TINY_BLOB: return "MYSQL_TYPE_TINY_BLOB";
      case MYSQL_TYPE_MEDIUM_BLOB: return "MYSQL_TYPE_MEDIUM_BLOB";
      case MYSQL_TYPE_LONG_BLOB: return "MYSQL_TYPE_LONG_BLOB";
      case MYSQL_TYPE_BLOB: return "MYSQL_TYPE_BLOB";
      case MYSQL_TYPE_VAR_STRING: return "MYSQL_TYPE_VAR_STRING";
      case MYSQL_TYPE_STRING: return "MYSQL_TYPE_STRING";
      case MYSQL_TYPE_GEOMETRY: return "MYSQL_TYPE_GEOMETRY";
      default:
        return "Unknown MYSQL_TYPE";
    }
}

BOREALIS_NAMESPACE_BEGIN;

class MySQLQBox : public QBox
{
  private:
    // Class for a mysql bound argument/result. This thing doesn't
    // have a virtual destructor, so one has to be a bit careful with
    // it.
    struct Bind : public MYSQL_BIND
    {
        Bind()
        {
            buffer = 0;
            is_null = 0;
            length = 0;
        }
        void setup(Expression *e)
        {
            setup(MySQLQBox::get_buffer_length_for_expression(e));
            buffer_type = MySQLQBox::translate_to_my_sqltype(e->getType());
        }

        void setup(MYSQL_FIELD *mf)
        {
            setup(MySQLQBox::get_buffer_length_for_my_sqlfield(mf));
            buffer_type = mf->type;
        }

        void setup(uint32 buffer_length)
        {
            DEBUG << "Setting up Bind with buffer_length=" << buffer_length;
            this->buffer_length = buffer_length;
            buffer = (char *)malloc(buffer_length);
            is_null = new my_bool;
            length = new uint32;
        }

        bool isNull() const
        {
            return *is_null;
        }

        uint32 getLength() const
        {
            return *length;
        }

        ~Bind()
        {
            if (buffer) free(buffer);
            delete is_null;
            delete length;
        }
    };

    MYSQL *_mysql;
    MYSQL_STMT *_statement;
    string  _query;

    uint32  _param_count;
    Bind   *_parameters;
    vector<ptr<Expression> > _param_exprs;

    EvalContext _ctxt;

    MYSQL_RES *_prepared_result;
    bool _has_results;
    uint32   _result_field_count;
    Bind    *_results;

    TupleDescription _output_description;
    //CatalogSchema   _output_description;

    //static TupleDescription::Field translateFromMySQLField(MYSQL_FIELD *mf)
    static SchemaField translate_from_my_sqlfield(MYSQL_FIELD *mf)
        throw (aurora_typing_exception);
    static enum enum_field_types translate_to_my_sqltype(DataType type)
        throw (aurora_typing_exception);

    // Create buffers with malloc.
    static uint32  get_buffer_length_for_expression(Expression *e)
        throw (aurora_typing_exception);

    static uint32 get_buffer_length_for_my_sqlfield(MYSQL_FIELD *mf)
        throw (aurora_typing_exception);

    void connect() throw (AuroraException);
    void setup_query_parameters() throw (AuroraException);
    void setup_output() throw (AuroraException);

  protected:
    void setup_impl() throw (AuroraException);
    void init_impl() throw (AuroraException);
    void run_impl(QBoxInvocation&) throw (AuroraException);

  public:
    MySQLQBox() : _mysql(new MYSQL), _statement(NULL),
                  _param_count(0), _parameters(NULL),
                  _prepared_result(NULL), _has_results(false),
                  _result_field_count(0), _results(NULL)
    {}
    virtual ~MySQLQBox();

    AURORA_DECLARE_QBOX(MySQLQBox, "mysql");
};


MySQLQBox::~MySQLQBox()
{
    DEBUG << "Started ~MySQLQBox";
    // This will free the statement as well.
    //    mysql_close(_mysql);
    delete _mysql;
    DEBUG << "Deleted _mysql";

    delete[] _parameters;
    DEBUG << "Deleted parameters.";

    //    if (_prepared_result)
    //    mysql_free_result(_prepared_result);

    delete[] _results;
    DEBUG << "Deleted results.";
}

//TupleDescription::Field
SchemaField  MySQLQBox::translate_from_my_sqlfield(MYSQL_FIELD *mf)
    throw (aurora_typing_exception)
{
    string name(mf->name, mf->name_length);
    uint32 length;
    DataType type;

    switch (mf->type)
    {
      case MYSQL_TYPE_LONG:
        type = DataType::INT;
        length = sizeof( int32 );
        break;
      case MYSQL_TYPE_LONGLONG:
        type = DataType::LONG;
        length = sizeof( int64 );
        break;
      case MYSQL_TYPE_FLOAT:
        type = DataType::SINGLE;
        length = sizeof( single );
        break;
      case MYSQL_TYPE_DOUBLE:
        type = DataType::DOUBLE;
        length = sizeof( double );
        break;
      default:
        Throw(aurora_typing_exception,
              "Don't know how to translate MySQL type "
              + to_string(mf->type) + " into a DataType.");
    }
    return SchemaField( name, type, length );
    //return TupleDescription::Field(name, type, length);
}

enum enum_field_types
MySQLQBox::translate_to_my_sqltype(DataType type)
        throw (aurora_typing_exception)
{
    if (type == DataType::INT)
        return MYSQL_TYPE_LONG;
    else if (type == DataType::LONG)
        return MYSQL_TYPE_LONGLONG;
    else if (type == DataType::SINGLE)
        return MYSQL_TYPE_FLOAT;
    else if (type == DataType::DOUBLE)
        return MYSQL_TYPE_DOUBLE;
    else
        Throw(aurora_typing_exception,
              "Don't know how to translate "
              + to_string(type) + " into a MySQL type.");
}

uint32 MySQLQBox::get_buffer_length_for_expression(Expression *e)
    throw (aurora_typing_exception)
{
    uint32  buffer_length;
    DEBUG << "createBufferForExpression(" << e << ")";
    DataType type = e->getType();

    if (type == DataType::INT)
    {
        buffer_length = sizeof(int32);
    } else if (type == DataType::LONG)
    {
        buffer_length = sizeof(int64);
    } else if (type == DataType::SINGLE)
    {
        buffer_length = sizeof(single);
    } else if (type == DataType::DOUBLE)
    {
        buffer_length = sizeof(double);
    } else if (type == DataType::STRING)
    {
        int32 length = e->getLength();

        if (buffer_length < 0)
            Throw(aurora_typing_exception,
                  "Can't create a variable length string buffer.");
        else
            buffer_length = (uint32) length;
    } else
    {
        Throw(aurora_typing_exception,
              "Don't know how find the length for a buffer of type "
              + to_string(type) + ".");
    }
    DEBUG << "Buffer length: " << buffer_length;
    return buffer_length;
}

uint32  MySQLQBox::get_buffer_length_for_my_sqlfield(MYSQL_FIELD *mf)
    throw (aurora_typing_exception)
{
    uint32  buffer_length;

    switch (mf->type)
    {
      case MYSQL_TYPE_LONG:
        buffer_length = sizeof(int32);
        break;
      case MYSQL_TYPE_LONGLONG:
        buffer_length = sizeof(int64);
        break;
      case MYSQL_TYPE_FLOAT:
        buffer_length = sizeof(single);
        break;
      case MYSQL_TYPE_DOUBLE:
        buffer_length = sizeof(double);
        break;
      case MYSQL_TYPE_STRING:
      case MYSQL_TYPE_VAR_STRING:
      case MYSQL_TYPE_TINY_BLOB:
      case MYSQL_TYPE_BLOB:
      case MYSQL_TYPE_MEDIUM_BLOB:
      case MYSQL_TYPE_LONG_BLOB:
        // TODO: Make sure this is really correct for all BLOBs and such.
        buffer_length = mf->length;
        break;
      default:
        Throw(aurora_typing_exception, string() +
              "Don't know how to find the length for a buffer of MYSQL type "
              + to_string(mf->type) + ".");
    }
    return buffer_length;
}

void MySQLQBox::connect() throw (AuroraException)
{
    if (!mysql_init(_mysql))
        Throw(AuroraBadEntityException, string() +
              "Couldn't initialize MySQL object: " +
              mysql_error(_mysql));
    DEBUG << "Initialized.";

    if (!mysql_real_connect(_mysql, "localhost", "", "", "test", 0, NULL, 0))
        Throw(AuroraBadEntityException, string() +
              "Couldn't connect to MySQL database: " +
              mysql_error(_mysql));
    DEBUG << "Connected.";
}

void MySQLQBox::setup_query_parameters()
    throw (AuroraException)
{
    ASSERT(_statement);
    ExprContext ctxt;
    //ctxt.setTupleDescription(getInputDescription(0));
    ctxt.set_context_schema( *get_in_schema( 0 ));
    int32 pc = 0;

    while (true)
    {
        string expr = param("parameter." + to_string(pc));
        DEBUG << "Parameter " << pc << ": " << expr;

        if (expr == "")
        {    break;
        }

        _param_exprs.push_back(Expression::parse(expr, ctxt));
        ++pc;
    }
    _param_count = mysql_stmt_param_count(_statement);

    if (_param_exprs.size() != _param_count)
        Throw(aurora_typing_exception, string() +
              "Provided " + to_string(_param_exprs.size()) +
              " parameters, but query wants " +
              to_string(_param_count));

    DEBUG << "Setting up " << _param_count << " parameters.";

    _parameters = new Bind[_param_count];
    DEBUG << "Allocated parameters.";

    // Fill the structure.
    for (uint32 i = 0; i < _param_count; ++i)
    {
        ptr<Expression> e = _param_exprs[i];
        DEBUG << "Doing parameter number " << i << " expression: " << e;
        _parameters[i].setup(e.get());
    }
    if (mysql_stmt_bind_param(_statement, _parameters))
        Throw(aurora_typing_exception, string() +
              "Unable to bind parameters to query: " +
              mysql_stmt_error(_statement));
    DEBUG << "Bound statement to parameters.";
}

void MySQLQBox::setup_output()
    throw (AuroraException)
{
    // Modified by Magda to go with more recent MySQL API
    // MYSQL_RES *_prepared_result = mysql_prepare_result(_statement);
    MYSQL_RES *_prepared_result = mysql_stmt_result_metadata(_statement);
    if (!_prepared_result)
    {
        _result_field_count = 0;
        DEBUG << "No query results.";
        _has_results = false;
        return;
    }
    _has_results = true;

    _result_field_count = mysql_num_fields(_prepared_result);

    DEBUG << "Setting up query results for "
          << _result_field_count << " fields.";

    //vector<TupleDescription::Field> result_fields;
    vector<SchemaField> result_fields;
    _results = new Bind[_result_field_count];

    for (uint32 i = 0; i < _result_field_count; ++i)
    {
        MYSQL_FIELD *mf = mysql_fetch_field_direct(_prepared_result, i);
        _results[i].setup(mf);
        //TupleDescription::Field tf = translateFromMySQLField(mf);
        SchemaField tf = translate_from_my_sqlfield(mf);
        DEBUG << "Created field " << tf << " for result field " << i;
        result_fields.push_back(tf);
    }

    if (mysql_stmt_bind_result(_statement, _results))
    {   Throw(AuroraBadEntityException, string() +
              "Failed to bind results structure to query: " +
              mysql_stmt_error(_statement));
    }

    _output_description = TupleDescription (result_fields.begin(),
                                            result_fields.end());
    //_output_description = CatalogSchema(result_fields.begin(),
    //                                        result_fields.end());

    DEBUG << "Got output description: " << _output_description;
    set_out_description(0, _output_description);
    //setOutputDescription(0, _output_description);
}

void MySQLQBox::setup_impl() throw (AuroraException)
{
    DEBUG << "Start setting up " << get_name();
    if (get_num_inputs() != 1)
        Throw(aurora_typing_exception,
              "MySQL requires exactly one input streams (not " +
              to_string(get_num_inputs()) + ")");

    this->connect();

    _query = param("query", PARAM_NON_EMPTY);

    // new in 4.1.5
    //    MYSQL_STMT *temp_mysql_stmt = mysql_stmt_init(_mysql);
    //    _statement = mysql_prepare(_mysql, _query.c_str(), _query.length());
    //    _statement = mysql_stmt_prepare(temp_mysql_stmt, _query.c_str(), _query.length());
    _statement = mysql_stmt_init(_mysql);
    //    mysql_stmt_prepare(_statement, _query.c_str(), _query.length());

    if (!mysql_stmt_prepare(_statement, _query.c_str(), _query.length()))
        Throw(aurora_typing_exception, string() +
              "Couldn't compile statement ``" + _query +
              "'': " + mysql_error(_mysql));
    DEBUG << "Complied query ``" << _query << "''";

    this->setup_query_parameters();

    this->setup_output();
    DEBUG << "Finished setting up " << get_name();
}

void MySQLQBox::init_impl() throw (AuroraException)
{
}

void MySQLQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{
    DeqIterator di = deq(0);
    EnqIterator ei;
    if (_has_results)
        ei  = enq(0);
    bool had_results = false;

    while (inv.continue_dequeue_on(di, 0))
    {
        _ctxt.reset();
        _ctxt.set_tuple(di.tuple());
        for (uint32 i = 0; i < _param_count; ++i)
        {
            DEBUG << "Evaluating parameter.";
            _param_exprs[i]->eval_into( (char *)_parameters[i].buffer, _ctxt );
            *(_parameters[i].is_null) = 0;
            *(_parameters[i].length) = _param_exprs[i]->getLength();
        }

        if (mysql_stmt_execute(_statement))
        {    WARN << "Execution failed: " << mysql_stmt_error(_statement);
        }
        DEBUG << "Executed query.";
        if (_has_results)
        {
            DEBUG << "Buffering results.";
            if (mysql_stmt_store_result(_statement))
            {
                WARN << "mysql_stmt_store_result() failed"
                     << mysql_stmt_error(_statement);
            } else
            {
                DEBUG << "Fetching results";
                while (!mysql_stmt_fetch(_statement))
                {
                    memcpy(ei.tuple(), di.tuple(), HEADER_SIZE);
                    for (uint32 i = 0; i < _result_field_count; ++i)
                    {
                        //const TupleDescription::Field &f = _output_description.getField(i);
                        const SchemaField *f = _output_description.get_schema_field(i);
                        const  Bind &res = _results[i];
                        if (res.getLength() != f->get_size())
                        {
                            WARN << "Result length is " << res.getLength()
                                 << " but field size is " << f->get_size()
                                 << " hope everything works out.";
                        }
                        if (res.isNull())
                        {
                            WARN << "Null result value. "
                                 << "We don't know how to deal with that.";
                            // Zero the memory at least.
                            memset((char *)ei.tuple() + f->get_offset(), 0,
                                                        f->get_size());
                        } else
                        {
                            memcpy((char *)ei.tuple() + f->get_offset(),
                                   res.buffer, f->get_size());
                        }
                    }
                    DEBUG << "Outputted row: " << _output_description.tuple_to_string(ei.tuple());
                    had_results = true;
                    ++ei;
                }
                DEBUG << "Done fetching results.";
            }
        } else
        {
            DEBUG << "Not expecting any results.";
        }
        ++di;
    }
    if (had_results)
        get_output(0).notify_enq();
}

AURORA_DEFINE_QBOX(MySQLQBox, "mysql");

BOREALIS_NAMESPACE_END;
