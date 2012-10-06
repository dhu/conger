#ifndef QUERYPROCESSOR_H
#define QUERYPROCESSOR_H

#include "AuroraNode.h"
#include "BasicComponent.h"
#include "DataPath.h"
#include "StreamState.h"
#include "QueryState.h"
#include "HAMethod.h"
#include "ConsistencyMngr.h"
#include "QBoxState.h"
#include "CPQBox.h"
#include "TupleHeader.h"
#include "PendingQuery.h"
#include "Diagram.h"
#include "UpdateClient.h"
#include "congerDeploy/DeployDescript.h"
#include "CongerDiagram.h"
#include "ParseContext.h"

BOREALIS_NAMESPACE_BEGIN

typedef map<MedusaID, QueueTrimmingMessage> QueueTrimmingMessages;

/**
 * Facade to the whole QueryProcessor component
 */
class QueryProcessor : public BasicComponent
{
  public:

    QueryProcessor(string id, InetAddress data_add);

    virtual ~QueryProcessor();

    // RPC: #include "Name.h"
    // RPC: #include "Query.h"
    // RPC: #include "Stats.h"
    // RPC: #include "HAMethod.h"
    // RPC: #include "StreamEvent.h"
    // RPC: #include "BoxPackage.h"
    // RPC: #include "Statistics.h"
    // RPC: #include "TSStats.h"
    // RPC: #include "QueueTrimmingMessage.h"
    // RPC: #include "CheckpointMessage.h"
    // RPC: #include "SlidingWindowStatistics.h"

    // Merged in from Admin:
    // RPC: #include "Schema.h"
    // RPC: #include "StreamDef.h"
    // RPC: #include "CPViewDescription.h"
    // RPC: #include "NodeStatus.h"


 public:

    /////////////////////////////////////////////////
    /// conger deploy interface       by hellojinjie
    ///.............................................

    /// 这个方法在 CongerProcessor.cc 里实现
    ///
    AsyncRPC<void> add_conger_string(string conger_config);

 private:
    /// add schema to catalog
    ///
    void add_conger_schema(DeployDescript deploy_descript);

    /// add input stream to catalog
    ///
    void add_conger_input(DeployDescript deploy_descript);

    /// add query or box to catalog
    ///
    void add_conger_query(DeployDescript deploy_descript);

    /// 先用 add_conger_query 把一个 CQL 拆分成若干个 Box，再用这个方法add box
    ///
    void add_conger_box(string box_name, string type, string in_streams,
            string out_stream, map<string, string> box_parameters);

    /// add subscribe to catalog
    ///
    void add_conger_subscribe(DeployDescript deploy_descript);

    /// 将 antlr 解析的结果转化成 boxes
    ///
    void transform_cql_to_boxes(ParseContext& context);

    /// 处理 cql 有 join 的情况
    ///
    void transform_cql_join(ParseContext& context);

    /// 处理 cql 有 aggregate 的情况
    ///
    void transform_cql_aggregate(ParseContext& context);

    /// 处理 cql 有 map 的情况
    ///
    void transform_cql_map(ParseContext& context);

    /// 处理 cql 有 filter 的情况
    ///
    void transform_cql_filter(ParseContext& context);

    /// 处理 cql 有 union 的情况
    ///
    void transform_cql_union(ParseContext& context);

    void transform_cql_multi_boxes(ParseContext& context);

    /// 往一个文件里 dump box_parameters 主要用来测试
    ///
    void dump_box_parameters(string box_name, map<string, string> box_parameters);

    /// 下面的函数都是用来测试的，可以忽略
    ///
    void try_map();
    void try_filter();
    void try_filter_map();
    void try_union();
    void try_join();
    void try_aggregate();
    void try_aggregate_join();

 public:

    //////////////////////////////////////////////////
    ///
    /// XML catalog interface.
    ///...............................................


    /// Add a network update in Xml to a Borealis node.
    /// @param update_xml
    ///
    AsyncRPC<void>  add_xml_string(string  update_xml);


    ///  Add a vector of network update in Xml to a Borealis node.
    ///
    static  Status  add_xml_batch(// Node updates in XML.
                                  vector< string >  update_xml,

                                  // Provide access to the QueryProcessor.
                                  QueryProcessor   &query_processor);

 private:

    ////////////////////////////////////////////////////////////////////////////
    //
    // Head XML interface and Regional catalog reflections
    //..........................................................................
    
    ///  Establish a port to reflect updates to a regional optimizer.
    ///
    void  add_region_element(const DOMElement  *region)
                       throw(AuroraBadEntityException);


    ///  Establish a port to reflect updates to the Head.
    ///
    void  add_head_element(const DOMElement  *head)
                     throw(AuroraBadEntityException);


    ///  Establish a set of replicas in a local catalog.
    ///
    void  add_replica_element(const DOMElement  *replica)
                        throw(AuroraBadEntityException);

    ///  Add a box to a node.
    ///
    void  add_box_element(const DOMElement  *box)
                    throw(AuroraException);


    ///  Add a table to a node.
    ///
    void  add_table_element(const DOMElement  *table)
                      throw(AuroraException);


    ///  Add a schema declaration to a Borealis node.
    ///
    void  add_schema_element(const DOMElement  *schema,
                            string update_xml)
                       throw(AuroraException);

    ///  Add a input stream declaration to a Borealis node.
    ///
    void  add_input_element(const DOMElement   *input,
                             AsyncRPC<void>    &completion,
                             string update_xml)
                       throw(AuroraBadEntityException);


    ///  Add a subscriber to a Borealis node.
    ///
    void  add_subscribe_element(const DOMElement  *sub,
                                AsyncRPC<void>    &completion)
                       throw(AuroraBadEntityException);


    ///  Add a connection point to a Borealis node.
    ///
    void  add_view_element(const DOMElement  *schema)
                     throw(AuroraException);


    ///  Modify parameters of an existing box or table.
    ///
    void modify_element(const DOMElement  *modify)
                  throw(AuroraBadEntityException);


    ///  Move a box between nodes.
    ///
    void move_element(const DOMElement  *move,
                      AsyncRPC<void>    &completion)
                throw(AuroraBadEntityException);


    ///  Delete a query or boxes.  Note:  We may also want to delete streams.
    ///
    void delete_element(const DOMElement  *remove,
                        AsyncRPC<void>    &completion)
                  throw(AuroraBadEntityException);


    ///  Modify a box or table parameter.
    ///
    void modify_element(const DOMElement  *modify,
                        AsyncRPC<void>    &completion)
                  throw(AuroraException);


    ///  Insert a Filter and connect it.  <<< work in progress >>>
    ///    
    void  connect_element(const DOMElement  *connect,
                          string             update_xml,
                          AsyncRPC<void>    &completion)
                          throw(AuroraException);


    ///  Second part if a Filter insertion.  <<< work in progress >>>
    ///
    void  connect_element2(AsyncRPC<void>  completion,
                           string          modify_xml,
                           RPC<void>       result);


    ///  Disconnect a box from it's streams (prepare for move or delete).
    ///  <<< work in progress >>>
    ///
    void  disconnect_element(const DOMElement  *disconnect,
                             AsyncRPC<void>    &completion)
                       throw(AuroraException);


    ///  Include optional Borealis components.
    ///
    void component_element(const DOMElement  *component)
                     throw(AuroraBadEntityException);


    ///  Reflect code to regional components covering this node.
    ///
    void   reflect_region_update(string   update_xml);


    ///  Reflect an update element to a region and to the head.
    ///
    void   reflect_global_update(string   update_xml);


    ///  Reflect an update to single regional compnent or the head.
    ///
    void   reflect_update_xml(string         update_xml,
                              UpdateClient  *Client);



    ////////////////////////////////////////////////////////////////////////////////
    ///
    /// Catalog object interface.
    ///

 public:
    ////////////////////////////////////////////////////////////////////////////
    // Query network modifiers.

    AsyncRPC<void> add_boxes(vector<CatalogBox> boxes,
                             BoxPackage packed_boxes,
                             set<Name> potential_subscriptions,
                             InetAddress prev_node);

    AsyncRPC<void> move_boxes(vector<Name> box_names, MedusaID dest); // DEPRECATED


    ////////////////////////////////////////////////////////////////////////////////
    // Connection modifiers
    AsyncRPC<void> subscribe_streams(vector<CatalogSubscription> subs);

    AsyncRPC<void> unsubscribe_stream(CatalogSubscription sub);

    AsyncRPC<void> add_subscriptions(vector<CatalogSubscription> subs_to_add);

    AsyncRPC<void> remove_subscriptions(vector<CatalogSubscription> subs_to_delete);


 private:
    struct ConnectBoxData
    {
        CatalogBox box;
        Name stream_name;
        Name dest_box_name;
        uint16 dest_box_port;
        InetAddress dest_box_node_ctrl;
        InetAddress dest_box_node_data;

        ConnectBoxData( CatalogBox _box,
                        Name _stream_name,
                        Name _dest_box_name,
                        uint16 _dest_box_port,
                        InetAddress _dest_box_node_ctrl = InetAddress(),
                        InetAddress _dest_box_node_data = InetAddress() )
            : box(_box),
              stream_name(_stream_name),
              dest_box_name(_dest_box_name),
              dest_box_port(_dest_box_port),
              dest_box_node_ctrl(_dest_box_node_ctrl),
              dest_box_node_data(_dest_box_node_data)
        {}
    };

    struct MoveBoxData
    {
        vector<CatalogBox*> catalog_boxes;
        MedusaID dest;
        set<Name> feed_outputs;
        set<Name> redirect_inputs;
        set<Name> redirect_outputs;

        MoveBoxData(vector<CatalogBox*> _catalog_boxes,
                    MedusaID _dest,
                    set<Name> _feed_outputs,
                    set<Name> _redirect_inputs,
                    set<Name> _redirect_outputs)
                    : catalog_boxes(_catalog_boxes),
                      dest(_dest),
                      feed_outputs(_feed_outputs),
                      redirect_inputs(_redirect_inputs),
                      redirect_outputs(_redirect_outputs)
        {}
    };

    ////////////////////////////////////////////////////////////////////////////
    // Dynamic query network modifications.


    void local_connect_box(AsyncRPC<void>          completion,
                           vector<ConnectBoxData>  connect_boxes);

    void local_disconnect_box(AsyncRPC<void>  &completion,
                              vector<Name>     box_names);

    void local_add_boxes(AsyncRPC<void>       &completion,
                         vector<CatalogBox>    boxes,
                         BoxPackage            packed_boxes,
                         set<Name>             potential_subscriptions,
                         InetAddress           prev_node);

    void local_remove_boxes(AsyncRPC<void>    &completion,
                            vector<Name>       box_names);

    void local_move_boxes(AsyncRPC<void> &completion, vector<Name> box_names, MedusaID dest);
    void local_move_boxes_orphan_subs(AsyncRPC<void> completion, MoveBoxData data, RPC<void> result);
    void local_move_boxes_redirect_streams(AsyncRPC<void> completion, MoveBoxData data, RPC<void> result);
    void local_move_boxes_engine_and_catalog(AsyncRPC<void> completion, MoveBoxData data, RPC<void> result);



    ////////////////////////////////////////////////////////////////////////////
    // Dynamic connection modifications.
    //
    void    local_add_subscriptions(//AsyncRPC<void>   completion,
                                    vector<CatalogSubscription>& subscriptions,
                                    Callback<void, RPC<void> > cb)
                              throw(AuroraException);

    void    local_remove_subscriptions(AsyncRPC<void> completion,
                                       vector<CatalogSubscription*>& subs);

    void    local_subscribe_streams(AsyncRPC<void>               &completion,
                                    vector<CatalogSubscription>  &subs);

    Status  local_subscribe_stream(
                                   // The CatalogSubscription object contains more information than just the
                                   // name and the address of the endpoint
                                   CatalogSubscription sub, 
                                   //Name         name, --> contained in the subscription
                                   //InetAddress  endpoint, --> contained in the subscription
                                   Boolean     *forward 
                                   );

    void    local_unsubscribe_stream(AsyncRPC<void> completion,
                                     CatalogSubscription& subs);

    void    remote_subscribe_check(InetAddress  source,
                                   RPC<void>    result);

    void    remote_unsubscribe_check(CatalogSubscription  sub,
                                               RPC<void>  result);


    // Port modification helpers.
    //vector<CatalogSubscription*> install_boxes(vector<CatalogBox*> boxes) throw(AuroraException);
    //vector<CatalogSubscription*> uninstall_boxes(vector<CatalogBox*> boxes) throw(AuroraException);



    ////////////////////////////////////////////////////////////////////////////////
    /// Magda --- new stuff to manage the consistency of input and local streams
    //..........................................................................

 public:

    ////////////////////////////////////////////////////////////////////////////////

    /// Return a copy of the replica_set to which this node belongs to
    RPC< vector<string> > get_replica_set();


    /// A replica can request permission to become temporarily unavailable
    /// in order to reconcile its state. Replicas use these messages to stagger
    /// their reconciliation and ensure they do not become unavailable at the same time
    RPC<bool> authorize_reconciliation(MedusaID replica);


 private:

    /// Check that requests made to the AvailabilityMonitor succeeded
    void avail_monitor_response(string command, RPC<void> result);

    /// Tell our replicas to open output paths because we got some subscriptions
    void forward_subs_to_replicas(vector<CatalogSubscription> subs);

    /// Result from forwarding a set of subscriptions to a replica
    void forward_subs_check(InetAddress replica, RPC<void> result);

    /// Request the replicas of a new upstream neighbor identifier with an endpoing
    void request_replicas(InetAddress node, Name stream_name);

    /// Replica set return by new upstream neighbor (includes the neighbor itself)
    void got_upstream_replica_set(Name stream_name, RPC< vector<string> > replica_set);


    /// Allow the ConsistencyMngr to update the state of local streams 
    /// when the consistency state of the processing engine changes.
    /// In this first version, we assign the overall node state to all streams
    void update_local_stream_states(ConsistencyState new_state);

    /// Callback from AvailabilityMonitor to notify the QueryProcessor
    /// that the state of output streams changed at one or more replicas of 
    /// one or more upstream neighbors. 
    void update_input_stream_states(RPC< RStreamStatesIndex > changed_nodes);

    typedef map<Name, pair<InetAddress,ConsistencyState> > AffectedSubscriptions;
    void modify_subscriptions(AffectedSubscriptions& affected_subscriptions);
    void switch_subscriptions(Name stream_name, 
                              InetAddress old_sub_endpoint, 
                              InetAddress new_sub_endpoint, 
                              InetAddress old_corr_endpoint,
                              InetAddress new_corr_endpoint);
    void remote_unsubscribe(InetAddress old_sub_endpoint, CatalogSubscription old_sub);
    void remote_subscribe(InetAddress new_sub_endpoint, CatalogSubscription add_sub);
    //..........................................................................



 public:

    RPC<void> notify_takeover();
    
 private:

    /**
     * Trims output queue for stream id up to tuple last_tuple
     */
    void trim(StreamID id, string last_tuple) throw(AuroraBadEntityException);


 public:
    typedef map<Name, ptr<PendingQuery> > PendingQueries;
    typedef map<Name, ptr<StreamDef> > StreamMap;
    typedef map<Name, vector< ptr<StreamDef> > > StreamVersionsMap;
    typedef map<Name, ptr<Query> > LocalQueries;

    // For each stream name, we hold a map of subscription names to subscriptions themselves
    typedef map<Name, ptr<Subscription> > SubscriptionMap;
    typedef map<Name, SubscriptionMap > Subscriptions;

    typedef map<Name, ptr<CPViewDescription> > CPViewDescriptionMap;

 private:

    /// The slice of the catalog of objects deployed to this node.
    /// 由 Diagram 改成 CongerDiagram  by hellojinjie
    ///
    CongerDiagram  _local_catalog;


    /// We may want to move the_region_client and _head_client into Diagram:
    ///    Local catalog:      Set of regions to reflect.
    ///    Regional catalog:   Parent regions to reflect (defer).
    ///    Global catalog:     Head reflects to global components.
    ///
    /// Control port address for the region.
    ///
    vector<UpdateClient *>   _region_client;


    /// Control port address for the head.
    ///
    UpdateClient            *_head_client;


    // Queries that are (being) setup. Once, setup, we remove queries from this data structure
    PendingQueries pending_queries;

    // Once set-up, we only remember the hunks that are running locally
    LocalQueries _local_queries;

    // Cache of locally owned streams
    StreamMap _local_streams;

    // locally owned cps
    CPViewDescriptionMap _local_cpviews;

    // As a first try we will keep info about stream versions in a separate structure
    StreamVersionsMap local_stream_versions;

    // Cache of responsabilities
    Subscriptions _subscriptions;

    // Defer subscriptions when the stream is not yet defined.
    // Add them when a box uses the stream as output.
    //
    typedef  map<Name, vector<InetAddress> >  DeferSubscribe;
    DeferSubscribe   _defer_subscribe;



    ////////////////////////////////////////////////////////////////////////////
    //  End:  Admin declarations merged into QP.
    //..........................................................................



  public:

    ////////////////////////////////////////////////////////////////////////////
    //  Begin:  QP declarations that existed prior to merging in Admin.
    //..........................................................................


    /**
     * @return Used by BasicComponent's to access the local catalog.
     * This is a temporary hack and will eventually be replaced.
     * Only Nesime is allowed to use this.
     */
    RPC< uint32 > get_local_catalog()
    {
        return(uint32(&_local_catalog));
    }; 

    // -- tatbul
    RPC<void> start_measure_period(uint32 period)
    {
        _aurora._monitor.measure_throughput(period);
        //_aurora.measure_throughput(period);
        return true;
    };

    // -- tatbul
    RPC<void> start_measure()
    {
        _aurora._monitor.measure_throughput();
        //_aurora.measure_throughput();
        return true;
    };

    /**
     * @return list of performance/load information
     * Each Stats element gives the load information for one query
     */
    RPC<vector<Stats> > get_stats();

    /**
     * Return the current statistics structure from the StatManager.
     * This returns the TSStats (time series).
     * Added by Olga @ 04/22/2005
     */

    RPC<Statistics> get_statistics() ;

    /** Returns the statistics with a given name
     * Added by Olga@ 04/26/2005
     */
    RPC<TSStats> get_statistics_name(StatisticsName name);

    RPC<map<string,Statistics> > get_region_statistics();

    RPC<map<string, map<string, double> > > get_tuple_delivery_statistics();

    RPC<map<string,double> > get_region_cpu_statistics();

    RPC<map < string, map<string,double> > > get_latency_statistics();

   /**
    * Returns the Cpu stats from AuroraNode
    * olga 27/04/05
    */
    RPC<double> get_cpu_statistics();

    RPC<double> get_sel();

    /**
     * Processes ACK0 from downstream node that it has received a tuple
     */
    RPC<void> ack(MedusaID node, StreamID id, string last_tuple);

    /**
     * Trims output queues.
     */
    RPC<void> trim(QueueTrimmingMessage message);
    
    /**
     * Sends the checkpoint message from primary to backup.
     */
    void send(ptr<CheckpointMessage> cm);
    
    /**
     * Enqueues the checkpoint message in the queue of pending checkpoint messages (from primary to backup).
     */
    RPC<void> enqueue(ptr<CheckpointMessage> cm);
    
    /**
     * Is invoked when the checkpoint message has safely arrived at the backup (from backup to primary)
     */
    void checkpoint_message_enqueued(MedusaID backup, RPC<void> result);
    
    /**
     * Acknowledges the completion of checkpoint (from backup to primary).
     */
    void ack(ptr<CheckpointMessage> cm);
    
    /**
     * Acknowledges the completion of checkpoint (from backup to primary).
     */
    RPC<void> end_checkpoint(ptr<CheckpointMessage> cm);

    /**
     * Is invoked when the checkpoint is completed on primary (from primary to backup).
     */
    void checkpoint_completed(MedusaID primary, RPC<void> result);

    /**
     * Pushes the processing load info to the backup server specified (from primary to backup).
     */
    void push_processing_times(MedusaID backup, vector<string> ha_units, vector<ptr<SlidingWindowStatistics> > processing_times);

    /**
     * Pushes the processing load info to the backup server specified (from primary to backup).
     */
    RPC<void> update_processing_times(MedusaID primary, vector<string> ha_units, vector<ptr<SlidingWindowStatistics> > processing_times);

    /**
     * Is invoked when the push is completed (from backup to primary).
     */
    void push_completed(MedusaID backup, RPC<void> result);

    /**
     * Schedules to wake up the scheduler after the specified time (used for high availability).
     */
    void schedule_wake_up(Time next_checkpoint_time);

    /**
     * Wakes up the sleeping scheduler (used for high availability).
     */
    void wake_up_scheduler();
    
    /**
     * Sets up the Scheduler Event Handler (used for high availability).
     */
    void set_scheduler_event_handler();
    
    /**
     * Handles the event of creating a box (used for high availability).
     */
    void handle_box_creation(ptr<QBox> box, bool primary);
    
    /**
     * Returns the state of QBox specified (used for high availability).
     */
    ptr<AbstractBoxState> get_box_state(string box_name);
    
    /**
     * Updates the state of a QBox (used for high availability).
     */
    void update_box_state(string box_name, ptr<AbstractBoxState> box_state);
        
    /**
     * Returns the xml strings to instantiate the QBox specified (used for high availability).
     */
    vector<string> get_xml_strings(string box_name);

    NMSTL_RPC_OBJECT(QueryProcessor);

    //olga @25/04/2005
    void output_stats()
    {
        //      cout << m_aurora.sm;
    }


 private: // methods

    void init();
    void in_thread_init();

    // Wait Aurora finish its processing before shutting down the qp.
    // -- tatbul
    //
    void finish_up();

    // Periodically print stats.
    // -- tatbul
    //
    void print_stats();


    void inform_partner(string partner);
    void inform_partner_response(string partner, RPC<void> result);
    void inform_partner_primary(string partner);

    void send_downstream(vector<StreamEvent>& tuples_to_send);
    void ack_upstream(vector<StreamEvent>& tuples_to_ack);

    /**
     * Acknowledges receiving input tuples to upstream neighbors
     */
    void send_acks();

    void send_acks(QueueTrimmingMessages& acks);

    void send_ack(MedusaID node, QueueTrimmingMessage& message);


    void send_queue_trimming_message(MedusaID node, QueueTrimmingMessage& m);

    /**
     * Composes the ack message and returns it.
     */
    QueueTrimmingMessages get_ack_messages();

    /**
     * Prints an error message if an acknowlegement or queue trimming message failed
     */
    void handle_ack_response(MedusaID node, RPC<void> result);

    void handle_trim_response(MedusaID node, RPC<void> result);
    
        
 private: // fields

    /// The Aurora query processor
    AuroraNode _aurora;
    bool _aurora_started;

    /// Address on which listen for incoming stream connections
    InetAddress _data_add;

    /// Handles all data input and output
    DataPath _data_path;

    /// Injected streams (streams that clients are directly enqueueing on)
    vector<Name> _injected_streams;

    /// All local queries
    typedef map<Name, ptr<QueryState> > QueryStates;
    QueryStates _queries;

    /// All local streams
    typedef map<Name, ptr<StreamState> > StreamStates;
    StreamStates _streams;

    /**
     * The secondaries that are supposed to receive the trimming messages
     * or the checkpoints
     */
    MedusaID _secondary;
    
/////////////////////////////////////////////    

    /**
     * Don't send an ack until previous ack returns. Trims are sent when
     * necessary independently of other pending trim messages
     */
    map<MedusaID,bool> _pending_ack;
    map<MedusaID,bool> _pending_trim;

    typedef vector< pair< ptr<StreamState> ,string> > TrimPairs;

    friend class ConsistencyMngr;

    /// Monitors consistency and performs reconciliation when needed
    ConsistencyMngr _constman;


    ////////////////////////////////////////////////////////////////////////////
    //  End:  QP declarations that existed prior to merging in Admin.
    //..........................................................................

    bool              _ongoing_dynamic_modification; // -- tatbul

};


BOREALIS_NAMESPACE_END

#endif                   // QUERYPROCESSOR_H
