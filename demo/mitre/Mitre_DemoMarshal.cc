#include  "Mitre_DemoMarshal.h"
#include  "util.h"

#define MITRE_DEMO_XML     "mitre_demo.xml mitre_demo_deployment.xml"
#define MAX_BUFFER      64000

#define MITRE_DEMO_ENDPOINT     "128.148.31.135:25000"

#define ELEMENTINPUT  "elementinput"
#define VIDEOINPUT  "videoinput"
#define ENEMYTANKSOUTPUT  "enemytanksoutput"
#define ANALYSISOUTPUT  "analysisoutput"
#define ACROSSTHELINEOUTPUT  "acrossthelineoutput"
#define ENEMYAIRCRAFTOUTPUT  "enemyaircraftoutput"
#define VIDEOOUTPUT  "videooutput"

////////////////////////////////////////////////////////////////////////////////
//
// Generated marshaling code for the Mitre_Demo program.
//..............................................................................


////////////////////////////////////////////////////////////////////////////////
//
//  Subscribe to input and output streams.
//
int32  Mitre_DemoMarshal::open()
{
    int32   status;
//..............................................................................


    // Open a client to send and receive data.
    //
    _client = new MedusaClient(InetAddress());

    // Subscribe to outputs.
    subscribeEnemytanksoutput();
    subscribeAnalysisoutput();
    subscribeAcrossthelineoutput();
    subscribeEnemyaircraftoutput();
    subscribeVideooutput();


    //  Launch the front-end with the xml file.
    //  Creates a client that communicates with a borealis node at ip
    //  though the given port.
    //  Then it sends the network diagram as XML strings
    //  and runs start_query.
    //  It waits until the front-end terminates and the port is now free.
    //
    status = launchDiagram(MITRE_DEMO_XML);

    if (status)
    {   ERROR << "launchDiagram failed ( " << status << " ).";
    }
    else
    {   // Establish input data path connections.
        connectElementinput();
        connectVideoinput();
    }

    return(status);
}



////////////////////////////////////////////////////////////////////////////////
//
//  Connect to inputs, subscribe to outputs.
//
void  Mitre_DemoMarshal::openInputOutput()
{
//..............................................................................


    // Open a client to send and receive data.
    //
    _client = new MedusaClient(InetAddress());

    // Subscribe to outputs.
    subscribeEnemytanksoutput();
    subscribeAnalysisoutput();
    subscribeAcrossthelineoutput();
    subscribeEnemyaircraftoutput();
    subscribeVideooutput();


    {   // Establish input data path connections.
        connectElementinput();
        connectVideoinput();
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Activate the client I/O event loop.
void  Mitre_DemoMarshal::runClient()
{
//..............................................................................


    //  This does not return unless terminated or on an exception.
    _client->run();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Terminate the client I/O event loop.
void  Mitre_DemoMarshal::terminateClient()
{
//..............................................................................


    _client->terminate();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Copy a string value to a fixed length array and zero fill.
//
void  Mitre_DemoMarshal::setStringField( string  value,
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
void  Mitre_DemoMarshal::connectElementinput()
{
//..............................................................................


    // Starting to produce events on input stream.
    //
    if (!_client->set_data_path(MAX_BUFFER, Util::get_host_address("128.148.31.135"), 15000))
    {   ERROR << "Failed setting data path";
    }
    else
    {   DEBUG << "Set data path";

        _eventElementinput = ptr<StreamEvent>(new StreamEvent(ELEMENTINPUT));
        _eventElementinput->_inject = true;
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  Mitre_DemoMarshal::batchElementinput( Elementinput  *tuple )
{
//..............................................................................


    // Tuples are buffered in a string.
    //
    _eventElementinput->insert_bin(string((const char *)tuple,
                                  sizeof(Elementinput)));

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  Mitre_DemoMarshal::sendElementinput( uint32  sleep )
{
//..............................................................................


    // Transmit data to the node.
    Status  status = _client->fast_post_event(_eventElementinput);

    while (!status)
    {   if (status.as_string() == DataHandler::NO_SPACE)
        {   // Wait if no more space in buffer.
            // At this point the data was never put in the buffer.
            // It needs to be requeued unless we want to drop it.
            WARN << "We dropped a tuple.";
            Thread::sleep(Time::msecs(sleep));

            // retry (make this conditional).
            status = _client->fast_post_event(_eventElementinput);
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
                             wrap(this, &Mitre_DemoMarshal::delayElementinput)))
                  ->arm(Time::now() + Time::msecs(sleep));

    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Resume here after sending a tuple.
//
void  Mitre_DemoMarshal::delayElementinput()
{
//..............................................................................


    // Release the previous event.
    //
    _eventElementinput.reset();

    // Construct a new Elementinput input event.
    //
    _eventElementinput = ptr<StreamEvent>(new StreamEvent(ELEMENTINPUT));
    _eventElementinput->_inject = true;

    // Return to the application code.
    //
    sentElementinput();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  Mitre_DemoMarshal::connectVideoinput()
{
//..............................................................................


    // Starting to produce events on input stream.
    //
    if (!_client->set_data_path(MAX_BUFFER, Util::get_host_address("128.148.31.135"), 15000))
    {   ERROR << "Failed setting data path";
    }
    else
    {   DEBUG << "Set data path";

        _eventVideoinput = ptr<StreamEvent>(new StreamEvent(VIDEOINPUT));
        _eventVideoinput->_inject = true;
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  Mitre_DemoMarshal::batchVideoinput( Videoinput  *tuple )
{
//..............................................................................


    // Tuples are buffered in a string.
    //
    _eventVideoinput->insert_bin(string((const char *)tuple,
                                  sizeof(Videoinput)));

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  Mitre_DemoMarshal::sendVideoinput( uint32  sleep )
{
//..............................................................................


    // Transmit data to the node.
    Status  status = _client->fast_post_event(_eventVideoinput);

    while (!status)
    {   if (status.as_string() == DataHandler::NO_SPACE)
        {   // Wait if no more space in buffer.
            // At this point the data was never put in the buffer.
            // It needs to be requeued unless we want to drop it.
            WARN << "We dropped a tuple.";
            Thread::sleep(Time::msecs(sleep));

            // retry (make this conditional).
            status = _client->fast_post_event(_eventVideoinput);
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
                             wrap(this, &Mitre_DemoMarshal::delayVideoinput)))
                  ->arm(Time::now() + Time::msecs(sleep));

    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Resume here after sending a tuple.
//
void  Mitre_DemoMarshal::delayVideoinput()
{
//..............................................................................


    // Release the previous event.
    //
    _eventVideoinput.reset();

    // Construct a new Videoinput input event.
    //
    _eventVideoinput = ptr<StreamEvent>(new StreamEvent(VIDEOINPUT));
    _eventVideoinput->_inject = true;

    // Return to the application code.
    //
    sentVideoinput();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Dispatch output on our fast datapath to a handler.
//
Status  Mitre_DemoMarshal::outputHandler(ptr<StreamEvent>  event)
{
//..............................................................................


    if (event->_stream == Name(ENEMYTANKSOUTPUT))
    {   return Mitre_DemoMarshal::EnemytanksoutputHandler(event);
    }

    if (event->_stream == Name(ANALYSISOUTPUT))
    {   return Mitre_DemoMarshal::AnalysisoutputHandler(event);
    }

    if (event->_stream == Name(ACROSSTHELINEOUTPUT))
    {   return Mitre_DemoMarshal::AcrossthelineoutputHandler(event);
    }

    if (event->_stream == Name(ENEMYAIRCRAFTOUTPUT))
    {   return Mitre_DemoMarshal::EnemyaircraftoutputHandler(event);
    }

    if (event->_stream == Name(VIDEOOUTPUT))
    {   return Mitre_DemoMarshal::VideooutputHandler(event);
    }

    return(string("Unknown output stream") + to_string(event->_stream));
}



////////////////////////////////////////////////////////////////////////////////
//
// Subscribing to receive output on a fast datapath.
//
void  Mitre_DemoMarshal::subscribeEnemytanksoutput()
{
//..............................................................................


    DEBUG << "Subscribing to receive output.";

    // Setup the subscription request.
    Status  status = _client->set_data_handler(
                                  InetAddress(Util::form_endpoint(MITRE_DEMO_ENDPOINT,
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
Status  Mitre_DemoMarshal::EnemytanksoutputHandler(ptr<StreamEvent>  event)
{
    int32         index;
    uint32        offset = 0;
    AppElement  *tuple;
//..............................................................................


    // For each tuple that was received,
    //
    for (index = 0; index < event->_inserted_count; index++)
    {
        offset += HEADER_SIZE;

        // At the tuple data past the header.
        //
        tuple = (AppElement *)&event->_bin_tuples[offset];
        DEBUG << "DATA:  " << to_hex_string(tuple, sizeof(AppElement));

        receivedEnemytanksoutput(tuple);
        offset += sizeof(AppElement);
    }

    // Signal done with this batch.
    //
    return(true);
}



////////////////////////////////////////////////////////////////////////////////
//
// Subscribing to receive output on a fast datapath.
//
void  Mitre_DemoMarshal::subscribeAnalysisoutput()
{
//..............................................................................


    DEBUG << "Subscribing to receive output.";

    // Setup the subscription request.
    Status  status = _client->set_data_handler(
                                  InetAddress(Util::form_endpoint(MITRE_DEMO_ENDPOINT,
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
Status  Mitre_DemoMarshal::AnalysisoutputHandler(ptr<StreamEvent>  event)
{
    int32         index;
    uint32        offset = 0;
    AppElement  *tuple;
//..............................................................................


    // For each tuple that was received,
    //
    for (index = 0; index < event->_inserted_count; index++)
    {
        offset += HEADER_SIZE;

        // At the tuple data past the header.
        //
        tuple = (AppElement *)&event->_bin_tuples[offset];
        DEBUG << "DATA:  " << to_hex_string(tuple, sizeof(AppElement));

        receivedAnalysisoutput(tuple);
        offset += sizeof(AppElement);
    }

    // Signal done with this batch.
    //
    return(true);
}



////////////////////////////////////////////////////////////////////////////////
//
// Subscribing to receive output on a fast datapath.
//
void  Mitre_DemoMarshal::subscribeAcrossthelineoutput()
{
//..............................................................................


    DEBUG << "Subscribing to receive output.";

    // Setup the subscription request.
    Status  status = _client->set_data_handler(
                                  InetAddress(Util::form_endpoint(MITRE_DEMO_ENDPOINT,
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
Status  Mitre_DemoMarshal::AcrossthelineoutputHandler(ptr<StreamEvent>  event)
{
    int32         index;
    uint32        offset = 0;
    AppElement  *tuple;
//..............................................................................


    // For each tuple that was received,
    //
    for (index = 0; index < event->_inserted_count; index++)
    {
        offset += HEADER_SIZE;

        // At the tuple data past the header.
        //
        tuple = (AppElement *)&event->_bin_tuples[offset];
        DEBUG << "DATA:  " << to_hex_string(tuple, sizeof(AppElement));

        receivedAcrossthelineoutput(tuple);
        offset += sizeof(AppElement);
    }

    // Signal done with this batch.
    //
    return(true);
}



////////////////////////////////////////////////////////////////////////////////
//
// Subscribing to receive output on a fast datapath.
//
void  Mitre_DemoMarshal::subscribeEnemyaircraftoutput()
{
//..............................................................................


    DEBUG << "Subscribing to receive output.";

    // Setup the subscription request.
    Status  status = _client->set_data_handler(
                                  InetAddress(Util::form_endpoint(MITRE_DEMO_ENDPOINT,
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
Status  Mitre_DemoMarshal::EnemyaircraftoutputHandler(ptr<StreamEvent>  event)
{
    int32         index;
    uint32        offset = 0;
    AppElement  *tuple;
//..............................................................................


    // For each tuple that was received,
    //
    for (index = 0; index < event->_inserted_count; index++)
    {
        offset += HEADER_SIZE;

        // At the tuple data past the header.
        //
        tuple = (AppElement *)&event->_bin_tuples[offset];
        DEBUG << "DATA:  " << to_hex_string(tuple, sizeof(AppElement));

        receivedEnemyaircraftoutput(tuple);
        offset += sizeof(AppElement);
    }

    // Signal done with this batch.
    //
    return(true);
}



////////////////////////////////////////////////////////////////////////////////
//
// Subscribing to receive output on a fast datapath.
//
void  Mitre_DemoMarshal::subscribeVideooutput()
{
//..............................................................................


    DEBUG << "Subscribing to receive output.";

    // Setup the subscription request.
    Status  status = _client->set_data_handler(
                                  InetAddress(Util::form_endpoint(MITRE_DEMO_ENDPOINT,
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
Status  Mitre_DemoMarshal::VideooutputHandler(ptr<StreamEvent>  event)
{
    int32         index;
    uint32        offset = 0;
    ImageData  *tuple;
//..............................................................................


    // For each tuple that was received,
    //
    for (index = 0; index < event->_inserted_count; index++)
    {
        offset += HEADER_SIZE;

        // At the tuple data past the header.
        //
        tuple = (ImageData *)&event->_bin_tuples[offset];
        DEBUG << "DATA:  " << to_hex_string(tuple, sizeof(ImageData));

        receivedVideooutput(tuple);
        offset += sizeof(ImageData);
    }

    // Signal done with this batch.
    //
    return(true);
}



////////////////////////////////////////////////////////////////////////////////
//
// Launch the Borealis front-end.
//
int32  Mitre_DemoMarshal::launchDiagram(string  xml)  // Path of an initial xml file.
{
    int32   status;
    string  command;
//..............................................................................


    INFO << "Connect with: " << xml;

    command = string() + "BigGiantHead  " + xml;
    status  = system(command.c_str());

    DEBUG << "Ran the Borealis front end:  " << status;

    return(status);
}

////////////////////////////  end  Mitre_DemoMarshal.cc  //////////////////////////
