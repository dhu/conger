#ifndef BUCKET_H
#define BUCKET_H

#include "Tuple.h"
#include "Stream.h"

BOREALIS_NAMESPACE_BEGIN

/**
 * The Bucket class represents a bucket of tuples
 * It starts empty and then we can add tuples to it.
 *
 * Each bucket accumulates tuples for a set of streams
 * The bucket holds tuples in one TupleBuffer per stream
 *
 * A bucket is tentative if the most recent tuples on any
 * of its streams are tentative.
 *
 * A bucket is bounded if all its streams are bounded
 */
class Bucket
{

 public:

    Bucket(int nb_streams, vector<size_t> tuple_sizes);

    /// Returns a deep copy of this bucket
    Bucket deep_copy();

    /// Bound a stream
    void bound(int stream_id);

    /// Insert a tuple on a stream
    void insert_tuple(int stream_id, const Tuple& data);

    /**
     * Insert a tuple on a stream but overwrite tentative tuples when inserting stable ones
     * and don't insert tentative ones when stable ones are already there.
     */
    void insert_tuple_overwriting(int stream_id, const Tuple& data);

    /// Delete all tentative tuples 
    void clear_all_tentative_tuples();

    /// Delete all tuples from the bucket
    void clear_all();

    /// Delete all tuples from the buffer associated with given stream
    void clear_stream(int stream_id);

    /// Delete only tuples that follow the given one
    void clear_stream_after(int stream_id, int tuple_id);

    /**
     * Returns the last tuple the wat emitted by emit_tuples
     * or the tuple that we expect to be emitted last. The latter
     * works when tuples are sorted within each buffer
     */
    const Tuple get_last_tuple();

    /**
     * Emit all tuples on the output. 
     * Adjust their types (STABLE or TENTATIVE) to the one given in parameter.
     * Assuming tuples are sorted within each buffer, interleave the tuples
     * on the output by increasing tuple_stime values. If they 
     * are not ordered then the output simply won't be ordered either
     */
    void emit_tuples(Stream::EnqIterator out, TupleType::Type tuple_type);

    /**
     * Emits all tuples assuming there is one output stream per
     * buffered stream. Also assumes that tuples are sorted on each stream
     */
    void emit_tuples_on_separate_outputs(vector<Stream::EnqIterator>& outs, TupleType::Type tuple_type);

    /// Assumes tuples are sorted within each stream buffer!
    int find_highest_index();

    /**
     * Returns index of stream holding tuples with the lowest tuple_stime
     * or -1 if all tuples have already been emitted.
     */
    int find_lowest_index();
    Timestamp find_second_lowest_value(int lowest_index, Timestamp lowest_value);

    /**
     * Verifies if tuples on any stream have been buffered longer than the 
     * corresponding maximum delay. Returns true if any tuples have been
     * held too long on any stream.
     * Assumes the integer stream identifiers match in _buffers and max_msec_delays
     */
    bool is_expired(map<int,double>& max_msec_delays);

    /// If a bucket is empty but we get data for another bucket, we use that data to
    /// determine if the bucket expired
    void set_timer(int stream_id);

    /// Return the time when the first tuple got inserted into this bucket. Timestamp::now() if empty
    Timestamp get_first_tuple_time();

    /// Are all stream bounded?
    bool is_bounded();

    /// Is the given stream bounded?
    bool is_bounded(int stream_id);

    /// Are there no tuples in the bucket?
    bool is_empty();

    /// Does the bucket contain any tentative tuples?
    bool is_tentative();

 private:

    void reset_stability();
    //void update_last_emitted_tuple(const Tuple& src_tuple);

    /**
     * Just a simple buffer for tuples. Grows automatically. XXX should add iterators.
     */
    class TuplesBuffer
    {
    public:

        static const int DEFAULT_SIZE = 1000;

        TuplesBuffer(size_t tuple_size);
        size_t tuple_size() { return _tuple_size; }
        bool is_empty();
        bool is_expired(double max_msec_delay);
        void set_timer();
        Timestamp get_first_tuple_time();
        void insert_tuple(const Tuple& data);
        void start_timer(int stream_id);
        int nb_tuples() { return _cur_nb_tuples; }
        const Tuple tuple_at(int i);

        /**
         * Returns the tuple at _lowest_index. 
         * Pre-condition: there are tuples left to emit.
         */
        const Tuple lowest_tuple();

        /**
         * Returns the tuple_stime value of tuple at _lowest_index. 
         * Pre-condition: there are tuples left to emit.
         */
        const Timestamp lowest_value();

        /// Returns true if there are tuples left to emit from buffer
        bool more_tuples();

        /**
         * Advances the index of the lowest tuple if there are more tuples
         * in the buffer. Leave the index unchanged once we reach the end
         * of the buffer
         */
        void advance_lowest();

        void reset_lowest() { _lowest_index = 0; }

        /**
         * Returns the last tuple in the buffer
         * Precondition: buffer must not be empty.
         */
        const Tuple highest_tuple();

        /**
         * Returns the tuple_stime value of the last tuple in the buffer
         * Precondition: buffer must not be empty.
         */
        const Timestamp highest_value();

        /// Deletes all tuples from this buffer
        void clear();

        /// Deletes all tuples following the one with given tuple id
        void clear_after(int tuple_id);

        /// Delete all tuples following the first tentative one
        void clear_tentative();

        /// Buffer is tentative if earliest tuple is tentative. It is stable otherwise
        bool is_stable_buffer();

    private:
        Timestamp _first_tuple_time;
        bool _timer_set;
        size_t _tuple_size;
        int _cur_nb_tuples;
        int _max_nb_tuples;
        int _lowest_index;
        ptr<dynbuf> _tuples;
    };


    /// A bucket associates windows of tuples received with their input stream (identified by port nb)
    typedef map<int, ptr<TuplesBuffer> > TuplesBufferSet;
    TuplesBufferSet _buffers;
    map<int, bool> _boundaries;

    bool _contains_tentative_tuples;
    bool _contains_tuples;
    vector<size_t> _tuple_sizes;
    bool _valid_last_emitted_tuple;
    ptr<dynbuf> _last_emitted_tuple;

};

BOREALIS_NAMESPACE_END

#endif
