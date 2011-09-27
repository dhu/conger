#ifndef PSEUDO_SCHEDULER_H
#define PSEUDO_SCHEDULER_H

#include "TupleQueue.h"
#include "Scheduler.h"
#include "PseudoSchedulerMonitor.h"

BOREALIS_NAMESPACE_BEGIN

class QBox;

/// A simple scheduler implementation that launches only a single
/// thread to run boxes.
//
class PseudoScheduler : public Scheduler
{
  public:

    PseudoScheduler();

    void start();
    void shutdown();
    void schedule_exclusive_task(ptr<SchedulerTask>);
    void topology_changed();
    void drain();

    // Make sure box is not in any data structure!!!
    //
    void remove_from_schedule(QBox* b);

    // Boxes that are disabled at the moment (names)
    //
    vector<string> _disabled_boxes;

  private:

    struct MyBoxTupleQueueListener : public TupleQueueListener
    {
        void notify(const TupleQueue&);

        PseudoScheduler *_ps;
    };
    friend struct MyBoxTupleQueueListener;

    struct MyInputTupleQueueListener : public TupleQueueListener
    {
        void notify(const TupleQueue&);

        PseudoScheduler *_ps;
    };
    friend struct MyInputTupleQueueListener;

    bool _die;
    bool _draining;

    // condition/lock used for sleeping purposes when draining
    //
    pthread_t           _ps_thread;
    pthread_mutex_t     *_wait_mutex;
    pthread_cond_t      *_wait_cond;

    // which map has the latest result (note that there is no atomicity,
    // so this flag is updated *after* new value is inserted into shadow map).
    //
    //bool _use_shadow_boxmap;
    //bool _use_shadow_qlength;

    //unsigned long _last_boxcost_update;
    //unsigned long _last_qlength_update;
    //unsigned long _last_irate_update;
    //unsigned long _last_selectivity_update;
    //unsigned long _last_stream_rate_update;

    // Boxes we've already added listeners to
    //
    set<QBox*> _setup_boxes;

    //typedef vector<ptr<TupleQueueListener> > TQListeners;
    typedef vector<TupleQueueListener*> TQListeners;
    TQListeners _tqlisteners;

    PtMutex     _ps_lock;
    PtCondition _ps_condition;
    set<QBox*>  _ext_boxes_to_run;

    bool _perf_stats;
    PseudoSchedulerMonitor   _monitor;


    set<QBox*> _boxes_to_run;

    int _box_counter;
    map<string, int> _box_to_id;
    map<int, string> _id_to_box;
    int _box_markers[300];

    //virtual void removeBoxes(vector<string> boxNames);

    /**
     *  Removes the listeners referring to the boxes 
     *  that are no longer in the query.
     *  There might be some other box-related maintenance that will be needed.
     */
    void invalidate_boxes(vector<string> boxNames);

    /**
     *  Reconnects the listeners according to a network change. Two primary
     *  actions:
     *  1. Remove the listeners of the boxes that are gone by now.
     *  2. Change listeners of boxes that become node inputs (or stopped
     *  being node inputs).
     */
    void update_listeners(vector<string> boxNames, bool add);

    void choke_inputs(vector<string> input_names); // -- tatbul
    void resume_inputs(vector<string> input_names); // -- tatbul

    /** This method will be taking over for remove boxes. Thus remove
     *  boxes will soon be disabled.
     */
    void choke_sub_network(vector<string> boxNames);

    /** This method will be taking over for remove boxes. Thus remove
     *  boxes will soon be disabled.
     */
    void suspend_sub_network(vector<string> boxNames);

    /** This method will resume network operation after some boxes have
     *  been introduced. It will unblock all buffering streams in the
     *  sub-network. This only unblocks the streams.
     */
    void resume_box_streams(vector<string> boxNames);

     /** This method will resume network operation after some boxes have
      *  been introduced. It will unblock all buffering streams in the
      *   sub-network. 
      *   This one unblocks the boxes and the listeners at the streams.
      */
    void resume_sub_network(vector<string> boxNames);

    /** This method will drain a given subnetwork. The subnetwork will
     *  be executed until there are no more tuples remaining in the
     *  input queues. The method will block until draining is done.
     */
    void drain_sub_network(vector<string> boxNames);

    //virtual void activateBoxes();

    // busywait on CPU
    //
    //double use_cpu_time(double time_to_use);

    // windowsSize unit -- ms, historySize unit -- windows
    // this is being phased out to use Ying's structures.
    //
    //virtual void init_sched_stats(int window_size, int history_size);

    virtual void delete_scheduler_statistics(string statistics_name,
                                             vector<string> &ids);

    virtual string to_string();

    typedef vector<ptr<SchedulerTask> > Tasks;
    Tasks _tasks;

    void setup_listeners();

    static void *launch(void *pthis) 
    { 
        ((PseudoScheduler*)pthis)->run(); 
        return 0; 
    }

    void run();
    bool has_stale_primary(set<ptr<QBox> > boxes);
    bool has_stale_primary(set<QBox*> boxes);
    bool has_stale_primary(QBox* box);
};

BOREALIS_NAMESPACE_END

#endif
