
#ifndef UPDATE_QUEUE_H
#define UPDATE_QUEUE_H

#include <common.h>
#include "TupleDescription.h"
#include <set>

#define RANDOM_NUMBER_COUNT 1000

BOREALIS_NAMESPACE_BEGIN;

class Stream;
class TupleQueue;

// Provides an index on top of the regular append queue stored as a TupleQueue.
// An update queue is one where new values overwrite old values. 
// It should be used when the application is only interested in the most 
// recent value, not all values. 
// If query is a windowed one with aggregates, then window integrity is
// preserved. -- tatbul
//
class UpdateQueue
{
  public:

    UpdateQueue() : _q(0) 
    {
        _head = NULL;
        _tail = NULL;
        _prior = NULL;
        _hash_index.clear();

        srand48(time(NULL));
        srandom(time(NULL));
        _random_numbers.clear();

        _garbage.clear();
    }

    // Linked-list of pointers to tuples that we want the operator to 
    // actually process. 
    //
    class DataCell
    {
      public:

        DataCell() 
        {
            _data_location = NULL;
            _prev_cell = NULL;
            _next_cell = NULL;
        }
        ~DataCell() 
        {
        }

        string as_string(const TupleDescription td) const
        {
            if (_data_location)
            {
                return td.tuple_to_string(_data_location);
            }

            return string();
        }

      public:

        void     *_data_location; // location in the corresponding tuple queue
        DataCell *_prev_cell;     // previous cell to be processed by the box 
        DataCell *_next_cell;     // next cell to be processed by the box 
    };

    // Queue information for a specific key group.
    //
    class WindowQueue
    {
      public:

        WindowQueue(string key_value)
        {
            _recent_win_start = NULL;
            _keep_until = 0;
            _key_value = key_value;
        }
        ~WindowQueue()
        {
        }

      public:

        DataCell    *_recent_win_start; // keep this win unless new one comes
        int32       _keep_until;        // don't drop any tuples until this
        string      _key_value;
        timeval     _arrival_time;      // earliest arrival time for this group
    };

    // Hash table keeping one window queue per key value.
    //
    typedef map<string, WindowQueue*> WindowQueueMap; // key value as string

    typedef map<string, double> AccessProbMap;

    typedef enum QueueType {FIFO, 
                            INPLACE, 
                            LINECUT_UF_FRONT, 
                            LINECUT_UF_MIDDLE, 
                            LINECUT_DF_FRONT, 
                            LINECUT_DF_MIDDLE, 
                            LINECUT_UF_MIDDLE_DETERMINISTIC, 
                            LINECUT_DF_MIDDLE_DETERMINISTIC,
                            LINECUT_PRIOR};

    void enqueue_fifo();    // must be called for each enq'ed tuple (FIFO)
    void enqueue_inplace(); // must be called for each enq'ed tuple (INPLACE)
    void enqueue_linecut(); // must be called for each enq'ed tuple (LINECUT_*)
    void *dequeue();        // must be called for each deq'ed tuple (*)
    void dequeue_prior();   // must be called for each deq'ed tuple 
                            // (LINECUT_PRIOR)

    bool is_empty();

    void set_field_info(Stream *stream);

    const void *get_head_location() { return _head->_data_location; }

    const void *get_prior_location();

    QueueType get_type() { return _type; }

    //typedef queue<double> TimeQueue;
    //typedef map<string, TimeQueue> TimeQueueMap;
    typedef map<string, int> TimeCountMap;

    double get_update_frequency(string key);
    string get_max_freq_key();

    int partition(vector<double> *v, vector<string> *s, int p, int r);
    void quick_sort(vector<double> *v, vector<string> *s, int p, int r);

    //string pick_drop_key();
    //string pick_random_drop_key();
    //void drop_window(string key);

    DataCell *linecut_uf_front(string key);
    DataCell *linecut_uf_middle(string key);
    DataCell *linecut_df_front(string key);
    DataCell *linecut_df_middle(string key);
    DataCell *linecut_uf_middle_deterministic(string key);
    DataCell *linecut_df_middle_deterministic(string key);

    string as_string();

    set<void *> _garbage;

  private:

    explicit UpdateQueue(TupleQueue &q, int queue_type, string key_field, 
                         string window_field) : _q(&q) 
    {
        _head = NULL;
        _tail = NULL;
        _prior = NULL;
        _hash_index.clear();

        _type = (QueueType)queue_type;
        if ((_type < 0) || (_type > 5))
        {
            WARN << "Invalid update queue type : " << (int)_type;
        }

        _key_field = key_field;
        _window_field = window_field;

        //_update_times.clear();
        
        // UF stats.
        _update_counts.clear();
        _total_update_count = 0;
        _max_update_count = 0;

        // DF stats.
        _win_dequeue_counts.clear();
        _total_dequeue_count = 0;
        _max_dequeue_count = 0;

        _num_queued_windows = 0;
        
        /*
        _stats_window = 10000;   // msec
        _stats_start_time = static_cast<double>(Time::now().to_msecs());
        _max_num_windows = 10;   // constant for now
        _prob_high = 0.8;
        _prob_low = 0.2;
        */

        srand48(time(NULL));
        srandom(time(NULL));
        _random_numbers.clear();

        _garbage.clear();
    }

    string _key_field;    // name of the key field
    uint16 _key_offset;
    uint16 _key_size;

    string _window_field; // name of the windowing field
    uint16 _window_offset;
    uint16 _window_size;

    WindowQueueMap _hash_index; // access by key

    AccessProbMap  _access_prob; // access probability of a given key

    DataCell *_head;   // operator must dequeue from the head
    DataCell *_tail;   // the most recently enqueued tuple
    DataCell *_prior;  // the highest priority tuple at the time of read

    QueueType _type; // FIFO, INPLACE, or LINECUT_*

    /*
    // load shedding parameters and stats
    //
    int    _max_num_windows; // we allow this many windows waiting in the Q
    double _stats_window;    // we collect stats for a recent time window
    double _stats_start_time; // when do we start collecting stats?
    double _prob_high;
    double _prob_low;
    //TimeQueueMap _update_times; // update times of each key group
    */

    int    _num_queued_windows;

    TimeCountMap _update_counts; // number of win updates per key group
    int    _total_update_count;
    int    _max_update_count;

    TimeCountMap _win_dequeue_counts; // number of win dequeues per key group
    int    _total_dequeue_count;
    int    _max_dequeue_count;

    map<string, queue<double> > _random_numbers;

    TupleQueue *_q; // pointer to the actual tuple queue
    friend class TupleQueue;
};

BOREALIS_NAMESPACE_END;

#endif

/*
Assumptions: 
Every tuple that arrives has a win_spec, marked by a preceeding WindowDrop.
If the winspec = 0 or -1, we do normal enqueue.
If the winspec > 0, there is opportunity to eliminate unnecessary tuples.
Principles:
1. Must keep all tuples that belong to a window which has already started.
2. Must keep tuples for the most recent new window.
3. Must keep tuples that are already marked (0, fake).
4. If a window is marked 0, it stays 0.
*/
