#include "args.h"
#include "Cp2TestMarshal.h"

using namespace Borealis;

const uint32  SLEEP_TIME = 1000;         // Delay between injections.
const uint32  BATCH_SIZE = 1;            // Number of input tuples per batch.
const uint32  PROTOCOL_SIZE = 4;         // Number of elements in PROTOCOL.

const string  PROTOCOL[] = { string( "dns" ),  string( "smtp" ),
                             string( "http" ), string( "ssh" )
};


const Time  time0 = Time::now() - Time::msecs( 100 );


////////////////////////////////////////////////////////////////////////////////
//
// Print the content of received tuples.
//
void  Cp2TestMarshal::receivedResult( PacketTuple  *tuple )
{
    //..............................................................................


    // Start time
    NOTICE << "Tuple " << tuple->time;

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Return here after sending a packet and a delay.
//
void  Cp2TestMarshal::sentPacket()
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
    {   random_index = rand() % PROTOCOL_SIZE;

        // This has to be in the loop scope so the constructor is rerun.
        Packet  tuple;

        tuple._data.time = timestamp;

        setStringField( PROTOCOL[ random_index ], tuple._data.protocol, 4 );

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
    string  ip   = "127.0.0.1";     // client host ip.
    int     port = 15000;           // client (and input) port.

    int32            status;
    Cp2TestMarshal   marshal;        // Client and I/O stream state.
    //
    // Maximum size of buffer with data awaiting transmission to Borealis
    //..............................................................................


    stdArgs( argc, argv, ip, port );

    // Run the front-end, open a client, subscribe to outputs and inputs.
    status = marshal.open();

    if ( status )
    {   WARN << "Could not deply the network.";
    }
    else
    {   DEBUG  << "time0 = " << time0;

        // Send the first batch of tuples.  Queue up the next round with a delay.
        marshal.sentPacket();

        // Run the client event loop.  Return only on an exception.
        marshal.runClient();
    }

    return( status );
}
