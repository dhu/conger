#include "args.h"
#include "Tuple_Stime_TestMarshal.h"

using namespace Borealis;

const uint32  SLEEP_TIME = 20;           // Delay between injections.
const uint32  BATCH_SIZE = 1;            // Number of input tuples per batch.

const Time  time0 = Time::now() - Time::msecs( 100 );

const int MAX_VALUES = 10;
const int BOUNDARY_INTERVAL = 10;

// To test tentative tuples. Don't use tentative tuples with SUnion
// const int TENTATIVE_INTERVAL = 18; 
// const int TENTATIVE_INTERVAL = 180;  go 10x less often when testing aggregate

////////////////////////////////////////////////////////////////////////////////
//
// Print the content of received tuples.
//
// Need to put the following patch inside Tuple_Stime_TestMarshal::FiltermapHandler
// Modified by Magda
// NOTICE << "Header is " << *((TupleHeader*)&event->_bin_tuples[ offset ]);
// End modification
void  Tuple_Stime_TestMarshal::receivedControloutput( EmptyTuple  *tuple )
{
    return;
}

void  Tuple_Stime_TestMarshal::receivedAggregate( AggregateTuple  *tuple )
{
    NOTICE << "Aggregate body (" << tuple->time << "," << tuple->count << ")";    
    return;
}

void  Tuple_Stime_TestMarshal::receivedUnion( TrivialTuple  *tuple )
{

    //NOTICE << "Union body (" << tuple->time << "," << tuple->join_attribute << ")";    
    return;
}

void  Tuple_Stime_TestMarshal::receivedFiltermap( TrivialTuple  *tuple )
{

    //NOTICE << "Filter map body (" << tuple->time << "," << tuple->join_attribute << ")";    
    return;
}

void  Tuple_Stime_TestMarshal::receivedJoin( JoinTuple  *tuple )
{

    //NOTICE << "Join body (" << tuple->time1 << "," << tuple->time2 << "," 
    //      << tuple->join_attribute << ")";    
    return;
}


////////////////////////////////////////////////////////////////////////////////
//
// Return here after sending a bunch of tuples and a delay.
//
void  Tuple_Stime_TestMarshal::sentTrivial()
{
    static int total_tuples_sent = 0;
    int32        random_value;
    int32        timestamp;
    Time         current_time;    

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

        NOTICE << "Producing tuple " << tuple._data.time << "," << tuple._data.join_attribute;
        // From time to time, make the tuple into a BOUNDARY
        if ( (total_tuples_sent % BOUNDARY_INTERVAL) == 0 )
        { tuple.tuple_type = TupleType::BOUNDARY;
        }
        
        //if ( (total_tuples_sent % TENTATIVE_INTERVAL) == 0 )
        //{ tuple.tuple_type = TupleType::TENTATIVE;
        //}
        
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
void  Tuple_Stime_TestMarshal::sentTrivial2()
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
        
        //if ( (total_tuples_sent % TENTATIVE_INTERVAL) == 0 )
        //{ tuple.tuple_type = TupleType::TENTATIVE;
        //}       

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
    Tuple_Stime_TestMarshal   marshal;        // Client and I/O stream state.
    //
    // Maximum size of buffer with data awaiting transmission to Borealis
    //..............................................................................


    stdArgs( argc, argv, ip, port );

    // Run the front-end, open a client, subscribe to outputs and inputs.
    status = marshal.open();

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
