#include "DeleteQBox.h"

BOREALIS_NAMESPACE_BEGIN

void DeleteQBox::setup_impl() throw( AuroraException )
{
    if ( get_num_inputs() != 1 ) {
        Throw(aurora_typing_exception,
              "Delete requires exactly one input stream, not " +
               to_string(get_num_inputs()));
    }

    _table = get_table();

    if ( !_table ) {
        Throw(AuroraBadEntityException,
              "Delete box " + get_name() + " does not have a table!");
    }
    
    ExprContext ctxt;
    ctxt.set_named_schema_info("input", get_in_schema(0), 0);
    ctxt.set_named_schema_info(_table->get_name(), _table->get_schema(), 1);
    
    DEBUG << "Delete box accesses " << *_table;
    
    string sql_expr = param("sql", PARAM_REQUIRED);
    DEBUG << "SQL expression " << sql_expr;
    
    try {
        _sql = SQLDelete::parse(sql_expr, ctxt, _table->get_name());
    } catch ( AuroraException& ae ) {
        DEBUG << "Exception while parsing delete: " << ae;
        throw;
    }
    
    _pass_deletions = false;
    typed_param("pass-deletions", _pass_deletions);
    if ( _pass_deletions )
    {
        CatalogSchema* table_schema = _table->get_schema();
        DEBUG << "Delete box pass deletions schema "
            << table_schema->as_string();
        set_out_description(0, TupleDescription(table_schema));
    }

    _pass_input = false;
    typed_param("pass-input", _pass_input);
    if ( _pass_input )
    {
        DEBUG << "Delete box pass input schema "
            << get_in_schema(0)->as_string();
        uint16 out_idx = (_pass_deletions? 1 : 0);
        set_out_description(out_idx, TupleDescription(get_in_schema(0)));
    }
    
    _query_handle = _table->compile_delete(_sql);
}

void DeleteQBox::init_impl() throw( AuroraException )
{
    TupleDescription in_td(get_in_schema(0));
    _input_tuple_size = in_td.get_size();
    _row_size = _table->get_schema()->get_size();
}

void DeleteQBox::run_impl(QBoxInvocation& inv) throw( AuroraException )
{
    DeqIterator di = deq(0);
    EnqIterator ei_del;
    EnqIterator ei_input;

    bool outputted_deletion = false;
    bool outputted_input = false;

    if ( _pass_deletions )
        ei_del = enq(0);

    if ( _pass_input )
        ei_input = enq( (_pass_deletions? 1 : 0) );
        
    while( inv.continue_dequeue_on(di, 0) )
    {
        const char* input = static_cast<const char*>(di.tuple());
        
        if ( _pass_input ) {
            memcpy(ei_input.tuple(), input, _input_tuple_size);
            ++ei_input;
            outputted_input = true;
        }

        _query_handle->do_delete(input);
        
        while ( _query_handle->avail() )
        {
            _query_handle->delete_row();

            if ( _pass_deletions ) {
                memcpy(ei_del.tuple(), _query_handle->deleted_row(), _row_size);
                ++ei_del;
                outputted_deletion = true;
            }

            ++(*_query_handle);
        }
        
        ++di;
    }

    DEBUG << get_name() << " ran, table size " << _table->get_size();
    
    if ( _pass_deletions && outputted_deletion )
        get_output(0).notify_enq();

    if ( _pass_input && outputted_input )
        get_output((_pass_deletions? 1 : 0)).notify_enq();
}

BOREALIS_NAMESPACE_END
