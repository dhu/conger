#include "AvailabilityMonitor.h"
#include "BasicComponent.h"

BOREALIS_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////
AvailabilityMonitor::AvailabilityMonitor(string id)
    : BasicComponent(id, "AvailabilityMonitor"),
      //_ping_interval(DEFAULT_NODE_MONITOR_INTERVAL),
      _ping_interval(100),
      _ping_start(DEFAULT_NODE_MONITOR_START),
      _started(false)
{}



///////////////////////////////////////////////////////////////////////////
void AvailabilityMonitor::in_thread_init()
{
    // Query processor starts the AvailabilityMonitor if need be

}

///////////////////////////////////////////////////////////////////////////
MultiRPC< RStreamStatesIndex >
AvailabilityMonitor::set_callback()
{
    MultiRPC< RStreamStatesIndex > completion;
    _callback = completion;
    return completion;

}

///////////////////////////////////////////////////////////////////////////
RPC<void> AvailabilityMonitor::start()
{

    INFO << "AvailabilityMonitor starting monitoring task";
    if ( !_started)
    {
        _started = true;

        // Prepare to monitor primaries
        INFO << "Will ping primaries every " << _ping_interval
             << "msec starting in " << _ping_start << "msec";
        (new CallbackTimer(_my_loop,wrap(this,&AvailabilityMonitor::monitor_nodes)))->arm(Time::now() + Time::msecs(_ping_start));
    }

    return true;

}

///////////////////////////////////////////////////////////////////////////
RPC<void> AvailabilityMonitor::stop()
{
    INFO << "AvailabilityMonitor suspending monitoring task";
    _started = false;
    return true;
}


///////////////////////////////////////////////////////////////////////////
RPC<void> 
AvailabilityMonitor::add_node(InetAddress node)
{

    // Don't add if it's already there
    AvailabilityMonitor::MonitoredNode* monitored_node = lookup(_nodes,node);

    if ( ! monitored_node )
    {
        INFO << "Adding node " << node;
        _nodes[node] = new AvailabilityMonitor::MonitoredNode(node);
    }

    return true;
}

 
///////////////////////////////////////////////////////////////////////////
RPC<void> 
AvailabilityMonitor::remove_node(InetAddress node)
{

    // Don't add if it's already there
    AvailabilityMonitor::MonitoredNode* monitored_node = lookup(_nodes,node);    

    if ( monitored_node )
    {
        INFO << "Removing node " << node;
        _nodes.erase(node);
        _changed_nodes.erase(node);
        delete monitored_node;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////
/// Update the state that is advertised to other ndoes
RPC<void> 
AvailabilityMonitor::set_state(PerOutputStreamStates updated_state)
{
    _local_state = updated_state;
    INFO << "Updated state to " << updated_state;
    return true;
}



///////////////////////////////////////////////////////////////////////////
RPC<PerOutputStreamStates>
AvailabilityMonitor::get_state()
{
    DEBUG << "Got keep alive message, returning state " << _local_state;
    return _local_state;
}




///////////////////////////////////////////////////////////////////////////
void
AvailabilityMonitor::monitor_nodes()
{

    // Check if we're supposed to be stopped
    if ( !_started)
        return;

    // If we accumulated a bunch of state changes since last time, notify our client
    if ( _changed_nodes.size() > 0 )
    {
        notify_client();
        _changed_nodes.clear();
    }


    AvailabilityMonitor::MonitoredNodes::iterator i;
    for ( i = _nodes.begin(); i != _nodes.end(); ++i )
    {

        AvailabilityMonitor::MonitoredNode* node = i->second;
        InetAddress node_id = i->first;
        DEBUG << "Monitoring node " << node_id;

        // If there was already a pending ping, increase nb failed pings
        // And declare a node failure when we reach a few consecutive ping failures
        if ( node->_started && node->_pending_ping )
        {
            DEBUG << "There is a pending ping ";
            node->_failed_pings++;
            if ( node->_failed_pings > AvailabilityMonitor::MonitoredNode::MAX_FAILED_PINGS)
            {
                INFO << "Failed node " << node_id;
                PerOutputStreamStates empty_state;
                if ( node->update_state(empty_state) )
                { _changed_nodes[node_id] = node;
                }
            }
        }

        // Otherwise send a ping
        //else
        //{
        node->_pending_ping = true;

        // Until the AvailabilityMonitor starts, we need to try and get a new endpoint every time
        if ( !node->_started )
        { clear_endpoints_for(MedusaID(to_string(i->first)));
        }

        Remote<AvailabilityMonitor> remote_avail_monitor = avail_mon_for(MedusaID(to_string(i->first)));
        if (!remote_avail_monitor)
        {  INFO << "No Availability monitor component for " << i->first << "!";
        }
        
        else
        { 
            DEBUG << "Requesting state";
            remote_avail_monitor.get_state(wrap(this,&AvailabilityMonitor::monitor_nodes_2,node_id));
        }
        //}
    }

    // Re-scheduling monitoring task
    (new CallbackTimer(_my_loop,wrap(this,&AvailabilityMonitor::monitor_nodes)))->arm(Time::now() + Time::msecs(_ping_interval));

}

///////////////////////////////////////////////////////////////////////////
void
AvailabilityMonitor::monitor_nodes_2(InetAddress node_id, RPC<PerOutputStreamStates> result)
{

    AvailabilityMonitor::MonitoredNode* node = lookup(_nodes,node_id);
    if ( node == NULL )
    {
        ERROR << "Got ping response for unknown primary";
        return;
    }

    node->_pending_ping = false;
    DEBUG << "Got response";

    if ( result.valid() )
    {
        node->_started = true;
        node->_failed_pings = 0;
        PerOutputStreamStates current_state = *result;
        DEBUG << "Keep alive response " << current_state;
        if ( node->update_state(current_state) )
        { _changed_nodes[node_id] = node;
        }
    }
 
    else if ( node->_started)
    {
        INFO << "Keep alive response: failed " << result.stat();
        node->_failed_pings +=1;
        if ( node->_failed_pings > AvailabilityMonitor::MonitoredNode::MAX_FAILED_PINGS)
        {
            node->_failed_pings = 0;
            DEBUG << "Node failed " << *node;
            PerOutputStreamStates current_state;
            if ( node->update_state(current_state) )
            { _changed_nodes[node_id] = node;
            }
        }
    }

}


///////////////////////////////////////////////////////////////////////////
void
AvailabilityMonitor::notify_client()
{
    INFO << "Should notify client about changes ";
    RStreamStatesIndex index_changes;
   
    // Go through all nodes that changed, and add their latest state to index
    MonitoredNodes::iterator node_iter;
    for ( node_iter = _changed_nodes.begin(); node_iter != _changed_nodes.end(); ++node_iter)
    {
        InetAddress node_id = node_iter->first;
        MonitoredNode* node = node_iter->second;
        index_changes[node_id] = node->_current_state;
    }

    _callback.post(index_changes);

}


///////////////////////////////////////////////////////////////////////////
bool
AvailabilityMonitor::MonitoredNode::update_state(PerOutputStreamStates& new_state)
{

    bool changed = false;

    // Initialize the state if not yet initialized
    if ( _current_state.size() == 0 )
    { 
        _current_state = new_state;
        return changed;
    }



    // First check for TOTAL failures
    // ALL streams should be in FAILURE state. Just checking the first one
    if ( ( new_state.size() == 0 ) && 
         ( _current_state.size() > 0 ) &&
         ( _current_state.begin()->second != ConsistencyState::FAILURE ) ) 
    {
        changed = true;
        DEBUG << "New failure. Updating state of all streams to FAILURE";
        PerOutputStreamStates::iterator i;
        for ( i = _current_state.begin(); i != _current_state.end(); ++i)
        { i->second = ConsistencyState::FAILURE;
        }
    } 

    // Otherwise compare the two states. As soon as one stream doesn't match, copy the whole thing over
    else
    {
        PerOutputStreamStates::iterator i;
        for ( i = _current_state.begin(); i != _current_state.end(); ++i )
        {
            Name stream_name = i->first;
            ConsistencyState stream_state = i->second;
            //DEBUG << "Old state for stream " << stream_name << " is " << stream_state
            //    << " new state is " << new_state[stream_name];           
            if ( ( new_state.find(stream_name) == new_state.end() ) ||
                 ( new_state[stream_name] != stream_state ) )
            {
                DEBUG << "Advertised state changed. Copy over new state ";
                changed = true;
                _current_state = new_state;
                break;
            }
        }
    }

    return changed;
}




BOREALIS_NAMESPACE_END

//--- Added by nmstl-rpcgen; do not modify.  Leave at end of file!
#define NMSTL_RPC_DEFINE
#include "rpc_AvailabilityMonitor.h"
#undef NMSTL_RPC_DEFINE
//---
