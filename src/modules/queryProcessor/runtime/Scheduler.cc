#include "Scheduler.h"

BOREALIS_NAMESPACE_BEGIN

Scheduler::Scheduler(bool dynamic_support, bool drain_support) :
    _dynamic_support(dynamic_support), _drain_support(drain_support)
{
}

Scheduler::~Scheduler()
{
}

void Scheduler::invalidate_boxes( vector<string> boxNames )
{}

void Scheduler::update_listeners( vector<string> boxNames, bool add )
{}

void Scheduler::choke_sub_network ( vector<string> boxNames )
{}

void Scheduler::suspend_sub_network ( vector<string> boxNames )
{}

/**
 * This one resumes boxes' streams.
 */
void Scheduler::resume_box_streams( vector<string> boxNames )
{}

/**
 *This one resume boxes and listeners
 */
void Scheduler::resume_sub_network( vector<string> boxNames )
{}

void Scheduler::drain_sub_network( vector<string> boxNames )
{}

/*************************
    // windowsSize unit -- ms, historySize unit -- windows
void Scheduler::init_sched_stats( int window_size, int history_size )
{}
********************/


// olga
void Scheduler::delete_scheduler_statistics(string statistics_name,
                                   vector <string> &ids)
{}


string Scheduler::to_string()
{
    return "SCHEDULER:";
}

void Scheduler::set_event_handler(ptr<SchedulerEventHandler> event_handler)
{
    _event_handler = event_handler;
}

ptr<SchedulerEventHandler> Scheduler::get_event_handler()
{
    return _event_handler;
}

    
BOREALIS_NAMESPACE_END
