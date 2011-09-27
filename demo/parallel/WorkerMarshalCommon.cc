//#include  "WorkerMarshal.h"
#include  "WorkerMarshalCommon.h"
#include  "util.h"

#define WORKER_XML     "Pass.xml PassDeploy.xml"
#define MAX_BUFFER      640000000

//#define WORKER_ENDPOINT     "128.148.37.52:25100"
#define GATHER1   "gather"
#define SCATTER1  "scatter"


////////////////////////////////////////////////////////////////////////////////
//
// Generated marshaling code for the Worker program.  Modified:  WorkerMarshal.cc
//..............................................................................


queue< ptr<StreamEvent> >  WorkerMarshal::_active_event;

string                     WorkerMarshal::_gather;
string                     WorkerMarshal::_scatter;



/****************************  Not used for workers
////////////////////////////////////////////////////////////////////////////////
//
//  Subscribe to input and output streams.
//
int32  WorkerMarshal::open()
{
    int32   status;
//..............................................................................


    // Open a client to send and receive data.
    //
    _client = new MedusaClient(InetAddress());

    // Subscribe to outputs.
    //
    subscribeScatter1();

    //  Launch the front-end with the xml file.
    //  Creates a client that communicates with a borealis node at ip
    //  though the given port.
    //  Then it sends the network diagram as XML strings
    //  and runs start_query.
    //  It waits until the front-end terminates and the port is now free.
    //
    status = launchDiagram(WORKER_XML);

    if (status)
    {   ERROR << "launchDiagram failed ( " << status << " ).";
    }
    else
    {   // Establish input data path connections.
        connectGather1();
    }

    return  status;
}
*******************************/


////////////////////////////////////////////////////////////////////////////////
//
//  Connect to inputs, subscribe to outputs.
//
void  WorkerMarshal::openInputOutput(uint16    instance,
                                     string    endpoint)
{
//..............................................................................


    // Open a client to send and receive data.
    //
    _client = new MedusaClient(InetAddress());

    // Subscribe to outputs.
    //
    subscribeScatter1(instance, endpoint);

    {   // Establish input data path connections.
        connectGather1(instance);
    }
    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Activate the client I/O event loop.
void  WorkerMarshal::runClient()
{
//..............................................................................


    //  This does not return unless terminated or on an exception.
    _client->run();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Terminate the client I/O event loop.
void  WorkerMarshal::terminateClient()
{
//..............................................................................


    _client->terminate();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Copy a string value to a fixed length array and zero fill.
//
void  WorkerMarshal::setStringField( string  value,
                                       char  field[],
                                     uint32  length )
                              throw( AuroraException )
{
//..............................................................................


    if (value.length() > length)
    {   Throw(AuroraException,
              "Protocol string over " + to_string(length) + ".");
    }

    strncpy(field, value.c_str(), length);

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  WorkerMarshal::connectGather1(uint16   instance)
{
//..............................................................................


    // Starting to produce events on input stream.
    //
    if (!_client->set_data_path(MAX_BUFFER, Util::get_host_address("128.148.33.216"), 15000))
    {   ERROR << "Failed setting data path";
    }
    else
    {   _gather = GATHER1 + to_string(instance);
        DEBUG << "Set data path for stream=" << _gather;

        _eventGather1 = ptr<StreamEvent>(new StreamEvent(_gather));
        _eventGather1->_inject = true;
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  WorkerMarshal::batchGather1( Gather1  *tuple )
{
    int32  size;
//..............................................................................


    // Inject the array data into the event.
    //
    size = 160;
    _eventGather1->insert_array(string((const char *)tuple->_data.array, size));

    // Replace the array pointer with the array size in bytes.
    //
    tuple->_data.array = (uint8  *)size;

    // Tuples are buffered in a string.
    //
    _eventGather1->insert_bin(string((const char *)tuple,
                                  sizeof(Gather1)));

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  WorkerMarshal::sendGather1( uint32  sleep )
{
//..............................................................................


    // Transmit data to the node.
    Status  status = _client->fast_post_event(_eventGather1);

    while (!status)
    {   if (status.as_string() == DataHandler::NO_SPACE)
        {   // Wait if no more space in buffer.
            // At this point the data was never put in the buffer.
            // It needs to be requeued unless we want to drop it.
            WARN << "We dropped a tuple.";
            Thread::sleep(Time::msecs(sleep));

            // retry (make this conditional).
            status = _client->fast_post_event(_eventGather1);
        }
        else
        {   ERROR << "Connection closed... stopping event stream";
            return;
        }
    }

    if (sleep)
    {    // The event loop is activated so that the queue can be processed.
         // The callback is enqueued with a timer.
         // We only callback with a timer because this is looping.
         // We also need a delayed callback so the queue can be processed.
         // If we just go to sleep the event loop will not be run.
         //
         (new CallbackTimer(_client->get_loop(),
                             wrap(this, &WorkerMarshal::delayGather1)))
                  ->arm(Time::now() + Time::msecs(sleep));

    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Resume here after sending a tuple.
//
void  WorkerMarshal::delayGather1()
{
//..............................................................................


    // Release the previous event.
    //
    _eventGather1.reset();

    // Construct a new Gather1 input event.
    //
    _eventGather1 = ptr<StreamEvent>(new StreamEvent(_gather));
    _eventGather1->_inject = true;

    // Return to the application code.
    //
    sentGather1();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Dispatch output on our fast datapath to a handler.
//
Status  WorkerMarshal::outputHandler(ptr<StreamEvent>  event)
{
//..............................................................................


    //if (event->_stream == Name(SCATTER1))
    if (event->_stream == Name(WorkerMarshal::_scatter))
    {   return  WorkerMarshal::Scatter1Handler(event);
    }

    return  string("Unknown output stream ") + to_string(event->_stream);
}



////////////////////////////////////////////////////////////////////////////////
//
// Subscribing to receive output on a fast datapath.
//
void  WorkerMarshal::subscribeScatter1(uint16    instance,
                                       string    endpoint)
{
//..............................................................................


    _scatter = SCATTER1 + to_string(instance);
    DEBUG << "Subscribing to receive output on _scatter=" << _scatter;

    // Setup the subscription request.
    Status  status = _client->set_data_handler(
                                  InetAddress(Util::form_endpoint(endpoint,
                                                                  DEFAULT_MONITOR_PORT)),
                                  wrap(&outputHandler));

    if (status)
    {   DEBUG << "Done subscribing to output.";
    }
    else
    {   ERROR << "Could not subscribe to output.";
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Receive output on a fast datapath.
//
Status  WorkerMarshal::Scatter1Handler(ptr<StreamEvent>  event)
{
    tile  *tuple;
    int32         index;
    uint32        offset = 0;
    uint32        size   = 0;
    uint8        *array;
    uint32        size_array;
//..............................................................................


    DEBUG << "In count=" << event.use_count();   // 5 references here.
    _active_event.push(event);
    DEBUG << "Out count=" << event.use_count();  // 6 references here.

/***********************
    // For each tuple that was received,
    //
    for (index = 0; index < event->_inserted_count; index++)
    {
        offset += HEADER_SIZE;

        // At the tuple data past the header.
        //
        tuple = (tile *)&event->_bin_tuples[offset];
        DEBUG << "DATA:  " << to_hex_string(tuple, sizeof(tile));

        // At the array data in the event.
        //
        array = (uint8 *)&(event->_bin_arrays[size]);

        // Offset past the array data.
        //
        size_array = *(uint32 *)&tuple->array;
        size += size_array;
        DEBUG << "size=" << size << "  size_array=" << size_array;

        // Modify the array field to reference the array data.
        //
        tuple->array = array;

        receivedScatter1(tuple);
        offset += sizeof(tile);
    }
******************/

    // Signal done with this batch.
    //
    return  true;
}



////////////////////////////////////////////////////////////////////////////////
//
// Launch the Borealis front-end.
//
int32  WorkerMarshal::launchDiagram(string  xml)  // Path of an initial xml file.
{
    int32   status;
    string  command;
//..............................................................................


    INFO << "Connect with: " << xml;

    command = string() + "BigGiantHead  " + xml;
    status  = system(command.c_str());

    DEBUG << "Ran the Borealis front end:  " << status;

    return  status;
}

////////////////////////////  end  WorkerMarshal.cc  //////////////////////////
