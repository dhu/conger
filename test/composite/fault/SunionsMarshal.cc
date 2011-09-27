#include  "SunionsMarshal.h"
#include  "util.h"
#include  "Tuple.h"

#define SUNIONS_XML     "sunions.xml"
#define MAX_BUFFER      64000

#define CONTROLOUTPUT1  "controloutput1"
#define CONTROLOUTPUT1_NODE  "127.0.0.1"
#define CONTROLOUTPUT1_PORT  25000

#define CONTROLOUTPUT2  "controloutput2"
#define CONTROLOUTPUT2_NODE  "127.0.0.1"
#define CONTROLOUTPUT2_PORT  25000

#define CONTROLOUTPUT3  "controloutput3"
#define CONTROLOUTPUT3_NODE  "127.0.0.1"
#define CONTROLOUTPUT3_PORT  25000

#define CONTROLUNION1  "controlunion1"
#define CONTROLUNION1_NODE  "127.0.0.1"
#define CONTROLUNION1_PORT  25000

#define CONTROLUNION2  "controlunion2"
#define CONTROLUNION2_NODE  "127.0.0.1"
#define CONTROLUNION2_PORT  25000

#define CONTROLUNION3  "controlunion3"
#define CONTROLUNION3_NODE  "127.0.0.1"
#define CONTROLUNION3_PORT  25000

#define INPUT1  "input1"
#define INPUT1_NODE  "127.0.0.1"
#define INPUT1_PORT  15000

#define INPUT2  "input2"
#define INPUT2_NODE  "127.0.0.1"
#define INPUT2_PORT  15000

#define INPUT3  "input3"
#define INPUT3_NODE  "127.0.0.1"
#define INPUT3_PORT  15000

#define SOUTPUT1  "soutput1"
#define SOUTPUT1_NODE  "127.0.0.1"
#define SOUTPUT1_PORT  25000

#define SOUTPUT2  "soutput2"
#define SOUTPUT2_NODE  "127.0.0.1"
#define SOUTPUT2_PORT  25000

#define SOUTPUT3  "soutput3"
#define SOUTPUT3_NODE  "127.0.0.1"
#define SOUTPUT3_PORT  25000

#define UNION1  "union1"
#define UNION1_NODE  "127.0.0.1"
#define UNION1_PORT  25000

#define UNION2  "union2"
#define UNION2_NODE  "127.0.0.1"
#define UNION2_PORT  25000

#define UNION3  "union3"
#define UNION3_NODE  "127.0.0.1"
#define UNION3_PORT  25000


////////////////////////////////////////////////////////////////////////////////
//
// Generated Martialing code for the Sunions program.
//..............................................................................


////////////////////////////////////////////////////////////////////////////////
//
//  Subscribe to input and output streams.
//
int32  SunionsMarshal::open( string  ip,     // Local medusa client ip.
                            uint32  port    // Local medusa client port.
                           )
{
    //int32   status;
//..............................................................................


    // Open a client to interact with Borealis nodes.
    _client = new MedusaClient( InetAddress( Util::get_host_address( ip ), port ));

    // Must setup datapaths first
    subscribeControloutput1();
    subscribeControloutput2();
    subscribeControloutput3();
    subscribeControlunion1();
    subscribeControlunion2();
    subscribeControlunion3();
    subscribeSoutput1();
    subscribeSoutput2();
    subscribeSoutput3();

    // Establish input data path connections.
    connectInput1();
    connectInput2();
    connectInput3();

    return launchDiagram( SUNIONS_XML, ip, port );

    //}
    //return( status );
}



////////////////////////////////////////////////////////////////////////////////
//
//  Connect to inputs, subscribe to outputs.
//
void  SunionsMarshal::openInputOutput( string  ip,     // Local medusa client ip.
                                       uint32  port    // Local medusa client port.
                                    )
{
//..............................................................................


    // Open a client to interact with Borealis nodes.
    _client = new MedusaClient( InetAddress( Util::get_host_address( ip ), port ));

    {
        // Subscribe to outputs.
        subscribeControloutput1();
        subscribeControloutput2();
        subscribeControloutput3();
        subscribeControlunion1();
        subscribeControlunion2();
        subscribeControlunion3();
        subscribeSoutput1();
        subscribeSoutput2();
        subscribeSoutput3();

        // Establish input data path connections.
        connectInput1();
        connectInput2();
        connectInput3();
    }
    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Activate all Borealis nodes.
void  SunionsMarshal::runClient()
{
//..............................................................................


    //  This does not return unless there is an exception.
    _client->run();

    ERROR << "Returned from running the client event loop.";

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Copy a string value to a fixed length array and zero fill.
//
void  SunionsMarshal::setStringField( string  value,
                                       char  field[],
                                     uint32  length )
                              throw( AuroraException )
{
//..............................................................................


    if ( value.length() > length )
    {   Throw( AuroraException,
               "Protocol string over " + to_string( length ) + "." );
    }

    strncpy( field, value.c_str(), length );

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  SunionsMarshal::connectInput1()
{
//..............................................................................


    // Starting to produce events on input stream.
    //
    if ( !_client->set_data_path( MAX_BUFFER, Util::get_host_address( INPUT1_NODE ), INPUT1_PORT ))
    {   FATAL << "Failed setting data path";
    }

    DEBUG << "Set data path";

    _eventInput1 = ptr<StreamEvent>( new StreamEvent( INPUT1 ));
    _eventInput1->_inject = true;

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  SunionsMarshal::batchInput1( Input1  *tuple )
{
//..............................................................................


    // Tuples are buffered in a string.
    //
    _eventInput1->insert_bin( string( (const char *)tuple,
                                  sizeof( Input1 )));

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  SunionsMarshal::sendInput1( uint32  sleep )
{
//..............................................................................


    // Transmit data to the node.
    Status  status = _client->fast_post_event( _eventInput1 );

    while ( !status )
    {   if ( status.as_string() == DataHandler::NO_SPACE )
        {   // Wait if no more space in buffer.
            // At this point the data was never put in the buffer.
            // It needs to be requeued unless we want to drop it.
            WARN << "We dropped a tuple.";
            Thread::sleep( Time::msecs( sleep ));

            // retry (make this conditional).
            status = _client->fast_post_event( _eventInput1 );
        }
        else
        {   ERROR << "Connection closed... stopping event stream";
            return;
        }
    }

    // The event loop is activated so that the queue can be processed.
    // The callback is enqueued with a timer.
    // We only callback with a timer because this is looping.
    // We also need a delayed callback so the queue can be processed.
    // If we just go to sleep the event loop will not be run.
    //
    ( new CallbackTimer( _client->get_loop(),
                         wrap( this, &SunionsMarshal::delayInput1 )))
              ->arm( Time::now() + Time::msecs( sleep ));

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Resume here after sending a tuple.
//
void  SunionsMarshal::delayInput1()
{
//..............................................................................


    // Release the previous event.
    //
    _eventInput1.reset();

    // Construct a new Input1 input event.
    //
    _eventInput1 = ptr<StreamEvent>( new StreamEvent( INPUT1 ));
    _eventInput1->_inject = true;

    // Return to the application code.
    //
    sentInput1();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  SunionsMarshal::connectInput2()
{
//..............................................................................


    // Starting to produce events on input stream.
    //
    if ( !_client->set_data_path( MAX_BUFFER, Util::get_host_address( INPUT2_NODE ), INPUT2_PORT ))
    {   FATAL << "Failed setting data path";
    }

    DEBUG << "Set data path";

    _eventInput2 = ptr<StreamEvent>( new StreamEvent( INPUT2 ));
    _eventInput2->_inject = true;

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  SunionsMarshal::batchInput2( Input2  *tuple )
{
//..............................................................................


    // Tuples are buffered in a string.
    //
    _eventInput2->insert_bin( string( (const char *)tuple,
                                  sizeof( Input2 )));

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  SunionsMarshal::sendInput2( uint32  sleep )
{
//..............................................................................


    // Transmit data to the node.
    Status  status = _client->fast_post_event( _eventInput2 );

    while ( !status )
    {   if ( status.as_string() == DataHandler::NO_SPACE )
        {   // Wait if no more space in buffer.
            // At this point the data was never put in the buffer.
            // It needs to be requeued unless we want to drop it.
            WARN << "We dropped a tuple.";
            Thread::sleep( Time::msecs( sleep ));

            // retry (make this conditional).
            status = _client->fast_post_event( _eventInput2 );
        }
        else
        {   ERROR << "Connection closed... stopping event stream";
            return;
        }
    }

    // The event loop is activated so that the queue can be processed.
    // The callback is enqueued with a timer.
    // We only callback with a timer because this is looping.
    // We also need a delayed callback so the queue can be processed.
    // If we just go to sleep the event loop will not be run.
    //
    ( new CallbackTimer( _client->get_loop(),
                         wrap( this, &SunionsMarshal::delayInput2 )))
              ->arm( Time::now() + Time::msecs( sleep ));

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Resume here after sending a tuple.
//
void  SunionsMarshal::delayInput2()
{
//..............................................................................


    // Release the previous event.
    //
    _eventInput2.reset();

    // Construct a new Input2 input event.
    //
    _eventInput2 = ptr<StreamEvent>( new StreamEvent( INPUT2 ));
    _eventInput2->_inject = true;

    // Return to the application code.
    //
    sentInput2();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  SunionsMarshal::connectInput3()
{
//..............................................................................


    // Starting to produce events on input stream.
    //
    if ( !_client->set_data_path( MAX_BUFFER, Util::get_host_address( INPUT3_NODE ), INPUT3_PORT ))
    {   FATAL << "Failed setting data path";
    }

    DEBUG << "Set data path";

    _eventInput3 = ptr<StreamEvent>( new StreamEvent( INPUT3 ));
    _eventInput3->_inject = true;

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  SunionsMarshal::batchInput3( Input3  *tuple )
{
//..............................................................................


    // Tuples are buffered in a string.
    //
    _eventInput3->insert_bin( string( (const char *)tuple,
                                  sizeof( Input3 )));

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  SunionsMarshal::sendInput3( uint32  sleep )
{
//..............................................................................


    // Transmit data to the node.
    Status  status = _client->fast_post_event( _eventInput3 );

    while ( !status )
    {   if ( status.as_string() == DataHandler::NO_SPACE )
        {   // Wait if no more space in buffer.
            // At this point the data was never put in the buffer.
            // It needs to be requeued unless we want to drop it.
            WARN << "We dropped a tuple.";
            Thread::sleep( Time::msecs( sleep ));

            // retry (make this conditional).
            status = _client->fast_post_event( _eventInput3 );
        }
        else
        {   ERROR << "Connection closed... stopping event stream";
            return;
        }
    }

    // The event loop is activated so that the queue can be processed.
    // The callback is enqueued with a timer.
    // We only callback with a timer because this is looping.
    // We also need a delayed callback so the queue can be processed.
    // If we just go to sleep the event loop will not be run.
    //
    ( new CallbackTimer( _client->get_loop(),
                         wrap( this, &SunionsMarshal::delayInput3 )))
              ->arm( Time::now() + Time::msecs( sleep ));

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Resume here after sending a tuple.
//
void  SunionsMarshal::delayInput3()
{
//..............................................................................


    // Release the previous event.
    //
    _eventInput3.reset();

    // Construct a new Input3 input event.
    //
    _eventInput3 = ptr<StreamEvent>( new StreamEvent( INPUT3 ));
    _eventInput3->_inject = true;

    // Return to the application code.
    //
    sentInput3();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Dispatch output on our fast datapath to a handler.
//
Status  SunionsMarshal::outputHandler( ptr<StreamEvent>  event )
{
//..............................................................................


    if ( event->_stream == Name(CONTROLOUTPUT1) )
    {   return SunionsMarshal::Controloutput1Handler(event);
    }

    if ( event->_stream == Name(CONTROLOUTPUT2) )
    {   return SunionsMarshal::Controloutput2Handler(event);
    }

    if ( event->_stream == Name(CONTROLOUTPUT3) )
    {   return SunionsMarshal::Controloutput3Handler(event);
    }

    if ( event->_stream == Name(CONTROLUNION1) )
    {   return SunionsMarshal::Controlunion1Handler(event);
    }

    if ( event->_stream == Name(CONTROLUNION2) )
    {   return SunionsMarshal::Controlunion2Handler(event);
    }

    if ( event->_stream == Name(CONTROLUNION3) )
    {   return SunionsMarshal::Controlunion3Handler(event);
    }

    if ( event->_stream == Name(SOUTPUT1) )
    {   return SunionsMarshal::Soutput1Handler(event);
    }

    if ( event->_stream == Name(SOUTPUT2) )
    {   return SunionsMarshal::Soutput2Handler(event);
    }

    if ( event->_stream == Name(SOUTPUT3) )
    {   return SunionsMarshal::Soutput3Handler(event);
    }

    return( string( "Unknown output stream" ) + to_string( event->_stream ));
}

////////////////////////////////////////////////////////////////////////////////
//
// Subscribing to receive output on a fast datapath.
//
void  SunionsMarshal::subscribeControloutput1()
{
//..............................................................................


    DEBUG << "Subscribing to receive output.";

    // Setup the subscription request.
    Status  stat = _client->set_data_handler(
                                InetAddress( Util::get_host_address( CONTROLOUTPUT1_NODE ), CONTROLOUTPUT1_PORT ),
                                wrap( &outputHandler ));


    DEBUG << "Done subscribing to output.";

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Receive output on a fast datapath.
//
Status  SunionsMarshal::Controloutput1Handler( ptr<StreamEvent>  event )
{
    uint32     offset = 0;
    int32      index;
//..............................................................................


    // For each tuple that was received,
    //
    for ( index = 0; index < event->_inserted_count; index++ )
    {   offset += HEADER_SIZE;
        receivedControloutput1( (EmptyTuple *)&event->_bin_tuples[ offset ]);
        offset += sizeof( EmptyTuple );
    }

    // Signal done with this batch.
    return( true );
}



////////////////////////////////////////////////////////////////////////////////
//
// Subscribing to receive output on a fast datapath.
//
void  SunionsMarshal::subscribeControloutput2()
{
//..............................................................................


    DEBUG << "Subscribing to receive output.";

    // Setup the subscription request.
    Status  stat = _client->set_data_handler(
                                InetAddress( Util::get_host_address( CONTROLOUTPUT2_NODE ), CONTROLOUTPUT2_PORT ),
                                wrap( &outputHandler ));


    DEBUG << "Done subscribing to output.";

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Receive output on a fast datapath.
//
Status  SunionsMarshal::Controloutput2Handler( ptr<StreamEvent>  event )
{
    uint32     offset = 0;
    int32      index;
//..............................................................................


    // For each tuple that was received,
    //
    for ( index = 0; index < event->_inserted_count; index++ )
    {   offset += HEADER_SIZE;
        receivedControloutput2( (EmptyTuple *)&event->_bin_tuples[ offset ]);
        offset += sizeof( EmptyTuple );
    }

    // Signal done with this batch.
    return( true );
}



////////////////////////////////////////////////////////////////////////////////
//
// Subscribing to receive output on a fast datapath.
//
void  SunionsMarshal::subscribeControloutput3()
{
//..............................................................................


    DEBUG << "Subscribing to receive output.";

    // Setup the subscription request.
    Status  stat = _client->set_data_handler(
                                InetAddress( Util::get_host_address( CONTROLOUTPUT3_NODE ), CONTROLOUTPUT3_PORT ),
                                wrap( &outputHandler ));


    DEBUG << "Done subscribing to output.";

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Receive output on a fast datapath.
//
Status  SunionsMarshal::Controloutput3Handler( ptr<StreamEvent>  event )
{
    uint32     offset = 0;
    int32      index;
//..............................................................................


    // For each tuple that was received,
    //
    for ( index = 0; index < event->_inserted_count; index++ )
    {   offset += HEADER_SIZE;
        receivedControloutput3( (EmptyTuple *)&event->_bin_tuples[ offset ]);
        offset += sizeof( EmptyTuple );
    }

    // Signal done with this batch.
    return( true );
}



////////////////////////////////////////////////////////////////////////////////
//
// Subscribing to receive output on a fast datapath.
//
void  SunionsMarshal::subscribeControlunion1()
{
//..............................................................................


    DEBUG << "Subscribing to receive output.";

    // Setup the subscription request.
    Status  stat = _client->set_data_handler(
                                InetAddress( Util::get_host_address( CONTROLUNION1_NODE ), CONTROLUNION1_PORT ),
                                wrap( &outputHandler ));


    DEBUG << "Done subscribing to output.";

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Receive output on a fast datapath.
//
Status  SunionsMarshal::Controlunion1Handler( ptr<StreamEvent>  event )
{
    uint32     offset = 0;
    int32      index;
//..............................................................................


    // For each tuple that was received,
    //
    for ( index = 0; index < event->_inserted_count; index++ )
    {   offset += HEADER_SIZE;
        receivedControlunion1( (EmptyTuple *)&event->_bin_tuples[ offset ]);
        offset += sizeof( EmptyTuple );
    }

    // Signal done with this batch.
    return( true );
}



////////////////////////////////////////////////////////////////////////////////
//
// Subscribing to receive output on a fast datapath.
//
void  SunionsMarshal::subscribeControlunion2()
{
//..............................................................................


    DEBUG << "Subscribing to receive output.";

    // Setup the subscription request.
    Status  stat = _client->set_data_handler(
                                InetAddress( Util::get_host_address( CONTROLUNION2_NODE ), CONTROLUNION2_PORT ),
                                wrap( &outputHandler ));


    DEBUG << "Done subscribing to output.";

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Receive output on a fast datapath.
//
Status  SunionsMarshal::Controlunion2Handler( ptr<StreamEvent>  event )
{
    uint32     offset = 0;
    int32      index;
//..............................................................................


    // For each tuple that was received,
    //
    for ( index = 0; index < event->_inserted_count; index++ )
    {   offset += HEADER_SIZE;
        receivedControlunion2( (EmptyTuple *)&event->_bin_tuples[ offset ]);
        offset += sizeof( EmptyTuple );
    }

    // Signal done with this batch.
    return( true );
}



////////////////////////////////////////////////////////////////////////////////
//
// Subscribing to receive output on a fast datapath.
//
void  SunionsMarshal::subscribeControlunion3()
{
//..............................................................................


    DEBUG << "Subscribing to receive output.";

    // Setup the subscription request.
    Status  stat = _client->set_data_handler(
                                InetAddress( Util::get_host_address( CONTROLUNION3_NODE ), CONTROLUNION3_PORT ),
                                wrap( &outputHandler ));


    DEBUG << "Done subscribing to output.";

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Receive output on a fast datapath.
//
Status  SunionsMarshal::Controlunion3Handler( ptr<StreamEvent>  event )
{
    uint32     offset = 0;
    int32      index;
//..............................................................................


    // For each tuple that was received,
    //
    for ( index = 0; index < event->_inserted_count; index++ )
    {   offset += HEADER_SIZE;
        receivedControlunion3( (EmptyTuple *)&event->_bin_tuples[ offset ]);
        offset += sizeof( EmptyTuple );
    }

    // Signal done with this batch.
    return( true );
}



////////////////////////////////////////////////////////////////////////////////
//
// Subscribing to receive output on a fast datapath.
//
void  SunionsMarshal::subscribeSoutput1()
{
//..............................................................................


    DEBUG << "Subscribing to receive output.";

    // Setup the subscription request.
    Status  stat = _client->set_data_handler(
                                InetAddress( Util::get_host_address( SOUTPUT1_NODE ), SOUTPUT1_PORT ),
                                wrap( &outputHandler ));


    DEBUG << "Done subscribing to output.";

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Receive output on a fast datapath.
//
Status  SunionsMarshal::Soutput1Handler( ptr<StreamEvent>  event )
{
    uint32     offset = 0;
    int32      index;
//..............................................................................


    // For each tuple that was received,
    //
    for ( index = 0; index < event->_inserted_count; index++ )
    {   
        //Tuple t = Tuple::factory((const char*)&event->_bin_tuples[ offset ]);
        //NOTICE << "  H1 ( " << t.get_tuple_type() << "," << t.get_tuple_stime() << ")";
        offset += HEADER_SIZE;
        receivedSoutput1( (TrivialTuple *)&event->_bin_tuples[ offset ]);
        offset += sizeof( TrivialTuple );
    }

    // Signal done with this batch.
    return( true );
}



////////////////////////////////////////////////////////////////////////////////
//
// Subscribing to receive output on a fast datapath.
//
void  SunionsMarshal::subscribeSoutput2()
{
//..............................................................................


    DEBUG << "Subscribing to receive output.";

    // Setup the subscription request.
    Status  stat = _client->set_data_handler(
                                InetAddress( Util::get_host_address( SOUTPUT2_NODE ), SOUTPUT2_PORT ),
                                wrap( &outputHandler ));


    DEBUG << "Done subscribing to output.";

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Receive output on a fast datapath.
//
Status  SunionsMarshal::Soutput2Handler( ptr<StreamEvent>  event )
{
    uint32     offset = 0;
    int32      index;
//..............................................................................


    // For each tuple that was received,
    //
    for ( index = 0; index < event->_inserted_count; index++ )
    {   
        //Tuple t = Tuple::factory((const char*)&event->_bin_tuples[ offset ]);
        //NOTICE << "  H2 ( " << t.get_tuple_type() << "," << t.get_tuple_stime() << ")";
        offset += HEADER_SIZE;
        receivedSoutput2( (TrivialTuple *)&event->_bin_tuples[ offset ]);
        offset += sizeof( TrivialTuple );
    }

    // Signal done with this batch.
    return( true );
}



////////////////////////////////////////////////////////////////////////////////
//
// Subscribing to receive output on a fast datapath.
//
void  SunionsMarshal::subscribeSoutput3()
{
//..............................................................................


    DEBUG << "Subscribing to receive output.";

    // Setup the subscription request.
    Status  stat = _client->set_data_handler(
                                InetAddress( Util::get_host_address( SOUTPUT3_NODE ), SOUTPUT3_PORT ),
                                wrap( &outputHandler ));


    DEBUG << "Done subscribing to output.";

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Receive output on a fast datapath.
//
Status  SunionsMarshal::Soutput3Handler( ptr<StreamEvent>  event )
{
    uint32     offset = 0;
    int32      index;
//..............................................................................


    // For each tuple that was received,
    //
    for ( index = 0; index < event->_inserted_count; index++ )
    {   
        Tuple t = Tuple::factory((const char*)&event->_bin_tuples[ offset ]);
        //NOTICE << "  H3 ( " << t.get_tuple_type() << "," << t.get_tuple_stime() << ")";
        offset += HEADER_SIZE;
        receivedSoutput3( (TrivialTuple *)&event->_bin_tuples[ offset ], t.get_tuple_type());
        offset += sizeof( TrivialTuple );
    }

    // Signal done with this batch.
    return( true );
}



////////////////////////////////////////////////////////////////////////////////
//
// Launch the Borealis front-end.
//
int32  SunionsMarshal::launchDiagram( string  xml,   // Path of an initial xml file.
                                      string  ip,    // Local medusa client ip.
                                      uint32  port   // Local medusa client port.
                                    )
{
    int32   status;
    string  command;
//..............................................................................


    INFO << "Connect with: " << xml;

    command = string() + "BigGiantHead -f " + xml;
    status  = system( command.c_str() );

    DEBUG << "Ran the Borealis front end:  " << status;

    return( status );
}

////////////////////////////  end  SunionsMarshal.cc  //////////////////////////
