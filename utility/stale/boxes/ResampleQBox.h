#ifndef RESAMPLE_QBOX_H
#define RESAMPLE_QBOX_H

#include "QBox.h"
#include "Predicate.h"
#include "Parse.h"
#include "FieldExt.h"
#include "BufferList.h"
//#include "NewStateList.H"
#include "HashForNewState.h"
#include <vector>

BOREALIS_NAMESPACE_BEGIN;

class ResampleQBox : public QBox
{

private:

    EnqIterator _my_enq;

    aggregate_function* _resample_function;
    //ptr<Aggregate> _resample_function;

    BufferList    *_left_buffer;
    BufferList    *_right_buffer;

    hash_for_new_state  *_state_hash;

    int    _window_size;

    int    _left_slack;
    int    _right_slack;

    char  *_left_order_att_value;
    char  *_left_order_att_str;
    char  _left_order_att_type;
    int   _left_order_att_size;

    char  *_right_order_att_value;
    //char  *_right_order_att_str; - this is not used, leave commented to remember
    char  _right_order_att_type;
    int   _right_order_att_size;

    FieldExt   *_left_field_att;
    FieldExt   *_right_field_att;

    // for order-by-tuplenum
    bool _LEFT_ORDER_BY_TUPLENUM;
    bool _RIGHT_ORDER_BY_TUPLENUM;
    char* _left_tuple_counter;
    char* _right_tuple_counter;
    unsigned int* _left_tuple_counter_state;
    unsigned int* _right_tuple_counter_state;

    char       *_left_curr_tuple;
    char       *_right_curr_tuple;
    Timestamp  _curr_ts;
    int32      _curr_seconds;

    int      _left_tuple_size;
    int      _right_tuple_size;

    char     *_output_tuple;
    int      _output_tuple_size;
    int      _num_tuples_emitted;

    void emit_tuple(new_state *s);

    // This stuff used to be in sched/src/QBox.C
    Timestamp get_ts(char *tuple)
    {
        int32 sec   = ((Timestamp*) tuple)->tv_sec;
        int32  micro = ((Timestamp*) tuple)->tv_usec;
        return Timestamp(sec,micro);
        //Timestamp time = *(new Timestamp(sec, micro));
        //return time;
    }
    int get_sid_size()
    {
        return sizeof(int);
    }

    int get_ts_size()
    {
        return sizeof(timeval);
    }

protected:

    void setup_impl() throw (AuroraException);
    void init_impl() throw (AuroraException);
    void run_impl(QBoxInvocation&) throw (AuroraException);

    AURORA_DECLARE_QBOX(ResampleQBox, "resample");
};

BOREALIS_NAMESPACE_END;

#endif
