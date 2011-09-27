#include "InsertQBox.h"

BOREALIS_NAMESPACE_BEGIN

void InsertQBox::setup_impl() throw (AuroraException)
{
    if (get_num_inputs() != 1)
        Throw(aurora_typing_exception,
              "Insert requires exactly one input stream (not " +
              to_string(get_num_inputs()) + ")");

    _table = get_table();
    if (!_table)
    {
        Throw(AuroraBadEntityException,
              "Insert box " + get_name() + " doesn't have a table.");
    }

    DEBUG << "Insert schema " << get_in_schema(0)->as_string();

    ExprContext ctxt;
    ctxt.set_context_schema(*get_in_schema(0));
    string sql_expr(param("sql", PARAM_REQUIRED));
    DEBUG << "SQL Expression: " << sql_expr;
    _sql = SQLInsert::parse(sql_expr, ctxt, _table->get_name());

    _row_size = _sql->get_insert_tuple_description().get_size();
    _row_buf = new char[_row_size];
    DEBUG << "Row size: " << _row_size;

    // accept optional parameter to pass on the input tuple when the insert completes
    _pass_input = false;
    typed_param("pass-input", _pass_input, PARAM_NOT_REQUIRED);
    if (_pass_input)
    {
        DEBUG << "Passing in the input after updates.";

        // Output type is input type.
        set_out_description(0, get_in_stream(0));
        return;
    }

    // TODO: Check that input and table schema match.
}


void InsertQBox::init_impl() throw (AuroraException)
{
    TupleDescription in_td(get_in_schema(0));
    _input_tuple_size = in_td.get_size();
}

void InsertQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{
    bool outputted = false;
    unsigned int max_tuples = inv.get_max_tuples_to_dequeue(0);
    DeqIterator di = deq(0);

    TupleDescription in_td(get_in_schema(0));

    while (max_tuples-- && di.avail())
    {
        const void *data = di.tuple();
        INFO << "InsertQBox IN: " << in_td.tuple_to_string(data);

        _ctxt.reset();
        _ctxt.set_tuple((char *)data);
        _sql->eval_into(_row_buf, _ctxt);

        DEBUG << "Inserting " << _sql->get_insert_tuple_description().
                                                tuple_data_to_string(_row_buf);
        DEBUG << "Bytes: " << to_hex_string(_row_buf, _row_size);

        _table->insert(_row_buf);

        ++di;

        if (_pass_input)
        {
            EnqIterator ei = enq(0);
            INFO << "Passing on the input tuple.";
            memcpy(ei.tuple(), data, _input_tuple_size);
            outputted = true;
            ++ei;
        }

    }

    DEBUG << get_name() << " ran, table size " << _table->get_size();
    
    // TODO: This is a speed hit. Probably not required in production.
    _table->sync();

    // Notify if anything was outputted
    if (outputted)
    {
        get_output(0).notify_enq();
    }
}

BOREALIS_NAMESPACE_END
