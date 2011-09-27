#ifndef DYNAMICFILTER_QBOX_H
#define DYNAMICFILTER_QBOX_H

#include "QBox.h"
#include "Expression.h"

BOREALIS_NAMESPACE_BEGIN;

class DynamicFilterQBox : public QBox
{
  friend class FilterBoxState;

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
    ptr<AbstractBoxState> pack_state();
    void unpack_state(ptr<AbstractBoxState> box_state) {};

    AURORA_DECLARE_QBOX(DynamicFilterQBox, "dfilter");
};

class FilterBoxState : public AbstractBoxState
{
 public:
  FilterBoxState() {}
  ~FilterBoxState() {}

 private:


};

BOREALIS_NAMESPACE_END;

#endif
