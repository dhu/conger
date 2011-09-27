#include  "PassMarshal.h"
#include  "util.h"

#define PASS_XML     "Pass.xml PassDeploy.xml"
#define MAX_BUFFER      640000000

#define PASS_ENDPOINT     "128.148.33.216:25000"

#define FRAME_IN  "frame_in"
#define FRAME_OUT  "frame_out"

////////////////////////////////////////////////////////////////////////////////
//
// Generated marshaling code for the Pass program.
//..............................................................................


////////////////////////////////////////////////////////////////////////////////
//
//  Subscribe to input and output streams.
//
int32  PassMarshal::open()
{
    int32   status;
//..............................................................................


    // Open a client to send and receive data.
    //
    _client = new MedusaClient(InetAddress());

    // Subscribe to outputs.
    //
    subscribeFrame_Out();

    //  Launch the front-end with the xml file.
    //  Creates a client that communicates with a borealis node at ip
    //  though the given port.
    //  Then it sends the network diagram as XML strings
    //  and runs start_query.
    //  It waits until the front-end terminates and the port is now free.
    //
    status = launchDiagram(PASS_XML);

    if (status)
    {   ERROR << "launchDiagram failed ( " << status << " ).";
    }
    else
    {   // Establish input data path connections.
        connectFrame_In();
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Connect to inputs, subscribe to outputs.
//
void  PassMarshal::openInputOutput()
{
//..............................................................................


    // Open a client to send and receive data.
    //
    _client = new MedusaClient(InetAddress());

    // Subscribe to outputs.
    //
    subscribeFrame_Out();

    {   // Establish input data path connections.
        connectFrame_In();
    }
    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Activate the client I/O event loop.
void  PassMarshal::runClient()
{
//..............................................................................


    //  This does not return unless terminated or on an exception.
    _client->run();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Terminate the client I/O event loop.
void  PassMarshal::terminateClient()
{
//..............................................................................


    _client->terminate();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Copy a string value to a fixed length array and zero fill.
//
void  PassMarshal::setStringField(string  value,
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
void  PassMarshal::connectFrame_In()
{
//..............................................................................


    // Starting to produce events on input stream.
    //
    if (!_client->set_data_path(MAX_BUFFER, Util::get_host_address("128.148.33.216"), 15000))
    {   ERROR << "Failed setting data path";
    }
    else
    {   DEBUG << "Set data path";

        _eventFrame_In = ptr<StreamEvent>(new StreamEvent(FRAME_IN));
        _eventFrame_In->_inject = true;
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  PassMarshal::batchFrame_In( Frame_In  *tuple )
{
    int32  size;
//..............................................................................


    // Inject the array data into the event.
    //
    size = 320;
    _eventFrame_In->insert_array(string((const char *)tuple->_data.array, size));

    // Replace the array pointer with the array size in bytes.
    //
    tuple->_data.array = (uint8  *)size;

    // Tuples are buffered in a string.
    //
    _eventFrame_In->insert_bin(string((const char *)tuple,
                                  sizeof(Frame_In)));

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  PassMarshal::sendFrame_In( uint32  sleep )
{
//..............................................................................


    // Transmit data to the node.
    Status  status = _client->fast_post_event(_eventFrame_In);

    while (!status)
    {   if (status.as_string() == DataHandler::NO_SPACE)
        {   // Wait if no more space in buffer.
            // At this point the data was never put in the buffer.
            // It needs to be requeued unless we want to drop it.
            WARN << "We dropped a tuple.";
            Thread::sleep(Time::msecs(sleep));

            // retry (make this conditional).
            status = _client->fast_post_event(_eventFrame_In);
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
                             wrap(this, &PassMarshal::delayFrame_In)))
                  ->arm(Time::now() + Time::msecs(sleep));

    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Resume here after sending a tuple.
//
void  PassMarshal::delayFrame_In()
{
//..............................................................................


    // Release the previous event.
    //
    _eventFrame_In.reset();

    // Construct a new Frame_In input event.
    //
    _eventFrame_In = ptr<StreamEvent>(new StreamEvent(FRAME_IN));
    _eventFrame_In->_inject = true;

    // Return to the application code.
    //
    sentFrame_In();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Dispatch output on our fast datapath to a handler.
//
Status  PassMarshal::outputHandler(ptr<StreamEvent>  event)
{
//..............................................................................


    if (event->_stream == Name(FRAME_OUT))
    {   return PassMarshal::Frame_OutHandler(event);
    }

    return  string("Unknown output stream ") + to_string(event->_stream);
}



////////////////////////////////////////////////////////////////////////////////
//
// Subscribing to receive output on a fast datapath.
//
void  PassMarshal::subscribeFrame_Out()
{
//..............................................................................


    DEBUG << "Subscribing to receive output.";

    // Setup the subscription request.
    Status  status = _client->set_data_handler(
                                  InetAddress(Util::form_endpoint(PASS_ENDPOINT,
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
Status  PassMarshal::Frame_OutHandler(ptr<StreamEvent>  event)
{
    frame  *tuple;
    int32         index;
    uint32        offset = 0;
    uint32        size   = 0;
    uint8        *array;
    uint32        size_array;
//..............................................................................


    // For each tuple that was received,
    //
    for (index = 0; index < event->_inserted_count; index++)
    {
        offset += HEADER_SIZE;

        // At the tuple data past the header.
        //
        tuple = (frame *)&event->_bin_tuples[offset];
        DEBUG << "DATA:  " << to_hex_string(tuple, sizeof(frame));

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

        receivedFrame_Out(tuple);
        offset += sizeof(frame);
    }

    // Signal done with this batch.
    //
    return  true;
}



////////////////////////////////////////////////////////////////////////////////
//
// Launch the Borealis front-end.
//
int32  PassMarshal::launchDiagram(string  xml)  // Path of an initial xml file.
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

////////////////////////////  end  PassMarshal.cc  //////////////////////////
