#ifndef  PSUEDOSCHEDULERMONITOR_H
#define  PSUEDOSCHEDULERMONITOR_H

#include  "Stream.h"
#include  "StatsMgr.h"
#include  "TSStats.h"
#include  "QBox.h"

BOREALIS_NAMESPACE_BEGIN

class SchedStats
{
 public:

    map<string, deque<double> > _selectivities;
    map<string, deque<double> > _box_costs;
    map<string, deque<double> > _avg_queue_increment;

    int cpu_load;
};


class SchedulerStats
{
 public:
    map<string, double > _total_cost;
    map<string, int > _execution_count;

    map<string, int> _total_input;
    map<string, int> _total_output;

    map<string, deque<double> > _selectivities;
    map<string, deque<double> > _avg_queue_increment;

    map<string, double > _box_costs, _shadow_box_costs;
    map<string, int > _box_executions, _shadow_box_executions;

    map<string, double > _queue_sizes, _shadow_queue_sizes;
    map<string, int> _queue_entries, _shadow_queue_entries;

    map<string, int> _tuples_per_port;

    map<string, int> _last_enq_tuples, _curr_enq_tuples;

    map<string, int> _last_stream_rate;
};


class PseudoSchedulerMonitor
{
  public:


    void  setup_listeners_statistics(QBox *box);
    void  update_listeners_statistics(QBox *box, uint32 j);

    void  delete_scheduler_statistics(string           statistics_name,
                                      vector<string>  &ids);

    void  run_statistics3(QBox            *box,
                          QBoxInvocation  &inv);

    void  run_statistics4_5(QBox *box);
    void  run_box_statistics(QBox *box);

    void  run_statistics(AuroraNode  &node);
    void  run_cost_statistics(AuroraNode  &node);
    void  run_selectivity_statistics(AuroraNode  &node);
    void  run_stream_rate_statistics(AuroraNode  &node);
    void  run_queue_length_statistics(AuroraNode  &node);
    void  run_input_rate_statistics(AuroraNode  &node);

    double  use_cpu_time(double time_to_use);


    void track_queues(QBox* box);

  private:

    uint32 _before_time;
    int _input_tuples_before;
    int _enq_tuples_before;
    map<int, int> _enq_tuples_after_stream;
    map<int, int> _enq_tuples_before_stream;
    map<int, int> _input_tuples_before_stream;

    // -alexr my stuff, put it here for now
    SchedulerStats    _sched_stats;

  public:
    // which map has the latest result (note that there is no atomicity,
    // so this flag is updated *after* new value is inserted into shadow map).
    //
    bool _use_shadow_boxmap;
    bool _use_shadow_qlength;

    uint32  _last_boxcost_update;
    uint32  _last_qlength_update;
    uint32  _last_irate_update;
    uint32  _last_selectivity_update;
    uint32  _last_stream_rate_update;

    uint32  _init_time;
};

BOREALIS_NAMESPACE_END

#endif                           // PSUEDOSCHEDULERMONITOR_H
