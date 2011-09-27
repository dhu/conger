
#ifndef RANDOM_DROP_QBOX_H
#define RANDOM_DROP_QBOX_H

#include "QBox.h"
#include "Expression.h"

BOREALIS_NAMESPACE_BEGIN

// RandomDrop drops its inputs randomly based on the _drop_rate parameter.
// At most _max_batch_size number of consecutive tuples can be dropped.
//
class RandomDropQBox : public QBox
{

private:

    unsigned int _output_tuple_size;

    double  _drop_rate;      // drop_rate = 1 - selectivity
    int     _max_batch_size; // max number of consecutive drops
    int     _drop_counter;   // number of consecutive dropped tuples
    int     _extra_tuples;   // tuples kept extra, to be compensated by drops

protected:

    void     setup_impl() throw (AuroraException);
    void     init_impl() throw (AuroraException);
    void     run_impl(QBoxInvocation&) throw (AuroraException);
    void     update_params(); // -- tatbul

public:

    double   get_drop_rate();
    void     set_drop_rate(double rate);
    int      get_max_batch_size();
    void     set_max_batch_size(int batch_size);
};

BOREALIS_NAMESPACE_END

#endif
