#ifndef MAP_QBOX_H
#define MAP_QBOX_H

#include "QBox.h"
#include <vector>
#include "Expression.h"

BOREALIS_NAMESPACE_BEGIN

class MapQBox : public QBox
{
private:
    vector<ptr<Expression> > _expressions;
    EvalContext _ctxt;

protected:
    void setup_impl() throw (AuroraException);
    void init_impl() throw (AuroraException);
    void run_impl(QBoxInvocation&) throw (AuroraException);
};

BOREALIS_NAMESPACE_END

#endif
