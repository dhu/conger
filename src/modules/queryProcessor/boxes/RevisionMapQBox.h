#ifndef REVISION_MAP_QBOX_H
#define REVISION_MAP_QBOX_H

#include "QBox.h"
#include <vector>
#include "Expression.h"
#include "SweeperQBox.h"
#include "TupleQueue.h"

BOREALIS_NAMESPACE_BEGIN

class RevisionMapQBox : public QBox
{
 private:
  vector<ptr<Expression> > _expressions;
  EvalContext _ctxt;

  // inverses for downstream processing
  vector<ptr<Expression> > _inverses;

 protected:
  void setup_impl() throw (AuroraException);
  void init_impl() throw (AuroraException);
  void run_impl(QBoxInvocation&) throw (AuroraException);
};

BOREALIS_NAMESPACE_END

#endif
