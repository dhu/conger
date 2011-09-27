#ifndef UNION_QBOX_H
#define UNION_QBOX_H

#include "QBox.h"
#include <vector>

BOREALIS_NAMESPACE_BEGIN

class UnionQBox : public QBox
{

 protected:
    unsigned int _number_of_inputs;
    unsigned int _output_tuple_size;

 protected:
    void setup_impl() throw (AuroraException);
    void init_impl() throw (AuroraException);
    void run_impl(QBoxInvocation&) throw (AuroraException);
};

BOREALIS_NAMESPACE_END

#endif
