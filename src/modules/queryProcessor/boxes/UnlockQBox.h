#ifndef UNLOCK_QBOX_H
#define UNLOCK_QBOX_H

#include "QBox.h"
#include <vector>

BOREALIS_NAMESPACE_BEGIN

class UnlockQBox : public QBox
{

private:
    ptr<LockSet> _lock_set;
    ptr<Expression> _key_expr;
    EvalContext _ctxt;
    size_t _tuple_size;

protected:
    void setup_impl() throw (AuroraException);
    void init_impl() throw (AuroraException);
    void run_impl(QBoxInvocation&) throw (AuroraException);
};

BOREALIS_NAMESPACE_END

#endif
