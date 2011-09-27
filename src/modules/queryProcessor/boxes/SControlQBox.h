#ifndef SCONTROL_QBOX_H
#define SCONTROL_QBOX_H

#include "QBox.h"
#include "../ConsistencyMngr.h" // For the benchmark types

BOREALIS_NAMESPACE_BEGIN

/**
 * Defines the generic behavior of control boxes. Most importantly
 * these boxes have a second output where they send control messages
 * to the runtime engine
 */
class SControlQBox : public QBox
{

public:
    void setup_impl() throw (AuroraException);
    void init_impl() throw (AuroraException);

protected:

    static const int CONTROL_STREAM = 0;
    static const int DATA_STREAM = 1;
    static const int TWO_OUTPUTS = 2;

    /// Creates the schema for the output stream with control messages
    TupleDescription create_control_output_description();

    /// Emit a tuple on the DATA_STREAM
    void emit_tuple(const Tuple& src_tuple);

    /// Emit a tuple on the DATA_STREAM but change its type first
    void emit_tuple_with_different_type(const Tuple& undo_tuple, TupleType type);

    /// Emit a control tuple on the CONTROL_STREAM (tuple_id will be set to zero)
    void emit_control_tuple(TupleType tuple_type, Timestamp tuple_data);

    /// Emit a control tuple on the DATA_STREAM or CONTROL_STREAM (tuple_id will be set to zero)
    void emit_control_tuple(int stream_id, TupleType tuple_type, Timestamp tuple_data);

    /// Emit an undo tuple given the last stable tuple
    void emit_undo_tuple(const Tuple& src_tuple);

    /// Notify appropriate outputs about newly available tuples
    void notify_outputs();

    /// True/false indicates whether new tuples have been produced
    bool _output[TWO_OUTPUTS];

    /// The size in bytes of DATA_STREAM output tuples
    size_t _output_tuple_size[TWO_OUTPUTS];
};


BOREALIS_NAMESPACE_END

#endif
