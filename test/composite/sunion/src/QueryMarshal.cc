#include  "QueryMarshal.h"
#include  "util.h"
#include  "Tuple.h"

#define MAX_BUFFER      64000

#define INPUT1  "input1"
#define INPUT2  "input2"
#define INPUT3  "input3"

#define OUTPUT  "output"
#define OUTPUT_NODE  "127.0.0.1"
#define OUTPUT_PORT  25000


// Results from experiments are stored in these two static fields
int QueryMarshal::_nb_tentative = 0;
Timestamp QueryMarshal::_max_proc_time = Timestamp();
bool QueryMarshal::_replicating = false;
Timestamp QueryMarshal::_recovery_start_time = Timestamp();
Timestamp QueryMarshal::_recovery_stop_time = Timestamp();


////////////////////////////////////////////////////////////////////////////////
//
// Generated BUT Modified Martialing code for the Query program.
//..............................................................................


////////////////////////////////////////////////////////////////////////////////
QueryMarshal::QueryMarshal(string  ip,     // Local medusa client ip.
                           uint32  port,   // Local medusa client port.
                           SUnionTestParams params
                           )
    : _params(params)
{

    // Open a client to interact with Borealis nodes.
    _client = new MedusaClient( InetAddress( Util::get_host_address( ip ), port ));

}


////////////////////////////////////////////////////////////////////////////////
void  QueryMarshal::openInput1( string  ip,     // Local medusa client ip.
                                uint32  port    // Local medusa client port.
                                )
{
    if ( !_client->set_data_path( MAX_BUFFER, Util::get_host_address( ip ), port ))
    {   FATAL << "Failed setting data path";
    }
    _eventInput1 = ptr<StreamEvent>( new StreamEvent( INPUT1 ));
    _eventInput1->_inject = true;
    DEBUG << "Set data path";
}


////////////////////////////////////////////////////////////////////////////////
void  QueryMarshal::openInput2( string  ip,     // Local medusa client ip.
                                uint32  port    // Local medusa client port.
                              )
{

    if ( !_client->set_data_path( MAX_BUFFER, Util::get_host_address( ip ), port ))
    {   FATAL << "Failed setting data path";
    }
    _eventInput2 = ptr<StreamEvent>( new StreamEvent( INPUT2 ));
    _eventInput2->_inject = true;
    DEBUG << "Set data path 2";

    return;
}

////////////////////////////////////////////////////////////////////////////////
void  QueryMarshal::openInput3( string  ip,     // Local medusa client ip.
                                uint32  port    // Local medusa client port.
                              )
{

    if ( !_client->set_data_path( MAX_BUFFER, Util::get_host_address( ip ), port ))
    {   FATAL << "Failed setting data path";
    }
    _eventInput3 = ptr<StreamEvent>( new StreamEvent( INPUT2 ));
    _eventInput3->_inject = true;
    DEBUG << "Set data path 2";

    return;
}

////////////////////////////////////////////////////////////////////////////////
//
//  Activate all Borealis nodes.
void  QueryMarshal::runClient()
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
void  QueryMarshal::setStringField( string  value,
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
void  QueryMarshal::batchInput1( Input1  *tuple )
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
void  QueryMarshal::sendInput1( uint32  sleep, Timestamp start, int total_tuples_sent)
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
                         wrap( this, &QueryMarshal::delayInput1, start, total_tuples_sent )))
              ->arm( Time::now() + Time::msecs( sleep ));

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Resume here after sending a tuple.
//
void  QueryMarshal::delayInput1(Timestamp start, int total_tuples_sent)
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
    sentInput1(start,total_tuples_sent);

    return;
}


////////////////////////////////////////////////////////////////////////////////
//
void  QueryMarshal::batchInput2( Input2  *tuple )
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
void  QueryMarshal::sendInput2( uint32  sleep, Timestamp start, int total_tuples_sent)
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
                         wrap( this, &QueryMarshal::delayInput2, start, total_tuples_sent )))
              ->arm( Time::now() + Time::msecs( sleep ));

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Resume here after sending a tuple.
//
void  QueryMarshal::delayInput2(Timestamp start, int total_tuples_sent)
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
    sentInput2(start, total_tuples_sent);

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  QueryMarshal::batchInput3( Input3  *tuple )
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
void  QueryMarshal::sendInput3( uint32  sleep, Timestamp start, int total_tuples_sent)
{
//..............................................................................


    // Transmit data to the node.
    Status  status = _client->fast_post_event( _eventInput3 );

    while ( !status )
    {   if ( status.as_string() == DataHandler::NO_SPACE )
        {   // Wait if no more space in buffer.
            // At this point the data was never put in the buffer.
            // It needs to be requeued unless we want to drop it.
            WARN << "We're going too fast a tuple.";
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
                         wrap( this, &QueryMarshal::delayInput3, start, total_tuples_sent )))
              ->arm( Time::now() + Time::msecs( sleep ));

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Resume here after sending a tuple.
//
void  QueryMarshal::delayInput3(Timestamp start, int total_tuples_sent)
{
//..............................................................................


    // Release the previous event.
    //
    _eventInput3.reset();

    // Construct a new Input2 input event.
    //
    _eventInput3 = ptr<StreamEvent>( new StreamEvent( INPUT3 ));
    _eventInput3->_inject = true;

    // Return to the application code.
    //
    sentInput3(start, total_tuples_sent);

    return;
}





////////////////////////////////////////////////////////////////////////////////
//
void  QueryMarshal::sendInput1_failure( uint32  sleep, Timestamp start, int total_tuples_sent)
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
                         wrap( this, &QueryMarshal::delayInput1_failure, start, total_tuples_sent )))
              ->arm( Time::now() + Time::msecs( sleep ));

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Resume here after sending a tuple.
//
void  QueryMarshal::delayInput1_failure(Timestamp start, int total_tuples_sent)
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
    sentInput1_failure(start,total_tuples_sent);

    return;
}




////////////////////////////////////////////////////////////////////////////////
//
// Subscribing to receive output on a fast datapath.
//
void  QueryMarshal::deployDiagram(string filename)
{
//..............................................................................


    DEBUG << "Subscribing to receive output.";

    // First make sure to listen for incoming connections before subscribing
    Status  stat = _client->set_data_handler(
                                             InetAddress( Util::get_host_address( OUTPUT_NODE ), OUTPUT_PORT  ),
                                             wrap( &outputHandler ));

    // Talk to the head to set-up the diagram
    HeadClient headclient( InetAddress( HEAD_NODE,DEFAULT_HEAD_PORT ));
    RPC<void>  rpc;
    rpc = headclient.deploy_xml_file( filename );
    if ( rpc.valid() )
    {   NOTICE << "Deployment succeeded";
    }
    else
    {   FATAL << "Deployment failed:  " << rpc.stat();
    }   

    return;
}


////////////////////////////////////////////////////////////////////////////////
//
// Dispatch output on our fast datapath to a handler.
//
Status  QueryMarshal::outputHandler( ptr<StreamEvent>  event )
{
//..............................................................................

    if ( event->_stream == Name(OUTPUT) )
    {   return QueryMarshal::OutputHandler(event);
    }

    return( string( "Unknown output stream" ) + to_string( event->_stream ));
}


////////////////////////////////////////////////////////////////////////////////
//
// Receive output on a fast datapath.
//
Status  QueryMarshal::OutputHandler( ptr<StreamEvent>  event )
{
    uint32     offset = 0;
    int32      index;

    Timestamp current_time = Timestamp::now();
    static bool time_insertions = true;
    static bool debug_compute_times = true;

    static int counter = 0;

    // For each tuple that was received,
    //
    for ( index = 0; index < event->_inserted_count; index++ )
    {   

        Tuple t = Tuple::factory((const char*)&event->_bin_tuples[ offset ]);

        if ( t.get_tuple_type() == TupleType::TENTATIVE)
        { _nb_tentative++;
        }

        if ( ( t.get_tuple_type() == TupleType::UNDO) && _replicating )
        { 
            WARN << "Suspending computing proc for insertions";
            time_insertions = false;
            _recovery_start_time = Timestamp::now();

            // For debugging purposes only
            debug_compute_times = false;
        }

        if ( t.get_tuple_type() == TupleType::RECONCILIATION_DONE)
        { 
            time_insertions = true;
            _recovery_stop_time = Timestamp::now();
        }


        if ( ( t.get_tuple_type() == TupleType::TENTATIVE) || 
             ( (t.get_tuple_type() == TupleType::INSERTION) && (time_insertions) ) )
        {
            Timestamp proc_time = current_time - t.get_tuple_stime();
            // DEBUG: Time tuples after the REC_DONE
            if ( time_insertions && !debug_compute_times && 
                 ( (counter < 10) || ( proc_time.to_msecs() > 1000 ) ) ) 
            {
                counter++;
                WARN << "Proc time is " << proc_time
                     << " for tuple: ( " << t.get_tuple_type() << "," << t.get_tuple_stime() << ")";
            }

            if ( ( proc_time > _max_proc_time ) &&  ( debug_compute_times ) )
            {
                _max_proc_time = proc_time;
                WARN << "Updated max proc time to " << _max_proc_time
                     << " for tuple: ( " << t.get_tuple_type() << "," << t.get_tuple_stime() << ")";
            }
        }

        if ( ( t.get_tuple_type() == TupleType::UNDO) || ( t.get_tuple_type() == TupleType::RECONCILIATION_DONE) )
        { NOTICE << " ( " << t.get_tuple_type() << "," << t.get_tuple_stime() << ") and proc time is " << current_time - t.get_tuple_stime();
        }

        offset += HEADER_SIZE;
        //receivedOutput( (TrivialTuple *)&event->_bin_tuples[ offset ]);
        offset += sizeof( TrivialTuple );
    }


    // Signal done with this batch.
    return( true );


}


////////////////////////////  end  QueryMarshal.cc  //////////////////////////
