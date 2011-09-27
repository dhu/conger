#ifndef SUNION_QBOX_H
#define SUNION_QBOX_H

#include "Bucket.h"
#include "UnionQBox.h"
#include "JoinQBox.h"
#include <vector>
#include "SControlQBox.h"

BOREALIS_NAMESPACE_BEGIN


// --------------------------------------------------
/**
 * Serializing union operator.
 * Note: I should turn some of these methods into utilities. Methods
 * such as emitting undo tuples or other types of control tuples.
 * Move these methods to QBox.
 */
class SUnionQBox : public SControlQBox
{

    friend class SUnionBoxNoBucketState;
    friend class SUnionBoxState;

 public:

    /// Creates the schema for the output stream with control messages
    static TupleDescription create_control_output_description();

 protected:
    virtual void setup_impl() throw (AuroraException);
    virtual void init_impl() throw (AuroraException);
    virtual void run_impl(QBoxInvocation&) throw (AuroraException);
    void pure_union_run_impl(QBoxInvocation&) throw (AuroraException);

    // packing and unpacking states
    void set_pending_box_state(ptr<AbstractBoxState> packed_box);
    virtual bool is_stable_for_checkpoint();
    virtual bool do_unpack_queues();

    //ptr<AbstractBoxState> pack_state();
    //void unpack_state(ptr<AbstractBoxState> box_state);

    ptr<AbstractBoxState> pack_state(bool delta_only = false);
    void unpack_state(ptr<AbstractBoxState> box_state);

 private:

    // XXX No good reason why this specific value, we just want to avoid hogging the CPU too long
    static const int MAX_BUCKETS_PER_ITERATION = 5;

    // We block for _max_delay = ALPHA_BLOCK * max-delay
    static const double MAX_ALPHA = 0.9; // For regular blocking

    // Once we are in Tentative state, then we only block for a very short time
    static const double MIN_ALPHA = 350; // Constant so it doesn't go too low when we modify the delay
    
    // Once a bucket is processed within SMALL_DELAY msec of its creation, then we did catch up
    static const int SMALL_DELAY = 200;

    /// Initialize box parameters such as _interval and _max_delay
    void setup_parameters();

    /// Processes an input tuple
    void handle_input_tuple(const Tuple& tuple, int stream_id);

    /// For sunions in the MIDDLE of the diagram, process a REC_DONE tuple received from upstream
    void process_rec_done(const Tuple& tuple, int stream_id);

    // Reconcile input as per corrections from upstream node
    // void apply_undo(const Tuple& tuple, int stream_id, long long interval_start);

    /// Launches undo-redo reconciliation
    void undo_redo(long long interval_start);

    /// Clear any remaining tentative tuples in any bucket
    void clear_all_tentative_tuples();

    /// Handles an input tuple
    void insert_tuple_into_bucket(const Tuple& tuple, int stream_id, long long interval_start);

    /// Save the last stable tuple so we can produce UNDO even if we forwardWithoutWaiting
    void save_last_stable(const Tuple& tuple);
    ptr<dynbuf> _last_tuple;

        
    /// Returns true if we should forward tuples as they arrive rather than sorting them
    //bool forward_without_waiting();

    /// If necessary, produces output tuples and control tuples.
    void produce_output();
    bool process_output_stable_bucket(Bucket& bucket, long long interval_nb);
    bool process_output_un_stable_bucket(Bucket& bucket, long long interval_nb);
    void emit_tuples(Bucket& bucket, TupleType::Type tuple_type);

    /// If re-processing tuples, check when we are done
    void check_for_end_of_reconciliation();

    /// Periodically produce boundary tuples
    void produce_boundary(Timestamp boundary_time);

    /// Periodically checkpoint state by remembering the interval to send
    void checkpoint();

    /// Converts a timestamp to a bucket nb
    long long timestamp_to_interval_nb(Timestamp timestamp);

    /// Converts a bucket number to the timestamp of the beginning of the bucket
    Timestamp interval_nb_to_timestamp(long long interval_nb);

    void garbage_collect(long long interval_nb);


    // The operator is a state machine
    struct State
    {
        public:
        /// Initial State of operator is STABLE_STATE
        State () : _state_value(STABLE_STATE), _tentative_start(Timestamp::now()),
                   _tentative_interval_nb(0),_overwrite_tentatives(false) {}

        /// Re-initialize the state machine
        void reset() { _state_value = STABLE_STATE; }

        /// True if the current state is STABLE_STATE
        bool is_stable() { return _state_value == STABLE_STATE; }

        /// True if the current state is TENTATIVE_STATE
        bool is_tentative() { return _state_value == TENTATIVE_STATE; }

        /// We can handle only one failure at the time, so cannot process tentative in all cases
        bool can_process_tentative();

        /// Updates state when operator needs to proceed with tentative tuples.
        void processing_tentative_bucket(Timestamp tentative_start, long long interval_nb);

        void undoing();

        /// Updates state when some buckets are full of stable tuples and are bounded
        void new_stable_bucket();

        /// Returns true if need to reconcile something
        bool something_to_correct();
        Timestamp correct_since_when(); // Time of beginning of unstability
        long long first_tentative_interval(); // First tentative bucket

        /// Requested reconciliation
        void reconciling();
        bool is_reconciling();
        void reconciling_rollforward();
        bool is_reconciling_forward();
        void reconciling_interrupted();
        bool is_reconciling_interrupted();

        /// Reconciliation is done
        void reconciled();

        /// SUnion can be forced to never recncile its state
        void no_reconciliation();

        /// During stabilization of input streams, we get both INSERTION and TENTATIVE tuples
        void start_overwrite_tentatives();
        void stop_overwrite_tentatives();
        bool get_overwrite_tentatives();

        private:
        static const int STABLE_STATE = 0;      // Produce tuples in their final form
        static const int TENTATIVE_STATE = 1;    // Produce tuples to avoid delaying
        static const int UNDONE_STATE = 2;     // In the process of undoing/redoing buffer content
        static const int CONVERGED_STATE = 3;   // Produce correct tuple but left-over incorrect tuples remain
        static const int RECONCILING_STATE_ROLLBACK = 4; // Correcting previous state (rolling back)
        static const int RECONCILING_STATE_ROLLFORWARD = 5; // Correcting previous state (rolling forward)
        static const int RECONCILING_STATE_INTERRUPTED = 6; // Failures during reconciliation

        int _state_value;
        Timestamp _tentative_start;
        long long _tentative_interval_nb;
        bool _overwrite_tentatives;

    };
    State _state;


    ///////////////////////////////////////////////////////////////////////////
    /// Fields below
    ///////////////////////////////////////////////////////////////////////////

    static const long MILLION = 1000000;
    static const long THOUSAND = 1000;

    /// Number of inputs to this union
    unsigned int _number_of_inputs;

    /// Number of outputs from this union
    unsigned int _number_of_data_outputs;

    /// The sizes of all the input streams
    vector<size_t> _data_input_tuple_sizes;

    /// Accumulate inputs over this interval before sorting them an unioning them
    int _interval;  // In msecs (size of buckets)

    /// Maximum delay for which we can buffer tuples before sending them out
    map<int,int> _max_msec_delays; // In msecs (max time we can wait for input tuples
    map<int,double> _actual_msec_delays; // In msecs (time we are actually going to wait for)
    bool _stagger; // Should we accelerate

    // Important parameter. Is this SUnion located on an input stream or not?
    bool _is_input_sunion;

    /// Keeping track of the number of REC_DONE tuples received from upstream (and their timestamps)
    map<int,Timestamp> _input_rec_done;
    Timestamp _max_input_rec_done_timestamp;

    /// If we get an UNDO and corrections, we may start to reconcile right away
    /// but we have to wait for a REC_DONE before we can claim that we are done reconciling
    map<int,bool> _expect_rec_done;


    // There is one bucket per time period. Periods (or intervals) are in msecs, same unit as _interval
    typedef map<long long, ptr<Bucket> > Buckets;

    // Set of buckets
    Buckets _buckets;
    long long _current_interval_nb;

    /// Remember the last input tuple that was labeled as stable
    //int32 _last_input_stable_tuple_id[];

    // Checkpointed state
    long long _checkpointed_interval_nb;

    // The benchmark that we are running
    MicroBenchmarkType _benchmark;

    // If there's a single input stream, should we sort or not?
    //bool _sort;

    /// For testing, we manually prevent some nodes from reconciling their state
    bool _should_reconcile;
};

/**
 * Captures the state of a sunion operator for INPUT sunions.
 * Currently, we only use this for checkpoint recovery and not to move
 * load, so we only save the number of the current interval
 */
class SUnionBoxNoBucketState : public AbstractBoxState
{
 public:
    SUnionBoxNoBucketState() : _interval_nb(INIT) {}
    SUnionBoxNoBucketState(long long interval_nb) : _interval_nb(interval_nb) {}
    ~SUnionBoxNoBucketState() {}

    long long get_interval_nb() { return _interval_nb; }
    NMSTL_SERIAL_SUBCLASS(SUnionBoxNoBucketState, AbstractBoxState, << _interval_nb);

 private:
    long long _interval_nb;
};


/**
 * Truely captures the state of an SUnion operator
 */
class SUnionBoxState : public AbstractBoxState
{
 public:
    SUnionBoxState() : _interval_nb(INIT) {}
    SUnionBoxState(long long interval_nb, SUnionQBox::Buckets& buckets);
    ~SUnionBoxState( ) {}

    long long get_interval_nb() { return _interval_nb; }
    void reset_buckets(SUnionQBox::Buckets& buckets);
    NMSTL_SERIAL_SUBCLASS(SUnionBoxState, AbstractBoxState, << _interval_nb);

 private:
    long long _interval_nb;
    typedef  vector< pair<long long, Bucket> > PackedBuckets;
    PackedBuckets _packed_buckets;
};


BOREALIS_NAMESPACE_END

#endif
