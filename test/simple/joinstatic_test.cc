#include "args.h"
#include "Joinstatic_TestMarshal.h"

///////////////////////////////  jointest.cc  //////////////////////////////////
//
//  Test a static join.  User defined boxed must be built.
//..............................................................................


using namespace Borealis;

const uint32  SLEEP_TIME = 100;          // Delay between injections.
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
void  Joinstatic_TestMarshal::receivedSuspicious( SuspiciousTuple  *tuple )
{
    //..............................................................................


    NOTICE  << "time = " << tuple->time
            << "  protocol = " << tuple->protocol
            <<    "  tsmin = " << tuple->tsmin
            <<    "  tsmax = " << tuple->tsmax
            <<  "  tempmin = " << tuple->tempmin
            <<  "  tempmax = " << tuple->tempmax;

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Return here after sending a packet and a delay.
//
void  Joinstatic_TestMarshal::sentPacket()
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
// Return here after sending a packet and a delay.
//
void  Joinstatic_TestMarshal::sentBound()
{
    //..............................................................................


    NOTICE << "Not currently used.";

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
int  main( int  argc, const char  *argv[] )
{
    string  ip   = "127.0.0.1";     // client host ip.
    int     port = 15000;           // client (and input) port.

    int32           status;
    Joinstatic_TestMarshal  marshal;        // Client and I/O stream state.
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

        DEBUG  << "run the client event loop...";
        // Run the client event loop.  Return only on an exception.
        marshal.runClient();
    }

    return( status );
}
