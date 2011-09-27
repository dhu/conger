#include  "AuroraNode.h"
#include  "AuroraNodeMonitor.h"
#include "LockHolder.h"

#include <fstream>
#include <sys/time.h>

BOREALIS_NAMESPACE_BEGIN

//static const int STATISTICS_WINDOW_SIZE = 10;
//static const int STATISTICS_WINDOW_PRECISION = 1;
static const int STATISTICS_WINDOW_SIZE      = 5;
static const int STATISTICS_WINDOW_PRECISION = 1;


typedef map<string, ptr<Stream> > StreamMap;


////////////////////////////////////////////////////////////////////////////////
//
// qp checks this before termination.
// When running experiments this can be used to make sure one experiment
// completes (the queues are empty) before terminating it.
//
void  AuroraNodeMonitor::done(AuroraNode  *node) // qp checks this before termination
{
    bool  done;

    StreamMap::const_iterator i;
//..............................................................................


    StreamMap  &streams = node->get_streams();

    while(1)
    {   if (_done)
        {   // Scheduler is not a reliable source.
            // Double-check that the tuple queues are all empty.
            //
            done = true;

            for (i = streams.begin(); i != streams.end(); ++i)
            {
                if (!(i->second->all_queues_empty()))
                {
                    done = false;
                    break;
                }
            }
        }

        if (!_done  ||  !done)
        {   DEBUG << "Aurora is not done, check back 1 minute later...";
            Thread::sleep(Time::msecs(60000));
        }
    }

    // #ifdef THROUGHPUT_STATS
    // #ifdef CASE_SPECIFIC_HACK
    done_statistics();

    return;
}


////////////////////////////////////////////////////////////////////////////////
//
// from AuroraNode.h
//
void  AuroraNodeMonitor::done_statistics()
{
//..............................................................................


#ifdef THROUGHPUT_STATS
    done_throughput_statistics();
#endif

#ifdef CASE_SPECIFIC_HACK
    done_case_specific();
#endif

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//#ifdef THROUGHPUT_STATS   // from AuroraNode.h
//
void  AuroraNodeMonitor::done_throughput_statistics()
{
    map<string, int >::iterator  iter;

    string  stream_name;
    double  avg_throughput;
    double  avg_latency;
//..............................................................................


    for (iter = _tuple_counts.begin(); iter != _tuple_counts.end(); iter++)
    {
        stream_name    = iter->first;
        avg_throughput = double(iter->second) / _total_times[stream_name];
        avg_latency    = _tuple_latencies[stream_name] / double(iter->second);

        *(_stats_files[stream_name]) << "number of tuples = "
                                     << iter->second << endl;

        *(_stats_files[stream_name]) << "total time = "
                                     << _total_times[stream_name] << endl;

        *(_stats_files[stream_name]) << "avg latency = "
                                     << avg_latency << endl;

        *(_stats_files[stream_name]) << "avg throughput = "
                                     << avg_throughput << endl;
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//#ifdef CASE_SPECIFIC_HACK  // from AuroraNode.h
//
void  AuroraNodeMonitor::done_case_specific()
{
    double  stale_sum   = 0;
    double  late_sum    = 0;
    int     stale_count = 0;

    map<string, double >::iterator iter;
//..............................................................................


    // Record the final stats summary.
    //
    double avg_latency = 0, avg_staleness = 0, avg_latency_key = 0;

    if (_latency_count > 0)
    {   avg_latency = _latency_sum/_latency_count;
    }

    (*_stats_file_key) << "# key count avg_tuple_latency avg_key_staleness" 
                       << endl;

    for (iter = _staleness.begin(); iter != _staleness.end(); iter++)
    {
        if (_key_time[iter->first] > 0)
        {
            stale_sum += (iter->second)/_key_time[iter->first];
            ++stale_count;

            assert(_count[iter->first] > 0);

            late_sum += _latency[iter->first]/double(_count[iter->first]);

            (*_stats_file_key) 
                << iter->first << " " 
                << _count[iter->first] << " " 
                << _latency[iter->first]/double(_count[iter->first]) << " "
                << (iter->second)/_key_time[iter->first] << endl;
        }
    }

    if (stale_count > 0)
    {   avg_staleness = stale_sum/stale_count;
        avg_latency_key = late_sum/stale_count;
    }

    (*_stats_file) << avg_latency << " " 
                   << avg_latency_key << " " 
                   << avg_staleness << " " 
                   << _output_count << endl;

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//#ifdef THROUGHPUT_STATS    // From AuroraNode.h
//
//  Use this whenever 
//
void  AuroraNodeMonitor::finish()
{
#ifdef THROUGHPUT_STATS

    map<string, int >::iterator iter;
    double   total_avg_throughput = 0;
    double   total_avg_latency = 0;
//..............................................................................


    NOTICE << "AuroraNode::finish()";

    for (iter = _tuple_counts.begin(); iter != _tuple_counts.end(); iter++)
    {
        string stream_name = iter->first;

        double avg_throughput = double(iter->second)/
                                _total_times[stream_name];

        total_avg_throughput += avg_throughput;

        double avg_latency = _tuple_latencies[stream_name]/
                             double(iter->second);

        total_avg_latency += avg_latency;

        *(_stats_files[stream_name]) << "number of tuples = "
                                     << iter->second 
                                     << endl;

        *(_stats_files[stream_name]) << "total time = "
                                     << _total_times[stream_name] 
                                     << endl;

        *(_stats_files[stream_name]) << "avg latency = "
                                     << avg_latency 
                                     << endl;

        *(_stats_files[stream_name]) << "avg throughput = "
                                     << avg_throughput 
                                     << endl;
    }

    *_total_stats_file << total_avg_throughput << " " << total_avg_latency
                       << endl;
#endif

    return;
}




////////////////////////////////////////////////////////////////////////////////
//
//#ifdef COLLECT_STATISTICS   // 1  AuroraNode::modify_network_task::run
//
void  AuroraNodeMonitor::modify_run_statistics(AuroraNode  &_node,
                                               IDVect      &_instream_ids,
                                               IDVect      &_anystream_ids,
                                               IDVect      &_outstream_ids,
                                               IDVect      &_selectivity_ids,
                                               IDVect      &_stream_ids,
                                               IDVect      &_box_ids,
                                               StatsMgr    &_sm)
{
#ifdef COLLECT_STATISTICS
//..............................................................................


    //if ( _node._monitor._add_stats && (_box_ids.size() != 0) )
    if ( _add_stats && (_box_ids.size() != 0) )
    {
        DEBUG <<"Added Cost +InputRate Stats" ;

        _sm.add_tsstats("Cost", STATISTICS_WINDOW_SIZE,
                        STATISTICS_WINDOW_PRECISION, _box_ids);

        _sm.add_tsstats("InputRate", STATISTICS_WINDOW_SIZE,
                        STATISTICS_WINDOW_PRECISION, _box_ids);
    }

    //need to add stats for streams here //olga
    if (_node._monitor._add_stats && (_instream_ids.size() != 0))
    {
        DEBUG <<"Added QLength stats" ;
        _sm.add_tsstats("QLength", STATISTICS_WINDOW_SIZE,
                        STATISTICS_WINDOW_PRECISION, _instream_ids);
    }

    if (_node._monitor._add_stats && (_anystream_ids.size() != 0))
    {
        _sm.add_tsstats("StreamRate", STATISTICS_WINDOW_SIZE,
                        STATISTICS_WINDOW_PRECISION, _anystream_ids);
    }

    // following olga's plan and adding selectivities here. -alexr
    if (_node._monitor._add_stats && (_selectivity_ids.size() != 0))
    {
        //DEBUG <<"Added Selectivity stats" ;
        _sm.add_tsstats("Selectivity", STATISTICS_WINDOW_SIZE,
                        STATISTICS_WINDOW_PRECISION, _selectivity_ids);
    }

    //need to add stats for streams here
    if (_node._monitor._add_stats && (_outstream_ids.size() != 0))
    {
        DEBUG <<"Add Latency Stats";
        _sm.add_tsstats("TupleNum", STATISTICS_WINDOW_SIZE,
                        STATISTICS_WINDOW_PRECISION, _outstream_ids);

        _sm.add_tsstats("OutLatency", STATISTICS_WINDOW_SIZE ,
                        STATISTICS_WINDOW_PRECISION, _outstream_ids);

        _sm.add_tsstats("OutLatencySqr", STATISTICS_WINDOW_SIZE ,
                        STATISTICS_WINDOW_PRECISION, _outstream_ids);
    }

    if (_node._monitor._add_stats && _stream_ids.size()!=0)
    {
        DEBUG << "Added StreamRate Stats";

        _sm.add_tsstats("StreamRate", STATISTICS_WINDOW_SIZE,
                        STATISTICS_WINDOW_PRECISION, _stream_ids);
    }
#endif

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//#ifdef COLLECT_STATISTICS   // 2  AuroraNode::unsubscribe_task::run
//
void  AuroraNodeMonitor::unsubscribe_run_statistics(// AuroraNode::DequeueContext
                                                    void    *context,

                                                    AuroraNode  *_node,

                                                    string   _stream_name)
{
#ifdef COLLECT_STATISTICS
     IDVect id;
//..............................................................................


    AuroraNode::DequeueContext *_context = (AuroraNode::DequeueContext *)context;

    // Delete stats collected within the dequeue thread.
    //
    if ( _context )
    {
        _context->_outlatency_sum.erase(_stream_name);
        _context->_outlatency_sumsqr.erase(_stream_name);
        _context->_tuplenum.erase(_stream_name);
        _context->_last_outlatency.erase(_stream_name);

        if (_node->get_stats_object().has_tsstats("OutLatency"))
        {
            id.push_back(_stream_name);

            _node->sm.delete_statistics("OutLatency",id);
            _node->sm.delete_statistics("OutLatencySqr",id);
            _node->sm.delete_statistics("TupleNum",id);
        }
    }
#endif

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  #ifdef THROUGHPUT_STATS   // AuroraNode::AuroraNode()
//
void  AuroraNodeMonitor::node_statistics()
{
//..............................................................................


    _done = false;
    _add_stats = true;
    _measure_throughput = false;


#ifdef THROUGHPUT_STATS
    node_throughput_statistics();
#endif

#ifdef CASE_SPECIFIC_HACK
    node_case_specific();
#endif

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  #ifdef THROUGHPUT_STATS   // AuroraNode::AuroraNode()
//
// This is used by Nesime's experiments.
//
void  AuroraNodeMonitor::node_throughput_statistics()
{
#ifdef THROUGHPUT_STATS
//..............................................................................


    _tuple_counts.clear();
    _tuple_latencies.clear();
    _start_times.clear();
    _total_times.clear();
    _total_stats_file.set(new ofstream("/tmp/borealis_stats_all.txt"));
#endif

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// ifdef CASE_SPECIFIC_HACK //1  AuroraNode::AuroraNode()
//
// This is used by Nesime's experiments.
//
void  AuroraNodeMonitor::node_case_specific()
{
//
// Stats will be written to a file in /tmp by default.
//..............................................................................


    _stats_file.set(new ofstream("/tmp/borealis_stats.txt"));
    _stats_file_key.set(new ofstream("/tmp/borealis_stats_key.txt"));
    _first_out = true;
    _last_out_times.clear();
    _time_queue_map.clear();
    _last_win_entry.clear();
    _no_window = true;

    char *win_size_c = getenv("WIN_SIZE");

    if (win_size_c)
    {   _win_size = atoi(win_size_c);
    }
    else
    {   _win_size = 1;
    }

    char *win_slide_c = getenv("WIN_SLIDE");

    if (win_slide_c)
    {   _win_slide = atoi(win_slide_c);
    }
    else
    {   _win_slide = 1;
    }

    char *win_flag_c = getenv("WIN_FLAG");

    if (win_flag_c)
    {   if (atoi(win_flag_c))
        {   _no_window = false;
        }
    }

    if (_no_window)
    {   assert((_win_size == 1) && (_win_slide == 1));
    }

    _staleness.clear();
    _key_time.clear();
    _latency_sum = 0;
    _latency_count = 0;
    _output_count = 0;
    _latency.clear();
    _count.clear();

    return;
}




////////////////////////////////////////////////////////////////////////////////
//
// ifdef CASE_SPECIFIC_HACK //2  AuroraNode::enqueue
//
// This is used by Nesime's experiments.
//
void  AuroraNodeMonitor::enqueue_case_specific(//(char *)(enq.tuple()
                                               char    *tup,

                                               Stream  *stream,

                                               timeval  now)
{
#ifdef CASE_SPECIFIC_HACK

    uint16  field_off, field_size;
    string  key="";
    int32   time_value = 0;
//
// This part is a case-specific hack.
// Just wanna keep track of all arriving tuples grouped by update key.
// It shouldn't break anybody else's query.
// Assumptions:
// - No gaps in "time" field.
// - Cumulative window size and slide are known (use env vars for now).
//..............................................................................


    const SchemaField *sf = (stream->get_tuple_description()).
                                get_schema_field("sensor_id");
    if (sf)
    {
        field_off = sf->get_offset();
        field_size = sf->get_size();
        key = string((char *)(tup + field_off), field_size);
        DEBUG << "received a tuple with key value = " << key;
    }

    sf = (stream->get_tuple_description()).get_schema_field("time");

    if (sf)
    {
        field_off  = sf->get_offset();
        field_size = sf->get_size();
        time_value = int32(*((int32 *)(tup + field_off)));
        DEBUG << "received a tuple with time value = " << time_value;
    }

    if ((key != "") && (time_value != 0))
    {
        if (_last_win_entry.find(key) == _last_win_entry.end())
        {
            _last_win_entry[key] = time_value;
        }

        if ((time_value == (_last_win_entry[key] + _win_size))  ||  _no_window)
        {
            pair<int32, timeval> time_pair;
            time_pair.first = _last_win_entry[key];
            time_pair.second = now;
            (_time_queue_map[key]).push(time_pair);

            DEBUG << "pushing the pair for time : " << time_pair.first;

            _last_win_entry[key] = _last_win_entry[key] + _win_slide;
        }
    }

#endif
    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// #ifdef COLLECT_OUTPUT_STATISTICS  // 1  2  AuroraNode::DequeueHandle::dequeue
//
void  AuroraNodeMonitor::dequeue_statistics( void  *dqh,

                                             const void        *tuple,

                                             TupleDescription  &td,

                                             string     stream_name,
                                             bool       output_stream)
{
#ifdef COLLECT_OUTPUT_STATISTICS

    timeval now;
//..............................................................................


    // temporarily using the local_timestamp field to track latencies
    // will remove later
    //
    gettimeofday(&now, 0);

    ///////////////////////////// ???hard coded offset???
    //((timeval*) (tuple + 1)->tv_sec  = now.tv_sec;
    //((timeval*) (tuple + 1)->tv_usec = now.tv_usec;

    Tuple  t = Tuple::factory(tuple);
    t.set_local_timestamp(now);
    
    DEBUG << "AURORA OUTPUTS: " << td.tuple_to_string(tuple);

    // olga calculate latency related statistics for the tuple;
    if (output_stream)
    {
        //DEBUG << "output stream " << stream->getName();
        timeval in_time = t.get_timestamp();

        // hard coded???
        //in_time.tv_sec  = ((timeval*)tuple)->tv_sec;
        //in_time.tv_usec = ((timeval*)tuple)->tv_usec;

        Time current_time = Time::now();
        double latency = static_cast<double>((current_time
                                                - Time(in_time)).to_usecs());

        DEBUG << "Latency = " << latency / 1000000.0;
        AuroraNode::DequeueHandle  *_dqh = (AuroraNode::DequeueHandle  *)dqh;
        _dqh->handle(Time(in_time));
#endif

        
#ifdef THROUGHPUT_STATS
        dequeue_throughput_statistics(stream_name, current_time, latency);
#endif


#ifdef CASE_SPECIFIC_HACK  // 3
        dequeue_case_specific(stream_name, (char *)tuple,
                              current_time, in_time, td, latency);
#endif

// 2
#ifdef COLLECT_OUTPUT_STATISTICS
         _dqh->get_context()->_outlatency_sum[stream_name] += latency / 1000000.0;

        _dqh->get_context()->_outlatency_sumsqr[stream_name] +=
                                 (latency/1000000.0) * (latency / 1000000.0);

        _dqh->get_context()->_tuplenum[stream_name]++;
    }
#endif

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  #ifdef THROUGHPUT_STATS   // AuroraNode::DequeueHandle::dequeue
//
void  AuroraNodeMonitor::dequeue_throughput_statistics(string   stream_name,
                                                       Time     current_time,
                                                       double   latency)
{
//..............................................................................


    if (_measure_throughput)
    {
        if (_stats_files.find(stream_name) == _stats_files.end())
        {
            string fname = string() + "/tmp/borealis_stats_" + stream_name;

            _stats_files[stream_name].set(new ofstream(fname.c_str()));

            *(_stats_files[stream_name]) << "# " << stream_name << endl;
            *(_stats_files[stream_name]) << "# time_passed    latency" << endl;

             _tuple_counts[stream_name] = 0;
             _tuple_latencies[stream_name] = 0;
             _start_times[stream_name] = current_time;
        }

        _tuple_counts[stream_name] += 1;
        _tuple_latencies[stream_name] += latency/1000000.0;

        double time_passed = (static_cast<double>((current_time -
                      _start_times[stream_name]).to_usecs()))/1000000.0;

        _total_times[stream_name] = time_passed;

        *(_stats_files[stream_name]) << time_passed << "    "
                                     << latency/1000000.0 << endl;

        //if (time_passed >= (double)_measure_period/1000.0)
        //{   _measure_throughput = false;
        //    NOTICE << "Stop measuring throughput...";
        //}
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
// ifdef CASE_SPECIFIC_HACK //3   AuroraNode::DequeueHandle::dequeue
//
void  AuroraNodeMonitor::dequeue_case_specific(string            &stream_name,

                                               //(char *)(deq.tuple());
                                               char              *tup,

                                               Time               current_time,

                                               timeval            in_time,

                                               TupleDescription  &td,

                                               double             latency)
{
//..............................................................................


    if (_stats_file)
    {
        if ((_lat_sum).find(stream_name) == (_lat_sum).end())
        {
            _lat_sum[stream_name] = 0;
            _lat_count[stream_name] = 0;
        }

        _lat_sum[stream_name] += latency / 1000000.0;
        ++(_lat_count[stream_name]);

        double running_avg = double(_lat_sum[stream_name])/
                             double(_lat_count[stream_name]);

        (*(_stats_file)) << stream_name << " "
                         << latency / 1000000.0 << " "
                         << _lat_count[stream_name] << " "
                         << running_avg << endl;

        // (*(_stats_file)) << td.tuple_to_string(deq.tuple()) << endl;
    }

    // Keeping track of some output stats for my experiments.
    // It is a case-specific hack,
    // but it shouldn't break others' queries.
    // -- tatbul
    //
    uint16  field_off, field_size;
    string  key = "";
    int32   time_value = 0;
    //char *tup = (char *)(deq.tuple());
    const SchemaField  *sf = td.get_schema_field("sensor_id");

    if (sf)
    {
        field_off  = sf->get_offset();
        field_size = sf->get_size();
        key = string((char *)(tup + field_off), field_size);
    }

    sf = td.get_schema_field("time");

    if (sf)
    {
        field_off = sf->get_offset();
        field_size = sf->get_size();
        time_value = int32(*((int32 *)(tup + field_off)));
    }

    if ((key != "") && (time_value != 0) && (_stats_file))
    {
        DEBUG << "new output = " << key << " " << time_value;

        // Initializations.
        //
        if (_last_out_times.find(key) == _last_out_times.end())
        {
            _last_out_times[key] = current_time;
            _staleness[key] = 0;
            _prev_delta[key] = 0;
            _latency[key] = 0;
            _count[key] = 0;

            string fname = string() + "/tmp/borealis_stats_stale_" + key;

            _stats_file_staleness[key].set(new ofstream(fname.c_str()));

            (*(_stats_file_staleness[key])) << "# time staleness" << endl;
            (*(_stats_file_staleness[key])) << "0 0" << endl;

            (*(_stats_file)) << "# time_passed tuple_latency key time_passed_key total_staleness_key total_time" << endl;
        }

        if (_first_out)
        {
            //_last_out_time = current_time;
            _last_out_time = Time(in_time);
            _first_out = false;
            _total_time = 0;
        }

        // Stats for the current output.
        //
        double time_passed = (static_cast<double>((current_time
                        - Time(_last_out_time)).to_usecs()))/1000000.0;

        DEBUG << "time_passed since last output = " << time_passed;

        double time_passed_key = (static_cast<double>((current_time
                        - Time(_last_out_times[key])).to_usecs()))
                                            /1000000.0;

        DEBUG << "time passed since last output of key \""
              << key << "\" = " << time_passed_key;

        _total_time += time_passed;
        _last_out_time = current_time;
        _last_out_times[key] = current_time;

        (*(_stats_file)) << time_passed << " ";
        (*(_stats_file)) << latency / 1000000.0 << " ";
        (*(_stats_file)) << key << " ";
        (*(_stats_file)) << time_passed_key << " ";

        _latency_sum += latency/1000000.0;
        ++(_latency_count);
        ++(_output_count);
        (_latency[key]) += latency/1000000.0;
        ++(_count[key]);

        // At least, the current output should be there.
        //
        assert(_time_queue_map[key].size() > 0);
        DEBUG << "q size = " << _time_queue_map[key].size();

        pair<int32, timeval> p;
        p = _time_queue_map[key].front();
        DEBUG << "q time = " << p.first;

        // Past arrivals that we may have missed.
        //
        timeval  missed;
        double   temp_stale = 0;

        if (p.first <= time_value)
        {
            missed.tv_sec  = p.second.tv_sec;
            missed.tv_usec = p.second.tv_usec;

            temp_stale = (static_cast<double>((current_time
                       - Time(missed)).to_usecs()))/1000000.0;

            assert(temp_stale >= 0);
        }

        double extra_area = temp_stale*temp_stale/2.0;

        if (!(_prev_delta[key] > 0))
        {
            _staleness[key] += extra_area;

            if (_total_time > temp_stale)
            {
                (*(_stats_file_staleness[key]))
                                << (_total_time - temp_stale) << " ";

                (*(_stats_file_staleness[key])) << " 0" << endl;
            }

            (*(_stats_file_staleness[key])) << _total_time << " ";
            (*(_stats_file_staleness[key])) << temp_stale << endl;
        }
        else
        {
            double area = ((time_passed_key + _prev_delta[key]
                                            + _prev_delta[key])/2.0)
                                            * time_passed_key;

            DEBUG << "area = " << area;
            _staleness[key] += area;
            (*(_stats_file_staleness[key])) << _total_time << " ";

            (*(_stats_file_staleness[key]))
                << time_passed_key + _prev_delta[key] << endl;
        }

        (*(_stats_file)) << _staleness[key] << " ";
        (*(_stats_file)) << _total_time << endl;

        _key_time[key] = _total_time;

        // Pop all the missed arrivals + the current tuple.
        //
        while (p.first <= time_value)
        {
            _time_queue_map[key].pop();

            if (_time_queue_map[key].size() > 0)
            {
                p = _time_queue_map[key].front();
                DEBUG << "q time = " << p.first;
            }
            else
            {   break;
            }
        }

        // Future arrivals that we may not have processed yet
        // (to determine our current staleness level).
        // If no such arrival, our staleness level drops to 0.
        //
        timeval next_ts;

        if (_time_queue_map[key].size() > 0)
        {
            p = _time_queue_map[key].front();
            DEBUG << "q time = " << p.first;

            next_ts.tv_sec  = (p.second).tv_sec;
            next_ts.tv_usec = (p.second).tv_usec;

            double delta = (static_cast<double>((current_time
                         - Time(next_ts)).to_usecs()))/1000000.0;

            DEBUG << "delta = " << delta;

            // delta must not be below 0.
            //
            if (delta < 0)
            {   _prev_delta[key] = 0;
            }
            else
            {   _prev_delta[key] = delta;
            }
        }
        else
        {   _prev_delta[key] = 0;
        }

        (*(_stats_file_staleness[key])) << _total_time << " ";
        (*(_stats_file_staleness[key])) << _prev_delta[key] << endl;
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
// 3
// #ifdef COLLECT_OUTPUT_STATISTICS   AuroraNode::DequeueHandle::dequeue
//
void  AuroraNodeMonitor::dequeue_output_statistics3(AuroraNode  *_node,
                                                    void        *context,
                                                    TupleQueue  *q,
                                                    bool         output_stream)
{
#ifdef COLLECT_OUTPUT_STATISTICS

    TupleQueue::DeqIterator  deq;
//..............................................................................


    AuroraNode::DequeueContext  *_context = (AuroraNode::DequeueContext  *)context; 

    //olga update the statistics structure in the stats Manager
    TimeSec precision;

    if (output_stream  &&  _node->get_stats_object().has_tsstats("OutLatency"))
    {
        precision = _node->get_stats_object().get_tsstats("OutLatency").get_precision();
        // perform a push update.

        unsigned long sched_ticks= Scheduler::ticks();

        // changed by Ying. (fill in 0s if no output data has been
        // detectd during a sample period)
        //
        while (sched_ticks - (precision/Scheduler::seconds_per_tick())
                                    >=_context->_last_outlatency_update)
// olga's old code            if ( sched_ticks-(precision / Scheduler::secondsPerTick())>=_context->_last_outlatency_update )
        {
            // fill missing samples (more than one precision time with no update -> fill the
            // empty samples

            /*************
                int so_far=0;
                if (_context->_last_outlatency_update<sched_ticks)
                {
                    so_far=(int)_context->_last_outlatency[stream->getName()];
                }
                else
                {   so_far =(int)_context->_last_outlatency_update;
                }

                int miss_samples= (sched_ticks - so_far)/((int)precision*sched_ticks);

                for (int i=0;i<miss_samples;i++)
                {
                    _context->_last_outlatency_update += (unsigned long)(precision / Scheduler::secondsPerTick());
                    _node->sm.readLock();
                    _node->getStatsObject().getStatistics( "OutLatency", stream->getName() ).addSample(_context->_outlatency_sum[stream->getName()]/miss_samples );
                    _node->getStatsObject().getStatistics( "OutLatencySqr", stream->getName()).addSample(_context->_outlatency_sumsqr[stream->getName()]/miss_samples);
                    _node->getStatsObject().getStatistics( "TupleNum", stream->getName()).addSample(_context->_tuplenum[stream->getName()]/miss_samples);

                    _node->getStatsObject().getTSStats( "OutLatency" ).time_stamp = _context->_last_outlatency_update*sched_secticks;
                    _node->getStatsObject().getTSStats( "OutLatencySqr" ).time_stamp =_context->_last_outlatency_update*sched_secticks;
                    _node->getStatsObject().getTSStats( "TupleNum" ).time_stamp =_context->_last_outlatency_update*sched_secticks;

                    _context->_outlatency_sumsqr[stream->getName()] =0;
                    _context->_outlatency_sum[stream->getName()]=0;
                    _context->_tuplenum[stream->getName()]=0;
                    _node->sm.readUnlock();
                }
            ***************/
            // fill current update
            // update tick
            _context->_last_outlatency_update +=
                    (unsigned long)(precision / Scheduler::seconds_per_tick());

            //_node->sm.readLock();
            _node->sm.write_lock_statistics("OutLatency");
            _node->sm.write_lock_statistics("OutLatencySqr");
            _node->sm.write_lock_statistics("TupleNum");

            for (map<string, double>::iterator
                 i = _context->_outlatency_sum.begin();
                 i != _context->_outlatency_sum.end(); i++)
            {
                //DEBUG << "added latency for "<< i->first;
                try
                {
                    if ( sched_ticks-(precision / Scheduler::seconds_per_tick())>=_context->_last_outlatency_update )
                    {
                        // in this case, we assume no output in the
                        // interval to be updated
                        _node->get_stats_object().get_statistics( "OutLatency", i->first ).add_sample(0.0);
                        _node->get_stats_object().get_statistics( "OutLatencySqr", i->first).add_sample(0.0);
                        _node->get_stats_object().get_statistics( "TupleNum", i->first).add_sample(0);
                        _context->_last_outlatency[i->first]=_context->_last_outlatency_update;
                    }
                    else
                    {
                        _node->get_stats_object().get_statistics( "OutLatency", i->first ).add_sample(_context->_outlatency_sum[i->first] );
                        _node->get_stats_object().get_statistics( "OutLatencySqr", i->first).add_sample(_context->_outlatency_sumsqr[i->first] );
                        _node->get_stats_object().get_statistics( "TupleNum", i->first).add_sample(_context->_tuplenum[i->first] );

                        _context->_outlatency_sum[i->first]=0;
                        _context->_outlatency_sumsqr[i->first] =0;
                        _context->_tuplenum[i->first]=0;
                        _context->_last_outlatency[i->first]=_context->_last_outlatency_update;
                    }
                }
                catch (Statistics::error &se)
                {
                    DEBUG << "Caught Statistics error: " << i->first;

                        //if ( se == Statistics::STATISTICS_NAME_NOT_FOUND )
                        //{   Throw(AuroraException,
                        //          "Statistics name not found for " + i->first);
                        //}
                        //else
                        //{  Throw(AuroraException, "Unknown Statistics error.");
                        //}
                }
                catch (TSStats::error &tse)
                {
                    DEBUG << "Caught TSStats error." << i->first;

                        //if ( tse == TSStats::ID_NOT_FOUND )
                        //{   Throw(AuroraException,
                        //         "TSStats id not found for " + i->first);
                        //}
                        //else
                        //{    Throw(AuroraException, "Unknown TSStats error.");
                        //}
                }
            }

            _node->get_stats_object().get_tsstats( "OutLatency" ).time_stamp = _context->_last_outlatency_update*Scheduler::seconds_per_tick();
            _node->get_stats_object().get_tsstats( "OutLatencySqr" ).time_stamp =_context->_last_outlatency_update*Scheduler::seconds_per_tick();
            _node->get_stats_object().get_tsstats( "TupleNum" ).time_stamp =_context->_last_outlatency_update*Scheduler::seconds_per_tick();

            //_node->sm.readUnlock();
            _node->sm.write_un_lock_statistics("OutLatency");
            _node->sm.write_un_lock_statistics("OutLatencySqr");
            _node->sm.write_un_lock_statistics("TupleNum");
        }// end if push
    }// end of Outlatency stats
#endif

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//#ifdef COLLECT_STATISTICS   // 3  AuroraNode::DequeueHandle::subscribe
//
void  AuroraNodeMonitor::dequeue_subscribe_statistics(AuroraNode  *_node,
                                                      void        *context,
                                                      string       stream_name)
{
#ifdef COLLECT_STATISTICS

    IDVect   v;

    CatalogStream  *catalog_stream;
//..............................................................................


    AuroraNode::DequeueContext *_context = (AuroraNode::DequeueContext *)context;

    catalog_stream = _node->get_local_stream(stream_name);

    if (catalog_stream->is_output_stream())
    {
        DEBUG << "Subscribing to an output stream : " << stream_name;

        _context->_outlatency_sum[stream_name]=0.0;
        _context->_outlatency_sumsqr[stream_name]=0.0;
        _context->_tuplenum[stream_name]=0;

        DEBUG << "Add statistics structure (Latency)" ;
        v.push_back(stream_name);

        _node->sm.add_tsstats("TupleNum", STATISTICS_WINDOW_SIZE,
                              STATISTICS_WINDOW_PRECISION, v);

        _node->sm.add_tsstats("OutLatency", STATISTICS_WINDOW_SIZE ,
                              STATISTICS_WINDOW_PRECISION, v);

        _node->sm.add_tsstats("OutLatencySqr", STATISTICS_WINDOW_SIZE ,
                              STATISTICS_WINDOW_PRECISION, v);
    }

#endif
    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//#ifdef COLLECT_STATISTICS   // 4   AuroraNode::create_add_task
//
// Record ids for collecting stats.
//
void  AuroraNodeMonitor::create_add_statistics(string     box_name,
                                               string     input_name,
                                               uint16     in_port,
                                               IDVect    &instream_ids,
                                               IDVect    &anystream_ids,
                                               IDVect    &selectivity_ids)
{
#ifdef COLLECT_STATISTICS
//
//..............................................................................


    instream_ids.push_back(string(box_name + " # " + in_port) );
    anystream_ids.push_back(input_name);

    selectivity_ids.push_back( string(box_name + " : " + in_port) );
#endif

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//#ifdef COLLECT_STATISTICS   // 5   AuroraNode::create_add_task
//
void  AuroraNodeMonitor::create_add_statistics5(string          box_name,
                                                CatalogStream  *stream,
                                                uint16          out_port,
                                                vector<IDVect> &outstream_ids,
                                                IDVect         &selectivity_ids)

{
#ifdef COLLECT_STATISTICS

    IDVect   v;
//..............................................................................


    selectivity_ids.push_back( string(box_name + " : " + out_port) );

    // Record stats per output stream
    //if ( (*output_it)->is_output_stream() )
    if ( stream->is_output_stream() )
    {
        //v.push_back(output_name);
        v.push_back(stream->get_stream_name().as_string());
        //DEBUG << "Add latency stats for :" << output_name;
        outstream_ids.push_back(v);
    }
#endif

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//#ifdef COLLECT_STATISTICS   // 6    AuroraNode::create_add_task
//
void  AuroraNodeMonitor::create_add_statistics6(IDVect         &ids,
                                                IDVect         &instream_ids,
                                                IDVect         &anystream_ids,
                                                IDVect         &selectivity_ids,
                                                vector<IDVect> &outstream_ids,
                                                vector<IDVect> &new_stream_ids,
                                                StatsMgr       &sm)
{
#ifdef COLLECT_STATISTICS
//..............................................................................


    // Add stats based on collected ids.
    if ( _add_stats && (ids.size() != 0) )
    {
         DEBUG << "Add statistics structure (Cost+InputRate)" ;
         sm.add_tsstats("Cost", STATISTICS_WINDOW_SIZE,
                        STATISTICS_WINDOW_PRECISION, ids);
         sm.add_tsstats("InputRate", STATISTICS_WINDOW_SIZE,
                        STATISTICS_WINDOW_PRECISION, ids);
    }

    if ( _add_stats && (instream_ids.size() != 0) )
    {
        DEBUG << "Add statistics structure (QLength)" ;
        sm.add_tsstats("QLength", STATISTICS_WINDOW_SIZE,
                       STATISTICS_WINDOW_PRECISION, instream_ids);
        //olga ?? (Maybe need to add this back) -- added back by yna.
    }

    if (_add_stats && (anystream_ids.size() != 0))
    {
        DEBUG << "Add statistics structure (StreamRate)" ;
        sm.add_tsstats("StreamRate", STATISTICS_WINDOW_SIZE,
                       STATISTICS_WINDOW_PRECISION, anystream_ids);
    }

    if (_add_stats && (selectivity_ids.size() != 0) )
    {
         DEBUG << "Add statistics structure (Selectivity)" ;
        sm.add_tsstats("Selectivity", STATISTICS_WINDOW_SIZE,
                       STATISTICS_WINDOW_PRECISION, selectivity_ids);
    }

    if (_add_stats && (outstream_ids.size() != 0) )
    {
        vector<IDVect>::iterator outstream_it = outstream_ids.begin();
        vector<IDVect>::iterator outstream_end = outstream_ids.end();
        for (; outstream_it != outstream_end; ++outstream_it)
        {
            DEBUG << "Add statistics structure (Latency)" ;
            sm.add_tsstats("TupleNum", STATISTICS_WINDOW_SIZE,
                           STATISTICS_WINDOW_PRECISION, *outstream_it);
            sm.add_tsstats("OutLatency", STATISTICS_WINDOW_SIZE ,
                           STATISTICS_WINDOW_PRECISION, *outstream_it);
            sm.add_tsstats("OutLatencySqr", STATISTICS_WINDOW_SIZE ,
                           STATISTICS_WINDOW_PRECISION, *outstream_it);
        }
    }

    if (_add_stats && (new_stream_ids.size() != 0) )
    {
        vector<IDVect>::iterator new_stream_it  = new_stream_ids.begin();
        vector<IDVect>::iterator new_stream_end = new_stream_ids.end();

        for (; new_stream_it != new_stream_end; ++new_stream_it)
        {
            DEBUG << "Add statistics structure (StreamRate)" ;
            sm.add_tsstats("StreamRate", STATISTICS_WINDOW_SIZE,
                           STATISTICS_WINDOW_PRECISION, *new_stream_it);
        }
    }
#endif

    return;
}


////////////////////////////////////////////////////////////////////////////////
//
//#ifdef COLLECT_STATISTICS   // 7  moved from AuroraNode::add_out_stream_stats
//
// Adds latency statistics for an output stream whenever a stream
// changes its type, e.g. during move box
//
void  AuroraNodeMonitor::add_out_stream_stats(// DeqContexts
                                              void       *deqs,

                                              StatsMgr   &sm,

                                              set<Name>   streams)
{
#ifdef COLLECT_STATISTICS

    IDVect  outstream_ids;
//..............................................................................


    AuroraNode::DeqContexts  *_deqs = (AuroraNode::DeqContexts *)deqs;

    for ( set<Name>::iterator i = streams.begin(); i != streams.end(); i++ )
    {
        string stream_name= i->as_string();
        outstream_ids.push_back(stream_name);
        DEBUG<< "Added statistics for stream " + stream_name +" as a new output stream";

        AuroraNode::DeqContexts::iterator deq_ctxt_it  = _deqs->begin();
        AuroraNode::DeqContexts::iterator deq_ctxt_end = _deqs->end();

        for (; deq_ctxt_it != deq_ctxt_end; ++deq_ctxt_it)
        {
            (*deq_ctxt_it)->_outlatency_sum[stream_name]=0.0;
            (*deq_ctxt_it)->_outlatency_sumsqr[stream_name]=0.0;
            (*deq_ctxt_it)->_tuplenum[stream_name]=0;
        }
    }

    if (_add_stats && outstream_ids.size()!=0)
    {
        DEBUG <<"Add Latency Stats for new output streams";

        sm.add_tsstats("TupleNum", STATISTICS_WINDOW_SIZE,
                       STATISTICS_WINDOW_PRECISION, outstream_ids);

        sm.add_tsstats("OutLatency", STATISTICS_WINDOW_SIZE ,
                       STATISTICS_WINDOW_PRECISION, outstream_ids);

        sm.add_tsstats("OutLatencySqr", STATISTICS_WINDOW_SIZE ,
                       STATISTICS_WINDOW_PRECISION, outstream_ids);
    }

#endif
    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//#ifdef COLLECT_STATISTICS   // 8  AuroraNode::create_cpview
//
void  AuroraNodeMonitor::cpview_statistics(Name        cp_name,
                                           StatsMgr   &sm)
{
#ifdef COLLECT_STATISTICS
//..............................................................................


    DEBUG << "About to set up CP stats";

    //Set up box statistics
    sm.add_tsstats("Cost", STATISTICS_WINDOW_SIZE,
                   STATISTICS_WINDOW_PRECISION,
                   vector<idtype>(1, cp_name.as_string()));

    sm.add_tsstats("InputRate", STATISTICS_WINDOW_SIZE,
                   STATISTICS_WINDOW_PRECISION,
                   vector<idtype>(1, cp_name.as_string()));

    sm.add_tsstats("QLength", STATISTICS_WINDOW_SIZE,
                   STATISTICS_WINDOW_PRECISION,
                   vector<idtype>(1, cp_name.as_string() + " # 0"));

    DEBUG << "Finished setting up CP stats";

#endif
    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//#ifdef COLLECT_STATISTICS   // 9   AuroraNode::create_cpview
//
void  AuroraNodeMonitor::cpview_statistics9(Name       sw_name,
                                            StatsMgr  &sm)
{
#ifdef COLLECT_STATISTICS
//..............................................................................


    DEBUG << "About to set up sweeper stats";

    sm.add_tsstats("Cost", STATISTICS_WINDOW_SIZE,
                   STATISTICS_WINDOW_PRECISION,
                   vector<idtype>(1, sw_name.as_string()));

    sm.add_tsstats("InputRate", STATISTICS_WINDOW_SIZE,
                   STATISTICS_WINDOW_PRECISION,
                   vector<idtype>(1, sw_name.as_string()));

    sm.add_tsstats("QLength", STATISTICS_WINDOW_SIZE,
                   STATISTICS_WINDOW_PRECISION,
                   vector<idtype>(1, sw_name.as_string() + " # 0"));

    sm.add_tsstats("QLength", STATISTICS_WINDOW_SIZE,
                   STATISTICS_WINDOW_PRECISION,
                   vector<idtype>(1, sw_name.as_string() + " # 1"));

    DEBUG << "Finished adding sweeper stats";
#endif

    return;
}

BOREALIS_NAMESPACE_END
