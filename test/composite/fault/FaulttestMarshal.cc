#include  "FaulttestMarshal.h"
#include  "util.h"

#define FAULTTEST_XML     "faulttest.xml"
#define MAX_BUFFER      64000

#define AGGREGATE  "aggregate"
#define AGGREGATE_NODE  "127.0.0.1"
#define AGGREGATE_PORT  25000

#define AGGREGATEB  "aggregateb"
#define AGGREGATEB_NODE  "127.0.0.1"
#define AGGREGATEB_PORT  25000

#define CONTROLOUTPUT1  "controloutput1"
#define CONTROLOUTPUT1_NODE  "127.0.0.1"
#define CONTROLOUTPUT1_PORT  25000

#define CONTROLOUTPUT2  "controloutput2"
#define CONTROLOUTPUT2_NODE  "127.0.0.1"
#define CONTROLOUTPUT2_PORT  25000

#define CONTROLUNION1  "controlunion1"
#define CONTROLUNION1_NODE  "127.0.0.1"
#define CONTROLUNION1_PORT  25000

#define CONTROLUNION2  "controlunion2"
#define CONTROLUNION2_NODE  "127.0.0.1"
#define CONTROLUNION2_PORT  25000

#define CONTROLUNION3  "controlunion3"
#define CONTROLUNION3_NODE  "127.0.0.1"
#define CONTROLUNION3_PORT  25000

#define FILTER  "filter"
#define FILTER_NODE  "127.0.0.1"
#define FILTER_PORT  25000

#define JOIN  "join"
#define JOIN_NODE  "127.0.0.1"
#define JOIN_PORT  25000

#define MAP  "map"
#define MAP_NODE  "127.0.0.1"
#define MAP_PORT  25000

#define MAPB  "mapb"
#define MAPB_NODE  "127.0.0.1"
#define MAPB_PORT  25000

#define SOUTPUT1  "soutput1"
#define SOUTPUT1_NODE  "127.0.0.1"
#define SOUTPUT1_PORT  25000

#define SOUTPUT2  "soutput2"
#define SOUTPUT2_NODE  "127.0.0.1"
#define SOUTPUT2_PORT  25000

#define TRIVIAL  "trivial"
#define TRIVIAL_NODE  "127.0.0.1"
#define TRIVIAL_PORT  15000

#define TRIVIAL2  "trivial2"
#define TRIVIAL2_NODE  "127.0.0.1"
#define TRIVIAL2_PORT  15000

#define UNION1  "union1"
#define UNION1_NODE  "127.0.0.1"
#define UNION1_PORT  25000

#define UNION2  "union2"
#define UNION2_NODE  "127.0.0.1"
#define UNION2_PORT  25000


////////////////////////////////////////////////////////////////////////////////
//
// Generated Martialing code for the Faulttest program.
//..............................................................................


////////////////////////////////////////////////////////////////////////////////
//
//  Subscribe to input and output streams.
//
int32  FaulttestMarshal::open( string  ip,     // Local medusa client ip.
                               uint32  port    // Local medusa client port.
                             )
{

    // Open a client to interact with Borealis nodes.
    _client = new MedusaClient( InetAddress( Util::get_host_address( ip ), port ));

    // Subscribe to outputs.
    subscribeControloutput1();
    subscribeControloutput2();
    subscribeControlunion1();
    subscribeControlunion2();
    subscribeControlunion3();
    subscribeSoutput1();
    subscribeSoutput2();
    
    // Establish input data path connections.
    connectTrivial();
    connectTrivial2();

    //  Launch the front-end with the xml file.
    //  Creates a client that communicates with a borealis node at ip
    //  though the given port.
    //  Then it sends the network diagram as XML strings
    //  and runs start_query.
    //  It waits until the front-end terminates and the port is now free.
    //
    return launchDiagram( FAULTTEST_XML, ip, port );


}



////////////////////////////////////////////////////////////////////////////////
//
//  Connect to inputs, subscribe to outputs.
//
void  FaulttestMarshal::openInputOutput( string  ip,     // Local medusa client ip.
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
        subscribeControlunion1();
        subscribeControlunion2();
        subscribeControlunion3();
        subscribeSoutput1();
        subscribeSoutput2();

        // Establish input data path connections.
        connectTrivial();
        connectTrivial2();
    }
    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Activate all Borealis nodes.
void  FaulttestMarshal::runClient()
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
void  FaulttestMarshal::setStringField( string  value,
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
void  FaulttestMarshal::connectTrivial()
{
//..............................................................................


    // Starting to produce events on input stream.
    //
    if ( !_client->set_data_path( MAX_BUFFER, Util::get_host_address( TRIVIAL_NODE ), TRIVIAL_PORT ))
    {   FATAL << "Failed setting data path";
    }

    DEBUG << "Set data path";

    _eventTrivial = ptr<StreamEvent>( new StreamEvent( TRIVIAL ));
    _eventTrivial->_inject = true;

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  FaulttestMarshal::batchTrivial( Trivial  *tuple )
{
//..............................................................................


    // Tuples are buffered in a string.
    //
    _eventTrivial->insert_bin( string( (const char *)tuple,
                                  sizeof( Trivial )));

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  FaulttestMarshal::sendTrivial( uint32  sleep )
{
//..............................................................................


    // Transmit data to the node.
    Status  status = _client->fast_post_event( _eventTrivial );

    while ( !status )
    {   if ( status.as_string() == DataHandler::NO_SPACE )
        {   // Wait if no more space in buffer.
            // At this point the data was never put in the buffer.
            // It needs to be requeued unless we want to drop it.
            WARN << "We dropped a tuple.";
            Thread::sleep( Time::msecs( sleep ));

            // retry (make this conditional).
            status = _client->fast_post_event( _eventTrivial );
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
                         wrap( this, &FaulttestMarshal::delayTrivial )))
              ->arm( Time::now() + Time::msecs( sleep ));

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Resume here after sending a tuple.
//
void  FaulttestMarshal::delayTrivial()
{
//..............................................................................


    // Release the previous event.
    //
    _eventTrivial.reset();

    // Construct a new Trivial input event.
    //
    _eventTrivial = ptr<StreamEvent>( new StreamEvent( TRIVIAL ));
    _eventTrivial->_inject = true;

    // Return to the application code.
    //
    sentTrivial();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  FaulttestMarshal::connectTrivial2()
{
//..............................................................................


    // Starting to produce events on input stream.
    //
    if ( !_client->set_data_path( MAX_BUFFER, Util::get_host_address( TRIVIAL2_NODE ), TRIVIAL2_PORT ))
    {   FATAL << "Failed setting data path";
    }

    DEBUG << "Set data path";

    _eventTrivial2 = ptr<StreamEvent>( new StreamEvent( TRIVIAL2 ));
    _eventTrivial2->_inject = true;

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  FaulttestMarshal::batchTrivial2( Trivial2  *tuple )
{
//..............................................................................


    // Tuples are buffered in a string.
    //
    _eventTrivial2->insert_bin( string( (const char *)tuple,
                                  sizeof( Trivial2 )));

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  FaulttestMarshal::sendTrivial2( uint32  sleep )
{
//..............................................................................


    // Transmit data to the node.
    Status  status = _client->fast_post_event( _eventTrivial2 );

    while ( !status )
    {   if ( status.as_string() == DataHandler::NO_SPACE )
        {   // Wait if no more space in buffer.
            // At this point the data was never put in the buffer.
            // It needs to be requeued unless we want to drop it.
            WARN << "We dropped a tuple.";
            Thread::sleep( Time::msecs( sleep ));

            // retry (make this conditional).
            status = _client->fast_post_event( _eventTrivial2 );
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
                         wrap( this, &FaulttestMarshal::delayTrivial2 )))
              ->arm( Time::now() + Time::msecs( sleep ));

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Resume here after sending a tuple.
//
void  FaulttestMarshal::delayTrivial2()
{
//..............................................................................


    // Release the previous event.
    //
    _eventTrivial2.reset();

    // Construct a new Trivial2 input event.
    //
    _eventTrivial2 = ptr<StreamEvent>( new StreamEvent( TRIVIAL2 ));
    _eventTrivial2->_inject = true;

    // Return to the application code.
    //
    sentTrivial2();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Dispatch output on our fast datapath to a handler.
//
Status  FaulttestMarshal::outputHandler( ptr<StreamEvent>  event )
{
//..............................................................................


    if ( event->_stream == Name(CONTROLOUTPUT1) )
    {   return FaulttestMarshal::Controloutput1Handler(event);
    }

    if ( event->_stream == Name(CONTROLOUTPUT2) )
    {   return FaulttestMarshal::Controloutput2Handler(event);
    }

    if ( event->_stream == Name(CONTROLUNION1) )
    {   return FaulttestMarshal::Controlunion1Handler(event);
    }

    if ( event->_stream == Name(CONTROLUNION2) )
    {   return FaulttestMarshal::Controlunion2Handler(event);
    }

    if ( event->_stream == Name(CONTROLUNION3) )
    {   return FaulttestMarshal::Controlunion3Handler(event);
    }

    if ( event->_stream == Name(SOUTPUT1) )
    {   return FaulttestMarshal::Soutput1Handler(event);
    }

    if ( event->_stream == Name(SOUTPUT2) )
    {   return FaulttestMarshal::Soutput2Handler(event);
    }

    return( string( "Unknown output stream" ) + to_string( event->_stream ));
}

////////////////////////////////////////////////////////////////////////////////
//
// Subscribing to receive output on a fast datapath.
//
void  FaulttestMarshal::subscribeControloutput1()
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
Status  FaulttestMarshal::Controloutput1Handler( ptr<StreamEvent>  event )
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
void  FaulttestMarshal::subscribeControloutput2()
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
Status  FaulttestMarshal::Controloutput2Handler( ptr<StreamEvent>  event )
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
void  FaulttestMarshal::subscribeControlunion1()
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
Status  FaulttestMarshal::Controlunion1Handler( ptr<StreamEvent>  event )
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
void  FaulttestMarshal::subscribeControlunion2()
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
Status  FaulttestMarshal::Controlunion2Handler( ptr<StreamEvent>  event )
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
void  FaulttestMarshal::subscribeControlunion3()
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
Status  FaulttestMarshal::Controlunion3Handler( ptr<StreamEvent>  event )
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
void  FaulttestMarshal::subscribeSoutput1()
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
Status  FaulttestMarshal::Soutput1Handler( ptr<StreamEvent>  event )
{
    uint32     offset = 0;
    int32      index;
//..............................................................................


    // For each tuple that was received,
    //
    for ( index = 0; index < event->_inserted_count; index++ )
    {   
        // Uncomment this if you want to see the tuples that arrive on the soutput1 stream
        //Tuple t = Tuple::factory((const char*)&event->_bin_tuples[ offset ]);
        //NOTICE << "  H1 ( " << t.get_tuple_type() << "," << t.get_tuple_stime() << ")";
        offset += HEADER_SIZE;
        receivedSoutput1( (AggregateTuple *)&event->_bin_tuples[ offset ]);
        offset += sizeof( AggregateTuple );
    }

    // Signal done with this batch.
    return( true );
}



////////////////////////////////////////////////////////////////////////////////
//
// Subscribing to receive output on a fast datapath.
//
void  FaulttestMarshal::subscribeSoutput2()
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
Status  FaulttestMarshal::Soutput2Handler( ptr<StreamEvent>  event )
{
    uint32     offset = 0;
    int32      index;
//..............................................................................


    // For each tuple that was received,
    //
    for ( index = 0; index < event->_inserted_count; index++ )
    {  
        Tuple t = Tuple::factory((const char*)&event->_bin_tuples[ offset ]);
        NOTICE << "  H2 ( " << t.get_tuple_type() << "," << t.get_tuple_stime() << ")";
        offset += HEADER_SIZE;
        receivedSoutput2( (JoinTuple *)&event->_bin_tuples[ offset ]);
        offset += sizeof( JoinTuple );
    }

    // Signal done with this batch.
    return( true );
}



////////////////////////////////////////////////////////////////////////////////
//
// Launch the Borealis front-end.
//
int32  FaulttestMarshal::launchDiagram( string  xml,   // Path of an initial xml file.
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

////////////////////////////  end  FaulttestMarshal.cc  //////////////////////////
