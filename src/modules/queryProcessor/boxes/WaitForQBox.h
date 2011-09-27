#ifndef WAITFOR_QBOX_H
#define WAITFOR_QBOX_H

#include "QBox.h"
#include "Timestamp.h"
#include "Expression.h"

BOREALIS_NAMESPACE_BEGIN

class WaitForQBox : public QBox
{

private:

    class TimestampTuple
    {
    public:
        TimestampTuple(Timestamp timestamp, const char* tuple, size_t size) :
            _timestamp(timestamp)
        {

            _tuple = new char[size];
            memcpy(_tuple, tuple, size);
        }
        Timestamp get_timestamp() { return _timestamp; }
        char* getTuple() { return _tuple; }
        ~TimestampTuple() { delete _tuple; }
    private:
        Timestamp _timestamp;
        char* _tuple;
    };

    ptr<EvalContext> _ctxt;
    ptr<Expression> _predicate;

    unsigned int _output_tuple_size;
    Timestamp _timeout; // sec, usec

    // Oldest tuple will be at the front
    list<ptr<TimestampTuple> > _buffer;

protected:
    void setup_impl() throw (AuroraException);
    void init_impl() throw (AuroraException);
    void run_impl(QBoxInvocation&) throw (AuroraException);
};

BOREALIS_NAMESPACE_END

#endif
