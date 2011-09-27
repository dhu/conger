#ifndef AVAILMONITOR_H
#define AVAILMONITOR_H

#include "BasicComponent.h"
#include "StreamState.h"

BOREALIS_NAMESPACE_BEGIN


/**
 * The AvailabilityMonitor monitors the state of 
 * output streams at other processing nodes.
 * It does so by periodically communicating with these
 * nodes and requestiong the information about the states of their
 * output streams. If any of these states change, the Availability Monitor
 * alerts the components that registered to know about state changes
 * The AvailabilityMonitor also informs other nodes about the state
 * of local output streams
 */
class AvailabilityMonitor : public BasicComponent
{

 private:

    // RPC: #include "ConsistencyState.h"

    /**
     * Represents one node that we are monitoring
     */
    class MonitoredNode
    {
    public:

        MonitoredNode(InetAddress id) :
            _endpoint(id), 
            _started(false), 
            _failed_pings(0) 
        {
        }

        /// Update the state of the monitored node. 
        /// An empty new_state structure indicates failure of 
        /// all streams
        /// Returns true if the state changed, false otherwise.
        bool update_state(PerOutputStreamStates& new_state);

        string as_string() const
        { return "Monitored {id=" + to_string(_endpoint) + "}";
        }

        /// Unique ID of this node
        InetAddress _endpoint;

        /// Was the monitored node ever up and running
        bool _started;

        /// Number of consecutive failed state_update 
        int _failed_pings;

        // After a few failed pings we consider monitored node to have failed
        static const int MAX_FAILED_PINGS = 3;

        /// Some keep alives just never return so we need to time them out
        bool _pending_ping;

        /// The last state information from the node
        PerOutputStreamStates _current_state;

    };


 public:

    /**
     * Constructor. The Availability Monitor is initially stopped.
     * @param id Name of the AvailabilityMonitor component (default is AvailabilityMonitor).
     */
    AvailabilityMonitor(string id);

    ~AvailabilityMonitor() {}

    /**
     * The AvailabilityMonitor performs its tasks on behalf of a client
     * The client registers a callback to get notifications of changes in other node's states
     * The callback is invoked every time the state of one node changes.
     */
    MultiRPC< RStreamStatesIndex > set_callback();


    /**
     * Start the periodic monitoring process
     */
    RPC<void> start();

    /**
     * Suspend periodic monitoring
     */
    RPC<void> stop();

    
    /// Add a node to list of processing nodes that are monitored
    RPC<void> add_node(InetAddress node);

    /// Remove a node from list of processing nodes that are monitored
    RPC<void> remove_node(InetAddress node);

    /// Update the state that is advertised to other ndoes
    RPC<void> set_state(PerOutputStreamStates updated_state);

    /// Invoked by remote AvailabilityMonitors to get the latest state information
    RPC<PerOutputStreamStates> get_state();
   

    NMSTL_RPC_OBJECT(AvailabilityMonitor);


 protected:
    // Starts periodic primary monitoring
    void in_thread_init();

 private:

    /// Contacts all primaries by invoking keep-alive on them
    void monitor_nodes();

    /// Handles responses from get_state calls
    void monitor_nodes_2(InetAddress node, RPC<PerOutputStreamStates> result);

    /// Invoke calllback when interesting events occur
    void notify_client();

    typedef map<InetAddress, MonitoredNode* > MonitoredNodes;
    MonitoredNodes _nodes;
    MonitoredNodes _changed_nodes;

    /// The local state advertised to others
    PerOutputStreamStates _local_state;

    /// The callback to invoke when interesting events occur
    MultiRPC< RStreamStatesIndex > _callback;
 
    int _ping_interval;
    int _ping_start;
   
    /// true if AvailabilityMonitor is currently started
    bool _started;

};


NMSTL_TO_STRING(AvailabilityMonitor::MonitoredNode);

BOREALIS_NAMESPACE_END
#endif                          // AVAILMONITOR_H
