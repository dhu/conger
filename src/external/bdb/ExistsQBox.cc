#include "ExistsQBox.h"

BOREALIS_NAMESPACE_BEGIN

/**
 * This box is a quick hack that Linear Road needed and Eddie asked
 * for. No warrantee, and it will likely be ripped out when we stop
 * caring about Linear Road or transition Linear Road to more accepted
 * operators. Anyone using this who is not Linear Road is probably a
 * bad person. It should probably get moved to a plugin directory of
 * the LR demo or something. --tibbetts
 */

void ExistsQBox::setup_impl() throw (AuroraException)
{
    if (get_num_inputs() != 1)
        Throw(aurora_typing_exception,
              "Exists requires exactly one input stream (not " +
              to_string(get_num_inputs()) + ")");

    _table = get_table();

    if (!_table)
    {
        Throw(AuroraBadEntityException,
              "Exists box " + get_name() + " doesn't have a table.");
    }

    ExprContext ctxt;
    ctxt.set_named_schema_info("input",
                           get_in_schema(0),
                           0);
    ctxt.set_named_schema_info(_table->get_name(),
                _table->get_schema(),
                1);

    string sql_expr(param("sql", PARAM_REQUIRED));
    DEBUG << "SQL Expression: " << sql_expr;
    try
    {
        _sql = SQLSelect::parse(sql_expr, ctxt, _table->get_name());
    } catch (AuroraException &e)
    {
        DEBUG << "Exception while parsing select: " << e;
        throw;
    }

    // Output will be the input tuple with an extra field : a single int named "exists".
    //vector<TupleDescription::Field> out_fields;
    vector<SchemaField> out_fields;

    copy( get_in_schema(0)->get_schema_field().begin(),
          get_in_schema(0)->get_schema_field().end(),
          back_inserter( out_fields ));

    out_fields.push_back(SchemaField("exists",
                     DataType::INT,
                     sizeof(int32)));

    TupleDescription out_td(out_fields.begin(), out_fields.end());
    set_out_description(0, out_td);
    DEBUG << "Output description: " << out_td;

    // Compile the query.
    _query_handle = _table->compile_select(_sql);
}

void ExistsQBox::init_impl() throw (AuroraException)
{
}

void ExistsQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{
    DeqIterator di = deq(0);
    //EnqIterator ei = enq(0);

    bool was_output = false;

    TupleDescription in_td(get_in_schema(0));

    while (inv.continue_dequeue_on(di, 0))
    {
        const char *input = (char *)di.tuple();
        INFO << get_name() << " IN: " << in_td.tuple_to_string(input);

        _query_handle->do_query(input);
        if (_query_handle->avail())
        {
            DEBUG << "Have results.";
            emit_tuple(input, 1);
        } else
        {
            DEBUG << "No results.";
            emit_tuple(input, 0);
        }
        was_output = true;
        ++di;
    }
    // Done, notify output.
    if (was_output)
        get_output(0).notify_enq();
}

void ExistsQBox::emit_tuple(const void *input_tuple, int exists)
{
    EnqIterator ei = enq(0);

    const Tuple input = Tuple::factory(input_tuple);
    Tuple output = Tuple::factory(ei.tuple());

    TupleDescription out_td(get_out_schema(0));

    // Header.
    //output.setTimestamp(input.getTimestamp());
    output.copy_header(input);

    // The input tuple copied
    //memcpy(output.getBody(), input.getBody(), getInputDescription(0).getSize() - HEADER_SIZE);

    // Note: the catalog schema's size does not include the header.
    memcpy(output.get_body(), input.get_body(), get_in_schema(0)->get_size());

    // the "exists" field
    memcpy(output.get_body() + get_in_schema(0)->get_size(), &exists, sizeof(int32));

    DEBUG << get_name() << " OUT: " << out_td.tuple_to_string(output);
    ++ei;
}

BOREALIS_NAMESPACE_END
