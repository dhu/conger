#ifndef SELECT_QBOX_H
#define SELECT_QBOX_H

#include "QBox.h"
#include "Table.h"
#include "SQLSelect.h"

BOREALIS_NAMESPACE_BEGIN

class SelectQBox : public QBox
{
private:
    ptr<SQLSelect> _sql;
    Table *_table;
    ptr<SelectQueryHandle> _query_handle;
    size_t _input_tuple_size;
    bool _pass_on_no_results;

    bool _pass_result_sizes;
    ExprContext _pass_result_sizes_ctxt;
    uint16 _pass_result_out_stream_id;
    vector< ptr<Expression> > _pass_result_sizes_fields;
    int32 _pass_result_sizes_fields_size;

protected:
    void setup_impl() throw (AuroraException);
    void init_impl() throw (AuroraException);
    void run_impl(QBoxInvocation&) throw (AuroraException);

    vector<pair<SchemaField, ptr<Expression> > >
    get_flat_subclause_fields(ptr<SQLWhereClause::SubClause> sub_clause);
    
    vector<pair<SchemaField, ptr<Expression> > >

    get_subclause_fields(ptr<SQLWhereClause::SubClause> sub_clause)
        throw( AuroraBadEntityException );
};

BOREALIS_NAMESPACE_END

#endif
