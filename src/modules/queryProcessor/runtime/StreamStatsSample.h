#ifndef STREAMSTATSSAMPLE_H
#define STREAMSTATSSAMPLE_H

#include<sstream>

BOREALIS_NAMESPACE_BEGIN;

/// Like BoxStatsSample, but this bears the data for a Stream instead of a QBox.
struct StreamStatsSample
{
    StreamStatsSample(long tv_sec, size_t tuples_enqueued) :
        _tv_sec(tv_sec),
        _tuples_enqueued(tuples_enqueued)
    {
    }

    long _tv_sec;

    // The total number of tuples enqueued into this Stream during this sample.
    size_t _tuples_enqueued;

    string to_string() const
    {
        ostringstream os;
        os << "_tv_sec = " << _tv_sec << endl
           << "_tuplesEnqueued = " << _tuples_enqueued << endl;
        return os.str();
    }
};

BOREALIS_NAMESPACE_END;

#endif
