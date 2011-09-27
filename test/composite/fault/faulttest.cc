#include "args.h"
#include "FaulttestMarshal.h"

using namespace Borealis;

const uint32  SLEEP_TIME = 20;           // Delay between injections.
const uint32  BATCH_SIZE = 1;            // Number of input tuples per batch.

const Time  time0 = Time::now() - Time::msecs( 100 );

const int MAX_VALUES = 20;
const int BOUNDARY_INTERVAL = 10;

// Testing failures from 5sec to 10 sec
// 50 = 1 sec
const int START_FAILURE = 250;
const int STOP_FAILURE = 500;


////////////////////////////////////////////////////////////////////////////////
//
// Print the content of received tuples.
//
// To see the contents of tuples *headers*, you need to modify the appropriate
// FaulttestMarshal::*Handler. Here's an example:
//
// #include "Tuple.h"
// // Add these lines to the appropriate handler function
// Tuple t = Tuple::factory((const char*)&event->_bin_tuples[ offset ]);        
// NOTICE << "  H ( " << t.get_tuple_type() << "," << t.get_tuple_stime() << ")";
// 
void  FaulttestMarshal::receivedControloutput1( EmptyTuple  *tuple ) { return; }
void  FaulttestMarshal::receivedControloutput2( EmptyTuple  *tuple ) { return; }
void  FaulttestMarshal::receivedControlunion1( EmptyTuple  *tuple ) { return; }
void  FaulttestMarshal::receivedControlunion2( EmptyTuple  *tuple ) { return; }
void  FaulttestMarshal::receivedControlunion3( EmptyTuple  *tuple ) { return; }

void  FaulttestMarshal::receivedSoutput1( AggregateTuple  *tuple )
{
    //NOTICE << "Result body (" << tuple->time << "," << tuple->max << ")";
    //return;
}

void  FaulttestMarshal::receivedSoutput2( JoinTuple  *tuple )
{

    NOTICE << "Result body (" << tuple->time1 << "," << tuple->time2 << "," 
           << tuple->join_attribute << ")";    
    return;
}

////////////////////////////////////////////////////////////////////////////////
//
// Return here after sending a bunch of tuples and a delay.
//
void  FaulttestMarshal::sentTrivial()
{
    static int total_tuples_sent = 0;
    int32        random_value;
    int32        timestamp;
    Time         current_time;    

    // We suspend boundary tuples to cause failure handling behavior
    static bool suspend_boundaries = false;
    static bool already_failed = false;

    current_time = Time::now();

    // Order on the time when tuple is created. Msec precision
    timestamp = (int32)( current_time - time0 ).to_msecs();    
    if ( timestamp < 0 ) timestamp = 0;

    // tuple_stime has exact creation time
    timeval stime = Time::now().to_timeval();

    for ( uint32  i = 0; i < BATCH_SIZE; i++ )
    {   

        total_tuples_sent++;

        random_value = rand() % MAX_VALUES;

        // This has to be in the loop scope so the constructor is rerun.
        Trivial  tuple;

        tuple._data.time = timestamp;
        tuple._data.join_attribute = random_value;
        
        tuple.tuple_stime.tv_sec = stime.tv_sec;
        tuple.tuple_stime.tv_usec = stime.tv_usec;

        // From time to time, make the tuple into a BOUNDARY
        if (( (total_tuples_sent % BOUNDARY_INTERVAL) == 0 ) && !suspend_boundaries)
        { tuple.tuple_type = TupleType::BOUNDARY;
        }

        if ( ( (total_tuples_sent % START_FAILURE) == 0 )  && !already_failed)
        { 
            suspend_boundaries = true;
            already_failed = true;
            WARN << "Starting FAILURE";
        }

        if ( (total_tuples_sent % STOP_FAILURE) == 0 )
        { 
            suspend_boundaries = false;
            WARN << "Stopping FAILURE";
        }

                
        batchTrivial( &tuple );
    }

    // Send a batch of tuples and delay.
    sendTrivial( SLEEP_TIME );

    return;
}


////////////////////////////////////////////////////////////////////////////////
//
// Return here after sending a bunch of tuples and a delay.
//
void  FaulttestMarshal::sentTrivial2()
{
    static int total_tuples_sent = 0;
    int32        random_value;
    int32        timestamp;
    Time         current_time;


    current_time = Time::now();

    timestamp = (int32)( current_time - time0 ).to_msecs();
    if ( timestamp < 0 ) timestamp = 0;

    // tuple_stime has exact creation time
    timeval stime = Time::now().to_timeval();
    
    for ( uint32  i = 0; i < BATCH_SIZE; i++ )
    {   
        total_tuples_sent++;

        random_value = rand() % MAX_VALUES;

        // This has to be in the loop scope so the constructor is rerun.
        Trivial2  tuple;

        tuple._data.time = timestamp;
        tuple._data.join_attribute = random_value;
        
        tuple.tuple_stime.tv_sec = stime.tv_sec;
        tuple.tuple_stime.tv_usec = stime.tv_usec;

        // From time to time, make the tuple into a BOUNDARY
        if ( (total_tuples_sent % BOUNDARY_INTERVAL) == 0 )
        { tuple.tuple_type = TupleType::BOUNDARY;
        } 
        
        batchTrivial2( &tuple );
    }

    // Send a batch of tuples and delay.
    sendTrivial2( SLEEP_TIME );

    return;
}

////////////////////////////////////////////////////////////////////////////////
//
int  main( int  argc, const char  *argv[] )
{
    string  ip   = "127.0.0.1";     // client host ip.
    int     port = 15000;           // client (and input) port.

    int32           status;
    FaulttestMarshal   marshal;        // Client and I/O stream state.
    //
    // Maximum size of buffer with data awaiting transmission to Borealis
    //..............................................................................


    stdArgs( argc, argv, ip, port );

    // Run the front-end, open a client, subscribe to outputs and inputs.
    status = marshal.open( ip, port );

    if ( status )
    {   
        WARN << "Could not deply the network.";
    }
    else
    {   
        DEBUG  << "time0 = " << time0;

        // Send the first batch of tuples.  Queue up the next round with a delay.
        marshal.sentTrivial();
        marshal.sentTrivial2();

        DEBUG  << "run the client event loop...";
        // Run the client event loop.  Return only on an exception.
        marshal.runClient();
    }

    return( status );
}
