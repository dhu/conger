#include "args.h"
#include "MytestMarshal.h"

using namespace Borealis;

const uint32  SLEEP_TIME = 1;          // Delay between injections.
const uint32  BATCH_SIZE = 20;           // Number of input tuples per batch.
const uint32  PROTOCOL_SIZE = 4;         // Number of elements in PROTOCOL.

const string  PROTOCOL[] = { string( "dns" ),  string( "smtp" ),
                             string( "http" ), string( "ssh" )
                           };


const Time  time0 = Time::now() - Time::msecs( 100 );


////////////////////////////////////////////////////////////////////////////////
//
// Print the content of received tuples.
//
void  MytestMarshal::receivedAggregate( AggregateTuple  *tuple )
{
//..............................................................................


    NOTICE << "For time interval starting at "
           << tuple->time << " tuple count was " << tuple->count;
    
    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Return here after sending a packet and a delay.
//
void  MytestMarshal::sentPacket()
{
    int32        random_index;
    int32        timestamp;
    Time         current_time;
    //..............................................................................


    current_time = Time::now();

    timestamp = (int32)( current_time - time0 ).to_secs();
    if ( timestamp < 0 ) timestamp = 0;
    //DEBUG  << "timestamp = " << timestamp << "  current_time = " << current_time;
    
    for ( uint32  i = 0; i < BATCH_SIZE; i++ )
    {   
        random_index = rand() % PROTOCOL_SIZE;

        // This has to be in the loop scope so the constructor is rerun.
        Packet  tuple;

        tuple._data.time = timestamp;
        setStringField( PROTOCOL[ random_index ], tuple._data.protocol, 4 );    
        
        // DEBUG  << "time=" << tuple._data.time << "  proto=" << tuple._data.protocol;
        batchPacket( &tuple );
    }

    // Send a batch of tuples and delay.
    //
    //DEBUG << "call sendPacket...";
    sendPacket( SLEEP_TIME );

    return;
}




////////////////////////////////////////////////////////////////////////////////
//
int  main( int  argc, const char  *argv[] )
{
    int32           status;
    MytestMarshal   marshal;        // Client and I/O stream state.
//
// Maximum size of buffer with data awaiting transmission to Borealis
//..............................................................................


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
        marshal.sentPacket();

        DEBUG  << "run the client event loop...";
        // Run the client event loop.  Return only on an exception.
        marshal.runClient();
    }

    return( status );
}
