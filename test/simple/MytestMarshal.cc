#include  "MytestMarshal.h"
#include  "util.h"

#define MYTEST_XML     "mytest.xml"
#define MAX_BUFFER      640000000

#define MYTEST_ENDPOINT     "127.0.1.1:25000"

#define PACKET  "packet"
#define AGGREGATE  "aggregate"

////////////////////////////////////////////////////////////////////////////////
//
// Generated marshaling code for the Mytest program.
//..............................................................................


////////////////////////////////////////////////////////////////////////////////
//
//  Subscribe to input and output streams.
//
int32  MytestMarshal::open()
{
    int32   status;
//..............................................................................


    // Open a client to send and receive data.
    //
    _client = new MedusaClient(InetAddress());

    // Subscribe to outputs.
    //
    subscribeAggregate();

    //  Launch the front-end with the xml file.
    //  Creates a client that communicates with a borealis node at ip
    //  though the given port.
    //  Then it sends the network diagram as XML strings
    //  and runs start_query.
    //  It waits until the front-end terminates and the port is now free.
    //
    status = launchDiagram(MYTEST_XML);

    if (status)
    {   ERROR << "launchDiagram failed ( " << status << " ).";
    }
    else
    {   // Establish input data path connections.
        connectPacket();
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Connect to inputs, subscribe to outputs.
//
void  MytestMarshal::openInputOutput()
{
//..............................................................................


    // Open a client to send and receive data.
    //
    _client = new MedusaClient(InetAddress());

    // Subscribe to outputs.
    //
    subscribeAggregate();

    {   // Establish input data path connections.
        connectPacket();
    }
    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Activate the client I/O event loop.
void  MytestMarshal::runClient()
{
//..............................................................................


    //  This does not return unless terminated or on an exception.
    _client->run();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Terminate the client I/O event loop.
void  MytestMarshal::terminateClient()
{
//..............................................................................


    _client->terminate();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Copy a string value to a fixed length array and zero fill.
//
void  MytestMarshal::setStringField(string  value,
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
void  MytestMarshal::connectPacket()
{
//..............................................................................


    // Starting to produce events on input stream.
    //
    if (!_client->set_data_path(MAX_BUFFER, Util::get_host_address("127.0.1.1"), 15000))
    {   ERROR << "Failed setting data path";
    }
    else
    {   DEBUG << "Set data path";

        _eventPacket = ptr<StreamEvent>(new StreamEvent(PACKET));
        _eventPacket->_inject = true;
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  MytestMarshal::batchPacket( Packet  *tuple )
{
//..............................................................................


    // Tuples are buffered in a string.
    //
    _eventPacket->insert_bin(string((const char *)tuple,
                                  sizeof(Packet)));

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  MytestMarshal::sendPacket( uint32  sleep )
{
//..............................................................................


    // Transmit data to the node.
    Status  status = _client->fast_post_event(_eventPacket);

    while (!status)
    {   if (status.as_string() == DataHandler::NO_SPACE)
        {   // Wait if no more space in buffer.
            // At this point the data was never put in the buffer.
            // It needs to be requeued unless we want to drop it.
            WARN << "We dropped a tuple.";
            Thread::sleep(Time::msecs(sleep));

            // retry (make this conditional).
            status = _client->fast_post_event(_eventPacket);
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
                             wrap(this, &MytestMarshal::delayPacket)))
                  ->arm(Time::now() + Time::msecs(sleep));

    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Resume here after sending a tuple.
//
void  MytestMarshal::delayPacket()
{
//..............................................................................


    // Release the previous event.
    //
    _eventPacket.reset();

    // Construct a new Packet input event.
    //
    _eventPacket = ptr<StreamEvent>(new StreamEvent(PACKET));
    _eventPacket->_inject = true;

    // Return to the application code.
    //
    sentPacket();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Dispatch output on our fast datapath to a handler.
//
Status  MytestMarshal::outputHandler(ptr<StreamEvent>  event)
{
//..............................................................................


    if (event->_stream == Name(AGGREGATE))
    {   return MytestMarshal::AggregateHandler(event);
    }

    return  string("Unknown output stream ") + to_string(event->_stream);
}



////////////////////////////////////////////////////////////////////////////////
//
// Subscribing to receive output on a fast datapath.
//
void  MytestMarshal::subscribeAggregate()
{
//..............................................................................


    DEBUG << "Subscribing to receive output.";

    // Setup the subscription request.
    Status  status = _client->set_data_handler(
                                  InetAddress(Util::form_endpoint(MYTEST_ENDPOINT,
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
Status  MytestMarshal::AggregateHandler(ptr<StreamEvent>  event)
{
    AggregateTuple  *tuple;
    int32         index;
    uint32        offset = 0;
//..............................................................................


    // For each tuple that was received,
    //
    for (index = 0; index < event->_inserted_count; index++)
    {
        offset += HEADER_SIZE;

        // At the tuple data past the header.
        //
        tuple = (AggregateTuple *)&event->_bin_tuples[offset];
        DEBUG << "DATA:  " << to_hex_string(tuple, sizeof(AggregateTuple));

        receivedAggregate(tuple);
        offset += sizeof(AggregateTuple);
    }

    // Signal done with this batch.
    //
    return  true;
}



////////////////////////////////////////////////////////////////////////////////
//
// Launch the Borealis front-end.
//
int32  MytestMarshal::launchDiagram(string  xml)  // Path of an initial xml file.
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

////////////////////////////  end  MytestMarshal.cc  //////////////////////////
