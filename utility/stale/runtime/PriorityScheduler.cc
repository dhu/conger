#include "PriorityScheduler.h"
#include "AuroraNode.h"
#include "QBox.h"
#include "LockHolder.h"

#include <fstream>

BOREALIS_NAMESPACE_BEGIN

AURORA_DEFINE_SCHEDULER(PriorityScheduler);

PriorityScheduler::PriorityScheduler() :
    Scheduler(true, true),
    _die(false),
    _draining(false)
{
}

void PriorityScheduler::start()
{
    setup_listeners();

    INFO << "Launching priority-scheduler with " << node().repr();

    // Start the work thread
    pthread_create(&_ps_thread, 0, &PriorityScheduler::launch, this);
    INFO << "Started priority-scheduler";
}

////////////////////////////////////////////////////////////////////////////////
//
void PriorityScheduler::setup_listeners()
{
    CatalogStream   *input;
    set<ptr<QBox> >::const_iterator  i;
//..............................................................................


    // Set up listeners
    set<ptr<QBox> > boxes = node().get_boxes();

    for ( i = boxes.begin(); i != boxes.end(); ++i)
    {
        QBox *box = i->get();

        if ( !_setup_boxes.insert( box ).second )   // IF already setup
        {   continue;
        }

        // Set up box data
        ptr<MyBoxData>  box_data( new MyBoxData(*box) );
        set_box_data(*box, box_data);

        INFO << "Box " << box->get_name()
             << " has priority " << box_data->_priority
             << " and weight " << box_data->_weight;

        for ( uint32 j = 0; j < box->get_num_inputs(); ++j )
        {
            DEBUG << "Adding listener to " << box->get_name() << " #" << j;
            TupleQueue *q = box->get_input_queue( j );

            input = box->get_input( j ).get_local_stream();
            DEBUG << " - its input stream is " << input->get_stream_name();

            if ( !input->is_input_stream() )
            {
                DEBUG << "   - which is from box "
                      << input->get_stream_source()->get_port_box()->get_box_name();

                ptr<MyBoxTupleQueueListener> tql( new MyBoxTupleQueueListener );
                tql->_ps = this;
                tql->set_box(box);
                _tqlisteners.push_back(tql);
                q->add_listener(tql.get());
            }
            else
            {
                DEBUG << "   - which is an input stream";

                ptr<MyInputTupleQueueListener> tql( new MyInputTupleQueueListener );
                tql->_ps = this;
                tql->set_box(box);
                _tqlisteners.push_back(tql);
                q->add_listener(tql.get());
            }
        }
    }
}

void PriorityScheduler::init_sched_stats( int window_size, int history_size )
{
    /*for (set<ptr<QBox> >::const_iterator i = node().getBoxes().begin();
         i != node().getBoxes().end(); ++i)
    {
        string box_name = i->get()->getName();

        _sched_stats._box_costs[ box_name ] = deque<double>( historySize );
        }*/
    //_sched_stats._selectivities = 1;
        //_sched_stats._box_costs = 0;
        //_sched_stats._avg_queue_length = 0;
}

void PriorityScheduler::shutdown()
{
    if (_ps_thread)
    {
        INFO << "Terminating scheduler thread...";

        {
            _die = true;
            LockHolder hold(_ps_lock);
            _ps_condition.signal();
        }

        pthread_join(_ps_thread, 0);
        INFO << "Scheduler thread has terminated.  Hasta la vista, baby.";
    }
}

PriorityScheduler::MyBoxData::MyBoxData(QBox& box) :
    _box(&box), _priority(0), _weight(100), _scheduled(false), _in_pending(false), _running_time(0)
{
    try
    {   box.typed_param("qos:priority", _priority);
    }
    catch (AuroraBadEntityException& e)
    {   WARN << e;
    }

    try
    {   box.typed_param("qos:weight", _weight);
    }
    catch (AuroraBadEntityException& e)
    {   WARN << e;
    }
}

void PriorityScheduler::schedule_box(MyBoxData& box)
{
    if (box._scheduled)
    {
        DEBUG << "Not scheduling box " << box._box->get_name() << " (already scheduled)";
        return;
    }

    DEBUG << "Scheduling box " << box._box->get_name();

    PriorityLevel& pri = _schedules[box._priority];
    if (!pri._scheduled)
    {
        DEBUG << " - adding priority " << box._priority << " to list of running priority levels";
        _priorities_to_run.push(box._priority);
        pri._scheduled = true;
    }
    pri._boxes_next.push_back(&box);
    box._scheduled = true;
}

void PriorityScheduler::schedule_pending()
{
    for (vector<MyBoxData*>::iterator i = _ext_boxes_to_run.begin();
         i != _ext_boxes_to_run.end();
         ++i)
    {
        (*i)->_in_pending = false;
        schedule_box(**i);
    }
    _ext_boxes_to_run.clear();
}

void PriorityScheduler::run()
{
    // Never run a box for less than MIN_SLICE_MSEC ms
    static const int MIN_SLICE_MSEC = 5;

    // Never run a box for more than MAX_SLICE_MSEC ms
    static const int MAX_SLICE_MSEC = 100;

    // Run each box at least every CYCLE_MSEC ms
    static const int CYCLE_MSEC = 500;

    NOTICE << "Priority Scheduler running";

    QBoxInvocation inv;
    vector<QBox*> boxes_to_run;
    Tasks tasks;

    while (1)
    {
        tasks.clear();
        boxes_to_run.clear();

        {
            LockHolder hold(_ps_lock);
            ASSERT(check_rep());

            while (_priorities_to_run.empty() && _ext_boxes_to_run.empty() && _tasks.empty() && !_die)
            {
                if (_draining)
                {
                    INFO << "Network is drained; exiting";
                    _die = true;
                    break;
                }

                DEBUG << "Nothing to do - scheduler is napping";
                if (_event_handler)
                    _event_handler->handle_idle_time();
                _ps_lock.wait_cond(_ps_condition);
            }

            if (_die)
            {
                // Still have to run any outstanding tasks
                DEBUG << "Scheduler: that's all folks";
                for (Tasks::iterator i = _tasks.begin(); i != _tasks.end(); ++i)
                {
                    DEBUG << "Running a task before dying";
                    (*i)->run();
                }

                return;
            }

            // save tasks to run
            tasks.swap(_tasks);

            // merge in pending tasks
            schedule_pending();

            ASSERT(check_rep());

            // release lock
        }

        // run tasks
        for (Tasks::iterator i = tasks.begin(); i != tasks.end(); ++i)
        {
            DEBUG << "Running a task " << typeid(**i);
            (*i)->run();
            DEBUG << "Done running task " << typeid(**i);
        }

        if (_priorities_to_run.empty())
            continue;

        int priority_to_run = _priorities_to_run.top();
        PriorityLevel& level = _schedules[priority_to_run];
        if (level._boxes_to_run.empty())
        {
            DEBUG << "Setting up priority level " << priority_to_run << " ("
                  << level._boxes_next.size() << " boxes)";
            level._boxes_next.swap(level._boxes_to_run);

            int total_weights = 0;
            for (vector<MyBoxData*>::const_iterator i = level._boxes_to_run.begin();
                 i != level._boxes_to_run.end();
                 ++i)
            {
                total_weights += (*i)->_weight;
            }
            DEBUG << " - total weights: " << total_weights;

            for (vector<MyBoxData*>::iterator i = level._boxes_to_run.begin();
                 i != level._boxes_to_run.end();
                 ++i)
            {
                (*i)->_running_time = max(MIN_SLICE_MSEC, (*i)->_weight * CYCLE_MSEC / total_weights);
                DEBUG << " - will run " << (*i)->_box->get_name() << " for " << (*i)->_running_time << " ms";
            }
        } else
        {
            DEBUG << "Resuming priority level " << priority_to_run << " ("
                  << level._boxes_to_run.size() << " left)";
        }

        while (1)
        {
            ASSERT(priority_to_run == _priorities_to_run.top());
            if (level._boxes_to_run.empty())
            {
                DEBUG << "Done running stuff in level " << priority_to_run;
                if (level._boxes_next.empty())
                {
                    DEBUG << " - nothing scheduled in the future either; unscheduling this level";
                    level._scheduled = false;
                    _priorities_to_run.pop();
                }
                break;
            }

            MyBoxData* d = level._boxes_to_run.back();
            level._boxes_to_run.front();

            QBoxInvocation inv;
            int running_time_now = min(MAX_SLICE_MSEC, d->_running_time);
            INFO << "Running box " << d->_box->get_name() << " for " << running_time_now << " ms ("
                 << d->_running_time << " ms left this cycle)";
            d->_running_time -= running_time_now;

            inv._end_time = Scheduler::ticks() + running_time_now * 1000 / Scheduler::ticks_per_second();
            d->_box->run(inv);

            // TODO: charge box for amount of time actually run

            bool box_done = true;

            unsigned int num_inputs = d->_box->get_num_inputs();
            for (unsigned int j = 0; box_done && j < num_inputs; ++j)
                if (d->_box->get_input_queue(j)->size())
                {
                    DEBUG << " - input " << j << " not done; will run again";
                    box_done = false;
                }

            if (box_done)
            {
                DEBUG << " - all done";
                d->_scheduled = false;
                ASSERT(level._boxes_to_run.back() == d);
                level._boxes_to_run.pop_back();
            } else if (d->_running_time)
            {
                DEBUG << " - need to run it again; will do so right now";
            } else
            {
                DEBUG << " - need to run it again; will do so next cycle";
                ASSERT(level._boxes_to_run.back() == d);
                level._boxes_to_run.pop_back();
                level._boxes_next.push_back(d);
            }

            // need to schedule any pending boxes in case anything
            // higher-priority came up
            {
                LockHolder hold(_ps_lock);
                schedule_pending();
            }

            if (priority_to_run != _priorities_to_run.top())
            {
                DEBUG << "Preempted by level " << _priorities_to_run.top() << "!";
                // something higher-priority came up!
                break;
            }
        }
    }
}

/*
void PriorityScheduler::addBoxes( )
{

    // is this it? add boxes needs no paramters, assumed that catalog has been updated already.
    topologyChanged();

}
*/

void PriorityScheduler::invalidate_boxes( vector<string> boxNames )
{}

void PriorityScheduler::update_listeners( vector<string> boxNames, bool add )
{}

/*
  Removes a set of boxes from the running network
*/

/*

void PriorityScheduler::removeBoxes( vector<string> boxNames )
{
    // check if dynamic removal/addition is supported?

    // this has to change, but for now lock the operation for box movement.
    // problem is, box could be either currently exucution or be in a similar precarious state.
    {
    LockHolder hold(_ps_lock);
    //bool box_exists = false;

    for(vector<string>::iterator i = boxNames.begin(); i != boxNames.end(); i++)
        {
        string box = (*i);
        DEBUG << "Removing box " << box;

          box_exists = false;

          for ( vector<string>::iterator iter = _disabled_boxes.begin();
          iter != _disabled_boxes.end(); iter++ )
          {
          if ( (*iter) == box )
          {
          cerr << "WARNING: disabling an already disabled box " << box;
          box_exists = true;
          }

          if ( !box_exists )
          _disabled_boxes.push_back( box );
          }

        // check the currently scheduled boxes and remove as well

        // don't see a convinient way to lookup box by name. Need to write it or to find it.
        PriorityLevel& pri = _schedules[ 0 ];

        for (vector<MyBoxData*>::iterator it =  pri._boxes_next.begin(); it != pri._boxes_next.end(); it ++ )
            if ( (*it)->_box->getName() == box )
            {
                pri._boxes_next.erase( it );
                break;
            }

        for (vector<MyBoxData*>::iterator it =  pri._boxes_to_run.begin(); it != pri._boxes_to_run.end(); it ++ )
            if ( (*it)->_box->getName() == box )
            {
                pri._boxes_to_run.erase( it );
                break;
            }

        // check all listeners, which may have pointers to our box.
        for (TQListeners::iterator i = _tqlisteners.begin(); i != _tqlisteners.end(); )
        {
            if ( (*i)->getBox() )
            {
             QBox* b = (*i)->getBox();
             if ( b->getName() == box)
             {
                 for (unsigned int j = 0; j < b->getNumInputs(); ++j)
                 {
                 TupleQueue *q = b->getInputQueue(j);
                 q->removeListener((*i).get());
                 }
                 i = _tqlisteners.erase(i);
            } else { i++; }
            } else { i++; }
        }
        }
    }
}

void PriorityScheduler::activateBoxes()
{
    // for now just call topology changed (and clear the disabled boxes)
    //_disabled_boxes.clear();

    topologyChanged();
}

*/

void PriorityScheduler::drain()
{
    _draining = true;

    ASSERT(_ps_thread);

    INFO << "Scheduler draining...";
    {
        LockHolder hold(_ps_lock);
        _ps_condition.signal();
    }
    pthread_join(_ps_thread, 0);
    _ps_thread = 0;
}

void PriorityScheduler::topology_changed()
{
    INFO << "Topology changed";
    setup_listeners();
}


void PriorityScheduler::MyBoxTupleQueueListener::notify(const TupleQueue&)
{
    //    DEBUG << "Notifying scheduler (from scheduler thread) that it has to run " << _box_to_run->getName();
    _ps->schedule_box(_ps->get_my_box_data(*_box_to_run));
}

void PriorityScheduler::MyInputTupleQueueListener::notify(const TupleQueue&)
{
    //    DEBUG << "Notifying scheduler (from input) that it has to run " << _box_to_run->getName();

    LockHolder hold(_ps->_ps_lock);
    MyBoxData& box = _ps->get_my_box_data(*_box_to_run);
    if (!box._scheduled && !box._in_pending)
    {
        box._in_pending = true;
        _ps->_ext_boxes_to_run.push_back(&_ps->get_my_box_data(*_box_to_run));
        _ps->_ps_condition.signal();
    }
}

void PriorityScheduler::schedule_exclusive_task(ptr<SchedulerTask> task)
{
    LockHolder hold(_ps_lock);
    _tasks.push_back(task);
    DEBUG << "Scheduled " << typeid(*task);
    _ps_condition.signal();
}

bool PriorityScheduler::check_rep() const
{
    // Make sure that each priority occurs only once,
    // and that level._scheduled <=> level in _priorities_to_run
    priorities_to_run prirun_copy(_priorities_to_run);

    set<int> prirun_set;
    while (!prirun_copy.empty())
    {
        int pri = prirun_copy.top();
        if (!prirun_set.insert(pri).second)
            WARN << "checkRep: Multiple copies of priority " << pri << " in _priorities_to_run";
        if (_schedules.find(pri) == _schedules.end())
            WARN << "checkRep: Unknown priority " << pri << " in _priorities_to_run";

        prirun_copy.pop();
    }

    for (map<int, PriorityLevel>::const_iterator i = _schedules.begin(); i != _schedules.end(); ++i)
    {
        bool in_set = prirun_set.count(i->first);
        if (i->second._scheduled != in_set)
            WARN << "checkRep: Level " << i->first << ": _scheduled="
                 << i->second._scheduled << "; in _priorities_to_run is " << in_set;
    }


    // Check box data stuff
    set<MyBoxData*> pending_set;
    for (vector<MyBoxData*>::const_iterator i = _ext_boxes_to_run.begin(); i != _ext_boxes_to_run.end(); ++i)
        if (!pending_set.insert(*i).second)
            WARN << "checkRep: Multiple copies of " << (*i)->_box->get_name() << " in _ext_boxes_to_run";

    for (set<QBox*>::const_iterator i = _setup_boxes.begin(); i != _setup_boxes.end(); ++i)
    {
        MyBoxData& d = get_my_box_data(**i);
        bool in_pending = pending_set.count(&d);
        if (d._in_pending != in_pending)
            WARN << "checkRep: Box " << d._box->get_name() << ": _in_pending="
                 << d._in_pending << "; in _ext_boxes_to_run is " << in_pending;


        map<int, PriorityLevel>::const_iterator j = _schedules.find(d._priority);
        if (j == _schedules.end())
        {
            if (d._scheduled)
            {
                WARN << "checkRep: Box " << d._box->get_name() << ": scheduled but no priority level "
                     << d._priority;
            }
            continue;
        }

        const PriorityLevel& l = j->second;
        int boxes_next_count = count(l._boxes_next.begin(), l._boxes_next.end(), &d);
        int boxes_to_run_count = count(l._boxes_to_run.begin(), l._boxes_to_run.end(), &d);
        int correct_count = d._scheduled ? 1 : 0;

        if (boxes_next_count + boxes_to_run_count != correct_count)
        {
            WARN << "checkRep: Box " << d._box->get_name() << ": "
                 << (d._scheduled ? "scheduled" : "unscheduled")
                 << " box box appears in _boxes_next "
                 << boxes_next_count << " times and _boxes_to_run_count " << boxes_to_run_count << " times"
                 << " (should be " << correct_count << " total)";
        }
    }

    return true;

    // Could return false if something catastrophic happened
}

string PriorityScheduler::to_string()
{
    return "STRING2";
}

BOREALIS_NAMESPACE_END
