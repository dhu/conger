#ifndef EDGE_DETECT_QBOX_H
#define EDGE_DETECT_QBOX_H

#include "QBox.h"
#include "Expression.h"

BOREALIS_NAMESPACE_BEGIN

class EdgeDetectQBox : public QBox
{
  friend class EdgeDetectBoxState;

private:
    EvalContext _ctxt;
    vector<ptr<Expression> > _expressions;

    bool _pass_on_false_port;
    int8 GX[3][3];
    int8 GY[3][3];
    unsigned int _output_tuple_size;
    int _ed_threshold;

    unsigned int _expr_count; // _expressions.size()

protected:
    void setup_impl() throw (AuroraException);
    void init_impl() throw (AuroraException);
    void run_impl(QBoxInvocation&) throw (AuroraException);

    list<void *> tuples;
    // packing and unpacking states
    void set_pending_box_state(ptr<AbstractBoxState> packed_box);
    ptr<AbstractBoxState> pack_state(bool delta_only = false);
    void unpack_state(ptr<AbstractBoxState> box_state) {};
};



class EdgeDetectBoxState : public AbstractBoxState
{
 public:
  EdgeDetectBoxState() {}
  ~EdgeDetectBoxState() {}
};

BOREALIS_NAMESPACE_END

#endif
