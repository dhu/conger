#include "PseudoScheduler.h"
#include "PseudoSchedulerMonitor.h"
#include "AuroraNode.h"
#include "QBox.h"
#include "LockHolder.h"
#include "TSStats.h"
#include "Name.h"

#include <fstream>

BOREALIS_NAMESPACE_BEGIN

// COLLECT_STATISTICS is defined in: AuroraNodeMonitor.h


////////////////////////////////////////////////////////////////////////////////
//
// 1
//
void PseudoSchedulerMonitor::setup_listeners_statistics(QBox *box)
{
#ifdef COLLECT_STATISTICS
//..............................................................................


    _sched_stats._shadow_box_costs[box->get_name()] =
        _sched_stats._box_costs[box->get_name()] = 0.0;

    _sched_stats._shadow_box_executions[box->get_name()] =
        _sched_stats._box_executions[box->get_name()] = 0;

    /*
    _sched_stats._queue_sizes[box->get_name()] =
        _sched_stats._shadow_queue_sizes[box->get_name()] = 0;

    _sched_stats._shadow_queue_entries[box->get_name()] =
        _sched_stats._queue_entries[box->get_name()] = 0;
    */

    // here I assume that we start with an empty queue...
    // if we don't, the very first window measurment will be skewed.
    //
    _sched_stats._last_enq_tuples[box->get_name()] = 0;
#endif

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// 2
void PseudoSchedulerMonitor::delete_scheduler_statistics(
                                                string          statistics_name,
                                                vector<string> &ids)
{
#ifdef COLLECT_STATISTICS
//..............................................................................


    for (vector<string>::iterator i = ids.begin(); i != ids.end(); i++)
    {
        string obj_name = (*i);

        if (statistics_name == "Cost")
        {
            _sched_stats._box_costs.erase(obj_name);
            _sched_stats._box_executions.erase(obj_name);
            _sched_stats._shadow_box_costs.erase(obj_name);
            _sched_stats._shadow_box_executions.erase(obj_name);
        }
        else if (statistics_name == "InputRate")
        {
            _sched_stats._last_enq_tuples.erase(obj_name);
            _sched_stats._curr_enq_tuples.erase(obj_name);
        }
        else if (statistics_name == "QLength")
        {
            _sched_stats._shadow_queue_sizes.erase(obj_name);
            _sched_stats._shadow_queue_entries.erase(obj_name);
            _sched_stats._queue_sizes.erase(obj_name);
            _sched_stats._queue_entries.erase(obj_name);
        }
        else if (statistics_name == "Selectivity")
        {
            _sched_stats._tuples_per_port.erase(obj_name);
        }
        else if (statistics_name == "StreamRate")
        {
            _sched_stats._last_stream_rate.erase(obj_name);
        }
    }
#endif

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// 3
void PseudoSchedulerMonitor::run_statistics3(QBox  *box,
                                             QBoxInvocation  &inv)
{
#ifdef COLLECT_STATISTICS

    uint32  num_inputs;
    uint32  num_outputs;
    uint32  count;

    vector<int> perf_queue_deq;
    vector<int> perf_queue_enq;
//..............................................................................


    DEBUG << "Running " << box->get_name() << "...";
                
    num_inputs = box->get_num_inputs();
    perf_queue_deq.resize(num_inputs);

    for (uint32 j = 0; j < num_inputs; ++j)
    {
        perf_queue_deq[j] = box->get_input_queue(j)->get_num_tuples_dequeued();
    }

    num_outputs = box->get_num_outputs();
    perf_queue_enq.resize(num_outputs);

    for (uint32 j = 0; j < num_outputs; ++j)
    {
        perf_queue_enq[j] = box->get_output(j).get_num_tuples_enqueued();
    }
                
    //NOTICE << box->get_name();
    box->run(inv);
                
    for (uint32 j = 0; j < num_inputs; ++j)
    {
        count = box->get_input_queue(j)
                               ->get_num_tuples_dequeued() - perf_queue_deq[j];

        if (count)
        {   DEBUG << " - on input #" << j << " ("
                  << box->get_input(j).get_name() << "): "
                  << count << " dequeued";
        }
    }
                
    for (uint32 j = 0; j < num_outputs; ++j)
    {
        WARN << "Why is this code running? Is it supposed to?";
        count = box->get_output(j).get_num_tuples_enqueued() - perf_queue_enq[j];

        if (count)
        {
            DEBUG << " - on output #" << j << " ("
                  << box->get_output(j).get_name() << "): "
                  << count << " enqueued";

            string out_id = box->get_name() + " o "+j;

            if ((_sched_stats._tuples_per_port.find(out_id) ==
                (_sched_stats._tuples_per_port.end())))
            {
                _sched_stats._tuples_per_port[out_id] = 0;
            }

            _sched_stats._tuples_per_port[out_id]+=count;
        }
    }

#endif

    return;
}

////////////////////////////////////////////////////////////////////////////////
//
//
void PseudoSchedulerMonitor::track_queues(QBox *box)
{
//..............................................................................

    _input_tuples_before = _enq_tuples_before = 0;
    _input_tuples_before_stream.clear();
    _enq_tuples_before_stream.clear();
    _enq_tuples_after_stream.clear();

    _before_time = Scheduler::ticks();

    // get tuples in all queues before execution
    // and also figure out what the enq state of the queues was
    // since we allow simulatenous box processing and 
    // tuple enqueuing
    //
    for (int j = 0; j < (int)box->get_num_inputs(); ++j)
    {
        _input_tuples_before += box->get_input_queue(j)->size();

        _input_tuples_before_stream[j] += box->get_input_queue(j)->size(); //olga

        _enq_tuples_before+= box->get_input_queue(j)->get_num_tuples_enqueued();

        _enq_tuples_before_stream[j] +=
                        box->get_input_queue(j)->get_num_tuples_enqueued();
    }
    
    // Remember how many tuples were enqueued before run on out
    //
    for (int j = 0; j < (int)box->get_num_outputs(); ++j)
    {
        _enq_tuples_after_stream[j] = box->get_output(j).get_num_tuples_enqueued();
    }

}


////////////////////////////////////////////////////////////////////////////////
//
// 4 5
void PseudoSchedulerMonitor::run_statistics4_5(QBox *box)
{
    //uint32 before_time = 0; //, after_time = 0;

    int input_tuples_after = 0;
    int enq_tuples_after = 0;

    map<int, int> input_tuples_after_stream;
    map<int, int> enq_tuples_after_stream;
//..............................................................................


    /**
     * 
    int input_tuples_before = 0, enq_tuples_before = 0;
    map<int, int> _enq_tuples_after_stream;
    map<int, int> enq_tuples_before_stream; //olga
    map<int, int> input_tuples_before_stream;

    before_time = Scheduler::ticks();
            
    // get tuples in all queues before execution
    // and also figure out what the enq state of the queues was
    // since we allow simulatenous box processing and 
    // tuple enqueuing
    //
    for (int j = 0; j < (int)box->get_num_inputs(); ++j)
    {
        input_tuples_before += box->get_input_queue(j)->size();

        input_tuples_before_stream[j] += box->get_input_queue(j)->size(); //olga

        enq_tuples_before+= box->get_input_queue(j)->get_num_tuples_enqueued();

        enq_tuples_before_stream[j] +=
                        box->get_input_queue(j)->get_num_tuples_enqueued();
    }

    // alexr. 
    // Remember how many tuples were enqueued before run on out
    //
    for (int j = 0; j < (int)box->get_num_outputs(); ++j)
    {
        _enq_tuples_after_stream[j] = box->get_output(j).get_num_tuples_enqueued();
    }
    */


    uint32 num_inputs = box->get_num_inputs();

    for (int j = 0; j < (int)num_inputs; ++j)
    {
        input_tuples_after += box->get_input_queue(j)->size(); //olga

        input_tuples_after_stream[j] += box->get_input_queue(j)->size();

        enq_tuples_after += box->get_input_queue(j)->get_num_tuples_enqueued();

        enq_tuples_after_stream[j] += box->get_input_queue(j)->get_num_tuples_enqueued(); 

        string in_id = box->get_name() + " i " + j;

        if (_sched_stats._tuples_per_port.find(in_id) ==
            _sched_stats._tuples_per_port.end())
        {
            _sched_stats._tuples_per_port[in_id] = 0;
        }
        
        //alexr
        // Please note!
        // This is a somewhat temporary fix,
        // since it assumes one input stream
        // Will probably not work for join, but otherwise ok...
        //
        _sched_stats._tuples_per_port[in_id] +=
                        _input_tuples_before_stream[j] -
                        input_tuples_after_stream[j]  +
                        (enq_tuples_after_stream[j] - 
                         _enq_tuples_before_stream[j]);

        /*
        DEBUG << " Added "
              << input_tuples_before_stream[j] - input_tuples_after_stream[j]
              << " TO " << box->as_string()
              << " itbs " << input_tuples_before_stream[j]
              << " itas " << input_tuples_after_stream[j]
              << " etbs " << enq_tuples_before_stream[j]
              << " etas " << enq_tuples_after_stream[j];
        */
    }

    // adjust for tuples that might have been enqueued 
    // while the box runs. 
    // Presumably this parallel operation is thread safe.
    //
    input_tuples_after -= (enq_tuples_after - _enq_tuples_before);

    DEBUG << " I am adjusting something by -"
          << (enq_tuples_after - _enq_tuples_before);

    //olga
    for (int j = 0; j < (int)num_inputs; ++j)
    {
        input_tuples_after_stream[j] -= (enq_tuples_after_stream[j] -
                                         _enq_tuples_before_stream[j]);
    }

    for (int j = 0; j < (int)box->get_num_outputs(); ++j)
    {
        string out_id = box->get_name() + " o " + j;

        if (_sched_stats._tuples_per_port.find(out_id) ==
            _sched_stats._tuples_per_port.end())
        {
            _sched_stats._tuples_per_port[out_id] = 0;
        }

        _sched_stats._tuples_per_port[out_id] +=
                        box->get_output(j).get_num_tuples_enqueued() -
                        _enq_tuples_after_stream[j];

        /*
        DEBUG << " And output  "
              <<  box->get_output(j).get_num_tuples_enqueued() 
                - _enq_tuples_after_stream[j]
              << " total " 
              << _sched_stats._tuples_per_port[box->get_name() + " o "+j]
              << " onte " 
              << box->get_output(j).get_num_tuples_enqueued()
              << " etas " << _enq_tuples_after_stream[j];
        */
    }

    // This BOX_COST is the amount of time CPU should be occupied
    // per processed tuple in micro-seconds
    //
    int BOX_COST = 0; //10000;
    int sleep_time = (_input_tuples_before-input_tuples_after) * BOX_COST;

    if (sleep_time > 0)
    {
        //WARN << "Scheduler before busywaiting for "
        //     << sleep_time << " microseconds.";

        use_cpu_time(sleep_time / 10000000.0);
    }

    //WARN << box->get_name() << " ADDING "
    //     << ((Scheduler::ticks() - before_time) *
    //         Scheduler::secondsPerTick()) << " executions "
    //     << (input_tuples_before-input_tuples_after);

    if ((_input_tuples_before-input_tuples_after) > 0)
    {
        if (_use_shadow_boxmap)
        {
            _sched_stats._shadow_box_costs[box->get_name()] +=
                           (Scheduler::ticks() - _before_time) *
                            Scheduler::seconds_per_tick();

            _sched_stats._shadow_box_executions[box->get_name()] 
                            += (_input_tuples_before-input_tuples_after);

            //WARN << " TOTAL OF "
            //     << _sched_stats._shadow_box_costs[
            //                         box->get_name()]
            //     << " and " 
            //     << _sched_stats._shadow_box_executions[
            //                           box->get_name()];
        }
        else
        {
            _sched_stats._box_costs[box->get_name()] +=
                           (Scheduler::ticks() - _before_time) *
                            Scheduler::seconds_per_tick();

            _sched_stats._box_executions[box->get_name()] +=
                           (_input_tuples_before-input_tuples_after);

            //WARN << " TOTAL OF "
            //     << _sched_stats._box_costs[box->get_name()]
            //     << " and " << _sched_stats._box_executions[
            //                               box->get_name()];
        }

        /*
        if (_use_shadow_qlength)
        {
            _sched_stats._shadow_queue_sizes[box->get_name()] +=
                            (input_tuples_before);

            _sched_stats._shadow_queue_entries[box->get_name()] += 1;

            //WARN << " TOTAL OF "
            //     << _sched_stats._shadow_box_costs[
            //                         box->get_name()]
            //     << " and "
            //     << _sched_stats._shadow_box_executions[
            //                              box->get_name()];
        }
        else
        {
            _sched_stats._queue_sizes[box->get_name()] +=
                            (input_tuples_before);

            _sched_stats._queue_entries[box->get_name()] += 1;

            //WARN << " TOTAL OF "
            //     << _sched_stats._box_costs[box->get_name()]
            //     << " and "
            //     << _sched_stats._box_executions[
            //                       box->get_name()];
        }
        */

        for (int j = 0; j < (int)num_inputs; ++j)
        {
            if ((_input_tuples_before_stream[j]
                - input_tuples_after_stream[j]) > 0)
            {
                if (_use_shadow_qlength)
                {
                     _sched_stats._shadow_queue_sizes[box->get_name()
                                                      + " # " + j] += 
                                    (_input_tuples_before_stream[j]);

                     _sched_stats._shadow_queue_entries[box->get_name()
                                                        + " # " + j] += 1;
                 }
                 else
                 {
                     _sched_stats._queue_sizes[box->get_name()
                                +" # " +j] +=  (_input_tuples_before_stream[j]);

                     _sched_stats._queue_entries[box->get_name()
                                                 + " # " +j] += 1;
                 }
             }
        }

        if (_sched_stats._last_enq_tuples.find(box->get_name()) ==
            _sched_stats._last_enq_tuples.end())
        {
            _sched_stats._last_enq_tuples[box->get_name()] = 0;
        }

        //WARN << " Sched recording for "
        //     << box->get_name().c_str() << endl;

        _sched_stats._curr_enq_tuples[box->get_name()] =
                        box->get_input_queue(0)->get_num_tuples_dequeued();

    } // end input before - after > 0

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// 6  combine with 5
void PseudoSchedulerMonitor::run_box_statistics(QBox *box)
{
#ifdef COLLECT_STATISTICS

//..............................................................................


    run_statistics4_5(box);

    INFO << "PseudoScheduler box stats started...";

    if (!box)
    {
        INFO << "PseudoScheduler found null box pointer!";
    }

    /*
    INFO << "Finished running " << box->get_name() 
         <<", queues: \n";

    string inputs = "";

    for (uint32 j = 0; j < num_inputs; ++j)
    {
        // this is debug not to be kept in a run time system.
        //
        std::stringstream ss, ss2;
        ss << j;  ss2 <<  box->get_input_queue(j)->size();

        inputs = inputs + string("Input: ")+ string(ss.str())  +
                 " Tuples: " + string(ss2.str());
    }

    INFO << inputs << "\n\n";
    */

    INFO << "PseudoScheduler box stats finished...";
#endif

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// 7
void PseudoSchedulerMonitor::run_statistics(AuroraNode  &node)
{
#ifdef COLLECT_STATISTICS
//
//..............................................................................


    run_cost_statistics(node);
    run_selectivity_statistics(node);
    run_stream_rate_statistics(node);
    run_queue_length_statistics(node);
    run_input_rate_statistics(node);
#endif

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// 7
void PseudoSchedulerMonitor::run_cost_statistics(AuroraNode  &node)
{
    TimeSec precision;
//..............................................................................


    if (node.get_stats_object().has_tsstats("Cost"))
    {
        precision = node.get_stats_object().get_tsstats("Cost").get_precision();

        // perform a push update.
        //
        if (Scheduler::ticks() - (precision / Scheduler::seconds_per_tick())
                    >= _last_boxcost_update)
        {
            //WARN << " Perform a BOX update at "
            //     << (Scheduler::ticks()-_init_time) * 
            //        Scheduler::secondsPerTick();

            _last_boxcost_update += (uint32)
                        (precision / Scheduler::seconds_per_tick());

            //node.sm.readLock();
            node.sm.write_lock_statistics("Cost");
            //DEBUG << "beep";

            if (_use_shadow_boxmap)
            {
                _use_shadow_boxmap = (!_use_shadow_boxmap);  // switch

                for (map<string, double>::iterator i =
                             _sched_stats._shadow_box_costs.begin();
                             i != _sched_stats._shadow_box_costs.end(); i++)
                {
                    /*
                    DEBUG << "adding shadow cost for :" << i->first;

                    if (_sched_stats._shadow_box_executions[i->first] == 0)
                    {
                        node.get_stats_object().get_statistics("Cost",
                                                                 i->first)
                                                      .add_sample(-1);
                    }
                    else
                    {
                    */

                        node.get_stats_object().get_statistics("Cost",
                                                                 i->first)
                                  .add_sample(
                              (_sched_stats._shadow_box_executions[i->first] ?
                               i->second / 
                               _sched_stats._shadow_box_executions[i->first] : 
                               _sched_stats._shadow_box_executions[i->first]));

                   //}

                     i->second = 0.0;
                     _sched_stats._shadow_box_executions[i->first] =0;
                }
            }
            else
            {
                // update non-shadows.
                //
                _use_shadow_boxmap = (!_use_shadow_boxmap);  // switch

                for (map<string, double>::iterator i =
                             _sched_stats._box_costs.begin();
                             i != _sched_stats._box_costs.end(); i++)
                {
                    /*
                    DEBUG <<"Adding cost for box:" << i->first ;

                    if (_sched_stats._box_executions[i->first] == 0)
                    {   node.getStatsObject().getStatistics("Cost",
                                                              i->first)
                                                     .addSample(-1);
                    }
                    else
                    */
                    {   node.get_stats_object().
                                get_statistics("Cost", i->first).add_sample(
                                (_sched_stats._box_executions[i->first] ?
                                i->second / 
                                _sched_stats._box_executions[i->first] :
                                _sched_stats._box_executions[i->first]));
                    }

                    i->second = 0.0;
                    _sched_stats._box_executions[i->first] = 0;
                }
            }

            node.get_stats_object().get_tsstats("Cost").time_stamp =
                        _last_boxcost_update* Scheduler::seconds_per_tick();

            //Time::now().to_usecs();
            //node.sm.readUnlock();
            node.sm.write_un_lock_statistics("Cost");
        }
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// 8  combine with 7
void PseudoSchedulerMonitor::run_selectivity_statistics(AuroraNode  &node)
{
    TimeSec precision;
//..............................................................................

    //DEBUG << "Finished doing costs, now do selectivity";

    if (node.get_stats_object().has_tsstats("Selectivity"))
    {
        precision = node.get_stats_object().
                    get_tsstats("Selectivity").get_precision();

        int total_input = 0;

        // perform a push update.
        //
        if (Scheduler::ticks() - (precision / Scheduler::seconds_per_tick())
                    >= _last_selectivity_update)
        {
            _last_selectivity_update += (uint32)
                            (precision / Scheduler::seconds_per_tick());

            //node.sm.readLock();
            node.sm.write_lock_statistics("Selectivity");

            // the box names, assume that cost is a mapping box name
            // to the cost of the box
            //
            IDVect* names = node.get_stats_object().get_tsstats(
                                                        "Cost").stat_names();

            for (IDVect::iterator it = names->begin();
                 it != names->end(); it++)
            {
                ptr<QBox> z = node.get_box((*it));
                if (!z)  { continue; }
                total_input = 0;

                for (uint32 k = 0; k < (z)->get_num_inputs(); k++)
                {
                    total_input += _sched_stats._tuples_per_port[
                                           (z)->get_name() + " i " + k];
                }

                for (uint32 j = 0; j<(z)->get_num_outputs(); j++)
                {
                    idtype sel_id = (z)->get_name()+" : " + j;
                    idtype out_id = (z)->get_name()+" o " + j;

                    node.get_stats_object().
                            get_statistics("Selectivity", sel_id).
                            add_sample(total_input?
                                1.0 * _sched_stats._tuples_per_port[out_id]/
                                total_input : node.get_stats_object().
                                get_last_sample("Selectivity", sel_id));

                    if ((total_input?
                                1.0 * _sched_stats._tuples_per_port[out_id]/
                                total_input :
                                node.get_stats_object().get_last_sample(
                                              "Selectivity", sel_id)) > 1)
                    {
                         INFO << "Selectivity seems to have exceeded 1."
                              << " total tuples? " << total_input
                              << " input sample "
                              << _sched_stats._tuples_per_port[out_id]
                              << " box " << (z)->as_string();
                    }
                }
            }

            if (total_input)
            {
                for (map<string, int>::iterator i =
                     _sched_stats._tuples_per_port.begin();
                             i != _sched_stats._tuples_per_port.end(); i++)
                {
                     i->second = 0;
                }
            }

            node.get_stats_object().get_tsstats("Selectivity").
                        time_stamp = _last_selectivity_update*
                                     Scheduler::seconds_per_tick();

            //Time::now().to_usecs();
            //node.sm.readUnlock();

            node.sm.write_un_lock_statistics("Selectivity");
        }
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// 9  combine with 7
void PseudoSchedulerMonitor::run_stream_rate_statistics(AuroraNode  &node)
{
    TimeSec precision;
//..............................................................................

    //DEBUG << "Finished selectivity, do streamrate";

    if (node.get_stats_object().has_tsstats("StreamRate"))
    {
        precision = node.get_stats_object().
                    get_tsstats("StreamRate").get_precision();

        // perform a push update.
        //
        if (Scheduler::ticks() - (precision / Scheduler::seconds_per_tick())
                    >= _last_stream_rate_update)
        {
            _last_stream_rate_update +=
                     (uint32)(precision/Scheduler::seconds_per_tick());

            //node.sm.readLock();
            node.sm.write_lock_statistics("StreamRate");

            IDVect* names = node.get_stats_object().
                        get_tsstats("StreamRate").stat_names();

            for (IDVect::iterator it = names->begin();
                          it != names->end(); it++)
            {
                if (_sched_stats._last_stream_rate.find((*it))
                            == _sched_stats._last_stream_rate.end())
                {
                    _sched_stats._last_stream_rate[*it] = 0;
                }

                ptr<Stream> _stream = node.get_stream(*it);

                int enq1 = 0;

                if (_stream)
                {   enq1 = _stream->get_num_tuples_enqueued();
                }

                node.get_stats_object().
                            get_statistics("StreamRate", (*it)).add_sample(
                            (enq1 - _sched_stats._last_stream_rate[*it]) / 
                            precision);

                _sched_stats._last_stream_rate[*it] = enq1;

                node.get_stats_object().get_tsstats("StreamRate").
                            time_stamp = _last_stream_rate_update*
                                         Scheduler::seconds_per_tick();

                //Time::now().to_usecs();
                //node.sm.readUnlock();
                //delete names;
                //node.sm.writeUnLockStatistics("StreamRate");
            }

            //node.sm.readUnlock();
            node.sm.write_un_lock_statistics("StreamRate");
            //delete names;
        }
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// 10  combine with 7
void PseudoSchedulerMonitor::run_queue_length_statistics(AuroraNode  &node)
{
    TimeSec precision;
//..............................................................................

    //DEBUG << "finished streamrate, do olength";

    if (node.get_stats_object().has_tsstats("QLength"))
    {
        precision = node.get_stats_object().get_tsstats(
                                               "QLength").get_precision();

        // perform a push update.
        //
        if (Scheduler::ticks() - (precision / Scheduler::seconds_per_tick())
                    >= _last_qlength_update)
        {
            _last_qlength_update += (uint32)
                        (precision / Scheduler::seconds_per_tick());

            //node.sm.readLock();

            node.sm.write_lock_statistics("QLength");

            if (_use_shadow_qlength)
            {
                _use_shadow_qlength = (!_use_shadow_qlength); // switch

                for (map<string, double>::iterator i =
                             _sched_stats._shadow_queue_sizes.begin();
                             i != _sched_stats._shadow_queue_sizes.end(); i++)
                {
                    /*
                    if (_sched_stats._shadow_box_executions[i->first] == 0)
                    {
                        node.getStatsObject().getStatistics(
                                   "Cost", i->first).addSample( -1);
                    }
                    else
                    */
                    node.get_stats_object().
                    get_statistics("QLength", i->first).add_sample(
                                (_sched_stats._shadow_queue_entries[i->first] ?
                                i->second / 
                                _sched_stats._shadow_queue_entries[i->first] :
                                _sched_stats._shadow_queue_entries[i->first]));

                    i->second = 0.0;
                    _sched_stats._shadow_queue_entries[i->first] =0;
                }
            }
            else
            {
                // update non-shadows.
                //
                _use_shadow_qlength = (!_use_shadow_qlength); // switch

                for (map<string, double>::iterator i =
                             _sched_stats._queue_sizes.begin();
                             i != _sched_stats._queue_sizes.end(); i++)
                {
                    /*
                    if (_sched_stats._box_executions[i->first] == 0)
                    {
                        node.getStatsObject().getStatistics(
                                    "Cost", i->first).addSample(-1);
                    }
                    else
                    */
                    node.get_stats_object().
                                get_statistics("QLength", i->first).add_sample(
                                (_sched_stats._queue_entries[i->first] ?
                                    i->second / 
                                    _sched_stats._queue_entries[i->first] :
                                    _sched_stats._queue_entries[i->first]));

                    i->second = 0.0;
                    _sched_stats._queue_entries[i->first] = 0;
                }
            }

            node.get_stats_object().get_tsstats("QLength").
                        time_stamp = _last_qlength_update*
                                     Scheduler::seconds_per_tick();

            //Time::now().to_usecs();
            //node.sm.readUnlock();

            node.sm.write_un_lock_statistics("QLength");
        }
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// 11 combine with 7
void PseudoSchedulerMonitor::run_input_rate_statistics(AuroraNode  &node)
{
    TimeSec precision;
//..............................................................................


    //DEBUG << "finished olength, do inputrate";

    if (node.get_stats_object().has_tsstats("InputRate"))
    {
        precision = node.get_stats_object().get_tsstats(
                       "InputRate").get_precision();

        // perform a push update.
        //
        if (Scheduler::ticks() - (precision / Scheduler::seconds_per_tick())
                    >= _last_irate_update)
        {
            _last_irate_update += (uint32)
                    (precision / Scheduler::seconds_per_tick());

            //node.sm.readLock();

            node.sm.write_lock_statistics("InputRate");

            // There is no need for shadow system because I am only
            // updating my array once in a precision window (in
            // particular at the same time as push) and I can assume
            // that precision is large enough to avoid conflicts.

            for (map<string, int>::iterator i =
                          _sched_stats._last_enq_tuples.begin();
                         i != _sched_stats._last_enq_tuples.end(); i++)
            {
                // assume that we start from 0.
                // and realize that this stat is unstable during
                // the first window because of this assumption
                // and because it is filled one by one during
                // first iteration.
                        
                // _last_enq_tuples;
                //ptr<QBox> br = node.getBox(i->first);
                        
                int curr_enq = _sched_stats._curr_enq_tuples[i->first];
                //br->get_input_queue(0)->getNumTuplesDequeued();
                        
                node.get_stats_object().
                          get_statistics("InputRate", i->first).add_sample(
                            ((curr_enq > 
                              _sched_stats._last_enq_tuples[i->first]) ?
                            (curr_enq - 
                             _sched_stats._last_enq_tuples[i->first])/
                            precision : 0));
                        
                 _sched_stats._last_enq_tuples[i->first] =
                          _sched_stats._curr_enq_tuples[i->first];

                        //br->get_input_queue(0)->getNumTuplesDequeued();
            }
 
            node.get_stats_object().get_tsstats("InputRate").time_stamp
                    = _last_irate_update * Scheduler::seconds_per_tick();

            //Time::now().to_usecs();
            //node.sm.readUnlock();

            node.sm.write_un_lock_statistics("InputRate");
        }
    }

    return;
}




////////////////////////////////////////////////////////////////////////////////
//
// 12
void PseudoSchedulerMonitor::update_listeners_statistics(QBox   *box,
                                                         uint32  j)
{
#ifdef COLLECT_STATISTICS
//
// need to make parameter an action add, remove, delete.
//..............................................................................


    //olga
    string in_id = box->get_name() + " # " + j;

    _sched_stats._queue_sizes[in_id] = 0;
    _sched_stats._shadow_queue_sizes[in_id] = 0;
    _sched_stats._shadow_queue_entries[in_id] = 0;
    _sched_stats._queue_entries[in_id] = 0;
    _sched_stats._shadow_box_costs[box->get_name()] = 0.0;
    _sched_stats._box_costs[box->get_name()] = 0.0;
    _sched_stats._shadow_box_executions[box->get_name()] = 0;
    _sched_stats._box_executions[box->get_name()] = 0;
    _sched_stats._last_enq_tuples[box->get_name()] = 0;

#endif

    return;
}


////////////////////////////////////////////////////////////////////////////////
//
// this should probably be in utility, but there is no Utility class yet.
// The return value is junk and should be discarded.
// It is only there so the compiler does not optimize it out.
//
double PseudoSchedulerMonitor::use_cpu_time(double time_to_use)
{
    //struct itimerval first;
    unsigned int  seed = 2;

    double secs_per_rand = 2.60260e-08;
    //double secs_per_rand = 2.414e-08;
    //double secs_per_rand = 2.78e-08;
    //double secs_per_rand = 1.09111e-07;
    double num_rands = time_to_use/secs_per_rand;

    //double elapsed_time = 0.0;
    //double secs;
//..............................................................................


    //secs = get_etime(_itimer_start);

    for (int i = 0; i < num_rands; i++)
    {
        time_to_use = (double)rand_r(&seed) / (double)(RAND_MAX);
        //drand48() / (double)(RAND_MAX);
    }

    //elapsed_time = get_etime(_itimer_start) - secs;
    //_measure->incrementTimeSpentExecutingBoxes(elapsed_time);

    return (time_to_use);
}


/*******************  stale
void PseudoScheduler::init_sched_stats(int window_size, int history_size)
{
    //for (set<ptr<QBox> >::const_iterator i = node().getBoxes().begin();
    //     i != node().getBoxes().end(); ++i)
    //{
    //    string box_name = i->get()->get_name();
    //    _sched_stats._box_costs[box_name] = deque<double>(historySize);
    //}
    //_sched_stats._selectivities = 1;
    //_sched_stats._box_costs = 0;
    //_sched_stats._avg_queue_length = 0;
}
*****************/

BOREALIS_NAMESPACE_END
