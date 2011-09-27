#include "UpdateQBox.h"

BOREALIS_NAMESPACE_BEGIN

void UpdateQBox::setup_impl() throw (AuroraException)
{
    if (get_num_inputs() != 1)
        Throw(aurora_typing_exception,
              "Update requires exactly one input stream (not " +
              to_string(get_num_inputs()) + ")");

    _table = get_table();
    if (!_table)
    {
        Throw(AuroraBadEntityException,
              "Update box " + get_name() + " doesn't have a table.");
    }

    ExprContext ctxt;
    ctxt.set_named_schema_info("input",
                           get_in_schema(0),
                           0);
    ctxt.set_named_schema_info(_table->get_name(),
                _table->get_schema(),
                1);

    DEBUG << "SelectQBox accesses " << *_table;

    DEBUG << "Setting named schema info for " << _table->get_name()
          << " : " << _table->get_schema()->as_string();

    string sql_expr(param("sql", PARAM_REQUIRED));
    DEBUG << "SQL Expression: " << sql_expr;
    _sql = SQLUpdate::parse(sql_expr, ctxt, _table->get_name());

    _query_handle = _table->compile_update(_sql);

    setup_output();

    // TODO: Check that input and table schema match.
}

// Set up the output, if any, based on parameters.
void UpdateQBox::setup_output() throw (AuroraException)
{
    _pass_input = false;
    typed_param("pass-input", _pass_input, PARAM_NOT_REQUIRED);
    if (_pass_input)
    {
        DEBUG << "Passing in the input after updates.";
        _has_output_expressions = false;
        // Output type is input type.
        set_out_description(0, get_in_stream(0));
        return;
    }

    // Check for output expressions.
    // TODO: Finish output expressions and test them.
    for (unsigned int i = 0; ; ++i)
    {
        string expr = param("output-expression." + to_string(i));
        if (expr.empty())
            break;

        ExprContext ctxt;
        ctxt.set_named_schema_info("input", get_in_schema(0), 0);
        // Name these old and new like Postgres triggers.
        ctxt.set_named_schema_info("old", _table->get_schema(), 1);
        ctxt.set_named_schema_info("new", _table->get_schema(), 2);
        _expressions.push_back(Expression::parse(expr, ctxt));
    }
    if (!_expressions.empty())
    {
        //FATAL << "Output expressions don't really work yet.";
        _has_output_expressions = true;
        vector<SchemaField> fields;
        for (unsigned int i = 0; i < _expressions.size(); ++i)
        {
            string name = param("output-field-name." + to_string(i),
                                PARAM_NON_EMPTY);
            fields.push_back(_expressions[i]->get_schema_field(name));
        }

        TupleDescription t(fields.begin(), fields.end());
        set_out_description(0, t);
    } else
    {
        _has_output_expressions = false;
        DEBUG << "No output expressions.";
        return;
    }

}

void UpdateQBox::init_impl() throw (AuroraException)
{
    TupleDescription in_td(get_in_schema(0));
    _input_tuple_size = in_td.get_size();
}

void UpdateQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{
    unsigned int max_tuples = inv.get_max_tuples_to_dequeue(0);
    UpdateQueryHandle &qh = *_query_handle;
    DeqIterator di = deq(0);
    EnqIterator ei;
    if (_has_output_expressions || _pass_input)
        ei = enq(0);
    bool outputted = false;

    TupleDescription in_td(get_in_schema(0));

    while (max_tuples-- && di.avail())
    {
        const char *input = (char *)di.tuple();
        INFO << "UpdateQBox IN: " << in_td.tuple_to_string(input);
        if (_has_output_expressions)
        {
            qh.do_update(input);
            _ctxt.set_tuple(0, input);
            while (qh.avail())
            {
                _ctxt.reset();
                _ctxt.set_tuple(1, qh.old_row());
                qh.update_row();
                _ctxt.set_tuple(2, qh.new_row());

                // create output tuple.
                memcpy(ei.tuple(), input, HEADER_SIZE);
                char *p = (char *)ei.tuple() + HEADER_SIZE;
                Expression::eval_vector_into(_expressions, p, _ctxt);

                outputted = true;
                ++ei;
                ++qh;
            }
        } else
        {
            DEBUG << "Doing the whole update";
            qh.do_whole_update(input);
            if (_pass_input)
            {
                DEBUG << "Passing on the input tuple.";
                memcpy(ei.tuple(), input, _input_tuple_size);
                outputted = true;
                ++ei;
            } else
            {
                DEBUG << "No output.";
            }
        }
        ++di;
    }

    DEBUG << get_name() << " ran, table size " << _table->get_size();
    
    // TODO: This is a speed hit. Probably not required in production.
    _table->sync();

    if (outputted)
        get_output(0).notify_enq();
}

BOREALIS_NAMESPACE_END
