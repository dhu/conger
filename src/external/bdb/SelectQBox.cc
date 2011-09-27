#include "SelectQBox.h"

BOREALIS_NAMESPACE_BEGIN

void SelectQBox::setup_impl() throw (AuroraException)
{
    if (get_num_inputs() != 1)
        Throw(aurora_typing_exception,
              "Select requires exactly one input stream (not " +
              to_string(get_num_inputs()) + ")");

    _table = get_table();
    if (!_table)
    {
        Throw(AuroraBadEntityException,
              "Select box " + get_name() + " doesn't have a table.");
    }

    ExprContext ctxt;
    ctxt.set_named_schema_info("input", get_in_schema(0), 0);
    ctxt.set_named_schema_info(_table->get_name(), _table->get_schema(), 1);

    DEBUG << "SelectQBox accesses " << *_table;

    DEBUG << "Setting named schema info for " << _table->get_name()
          << " : " << _table->get_schema()->as_string();

    string sql_expr(param("sql", PARAM_REQUIRED));
    DEBUG << "Expr context schema " << ctxt.get_context_schema()->as_string();
    DEBUG << "SQL Expression: " << sql_expr;
    try
    {
        _sql = SQLSelect::parse(sql_expr, ctxt, _table->get_name());
    } catch (AuroraException &e)
    {
        DEBUG << "Exception while parsing select: " << e;
        throw;
    }

    _pass_on_no_results = false;
    typed_param("pass-on-no-results", _pass_on_no_results);
    if (_pass_on_no_results)
    {
        // Second output port that matches the input.
        DEBUG << "Setting up pass-on-no-results output.";
        set_out_description(1, get_in_stream(0));
    }

    _pass_result_sizes = false;
    typed_param("pass-result-sizes", _pass_result_sizes);
    if ( _pass_result_sizes )
    {
        // Second or third output stream that includes values of fields in the
        // where clause and the number of outputs.
        DEBUG << "Setting up pass-result-sizes output.";
        ptr<SQLWhereClause::SubClause> wc_sub =
            _sql->get_where_clause()->get_sub_clause();
        
        // Get fields used in the where clause.
        _pass_result_sizes_ctxt = ctxt;
        _pass_result_sizes_fields_size = 0;
        vector<SchemaField> pass_rs_fields;

        vector< pair<SchemaField, ptr<Expression> > > pass_rs_fields_exprs =
            get_subclause_fields(wc_sub);

        vector< pair<SchemaField, ptr<Expression> > >::iterator pass_rs_it =
            pass_rs_fields_exprs.begin();
        vector< pair<SchemaField, ptr<Expression> > >::iterator pass_rs_end =
            pass_rs_fields_exprs.end();

        unsigned int unnamed_field_counter = 0;
        for (; pass_rs_it != pass_rs_end; ++pass_rs_it)
        {
            pair<SchemaField, ptr<Expression> >& sf_expr_pair = *pass_rs_it;
            ptr<Expression>& field_expr = sf_expr_pair.second;

            if ( sf_expr_pair.first.get_name().empty() )
            {

                string field_name = "out" + to_string(unnamed_field_counter);

                DEBUG << "Found result size field " << field_name << " "
                    << field_expr->getType();

                if ( field_expr->getType() != DataType::INT )
                    continue;

                SchemaField field(field_name, field_expr->getType(),
                    field_expr->getLength());
                    
                pass_rs_fields.push_back(field);
                ++unnamed_field_counter;
            }
            else
                pass_rs_fields.push_back(sf_expr_pair.first);

            _pass_result_sizes_fields.push_back(field_expr);
            _pass_result_sizes_fields_size += field_expr->getLength();
        }

        // Add a field for the number of results.
        SchemaField result_size_field("count", DataType::INT, sizeof(int32));
        pass_rs_fields.push_back(result_size_field);

        // Create and set up the output stream schema.        
        TupleDescription pass_rs_td( pass_rs_fields.begin(), pass_rs_fields.end() );
        DEBUG << get_name() << " result sizes description " << pass_rs_td;
        _pass_result_out_stream_id = (_pass_on_no_results? 2 : 1);
        set_out_description(_pass_result_out_stream_id, pass_rs_td);
    }

    CatalogSchema* table_schema = _sql->get_result_catalog_schema();
    set_out_description(0, TupleDescription(table_schema));

    DEBUG << "SelectQBox out schema " << get_out_schema(0)->as_string();

    // Compile the query.
    _query_handle = _table->compile_select(_sql);

    // TODO: Check that input and table schema match.
}

vector< pair<SchemaField, ptr<Expression> > >
SelectQBox::get_flat_subclause_fields(ptr<SQLWhereClause::SubClause> sub_clause)
{
    assert(sub_clause->getType() == SQLWhereClause::EXPRESSION ||
           sub_clause->getType() == SQLWhereClause::FIELD_EQ_EXPRESSION);

    vector< pair<SchemaField, ptr<Expression> > > r;

    if ( sub_clause->getType() == SQLWhereClause::EXPRESSION )
        r.push_back(make_pair(SchemaField(),
            sub_clause->asExpression(_pass_result_sizes_ctxt)));
    
    else if ( sub_clause->getType() == SQLWhereClause::FIELD_EQ_EXPRESSION )
    {
        ptr<SQLWhereClause::FieldEqSubClause> fe =
            sub_clause.dynamic_cast_to<SQLWhereClause::FieldEqSubClause>();

        SchemaField fe_field(fe->get_field_name(),
                             fe->get_field_value()->getType(),
                             fe->get_field_value()->getLength());

        r.push_back(make_pair(fe_field, fe->get_field_value()));
    }

    return r;
}

vector<pair<SchemaField, ptr<Expression> > >
SelectQBox::get_subclause_fields(ptr<SQLWhereClause::SubClause> sub_clause)
    throw( AuroraBadEntityException )
{
    if ( sub_clause->getType() == SQLWhereClause::EXPRESSION ||
         sub_clause->getType() == SQLWhereClause::FIELD_EQ_EXPRESSION )
    {
        return get_flat_subclause_fields(sub_clause);
    }

    vector< pair<SchemaField, ptr<Expression> > > r;
    
    vector< ptr<SQLWhereClause::SubClause> > rem_clauses;
    rem_clauses.push_back( sub_clause );
    
    while ( !rem_clauses.empty() )
    {
        ptr<SQLWhereClause::SubClause> sc = rem_clauses.back();
        rem_clauses.pop_back();
        
        ptr<SQLWhereClause::ConjunctionSubClause> csc =
            sc.dynamic_cast_to<SQLWhereClause::ConjunctionSubClause>();

        const vector< ptr<SQLWhereClause::SubClause> >& more_sub_clauses =
            csc->get_sub_clauses();

        vector< ptr<SQLWhereClause::SubClause> >::const_iterator msc_it =
            more_sub_clauses.begin();
        vector< ptr<SQLWhereClause::SubClause> >::const_iterator msc_end =
            more_sub_clauses.end();
        
        for (; msc_it != msc_end; ++msc_it)
        {
            if ( (*msc_it)->getType() == SQLWhereClause::CONJUNCTION )
                rem_clauses.push_back(*msc_it);

            else {
                vector<pair<SchemaField, ptr<Expression> > > more_fields =
                    get_flat_subclause_fields(*msc_it);
                copy(more_fields.begin(), more_fields.end(), back_inserter(r));
            }
        }
    }
    
    return r;
}


void SelectQBox::init_impl() throw (AuroraException)
{
    TupleDescription in_td(get_in_schema(0));
    _input_tuple_size = in_td.get_size();
}

void SelectQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{
    DeqIterator di = deq(0);
    EnqIterator ei = enq(0);
    EnqIterator ei_unmatch;
    if (_pass_on_no_results)
        ei_unmatch = enq(1);
    
    EnqIterator ei_num_results;
    if ( _pass_result_sizes )
        ei_num_results = enq(_pass_result_out_stream_id);    

    bool was_output = false;
    bool was_unmatched_output = false;

    int output_count = 0;
    bool was_result_sizes_output = false;

    // Set up TupleDescriptions
    TupleDescription in_td(get_in_schema(0));
    TupleDescription out0_td(get_out_schema(0));

    ptr<TupleDescription> out1_td;
    if (_pass_on_no_results)
        out1_td = ptr<TupleDescription>(new TupleDescription(get_out_schema(1)));

    ptr<TupleDescription> rs_td;
    if ( _pass_result_sizes )
        rs_td = ptr<TupleDescription>
            (new TupleDescription(get_out_schema(_pass_result_out_stream_id)));

    // Dequeue loop.
    while (inv.continue_dequeue_on(di, 0))
    {
        const char *input = (char *)di.tuple();
        INFO << "SelectQBox(" << get_name()
         << ") IN: " << in_td.tuple_to_string(input);

        _query_handle->do_query(input);
        if (!_query_handle->avail())
        {
            if (_pass_on_no_results)
            {
                void *output = ei_unmatch.tuple();
                memcpy(output, input, _input_tuple_size);
                INFO << "SelectQBox(" << get_name()
                     << ") OUT(2): Passing on "
                     << out1_td->tuple_to_string(output);
                was_unmatched_output = true;
                ++ei_unmatch;
            } else
            {
                INFO << "SelectQBox(" << get_name()
                     << ") OUT: No Results.";
            }
        }
        while (_query_handle->avail())
        {
            // Create the output tuple.
            char *output = (char *)ei.tuple();
            // Copy header from input tuple.
            memcpy(output, input, HEADER_SIZE);
            // Get the data row.
            _query_handle->eval_into(output + HEADER_SIZE);

            INFO << "SelectQBox(" << get_name()
                << ") OUT: " << out0_td.tuple_to_string(ei.tuple());
            was_output = true;

            ++(*_query_handle);
            ++ei;
            ++output_count;
        }
        
        if ( _pass_result_sizes )
        {
            EvalContext ctxt(1);
            ctxt.reset();
            ctxt.set_tuple(0, input);

            // Create the output tuple.
            char *output = static_cast<char*>(ei_num_results.tuple());
            // Copy header from input tuple.
            memcpy(output, input, HEADER_SIZE);

            Expression::eval_vector_into(_pass_result_sizes_fields,
                output + HEADER_SIZE, ctxt);

            memcpy(output + HEADER_SIZE + _pass_result_sizes_fields_size,
                   &output_count, sizeof(output_count));

            ++ei_num_results;
            was_result_sizes_output = true;
        }
        ++di;
    }
    
    DEBUG << get_name() << " ran, table size " << _table->get_size();

    // Done, notify output.
    if (was_output)
        get_output(0).notify_enq();

    if (was_unmatched_output)
        get_output(1).notify_enq();

    if (was_result_sizes_output)
        get_output(_pass_result_out_stream_id).notify_enq();
}


BOREALIS_NAMESPACE_END
