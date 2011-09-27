#include  "MytestdistMarshal.h"
#include  "util.h"

#include "HeadClient.h"
#define   HEAD_NODE   "localhost"

#define MYTESTDIST_XML  "mytestdist.xml"
#define MYTESTDIST_DEPLOY_XML  "mytestdist-deploy.xml"
#define MAX_BUFFER      64000

#define MYTESTDIST_ENDPOINT     "127.0.0.1:25000"

#define PACKET  "packet"
#define AGGREGATE  "aggregate"

////////////////////////////////////////////////////////////////////////////////
//
// Generated marshaling code for the Mytestdist program.
//..............................................................................


////////////////////////////////////////////////////////////////////////////////
//
//  Subscribe to input and output streams.
//
void  MytestdistMarshal::open()
{

    // Open a client to send and receive data.
    //
    _client = new MedusaClient(InetAddress());

    // Subscribe to outputs.
    subscribeAggregate();

    // Deploy the query diagram
    launchDiagram();

    // Prepare connection to send inputs
    connectPacket();

}



////////////////////////////////////////////////////////////////////////////////
//
//  Connect to inputs, subscribe to outputs.
//
void  MytestdistMarshal::openInputOutput()
{
//..............................................................................


    // Open a client to send and receive data.
    //
    _client = new MedusaClient(InetAddress());

    // Subscribe to outputs.
    subscribeAggregate();


    {   // Establish input data path connections.
        connectPacket();
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Activate the client I/O event loop.
void  MytestdistMarshal::runClient()
{
//..............................................................................


    //  This does not return unless terminated or on an exception.
    _client->run();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Terminate the client I/O event loop.
void  MytestdistMarshal::terminateClient()
{
//..............................................................................


    _client->terminate();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Copy a string value to a fixed length array and zero fill.
//
void  MytestdistMarshal::setStringField( string  value,
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
void  MytestdistMarshal::connectPacket()
{
//..............................................................................


    // Starting to produce events on input stream.
    //
    if (!_client->set_data_path(MAX_BUFFER, Util::get_host_address("127.0.0.1"), 15000))
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
void  MytestdistMarshal::batchPacket( Packet  *tuple )
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
void  MytestdistMarshal::sendPacket( uint32  sleep )
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
                             wrap(this, &MytestdistMarshal::delayPacket)))
                  ->arm(Time::now() + Time::msecs(sleep));

    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Resume here after sending a tuple.
//
void  MytestdistMarshal::delayPacket()
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
Status  MytestdistMarshal::outputHandler(ptr<StreamEvent>  event)
{
//..............................................................................


    if (event->_stream == Name(AGGREGATE))
    {   return MytestdistMarshal::AggregateHandler(event);
    }

    return(string("Unknown output stream") + to_string(event->_stream));
}



////////////////////////////////////////////////////////////////////////////////
//
// Subscribing to receive output on a fast datapath.
//
void  MytestdistMarshal::subscribeAggregate()
{
//..............................................................................


    DEBUG << "Subscribing to receive output.";

    // Setup the subscription request.
    Status  status = _client->set_data_handler(
                                  InetAddress(Util::form_endpoint(MYTESTDIST_ENDPOINT,
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
Status  MytestdistMarshal::AggregateHandler(ptr<StreamEvent>  event)
{
    uint32     offset = 0;
    int32      index;
//..............................................................................


    // For each tuple that was received,
    //
    for (index = 0; index < event->_inserted_count; index++)
    {   offset += HEADER_SIZE;
        receivedAggregate((AggregateTuple *)&event->_bin_tuples[offset]);
        offset += sizeof(AggregateTuple);
    }

    // Signal done with this batch.
    return(true);
}



////////////////////////////////////////////////////////////////////////////////
//
// Deploy the query diagram
//
void  MytestdistMarshal::launchDiagram()  // Path of an initial xml file.
{

    HeadClient  *client;
    RPC<void>  rpc;
 
    client = (HeadClient *)new HeadClient( InetAddress( HEAD_NODE,
                                                        DEFAULT_HEAD_PORT ));

    string query_file        = MYTESTDIST_XML;
    string deployment_file   = MYTESTDIST_DEPLOY_XML;

    rpc = client->deploy_xml_file( query_file);
    if ( ! rpc.valid() )
    { 
        WARN << "Error with query file " << rpc.stat();
        exit(0);
    }

    rpc = client->deploy_xml_file(deployment_file);        
    if ( ! rpc.valid() )
    { 
        WARN << "Error with deployment file " << rpc.stat();
        exit(0);
    }

}

////////////////////////////  end  MytestdistMarshal.cc  //////////////////////////
