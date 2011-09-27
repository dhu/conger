#ifndef DATAPATH_H
#define DATAPATH_H

#include <NMSTL/callback>
#include <NMSTL/thread>
#include <NMSTL/tqueue>
#include <NMSTL/netioevent>
#include <NMSTL/serial>

#include "runtime/AuroraNode.h"
#include "Exceptions.h"

#include "TupleHeader.h"
#include "DataHandler.h"
#include "HAMethod.h"
#include "IOQueue.h"
#include "StreamState.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

BOREALIS_NAMESPACE_BEGIN


/**
 * Enuration for different types of input paths
 */
class InputType
{

 public:

    enum Type
    {
        REGULAR = 0,
        CORRECTIONS,
        CORRECTED
    };

    InputType(Type t = REGULAR) :
        _type(t)
    {}

    string as_string() const
    {
        switch(_type)
        {
          case  REGULAR:
            return  string("regular");
            break;

          case  CORRECTIONS:
            return  string("corrections");
            break;

          case  CORRECTED:
            return  string("corrected");
            break;

          default:
            return  string("unknown");
        }
    }


    bool operator==(const InputType& other_type) const
    {   return (other_type._type == _type);
    }


    bool operator!=(const InputType& other_type) const
    {   return (other_type._type != _type);
    }


 private:
    Type    _type;
};


/**
 * This class handles all data inputs and outputs.
 * It listens for incoming streams of tuples and pushes
 * incoming tuples to the processing engine
 * It gets tuples from the engine and forwards them to
 * all subscribed clients
 */
class DataPath
{
  public:
    DataPath(MedusaID id, AuroraNode& node, InetAddress data_add);
    ~DataPath();
    void init();


    /// So we don't set-up queries and enqueue at the same time
    ///
    Mutex _aurora_lock;


    /// Stop all enqueing activities
    ///
    void stop_enqueue();


    /// Suspends all dequeueing activity
    ///
    void suspend_dequeue();


    /// Allow all dequeueing activity again
    ///
    void resume_dequeue();


    /// Clears all the tuples in the send queues.
    ///
    void clear_send_queues();


    /// Prepare to receive input from a new upstream neighbor
    /// The identification of the neighbor is option. When no neighbor
    /// is indicated, then data is accepted from any point
    ///
    void add_input_path(Name stream_name, InetAddress source = InetAddress());


    /// Update the history information in this subscription to reflect
    /// the most recent data received on the corresponding input
    /// The subscription is not modified if we do not find the corresponding
    /// input stream
    /// @param sub_to_update is the subscription whose history will be updated
    ///
    void update_input_history(CatalogSubscription& sub_to_update);


    /// Add a new allowed source for an input stream
    ///
    void add_input_path_source(Name stream_name,
                               InetAddress new_endpoint,
                               InputType type = InputType());


    /// This is needed until we merge the info stored in DataPath with the Catalog
    /// @param stream_name: the requested input path
    /// @param type: optionally, a filter on the types of inputs sought
    /// @return One source for the given input path, if it exists. By default
    /// examines only REGULAR input paths but type param can change the type of input sought.
    ///
    InetAddress get_input_path_source(Name stream_name, InputType type = InputType());


    /// No longer accept inputs on an input stream
    ///
    void remove_input_path(Name stream_name);


    /// No longer accept inputs on an input stream
    ///
    void remove_input_path_source(Name stream_name, InetAddress old_endpoint);


    /// Check if we already have an output path for a stream
    ///
    bool has_output_path(Name stream_name);


    /// Start getting tuples from Aurora on one stream
    ///
    void add_output_path(Name stream_name);


    /// Add one downstream node for an output stream
    ///
    void add_output_path_client(CatalogSubscription sub);


    /// A stream is no longer an output stream
    ///
    void remove_output_path(Name stream_name);


    /// Remove one downstream neighbor
    ///
    void remove_output_path_client(CatalogSubscription sub);


    /// Remove all clients
    ///
    void remove_all_clients(Name stream_name);


    ///////////////////////////////////////////////////////////////////////////
    /// For Magda's HA

    /// Enqueue a stream event originating at this node
    /// This method is used by the ConsistencyMngr to send info to SUnions
    ///
    void enqueue(ptr<StreamEvent> event)
    {   _enq_thread.do_handle_input_event(event);
    }

    /// Lets a local callback get output tuples. We use this for
    /// the ConsistencyMngr to get outputs from SUnions
    ///
    void add_local_output_path_client(Name stream_name, DataHandler::dhcallback cb);

  private:

    /// Check if the output of this stream is stable
    ///
    bool stable_output(Name stream_name);

    /// Check if the otuput of this stream is stabilizing (tentative tuples are being corrected)
    ///
    bool stabilizing_output(Name stream_name);

    /// Carefully control incoming events. We need this to handle background corrections properly
    /// @return true when event can be processed. An error message otherwise.
    ///
    Status fault_tolerance_checks(ptr<StreamEvent> evt);

    /// If query network has output *control* streams, callbacks handle their output
    /// We use this for SUnions to send info to the ConsistencyMngr
    ///
    map< Name, DataHandler::dhcallback > _local_clients;
    ///
    //////////////////////  end Magda's HA  ////////////////////////////////////


  private:

    /// Clean the failed dequeue handlers with care. As part of the FastDequeueForwarder event loop
    ///
    void clean_failed_handlers();

    static const int CLEANUP_DELAY = 20; // Schedule removal ASAP

    /// Buffer tuples on an output strea
    ///
    void inspect_and_buffer_output(Name stream_name, char* buf, size_t total_read, size_t tuple_size);

    /// Is the thread that enqueues into Aurora running?
    ///
    bool            _enqueuing;

    /// Stream processing engine
    ///
    AuroraNode     &_aurora;

    /// Handle through which we dequeue tuples from Aurora
    ///
    DequeueHandle   _deq_handle;


    ///////////////////////////////////////////////////////////////////////////
    ///
    /// DataPath catalog. All information that the DataPath uses is stored here
    ///
    MedusaID _id; // The unique identifier (i.e., InetAddress) of this node

    /// Input queues and output queues for input and output streams
    ///
    typedef map< Name, IOQueue* > IOPaths;

    IOPaths    _input_paths;
    Mutex      _input_paths_lock;
    IOPaths    _output_paths;
    Mutex      _output_paths_lock;


    /// For each output stream, a client is fully described by its
    /// subscription and its data handler. We use the DataHandler to
    /// send the data to the client.
    /// The history information helps get the client up-to-date
    /// starting from the correct point in the stream
    ///
    class ClientInfo
    {
      public:

        ClientInfo() {}

        ClientInfo(CatalogSubscription sub, ptr<DataHandler> dh, bool up_to_date = true)
            : _sub(sub), _data_handler(dh), _is_up_to_date(up_to_date), _independent_failure(false)
        {
            if (_sub.tentative_tuple_to_correct())
            {   _independent_failure = true;
            }
        }

        CatalogSubscription _sub;
        ptr<DataHandler>    _data_handler;        // TCP connection to client
        bool                _is_up_to_date;       // Did client get all history and corrections?
        bool                _independent_failure; // Has the client seen a failure that we have not (default is yes)?
    };

    typedef map< InetAddress, ClientInfo > OneClientMap;
    typedef map< Name, OneClientMap >  ClientsPerStream;
    ClientsPerStream _output_clients;
    vector< ptr<DataHandler> > _failed; // Failed data handlers must be de-allocated with care
    Mutex _output_clients_lock;


    /// For each input stream, we need to remember one or more remote sources
    /// Each remote source can serve a different purpose (regular or background corrections)
    ///
    typedef map< InetAddress, InputType > InputSources;
    typedef map< Name, InputSources > InputSourcesPerStream;

    InputSourcesPerStream  _input_sources;
    Mutex                  _input_sources_lock;
    ///
    ///////////////////////////////////////////////////////////////////////////



    // --------------------------------------------------
    /**
     * Accepts new incoming data connections (i.e., new input streams)
     * The EnqueueThread receives input events on each input stream
     * and pushes these events to Aurora.
     * The EnqueueThread returns errors if someone pushes events on
     * an unknown stream
     */
    // org??? friend class EnqueueThread : public Thread
    class EnqueueThread : public Thread
    {
        DataPath    &_data_path;
        InetAddress  _data_add;    // Adress to listen for new data connections.
        IOEventLoop  _data_loop;   // Event loop for this thread only.

    public:
        EnqueueThread(DataPath     &data_path,
                      InetAddress   data_add)
            : _data_path(data_path),
              _data_add(data_add)
        {}


        virtual ~EnqueueThread() { stop(); }


        /// Pushes one event into Aurora
        Status  do_handle_input_event(ptr<StreamEvent> event);

        void stop() { _data_loop.terminate(); }

        void run();
    };

    EnqueueThread  _enq_thread;



    // --------------------------------------------------
    /// Grabs events from Aurora and hands them over to the FastDequeueForwarder
    // org??? friend class DequeueThread : public Thread
    class DequeueThread : public Thread
    {
        DataPath   &_data_path;
        bool        _done;

      public:
        DequeueThread(DataPath& data_path)
            : _data_path(data_path)
        {}


        virtual ~DequeueThread()
        {  stop();
        }


        void  stop()
        {   _done = true;
        }


        void  run();
    };

    DequeueThread _deq_thread;

    // Should we suspend forwarding all events
    bool _fwd_suspend;


    /// List of accumulated events that we should send downstream
    list< ptr<StreamEvent> > _events_to_send;

    /// Prevents dequeue and forward threads from modifying the list at the same time
    Mutex _events_to_send_lock;

    // Allows dequeue thread to signal forward thread in a non-blocking manner
    IOHandle _deq_pipe_write, _deq_pipe_read;



    // --------------------------------------------------
    /// Forwards tuples from Aurora to destinations
    //friend class FastDequeueForwarder : public Thread  // org does not compile??? 
    class FastDequeueForwarder : public Thread         // huh???
    {
        DataPath    &_data_path;

    public:

        IOEventLoop  _data_fwd_loop;

        /**
         * Handler to attach to event loop. This handler will forward
         * events downstream when any are available
         */
        // friend class ForwardHandler : public IOHandler  // org does not compile??? 
        class ForwardHandler : public IOHandler   // huh???
        {
          public:
            ForwardHandler(IOEventLoop  &loop,
                           DataPath::FastDequeueForwarder  &fwd_thread);

            void ravail();

          private:
            DataPath                        &_data_path;
            DataPath::FastDequeueForwarder  &_fast_dequeue_fwd;
        };


        FastDequeueForwarder(DataPath& data_path)
            : _data_path(data_path)
        {}


        ~FastDequeueForwarder()
        {
            stop();
        }


        ptr<ForwardHandler> _fwd_handler;


        /// Send one event on one stream (multiple destinations)
        ///
        Status send_data(Name stream_name, ptr<StreamEvent> event);


        /// Send the appropriate history/corrections to a new client
        ///
        Status update_client(ClientInfo& client_info, Name stream_name);


        void run();


        void stop();
    };


    FastDequeueForwarder  _fast_dequeue_fwd;



    ///////////////////////////////////////////////////////////////////////////
    // OLD HA STUFF BELOW. CLEANED-UP FOR NOW

  public:
    /**
     * Set the recovery method
     */
    void set_recovery_method(HAMethod method) {}


    /**
     * @return the recovery method
     */
    HAMethod get_recovery_method()
    {
        // RETURN RANDOM STUFF, JUST SO THINGS COMPILE
        return HAMethod();
    }


    /**
     * Change the status to primary (if true) or to secondary (if false)
     */
    void set_primary_status(bool status) {}


    /**
     * @return primary status of this query processor
     */
    bool is_primary()
    {
        // RETURN RANDOM STUFF JUST SO THINGS COMPILE
        return false;
    }


    /**
     * @return pairs mapping input streams to the last tuple that
     * was enqueued on that stream
     */
    vector< pair< ptr<StreamState> , string> >  last_input_tuples()
    {
        // RETURN RANDOM STUFF JUST SO THINGS COMPILE
        vector< pair< ptr<StreamState>, string> >  random_vector;
        return  random_vector;
    }


    /**
     * @return pairs mapping output streams to the last tuple that
     * was enqueued on that stream
     */
    vector< pair< ptr<StreamState> , string> > last_output_tuples()
    {
        // RETURN RANDOM STUFF JUST SO THINGS COMPILE
        vector< pair< ptr<StreamState>, string> > random_vector;
        return random_vector;
    }


    /**
     * @return For each output stream, return the StreamEvent that represents
     * all the tuples that need to be checkpointed
     */
    vector<StreamEvent> get_tuples_to_checkpoint()
    {
        // RETURN RANDOM STUFF JUST SO THINGS COMPILE
        vector<StreamEvent> random_vector;
        return random_vector;
    }


    /**
     * Updates output streams with given stream events
     */
    void update(StreamEvent& stream_event)
    {
        // JUST SO THINGS COMPILE
    }

    /**
     * Trims output streams up to given event
     */
    void update(StreamID stream_id, string last_tuple)
    {
        // JUST SO THINGS COMPILE
    }

    /**
     * Send downstream a specific set of stream events.
     * These events are typically those that we have checkpointed with our secondary
     */
    Status send_downstream(vector<StreamEvent>& events_to_send)
    {
        // RETURN RANDOM STUFF JUST SO THINGS COMPILE
        return true;
    }
};

NMSTL_TO_STRING(DataPath::InputType)

BOREALIS_NAMESPACE_END

#endif                   // BOREALIS_DATAPATH_H
