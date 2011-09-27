#ifndef REVISION_FILTER_QBOX_H
#define REVISION_FILTER_QBOX_H

#include "QBox.h"
#include "Expression.h"
#include "SweeperQBox.h"
#include "TupleQueue.h"

BOREALIS_NAMESPACE_BEGIN

class RevisionFilterQBox : public QBox
{
  friend class RevisionFilterBoxState;

 private:
  EvalContext _ctxt;
  vector<ptr<Expression> > _expressions;

  bool _pass_on_false_port;
  unsigned int _output_tuple_size;

  unsigned int _expr_count; // _expressions.size()

 protected:
  void setup_impl() throw (AuroraException);
  void init_impl() throw (AuroraException);
  void run_impl(QBoxInvocation&) throw (AuroraException);

  // packing and unpacking states
  void set_pending_box_state(ptr<AbstractBoxState> packed_box);
  ptr<AbstractBoxState> pack_state(bool delta_only = false);
  void unpack_state(ptr<AbstractBoxState> box_state) {};
};

class RevisionFilterBoxState : public AbstractBoxState
{
 public:
  RevisionFilterBoxState() {}
  ~RevisionFilterBoxState() {}

 private:

};

BOREALIS_NAMESPACE_END

#endif
