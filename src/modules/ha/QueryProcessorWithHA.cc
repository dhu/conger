#include "QueryProcessor.h"
#include "HA.h"

BOREALIS_NAMESPACE_BEGIN

#define ptr_HA _aurora.get_scheduler_event_handler().dynamic_cast_to<HA>()
   
void QueryProcessor::set_scheduler_event_handler()
{
    if (HA::get_HA_method() == PASSIVE_STANDBY || HA::get_HA_method() == ACTIVE_STANDBY)
        _aurora.set_scheduler_event_handler(ptr<SchedulerEventHandler>(new HA(_id.get_id(), *this)));
}

void QueryProcessor::handle_box_creation(ptr<QBox> box, bool primary)
{
    if (HA::get_HA_method() != NO_HA)
    {
        ptr_HA->form_ha_unit(box, primary);
        if (!primary) // if the box is a backup
        {
            if (HA::get_HA_method() == PASSIVE_STANDBY)
                box->deactivate();
            box->set_backup();
        }
    }
}

void QueryProcessor::send(ptr<CheckpointMessage> cm)
{
    MedusaID backup_server = cm->get_backup_server();
    Remote<QueryProcessor> qp = qp_for(backup_server);
    if (qp)
    {
        qp.enqueue(wrap(this,&QueryProcessor::checkpoint_message_enqueued, backup_server), cm);
    } else
    {
        ERROR << "No query processor at " << backup_server << "!";
    }
}

RPC<void> QueryProcessor::enqueue(ptr<CheckpointMessage> cm)
{
    ptr_HA->enqueue(cm);
    return true;
}

void QueryProcessor::checkpoint_message_enqueued(MedusaID backup, RPC<void> result)
{
    ptr_HA->checkpoint_message_enqueued(backup);
}

void QueryProcessor::ack(ptr<CheckpointMessage> cm)
{
    string primary_server = cm->get_primary_server();
    Remote<QueryProcessor> qp = qp_for(primary_server);
    if (qp)
    {
        qp.end_checkpoint(wrap(this,&QueryProcessor::checkpoint_completed, primary_server), cm);
    } else
    {
        ERROR << "No query processor at " << primary_server << "!";
    }
}

RPC<void> QueryProcessor::end_checkpoint(ptr<CheckpointMessage> cm)
{
    ptr_HA->end_checkpoint(cm);
    return true;
}

void QueryProcessor::checkpoint_completed(MedusaID primary, RPC<void> result)
{
}

void QueryProcessor::push_processing_times(MedusaID backup, vector<string> ha_units, vector<ptr<SlidingWindowStatistics> > processing_times)
{
    Remote<QueryProcessor> qp = qp_for(backup);
    if (qp)
    {
        qp.update_processing_times(wrap(this,&QueryProcessor::push_completed, backup), _id, ha_units, processing_times);
    } else
    {
        ERROR << "No query processor at " << backup << "!";
    }

}

RPC<void> QueryProcessor::update_processing_times(MedusaID primary, vector<string> ha_units, vector<ptr<SlidingWindowStatistics> > processing_times)
{
    ptr_HA->update_processing_times(primary, ha_units, processing_times);
    return true;
}

void QueryProcessor::push_completed(MedusaID backup, RPC<void> result)
{
}

BOREALIS_NAMESPACE_END

