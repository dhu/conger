#include  "WorkerMarshal.h"
#include  "util.h"

#define WORKER_XML     "Pass.xml PassDeploy.xml"
#define MAX_BUFFER      640000000

#define WORKER_ENDPOINT     "128.148.33.216:25100 xyellow:25100"

#define GATHER  "gather"

////////////////////////////////////////////////////////////////////////////////
//
// Generated marshaling code for the Worker program.
//..............................................................................


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
        connectGather();
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Connect to inputs, subscribe to outputs.
//
void  WorkerMarshal::openInputOutput()
{
//..............................................................................


    // Open a client to send and receive data.
    //
    _client = new MedusaClient(InetAddress());

    // Subscribe to outputs.
    //

    {   // Establish input data path connections.
        connectGather();
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
void  WorkerMarshal::setStringField(string  value,
                                       char  field[],
                                     uint32  length)
                              throw(AuroraException)
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
void  WorkerMarshal::connectGather()
{
//..............................................................................


    // Starting to produce events on input stream.
    //
    if (!_client->set_data_path(MAX_BUFFER, Util::get_host_address("128.148.33.216"), 15000))
    {   ERROR << "Failed setting data path";
    }
    else
    {   DEBUG << "Set data path";

        _eventGather = ptr<StreamEvent>(new StreamEvent(GATHER));
        _eventGather->_inject = true;
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  WorkerMarshal::batchGather( Gather  *tuple )
{
    int32  size;
//..............................................................................


    // Inject the array data into the event.
    //
    size = 160;
    _eventGather->insert_array(string((const char *)tuple->_data.array, size));

    // Replace the array pointer with the array size in bytes.
    //
    tuple->_data.array = (uint8  *)size;

    // Tuples are buffered in a string.
    //
    _eventGather->insert_bin(string((const char *)tuple,
                                  sizeof(Gather)));

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  WorkerMarshal::sendGather( uint32  sleep )
{
//..............................................................................


    // Transmit data to the node.
    Status  status = _client->fast_post_event(_eventGather);

    while (!status)
    {   if (status.as_string() == DataHandler::NO_SPACE)
        {   // Wait if no more space in buffer.
            // At this point the data was never put in the buffer.
            // It needs to be requeued unless we want to drop it.
            WARN << "We dropped a tuple.";
            Thread::sleep(Time::msecs(sleep));

            // retry (make this conditional).
            status = _client->fast_post_event(_eventGather);
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
                             wrap(this, &WorkerMarshal::delayGather)))
                  ->arm(Time::now() + Time::msecs(sleep));

    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Resume here after sending a tuple.
//
void  WorkerMarshal::delayGather()
{
//..............................................................................


    // Release the previous event.
    //
    _eventGather.reset();

    // Construct a new Gather input event.
    //
    _eventGather = ptr<StreamEvent>(new StreamEvent(GATHER));
    _eventGather->_inject = true;

    // Return to the application code.
    //
    sentGather();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Dispatch output on our fast datapath to a handler.
//
Status  WorkerMarshal::outputHandler(ptr<StreamEvent>  event)
{
//..............................................................................


    return  string("Unknown output stream ") + to_string(event->_stream);
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
