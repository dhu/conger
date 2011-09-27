#ifndef  AURORANODEMONITOR_H
#define  AURORANODEMONITOR_H

#include  "Stream.h"
#include  "StatsMgr.h"
#include  "TSStats.h"

BOREALIS_NAMESPACE_BEGIN

#define COLLECT_STATISTICS
#define COLLECT_OUTPUT_STATISTICS
//#define CASE_SPECIFIC_HACK
//#define THROUGHPUT_STATS

class AuroraNodeMonitor
{
  public:

    // this flag is used to tell Aurora that it has to start measuring
    // throughput for a certain period.
    //
    bool     _measure_throughput;
    uint32   _measure_period;

    void measure_throughput(uint32 period)
    {
        _measure_throughput = true;
        _measure_period = period;
        NOTICE << "Start measuring throughput...";
    }

    void measure_throughput()
    {
        _measure_throughput = true;
        NOTICE << "Start measuring throughput...";
    }


    /// qp runs this before termination.
    ///
    void  done(AuroraNode  *node);


    // _done flag is used to detect that Aurora has finished processing all
    // tuples in all of its queues.
    // -- tatbul
    //
    bool  _done;


    void set_done(bool flag) // scheduler sets this
    {   _done = flag;
    }


    //#ifdef THROUGHPUT_STATS
    // Everything is per output stream.
    //
    map<string, ptr<ofstream> >  _stats_files;
    map<string, int >            _tuple_counts;
    map<string, double >         _tuple_latencies;
    map<string, Time >           _start_times;
    map<string, double >         _total_times;
    ptr<ofstream>                _total_stats_file;
    //
    //#endif


    //#ifdef CASE_SPECIFIC_HACK
    //
    /// I am writing some stats to this file for experiments.
    ///
    ptr<ofstream> _stats_file;
    ptr<ofstream> _stats_file_key;
    map<string, ptr<ofstream> > _stats_file_staleness; // one for each key


    /// please ignore this hack. it is implemented until the
    /// stats manager implements a removeTSStat
    ///
    bool                  _add_stats;

    /// Stats I need to track about the output.
    ///
    bool                  _first_out;       // if any output has been produced.
    Time                  _last_out_time;   // the last time for output (usec).
    map<string, Time>     _last_out_times;  // last output times by key (usec).
    map<string, queue<pair<int32, timeval> > > _time_queue_map; // (usec).
    map<string, double >  _staleness;       // total staleness so far   (sec).
    map<string, double >  _key_time;        // total time per key so far(sec).
    map<string, double >  _prev_delta;      // previous staleness level (sec).
    double                _total_time;      // total time passed so far (sec). 
    double                _latency_sum;
    double                _latency_count;
    int                   _output_count;
    map<string, double >  _lat_sum;   // per output stream
    map<string, int >     _lat_count; // per output stream
    map<string, int32>    _last_win_entry; // the last win arrived for this key.
    int32                 _win_size, _win_slide;
    bool                  _no_window;
    map<string, double >  _latency; // total per-key latency so far (sec).
    map<string, double >  _count;   // total per-key count so far   (sec).
    //
    // #endif


    void  done_statistics();


    //#ifdef THROUGHPUT_STATS
    //
    void  done_throughput_statistics();


    //#ifdef CASE_SPECIFIC_HACK
    void  done_case_specific();

    void  finish();


    void  node_statistics();
    void  node_throughput_statistics();
    void  node_case_specific();

    void  dequeue_throughput_statistics(string       stream_name,
                                        Time         current_time,
                                        double       latency);


    void  enqueue_case_specific(char    *tup,
                                Stream  *stream,
                                timeval  now);

    void  dequeue_statistics(void              *dqh,
                             const void        *tuple,
                             TupleDescription  &td,
                             string             stream_name,
                             bool               output_stream);

    void  dequeue_case_specific(string            &stream_name,

                                //(char *)(deq.tuple());
                                char              *tup,

                                Time               current_time,

                                timeval            in_time,

                                TupleDescription  &td,

                                double             latency);

    void  dequeue_output_statistics(const void        *tuple,
                                    TupleDescription  &td,
                                    size_t             tuple_size,
                                    string            &stream_name,
                                    bool               output_stream);

    void  dequeue_output_statistics3(AuroraNode  *_node,
                                     void        *context,
                                     TupleQueue  *q,
                                     bool         output_stream);

    void  modify_run_statistics(AuroraNode  &_node,
                                IDVect      &_instream_ids,
                                IDVect      &_anystream_ids,
                                IDVect      &_outstream_ids,
                                IDVect      &_selectivity_ids,
                                IDVect      &_stream_ids,
                                IDVect      &_box_ids,
                                StatsMgr    &_sm);


    void  unsubscribe_run_statistics(void  *context,
                                     AuroraNode  *_node,
                                     string  _stream_name);

    void  dequeue_subscribe_statistics(AuroraNode  *_node,
                                       void  *context,
                                       string  stream_name);

    void  create_add_statistics(string     box_name,
                                string     input_name,
                                uint16     in_port,
                                IDVect    &instream_ids,
                                IDVect    &anystream_ids,
                                IDVect    &selectivity_ids);

    void  create_add_statistics5(string          box_name,
                                 CatalogStream  *stream,
                                 uint16          out_port,
                                 vector<IDVect> &outstream_ids,
                                 IDVect         &selectivity_ids);

    void  create_add_statistics6(IDVect         &ids,
                                 IDVect         &instream_ids,
                                 IDVect         &anystream_ids,
                                 IDVect         &selectivity_ids,
                                 vector<IDVect> &outstream_ids,
                                 vector<IDVect> &new_stream_ids,
                                 StatsMgr       &sm);


    /// Adds latency statistics for an output stream
    /// used for the move box case
    ///
    void  add_out_stream_stats(// DeqContexts
                               void      *deqs,

                               StatsMgr  &sm,

                               set<Name>  streams);

    void  cpview_statistics(Name        cp_name,
                            StatsMgr   &sm);

    void  cpview_statistics9(Name       sw_name,
                             StatsMgr  &sm);

};

BOREALIS_NAMESPACE_END

#endif                      // AURORANODEMONITOR_H
