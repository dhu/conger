#ifndef FILTER_QBOX_H
#define FILTER_QBOX_H

#include "QBox.h"
#include "Expression.h"
#include "DynamicArray.h"

BOREALIS_NAMESPACE_BEGIN

class FilterQBox : public QBox
{
  friend class FilterBoxState;

private:
    EvalContext _ctxt;
    vector<ptr<Expression> > _expressions;
	bool _array_rewrite;
	string _array_field;
	uint32 _array_offset;
	CatalogSchema _filt_ctxt;
	int32 _eval_size;
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
    void *create_output_tuple(const void *data, int i);
    DynamicArray *clone_array(DynamicArray *orig);
    void *iterate_over_and_null_vals(const void *data, int i);
};



class FilterBoxState : public AbstractBoxState
{
 public:
  FilterBoxState() {}
  ~FilterBoxState() {}
};

BOREALIS_NAMESPACE_END

#endif
