#ifndef SJOIN_QBOX_H
#define SJOIN_QBOX_H

#include "JoinQBox.h"
#include "../ConsistencyMngr.h" // For benchmark types
#include <fstream>

BOREALIS_NAMESPACE_BEGIN;

/**
 * Same as a join operator but also supports reconciliation
 * with undo/redo or checkpoint redo.
 * Gets join functionality by inheritance
 * Currently, this join only supports windows of the *nb of tuples* not the values
 */
class ExperimentalJoinQBox : public JoinQBox
{

 public:
    ~ExperimentalJoinQBox(); // So we can log the stats in a file

 protected:
    void setup_impl() throw (AuroraException);
    void init_impl() throw (AuroraException);
    void run_impl(QBoxInvocation&) throw (AuroraException);

  private:

    // Using a struct instead of a pair so we can do tests like
    // if ( _state_bounds[tuple_id] )
    struct StateBound
    {
        int32 _left;
        int32 _right;
        StateBound(int32 left = -1, int32 right = -1)
            : _left(left), _right(right) {}
        operator const void *() const
        {
            return ((_left == -1) || (_right == -1)) ? 0 : this;
        }
    };

    static const int MAX_HISTORY = 101001000; // In tuples
    static const int PAGE_SIZE = 16384;   // Arbitrary small value

    static const int TWO_INPUTS = 2; // There are 2 input streams JoinQBox::LEFT && RIGHT

    void handle_input_tuple(const Tuple& tuple, int stream_id, bool& outputted);

    // Boundary
    void handle_boundary(const Tuple& left_tuple, const Tuple& right_tuple);
    //void handleBoundary(const Tuple& left_tuple, int stream_id, bool& outputted);
    void pass_through(const Tuple& tuple);
    Timestamp find_min_timestamp(int stream_id);
    int32 find_oldest_tuple_id(int stream_id);
    void emit_boundary_tuple(Timestamp min_timestamp);

    // Undo-redo recovery
    void handle_undo();
    void scan_for_undo(int stream_id, bool& has_undo, int32& undo_id);
    void dequeue_until_undo(int stream_id, int32 undo_tuple_id);
    void check_iterator(int stream_id);
    void undo_state(bool has_undo[], int32 undo_tuple_id[]);
    StateBound find_bound(TupleQueue::RSeekIterator ri[], bool has_undo[], int32 undo_tuple_ids[]);
    int rollback(TupleQueue::RSeekIterator ri[], int32 bound_ids[]);
    void clear_state();
    void rollforward(TupleQueue::RSeekIterator ri[], int counter);

    /// Emit a tuple of the given type
    void emit_tuple_with_different_type(const Tuple& undo_tuple, TupleType type);

    /// Emit a tuple as an UNDO and a REDO
    void emit_undo_tuple(const Tuple& tuple);

    // Normal operation
    void store_tuple_for_undo(const Tuple& tuple, int stream_id);
    void save_state_bound(const Tuple& tuple, int stream_id);
    void cut_history(int stream_id);
    void process_tuple(const Tuple& tuple, int stream_id, bool& outputted);

    int _output_tuple_size;
    int _input_tuple_size[TWO_INPUTS];
    ptr<TupleQueue> _input_history[TWO_INPUTS];
    int _max_history; // Maximum undo/redo size
    ptr<PagePool> _pool; // Small page pool because TupleQueues use page pools

    bool _boundary[TWO_INPUTS]; // We wait to receive both boundarys before producing one ourselves
    Timestamp _boundary_time[TWO_INPUTS]; // Timestamp received as boundary

    // Information about the state of the join
    size_t _buffer_size[TWO_INPUTS];

    typedef map< int32, StateBound > tuple_state_bound;
    tuple_state_bound _state_bounds;
    //int32 _oldest_id[TWO_INPUTS];

    int _state_bound_interval;
    Timestamp _last_state_bound_time;
    MicroBenchmarkType _benchmark;

    AURORA_DECLARE_QBOX(ExperimentalJoinQBox, "experimentaljoin");

    // Temporary for experiments
    ofstream _log_file;
    vector<long long> _measurements; //Sorted list of measurements
};


BOREALIS_NAMESPACE_END;

#endif // SJOIN_QBOX_H
