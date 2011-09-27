#ifndef PRIORITY_SCHEDULER_H
#define PRIORITY_SCHEDULER_H

#include "TupleQueue.h"
#include "Scheduler.h"
#include <queue>

BOREALIS_NAMESPACE_BEGIN;

class QBox;

/// A simple scheduler implementation that launches only a single
/// thread to run boxes.
class PriorityScheduler : public Scheduler
{
  public:
    PriorityScheduler();

    void start();
    void shutdown();
    void schedule_exclusive_task(ptr<SchedulerTask>);
    void topology_changed();
    void drain();

    // Boxes that are disabled at the moment (names)
    vector<string> _disabled_boxes;

  private:


    struct MyBoxData : public BoxData
    {
        MyBoxData(QBox& box);

        QBox *_box;
        int _priority;
        int _weight;
        bool _scheduled;
        bool _in_pending;
        int _running_time;
    };
    struct PriorityLevel
    {
        PriorityLevel() : _scheduled(false) {}

        vector<MyBoxData*> _boxes_next;
        vector<MyBoxData*> _boxes_to_run;
        bool _scheduled;
    };
    map<int, PriorityLevel> _schedules;
    typedef priority_queue<int, vector<int>, greater<int> > priorities_to_run;
    priorities_to_run _priorities_to_run;

    void schedule_box(MyBoxData&);
    void schedule_pending();
    bool check_rep() const;

    MyBoxData& get_my_box_data(QBox& box) const
    {
        MyBoxData *data = static_cast<MyBoxData*>(Scheduler::get_box_data(box));
        ASSERT(data);
        return *data;
    }

    bool _die;
    bool _draining;

    pthread_t _ps_thread;

    virtual void invalidate_boxes( vector<string> boxNames );

    virtual void update_listeners( vector<string> boxNames, bool add );

    /*virtual void removeBoxes( vector<string> boxNames );

    virtual void addBoxes( );

    virtual void activateBoxes( );
    */

    // windowsSize unit -- ms, historySize unit -- windows
    virtual void init_sched_stats( int window_size, int history_size );

    virtual string to_string();

    struct MyBoxTupleQueueListener : public TupleQueueListener
    {
        void notify(const TupleQueue&);

        PriorityScheduler *_ps;
    };
    friend struct MyBoxTupleQueueListener;

    struct MyInputTupleQueueListener : public TupleQueueListener
    {
        void notify(const TupleQueue&);

        PriorityScheduler *_ps;
    };
    friend struct MyInputTupleQueueListener;

    // Boxes we've already added listeners to
    set<QBox*> _setup_boxes;

    typedef vector<ptr<TupleQueueListener> > TQListeners;
    TQListeners _tqlisteners;

    PtMutex _ps_lock;
    PtCondition _ps_condition;

    vector<MyBoxData*> _ext_boxes_to_run;

    typedef vector<ptr<SchedulerTask> > Tasks;
    Tasks _tasks;

    void setup_listeners();

    static void *launch(void *pthis) { ((PriorityScheduler*)pthis)->run(); return 0; }
    void run();

    AURORA_DECLARE_SCHEDULER(PriorityScheduler);
};

BOREALIS_NAMESPACE_END;

#endif
