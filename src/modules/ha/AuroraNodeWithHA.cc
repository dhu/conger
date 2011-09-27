#include "AuroraNode.h"
#include "HA.h"

BOREALIS_NAMESPACE_BEGIN

Time next_time = Time::now();

void AuroraNode::DequeueHandle::handle(Time in_time)
{
    long latency = (Time::now() - in_time).to_msecs();
    if (HA::get_HA_method() == PASSIVE_STANDBY || HA::get_HA_method() == ACTIVE_STANDBY)
    {
        if(in_time > HA::last_end_time && latency < HA::get_checkpoint_slack() && (!HA::ready))
            HA::ready = true;
    }
    if (HA::get_HA_method() == ACTIVE_STANDBY)
    {
        if (_node != NULL && (in_time > next_time)) {
            ptr<HA> ha = _node->get_scheduler_event_handler().dynamic_cast_to<HA>();
            ha->get_backup_staleness();
            while (in_time > next_time)
                next_time += Time::msecs(1000);
        }
    }
}

BOREALIS_NAMESPACE_END
