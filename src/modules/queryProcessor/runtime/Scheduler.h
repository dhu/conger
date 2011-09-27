#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <nmstl_util.h>

#include "common.h"

#include <queue>
#include <string>

BOREALIS_NAMESPACE_BEGIN

class AuroraNode;
class QBox;


/********************************
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
*****************************************/

/// A task to be performed by the scheduler.  Code using the Scheduler
/// (generally AuroraNode) may create a subclass of SchedulerTask,
/// instantiate that subclass, and pass the resultant object to
/// Scheduler::scheduleExclusiveTask() to request that it be run.
class SchedulerTask
{
  public:
    /// Destructor.
    virtual ~SchedulerTask() {}

    /// Runs the task.  Invoked by a Scheduler's event loop.
    virtual void run() = 0;
};


///// beginning of HA-related classes (maintained by Jeong-Hyon)

/** 
 * The handler for the idle times (i.e., the scheduler has no jobs to run).
 */
class SchedulerEventHandler
{
  public:
  
    /**
     * Constructs an IdleTimeHanlder.
     */
    SchedulerEventHandler() {}
    
    /**
     * Destroys an IdleTimeHanlder.
     */
    virtual ~SchedulerEventHandler() {}
    
    /**
     * This method is invoked whenenver the scheduler has no jobs to do.
     */
    virtual void handle_idle_time() = 0;
    
    /**
     * Updates the statistics related to the specified box.
     */
    virtual void update_statistics(QBox* box, Time time_span) = 0;
    
};

///// end of HA-related classes (maintained by Jeong-Hyon)


/// A scheduler in charge of running boxes in a catalog.
/// An AuroraNode has an associated scheduler responsible for
/// managing box invocation.
///
/// AuroraNode::start() calls Scheduler::start(), which should create
/// one or more threads to run boxes.  AuroraNode::shutdown() calls
/// Scheduler::shutdown(), which should signal for those threads to
/// stop and wait for their termination (i.e., join them).
///
/// AuroraNode may call scheduleExclusiveTask() to request that
/// some code be run exclusively of all boxes.  This is useful when
/// AuroraNode must make some change to the topology of the
/// network (currently limited to adding/removing subscriptions
/// to streams).

class Scheduler
{
    friend class AuroraNode;

  public:

    /// Base class for data about individual boxes.
    class BoxData
    {
      public:
        virtual ~BoxData() {}
    };

    virtual ~Scheduler();

    /// Returns the node associated with this scheduler.  Scheduler
    /// code may invoke this to gain access to the node's catalog.
    AuroraNode& node() { return *_node; }

    /// Called when a node is started.  The implementation of this
    /// method should create one or more threads to run boxes.
    virtual void start() = 0;

    /// Called when a node is stopped.  The implementation of this
    /// method must stop any threads that are running boxes and
    /// wait for their termination (i.e., join them).
    virtual void shutdown() = 0;

    static double  ticks_adj_factor;

    /// Schedules a task to be run as soon as possible, at some point
    /// when no boxes are running.
    ///
    /// For example, the scheduler may maintain a thread-safe vector
    /// of ptr<SchedulerTask>s.  Whenever this vector becomes
    /// non-empty, it can signal box-running threads to pause;
    /// wait for all pending run() calls to complete; and
    /// then run the tasks in one of the box-running threads.
    virtual void schedule_exclusive_task(ptr<SchedulerTask>) = 0;

    /// Invoked when the network topology changes, i.e., one or more
    /// boxes or streams were added or removed.  This method may only
    /// be invoked from within an exclusive task (since the network
    /// topology may only change within an exclusive task), and only
    /// if the scheduler supports dynamic networks.
    virtual void topology_changed() { ASSERT(_dynamic_support); }

    /// Returns true iff the scheduler supports dynamic networks
    /// (<i>n&eacute;e</i> MMH2).
    bool has_dynamic_support() { return _dynamic_support; }

    /// Drains the network: waits until there are no more tuples at
    /// all in the network and then shuts the network.  Blocks until
    /// this is complete, at which point the only valid operation is
    /// shutdown().  May be invoked only if the scheduler supports
    /// draining.
    virtual void drain() { ASSERT(_drain_support); }

    /// Returns true iff the scheduler support draining.
    bool has_drain() { return _drain_support; }

    static unsigned long ticks();

    // On a 2.8GHz box.  Yee-haw!
    // 3932 on my Athlon machine. I think.
    // TODO: Need to measure this properly
    // PseudoScheduler will now measure this number by sleeping for a second
    // and computing the number of instructions. There must be a better way (?)
    static double seconds_per_tick()
      {
    return (1.0/Scheduler::ticks_adj_factor) * ((1<<21) / 3.9e9);
      }

    // 2.8GHz box; throwing away the low-order 21 bits of the
    // RTDSC register; so there are 2.8/(2^21) ticks per second
    static unsigned long ticks_per_second() { return 1335; }

    // -alexr my stuff, put it here for now
    // SchedulerStats _sched_stats;  // moved to psm???

    //    virtual void removeBoxes( vector<string> boxNames );
    virtual void invalidate_boxes( vector<string> boxNames );

    virtual void update_listeners( vector<string> boxNames, bool add );

    virtual void choke_inputs(vector<string> input_names) {} // -- tatbul
    virtual void resume_inputs(vector<string> input_names) {} // -- tatbul

    virtual void choke_sub_network ( vector<string> boxNames );

    virtual void suspend_sub_network ( vector<string> boxNames );

    /**
     *This one resume Boxes and Listeners
     */
    virtual void resume_sub_network( vector<string> boxNames );

    /**
     * This one resumes boxes' streams.
     */
    virtual void resume_box_streams( vector<string> boxNames );

    virtual void drain_sub_network( vector<string> boxNames );

    virtual void remove_from_schedule(QBox* box) { }

    virtual string to_string();

/************************
    // windowsSize unit -- ms, historySize unit -- windows
    virtual void init_sched_stats( int window_size, int history_size );
***************************/    

    // olga
    virtual void delete_scheduler_statistics(string statistics_name, vector <string> &ids);

    void set_event_handler(ptr<SchedulerEventHandler> event_handler);
    
    ptr<SchedulerEventHandler> get_event_handler();
    
  protected:
    //unsigned int _init_time;

    void set_box_data(QBox& box, ptr<BoxData> data);
    BoxData *get_box_data(const QBox& box) const;
    ptr<SchedulerEventHandler> _event_handler;

    /// Constructor.
    ///
    /// @param dynamic_support indicates whether the scheduler
    ///     implementation supports dynamic networks.
    /// @param drain_support indicates whether the scheduler
    ///     implementation supports draining.
    Scheduler(bool dynamic_support = false, bool drain_support = false);

  private:
    AuroraNode *_node;

    bool _dynamic_support;
    bool _drain_support;
};

inline unsigned long Scheduler::ticks()
{
    unsigned int low, high;
    asm volatile ("rdtsc" : "=a" (low), "=d" (high));

    return (high<<11) + (low>>21);
}


BOREALIS_NAMESPACE_END

#endif
