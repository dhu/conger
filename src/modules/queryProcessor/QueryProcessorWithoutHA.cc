#include "QueryProcessor.h"

BOREALIS_NAMESPACE_BEGIN

void QueryProcessor::set_scheduler_event_handler()
{
}

void QueryProcessor::handle_box_creation(ptr<QBox> box, bool primary)
{
}

void QueryProcessor::send(ptr<CheckpointMessage> cm)
{
}

RPC<void> QueryProcessor::enqueue(ptr<CheckpointMessage> cm)
{
    return true;
}

void QueryProcessor::checkpoint_message_enqueued(MedusaID backup, RPC<void> result)
{
}

void QueryProcessor::ack(ptr<CheckpointMessage> cm)
{
}

RPC<void> QueryProcessor::end_checkpoint(ptr<CheckpointMessage> cm)
{
    return true;
}

void QueryProcessor::checkpoint_completed(MedusaID primary, RPC<void> result)
{
}

void QueryProcessor::push_processing_times(MedusaID backup, vector<string> ha_units, vector<ptr<SlidingWindowStatistics> > processing_times)
{
}

RPC<void> QueryProcessor::update_processing_times(MedusaID primary, vector<string> ha_units, vector<ptr<SlidingWindowStatistics> > processing_times)
{
    return true;
}

void QueryProcessor::push_completed(MedusaID backup, RPC<void> result)
{
}

BOREALIS_NAMESPACE_END

