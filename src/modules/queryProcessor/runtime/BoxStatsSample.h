#ifndef BOXSTATSSAMPLE_H
#define BOXSTATSSAMPLE_H

#include<vector>
#include<sstream>

BOREALIS_NAMESPACE_BEGIN;

/// Describes, for some particular second in time, the runtime stats
/// of a some QBox in the Aurora runtime.
/// Note that this doesn't necessarily count all the activity that occured
/// during that second:  It may be added to later, and may need to be merged
/// with other BoxStatsSamples from the same second, to get the final answer
/// for what happened during that second.
struct BoxStatsSample
{
    BoxStatsSample(unsigned long tv_sec,
                   size_t tuples_consumed,
                   const vector<size_t> tuples_produced) :
        _tv_sec(tv_sec),
        _tuples_consumed(tuples_consumed),
        _tuples_produced(tuples_produced)
    {
    }

    // This is intended to correspond to the timeval._tv_sec field that gets
    // filled in by a call to gettimeofday(...).
    long _tv_sec;

    // The total number of tuples consumed, from all streams entering the
    // box, during this sample.
    size_t _tuples_consumed;

    // For each output port, this gives the total number of tuples emitted
    // by this box during this sample.  Elements in this vector correspond
    // to elements in the QBox::_inq vector, on a per-index basis.
    vector<size_t> _tuples_produced;

    string to_string() const
    {
        ostringstream os;
        os << "_tv_sec = " << _tv_sec << endl
           << "_tuplesConsumed = " << _tuples_consumed << endl;

        for (vector<size_t>::size_type i = 0; i < _tuples_produced.size(); ++i)
        {
            os << "_tuplesProduced[" << i << "] = " << _tuples_produced[i] << endl;
        }

        return os.str();
    }
};

BOREALIS_NAMESPACE_END;

#endif
