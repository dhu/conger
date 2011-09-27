#ifndef UPDATE_QBOX_H
#define UPDATE_QBOX_H

#include "QBox.h"
#include "Table.h"
#include "SQLUpdate.h"

BOREALIS_NAMESPACE_BEGIN

class UpdateQBox : public QBox
{
  private:
    ptr<SQLUpdate> _sql;
    Table *_table;
    ptr<UpdateQueryHandle> _query_handle;
    bool _has_output_expressions;
    EvalContext _ctxt;
    vector<ptr<Expression> > _expressions;

    bool _pass_input;
    size_t _input_tuple_size;

    void setup_output() throw (AuroraException);

  public:
    UpdateQBox() : _ctxt(3) {}

  protected:
    void setup_impl() throw (AuroraException);
    void init_impl() throw (AuroraException);
    void run_impl(QBoxInvocation&) throw (AuroraException);
};

BOREALIS_NAMESPACE_END

#endif
