#ifndef AURORANODE_H
#define AURORANODE_H

#include <fstream>

#include "Exceptions.h"

#include "Diagram.h"
#include "CpuStats.h"
#include "QBox.h"
#include "Stream.h"
#include "TupleDescription.h"
#include "Scheduler.h"
#include "StreamProperties.h"
#include "LockSet.h"

#include "StreamDef.h"
#include "StatsMgr.h"
#include "VersionRWLock.h"

#include "BoxPackage.h"
#include "NodeTable.h"

#include "PseudoScheduler.h"
#include "AuroraNodeMonitor.h"


BOREALIS_NAMESPACE_BEGIN

class SchedulerTask;

class CPQBox;
class CPView;
class CPViewDescription;


/// An Aurora server node.  An AuroraNode contains a catalog and a
/// scheduler, and a manager for dequeuers (see getDequeueHandle()).
///
/// Thread safety requirements:
///
/// <ol>
///
/// <li>Construction, modifyNetwork(), start(), and destruction are
/// not thread-safe.  No other operation except dequeue() may be pending
/// during these operations.
///


// --------------yna -- these accessors aren't necessary anymore!
/// <li>shutdown(), typecheck(), validate(), and all accessors (e.g.,
/// describe(), listEntities(), getBoxes(), etc.) may be invoked at
/// any time, except as described in #1.
/// -----------------------------end yna

///
/// <li>getDequeueHandle(), and all methods of DequeueHandle, are
/// thread-safe, except as described in #1.  For a single
/// DequeueHandle, only one thread may invoke its dequeue()
/// method at any given time.  All DequeueHandles must be destroyed
/// before shutdown() is called.
///
/// <li>For any given stream name, only one thread may invoke
/// enqueue() at any given time.  (Two threads may simultaneously
/// invoke enqueue with <i>different</i> stream names, however.)
///
/// </ol>
class AuroraNode
{
  public:
    /// An enumeration of the types of entities stored in a catalog.
    enum entity_type
    {
        /// A stream.
        STREAM = 1,

        /// A schema.
        SCHEMA = 2,

        /// A box.
        BOX = 3,

        /// Pseudo-entity-type for input streams only (in listEntities()).
        INPUT_STREAMS = 100,

        /// Pseudo-entity-type for output streams only, i.e., streams
        /// with no boxes downstream (in listEntities()).
        OUTPUT_STREAMS = 101
    };


    typedef ::StreamProperties StreamProperties;


    /// Creates a node.
    ///
    AuroraNode(Diagram  &local_catalog,
               string    id = "")
         throw(AuroraException);


    /// Destroys the node.
    ///
    ~AuroraNode();


    ////////////////////////////////////////////////////////////////////////////    
    ///
    /// Engine execution control.
    /// ........................................................................


    /// Starts the network.
    ///
    void  start() throw( AuroraException );


    /// Shuts down the network.
    ///
    void  shutdown() throw( AuroraException );


    /// Drains the network: waits until there are no more tuples at
    /// all in the network and then shuts the network.  Blocks until
    /// this is complete.  Throws an exception if the scheduler
    /// does not support draining.
    ///
    void  drain() throw( AuroraException );


    void choke_inputs(vector<string> input_names); // -- tatbul


    void resume_inputs(vector<string> input_names); // -- tatbul


    void drain_boxes(vector<string> input_names);


    /// Chokes off inputs to the given boxes.
    ///
    void  choke_boxes( vector<string>  names );


    /// Chokes off inputs to the given boxes.
    ///
    void  suspend_boxes( vector<string>  names );


    /// Resumes any inputs previously choked.
    ///
    void  resume_boxes( vector<string>  names );


    /// Resumes choked streams only, not boxes themselves.
    ///
    void  resume_box_streams( vector<string>  names );



    ////////////////////////////////////////////////////////////////////////////
    ///
    /// Tuple I/O.
    ///
    /// Enqueue tuples to a stream.  The stream must not be tied to
    /// the output of any box in the network.
    ///
    /// @param stream_name the name of the stream to which to enqueue
    /// @param data a buffer containing the data to enqueue.  Each tuple
    ///     must be preceded by twelve bytes of padding (the padding bytes
    ///     are accounted for in TupleDescription).
    ///     Array elements contain the array data size and is changed
    ///     to point to the allocated array data.
    /// @param array a buffer containing array data to allocate.
    ///     The order corresponds to the references in the tuple data.
    /// @param tuple_length the length of each tuple in the buffer.
    ///     Must match the TupleDescription of the stream currently in catalog
    /// @param num_tuples the number of tuples to enqueue
    /// @param schema_uuid if provided, the UUID of the stream's schema, 
    ///     which is checked for consistency.  Not checked if schema_uuid 
    ///     is null, or it points to a block of all zeroes
    ///
    void enqueue( const char  *stream_name,
                  const void  *data,
                  const void  *array,
                      size_t   tuple_length,
                      size_t   num_tuples
                  //const void  *schema_uuid = 0
                )
           throw( AuroraException );


    ///  Allocate any arrays in a tuple.
    ///
    const void  *array_allocator(
                    // Stream for the tuple.
                    Stream      *stream,

                    // An enqueued tuple including the header.
                    void        *tuple,

                    // Array data ordered as referenced in the tuple.
                    const void  *array);


  private:

    /// A single subscription to a stream.  Currently just a pointer to
    /// the applicable TupleQueue.
    struct Subscription
    {   ptr<TupleQueue> _q;

    };


  public:

    /// A dequeue set.  Each DequeueHandle points to a one of these;
    /// however the actual context objects themselves are owned by
    /// the AuroraNode.
    ///
    struct DequeueContext
    {   TuplesAvailableMailbox      _mailbox;
        map<Stream*, Subscription>  _subscriptions;
        list<ptr<TupleQueue> >      _deleted_queues;

        /////////////////////Statistics for output latency////////////////////
        //olga
        map<string, int>      _tuplenum;
        map<string, double>   _outlatency_sum, _outlatency_sumsqr;
        unsigned long         _last_outlatency_update;
        map <string, long>    _last_outlatency ; // mapping of streams 
                                                 // to last update


        /// Called by UnsubscribeAllTask
        ///
        void  unsubscribe_all();
    };


 public:

    /// A handle that may be used to dequeue tuples.  You can obtain a
    /// handle using AuroraNode::getDequeueHandle() Once a handle is
    /// closed, it is invalid and may not be used further for dequeuing.
    ///
    /// This class is threadsafe: you may call
    /// AuroraNode::getDequeueHandle(), and the subscribe(), dequeue(),
    /// and close() methods of this class, at any time when the network is
    /// running.  subscribe() and close() may be invoked even when
    /// dequeue() is blocking.
    ///
    class DequeueHandle
    {
      public:
        DequeueHandle() : _node(0), _context() {}


        /// Subscribe to a stream.  Future calls to dequeue() will
        /// return tuples from this stream.
        ///
        void  subscribe(string name) throw( AuroraException );


        /// Subscribe to a stream.  Future calls to dequeue() will
        /// not return tuples from this stream.
        ///
        void  unsubscribe( string  name ) throw( AuroraException );


        /// Dequeue tuples from any subscribed stream.  This method
        /// blocks until
        ///
        ///  - at least one tuple is available on any subscribed stream, or
        ///  - close() is called, or
        ///  - the node is shut down.
        ///
        /// Sets stream_name to the name of stream and td to the description
        /// of the tuples that have been dequeued.  Up to buffer_size bytes
        /// are written to the buffer (the actual number of bytes written
        /// can be calculated as <i>return-value</i> * td.getSize()).
        ///
        /// @return the number of tuples dequeued, or 0 if close() was called
        ///   or the node has been shut down
        ///
        size_t  dequeue( string  &stream_name,
               TupleDescription  &td,
                           void  *buffer,
                         size_t   buffer_size
                       )
                  throw( AuroraException );

        /// Closes the handle for dequeueing, causing any pending dequeue()
        /// operations to return 0.
        ///
        void close();
        
        /// placeholder for HA: Jeong-Hyon
        void handle(Time in_time);


        /// For use only by AuroraNodeMonitor to access the context.
        ///
        DequeueContext  *get_context()
        {   return(_context.get());
        }

      private:

        /// Handle to access methods in AuroraNode.
        ///
        AuroraNode *_node;

        ptr<DequeueContext> _context;

        friend class AuroraNode;
        friend struct unsubscribe_all_task;
    };


    friend class DequeueHandle;


    /// Allocate a new handle for dequeuing
    ///
    DequeueHandle  get_dequeue_handle();



    ////////////////////////////////////////////////////////////////////////////
    //
    // Dynamic query network modifications from new catalog elements. -- yna
    // .........................................................................
    //

    /// Structures for dynamic modifications.
    ///
    struct PortSwap
    {
        CatalogStream*  _new_stream;    // The new stream.
        CatalogBox*     _dest_box;      // Dest for the new stream.
        uint16          _dest_port;     // destination Box In for the new stream.
        
        PortSwap() {}

        PortSwap(CatalogStream* ns, CatalogBox* db, uint16 dp)
            : _new_stream(ns), _dest_box(db), _dest_port(dp)
        {}
    };

    typedef vector<PortSwap> PortSwaps;

    /// Stream and schema modifications.
    ///
    void add_schema(vector<CatalogSchema*> schemas) throw( AuroraException );


    void add_stream( vector<CatalogStream*> streams ) throw( AuroraException );


    void add_table( vector<CatalogTable*> tables ) throw( AuroraException );


    /// Update box params. -- tatbul
    ///
    void update_box(CatalogBox *box) throw( AuroraException );


    /// Reconnect an In stream on a box for the last phase of a remote connect.
    ///
    void update_box_in(/// Destination box.
                       CatalogBox *box,

                       /// New stream between the filter and destination.
                       CatalogStream  *new_stream,

                       /// Destination Box In port.
                       uint16          port)
                 throw(AuroraException);


    /// Connect an inserted box within the engine on a local node.
    ///
    void  connect_box(/// New stream between the filter and destination.
                      CatalogStream  *add_stream,

                      /// Box to receive the filtered Out stream.
                      CatalogBox     *dest_box,

                      /// Box being inserted.
                      CatalogBox     *add_box,

                      /// In stream to the added box.
                      CatalogStream  *old_stream,

                      /// Destination Box In port.
                      uint16          port)
                throw(AuroraException);

    
    void disconnect_box( vector<CatalogBox*> boxes ) throw( AuroraException );


    /// Modify an input port on a box.
    ///
    void modify_box_in(// Box to be modified.
                       CatalogBox     *box,

                       // Box in port to be modified.
                       uint16          port,

                       // New stream to connect to the box in port.
                       CatalogStream  *stream
                      )
                 throw(AuroraException);


    /// Append/truncate modifications.
    ///
    void add_box( vector<CatalogBox*> boxes, BoxPackage packed_boxes )
        throw( AuroraException );


    void remove_box( vector<CatalogBox*> boxes,
                     set<Name> streams_to_preserve = set<Name>() )
        throw( AuroraException );


    /// Creates a connection point
    ///
    void create_cpview( CPViewDescription  view_desc, StreamDef  streamdef )
                 throw( AuroraException );


    ////////////////////////////////////////////////////////////////////////////
    /// Serialization helpers.

    /// Populates the package with frozen box states.
    ///
    void  pack_boxes( vector<string>  names, BoxPackage  &packs );


    ////////////////////////////////////////////////////////////////////////////
    //
    // Engine contents.

    /// Returns a multi-line string representation of the node.
    ///
    string  repr() const;


    /// Returns an XML description of an entity, or an empty string if
    /// the method does not exist.
    ///
    string  describe( string  entity ) throw( AuroraException );


    /// Lists all entities of a particular type.  (One can then use
    /// "describe" to obtain a description of an entity.)  names is
    /// cleared first.
    ///
    void  list_entities( entity_type  type, vector<string>  &names )
                 throw( AuroraException );


    /// TODO: this is already implemented as schedule()
    /// why would we allow external components to schedule tasks!!!
    /// breaks engine encapsulation model -- yna
    ///
    void schedule_task(ptr<SchedulerTask> task)
    {
        _scheduler->schedule_exclusive_task(task);
    }

    
    void set_scheduler_event_handler(ptr<SchedulerEventHandler> event_handler)
    {
        _scheduler->set_event_handler(event_handler);
    }

    
    ptr<SchedulerEventHandler> get_scheduler_event_handler()
    {
        return _scheduler->get_event_handler();
    }


  private:

    ////////////////////////////////////////////////////////////////////////////
    /// Query Network implementation structures.
    ///
    typedef map<string, ptr<Stream> > StreamMap;
    typedef map<string, ptr<QBox> > BoxMap;
    typedef map<string, TupleDescription> SchemaMap;

    struct Topology
    {
        StreamMap      _streams;
        TableMap       _tables;
        BoxMap         _boxes;
        SchemaMap      _schemas;
        //CPointMap      _cpoints;

        bool contains_entity( string name ) const;

        /// Copy a node's Topology to another.
        void copy_from( const Topology  &other );

        string repr() const;
    };


    struct subscribe_task : public SchedulerTask
    {
        ptr<Stream> _stream;
        ptr<TupleQueue> _q;

        void run();
    };


    //friend struct unsubscribe_task;
    struct unsubscribe_task : public SchedulerTask
    {
        AuroraNode     *_node;
        string          _stream_name;
        TupleQueue     *_q;
        ptr<AuroraNode::DequeueContext> _context;

        void run();
    };

    // friend struct unsubscribe_all_task;
    struct unsubscribe_all_task : public SchedulerTask
    {
        ptr<AuroraNode::DequeueContext>  _context;

        void run();
    };


    //friend struct modify_network_task;
    struct modify_network_task : public SchedulerTask
    {
        modify_network_task( AuroraNode  &node, StatsMgr &sm ) :
            _node(node), _sm(sm), _lock(0), _cond(0) {}

        void run();

        AuroraNode   &_node;
        StatsMgr     &_sm;
        PtMutex      *_lock;
        PtCondition  *_cond;

        AuroraNode::Topology _new_topology;
        AuroraNode::Topology _deleted_topology;

        //stats
        IDVect  _instream_ids;
        IDVect  _anystream_ids; // -- tatbul
        IDVect  _outstream_ids;
        IDVect  _selectivity_ids;
        IDVect  _box_ids;
        IDVect  _stream_ids;

        // dynamic modifcation
        AuroraNode::PortSwaps  _port_swaps;
    };


    //friend struct generic_checkpoint_task;
    /**
     * Checkpoint and recovery tasks are exclusive, just like a ModifyNetworkTask
     */
    struct generic_checkpoint_task : public SchedulerTask
    {

        generic_checkpoint_task(AuroraNode &node)
            : _node(node), _lock(0), _cond(0) {}

        virtual void perform_task() = 0;

        void run();

        AuroraNode   &_node;
        PtMutex      *_lock;
        PtCondition  *_cond;
    };


    //friend struct checkpoint_task;
    /**
     * A checkpoint task makes a snapshot of the state of all queues and all boxes
     */
    struct checkpoint_task : public generic_checkpoint_task
    {

        checkpoint_task( AuroraNode  &node, vector<long long>  &measurement )
            : generic_checkpoint_task( node ), _measurements( measurement ) {}

        virtual void perform_task();

        vector<long long>  &_measurements;
    };


    //friend struct recovery_task;

    /**
     * An update task re-initializes the state of all queues
     * and boxes from the checkpointed state
     */
    struct recovery_task : public generic_checkpoint_task
    {
        recovery_task(AuroraNode &node)
            : generic_checkpoint_task(node) {}

        virtual void perform_task();
    };


  public:
    
    /**
     * Take a snapshot of the state of the complete query network
     * The measurements parameter is temporarily here so we can register the duration
     * of checkpointing
     */
    void checkpoint(vector<long long>& measurements);

    /**
     * Recover the query network from the last checkpoint.
     */
    void recover();

    /// Helper function to scheduler either a checkpoint or an update task
    void schedule_checkpoint_task(ptr<generic_checkpoint_task> task);



    ////////////////////////////////////////////////////////////////////////////
    ///
    /// Catalog interface
    ///
    ///.........................................................................
    /// fix up by renaming to getBox, i.e. removing "Local"
    ///
    CatalogBox  *get_local_box( Name  box_name ) const
    {  return( _local_catalog.get_box( box_name ));
    }

    CatalogStream  *get_local_stream( Name  stream_name ) const
    {  return( _local_catalog.get_stream( stream_name ));
    }

    CatalogSchema  *get_local_schema( Name schema_name ) const
    {  return ( _local_catalog.get_schema( schema_name ));
    }


    /// True when replica sets are in use.
    ///
    Boolean  is_replica_set()
    {   return  !_local_catalog.get_replica_set()->empty();
    }
    

    
    /// Diagram implementation interface.
    /// fix up by renaming calls *Impl, i.e. getBoxImpl, etc.

    /// Returns a schema with a particular name, or null if it doesn't exist.
    ///
    TupleDescription  get_tuple_description( string  name ) const
    {   return( lookup( _schemas, name ));
    }


    /// Returns a stream with a particular name, or null if it doesn't exist.
    ///
    ptr<Stream>  get_stream( string  name ) const
    {   return( lookup( _streams, name ));
    }


    /// For use only by AuroraNodeMonitor to access streams.
    ///
    StreamMap  &get_streams()
    {   return(_streams);
    }


    /// Returns a set of all the boxes presently in the network.
    ///
    set<ptr<QBox> > get_boxes() const;


    /// Returns a box with a particular name, or null if it doesn't exist.
    ///
    ptr<QBox> get_box( string  name ) const
    {   return( lookup( _boxes, name ));
    }


    /// Returns a table with a particular name, or null if it doesn't exist.
    ///
    ptr<Table> get_table( string  name ) const
    {   return( _node_table.get_table( name, _tables ));
    }


    /// Returns a lockset with a particular name, creating a new one
    /// if it doesn't exist.
    ///
    ptr<LockSet>  get_lock_set( string  name );


    /// local boxid for downstream revision processing
    ///
    int lookup_box_id( Name name )
    {
        map<Name, int>::iterator i = _box_id_by_name.find(name);

        if ( i == _box_id_by_name.end() )
        {
            return -1;
        }

        return i->second;
    }


    PagePool  &get_page_pool()
    {   return( *_page_pool );
    }

    
    /// Schedule a task if the network is running, else run it immediately
    ///
    void schedule( ptr<SchedulerTask>  task );


    ////////////////////////////////////////////////////////////////////////////
    /// Engine Statistics
    ///.........................................................................
    ///
    /// Statistics manager that keeps all the statistics per this node.
    /// this should be private, but locking/unlocking creates some issues
    /// need to find out what the appropriate approach would be (i.e. if
    /// it is safe to have multiple classes having access to the same object).
    ///
    StatsMgr sm;


    /// Return the statistics object
    ///
    Statistics  &get_stats_object()
    {
        //DEBUG << "get_stats_object running";
        return( sm.get_stats() );
    }

    const Statistics  &get_stats_object() const
    {   return( sm.get_stats() );
    }

    //added for stats RPC call
    //
    const double  get_cpu_statistics() const
    {   return( _cpu_stats.get_cpu_usage() );
    }

    const CpuStats  &get_cpu_stats() const
    {   return( _cpu_stats );
    }


    /// Hack used by:  external/udb/joinstatic.cc, optimizer/nhopt/qosoptimizer.cc
    ///
    double  selval;


  private:

    enum State
    {   STATE_INIT,
        STATE_RUNNING,
        STATE_STOPPED
    };
    
    string      _id;
    State       _state;

    /// Local catalog for this node.
    ///
    Diagram    &_local_catalog;

    /// Streams, boxes, schemas and tables for this node.
    ///
    Topology    _topology;

    /// Optional table map reference and environment for this node.
    ///
    NodeTable   _node_table;

    /// references to elements in Topology.
    ///
    StreamMap  &_streams;
    TableMap   &_tables;
    BoxMap     &_boxes;
    SchemaMap  &_schemas;


    ////////////////////////////////////////////////////////////////////////////
    /// Local helpers for dynamic engine modifications -- yna
    ///.........................................................................

    struct TopologyModifications
    {
        vector<CatalogSchema*> schemas;
        vector<CatalogStream*> streams;
        vector<CatalogBox*>    boxes;
        vector<CatalogTable*>  tables;
    };

    /// Main topology modification methods.
    ///
    ptr<modify_network_task> create_add_task(TopologyModifications& additions,
                                             BoxPackage& packed_boxes)
        throw(AuroraException);


    ptr<modify_network_task> create_remove_task(TopologyModifications& removals,
                                  set<Name> streams_to_preserve = set<Name>())
        throw(AuroraException);


    /// Installs boxes in the catalog.
    ///
    void  install_box_ports(BoxMap& boxes);


    /// Uninstalls boxes in the catalog.
    ///
    void  uninstall_box_ports(vector<CatalogBox*> boxes);


    /// Swaps box ports in the catalog, used for connect/disconnect.
    ///
    void  swap_box_ports(PortSwaps& swaps);



    ////////////////////////////////////////////////////////////////////////////
    ///
    /// Connection points.
    ///
    /// Note that order is important here; the streams/boxes may depend
    /// on the PagePool, so they must be destroyed *before* the
    /// PagePool.
    ///
    /// Note that these are not currently protected by a mutex (which
    /// is fine because the network cannot be modified after it has
    /// started).
    /// All (local?) connection points
    /// maps stream names to CP's on these streams
    ///.........................................................................


    typedef map<Name, ptr<CPQBox> > cps;
    cps      _cps;


    /// All (local?) CPViews
    ///
    vector<ptr<CPView> >   _cp_views;


    ptr<PagePool>          _page_pool;
    

    map<string, ptr<LockSet> >  _lock_sets;


    CpuStats  _cpu_stats;

    /// Exclusive lock on _deqs.  Necessary because different threads
    /// can manipulate dequeueing contexts
    ///
    PtMutex  _lock;


  public:
    /// Contexts for dequeueing
    ///
    typedef set< ptr<DequeueContext> >  DeqContexts;
    DeqContexts  _deqs;

  private:
    /// Scheduler for this node.
    ///
    ptr<Scheduler>  _scheduler;


    /// _scheduler pointes to this; remove _scheduler???
    PseudoScheduler     scheduler;


    /// Checkpointed state of queues and boxes
    ///
    BoxPackage _checkpointed_state;

    /// ID assignments to boxes
    ///
    map<Name, int> _box_id_by_name;
    

  public:
    AuroraNodeMonitor         _monitor;
};

typedef  AuroraNode::DequeueHandle  DequeueHandle;

BOREALIS_NAMESPACE_END

#endif                      // AURORANODE_H
