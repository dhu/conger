#ifndef INSERT_QBOX_H
#define INSERT_QBOX_H

#include "QBox.h"
#include "Table.h"
#include "SQLInsert.h"

BOREALIS_NAMESPACE_BEGIN

class InsertQBox : public QBox
{

private:
    ptr<SQLInsert> _sql;
    Table *_table;
    EvalContext _ctxt;
    size_t _row_size;
    char *_row_buf;
    bool _pass_input;
    size_t _input_tuple_size;

protected:
    void setup_impl() throw (AuroraException);
    void init_impl() throw (AuroraException);
    void run_impl(QBoxInvocation&) throw (AuroraException);

public:
    InsertQBox() : _row_buf(0) {}
    ~InsertQBox() { delete[] _row_buf; }
};

BOREALIS_NAMESPACE_END

#endif
