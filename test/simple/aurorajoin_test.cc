#include "args.h"
#include "Aurorajoin_TestMarshal.h"

using namespace Borealis;

const uint32  SLEEP_TIME = 1000;        // Delay between injections.
const uint32  BATCH_SIZE = 1;          // Number of input tuples per batch.

const Time  time0 = Time::now() - Time::msecs( 100 );

////////////////////////////////////////////////////////////////////////////////
//
// Print the content of received tuples.
//
void  Aurorajoin_TestMarshal::receivedJoinoutput( JoinTuple  *tuple )
{
    NOTICE << "received output: "
           << tuple->time1 << " " << tuple->value1 << " "
           << tuple->time2 << " " << tuple->value2;

    return;
}

////////////////////////////////////////////////////////////////////////////////
//
// Return here after sending a packet and a delay.
//
void  Aurorajoin_TestMarshal::sentInput1()
{
    int32        timestamp;
    Time         current_time;

    current_time = Time::now();

    timestamp = (int32)( current_time - time0 ).to_secs();
    if ( timestamp < 0 ) timestamp = 0;

    for ( uint32  i = 0; i < BATCH_SIZE; i++ )
    {
        // This has to be in the loop scope so the constructor is rerun.
        Input1  tuple;

        tuple._data.time1 = timestamp;
        tuple._data.value1 = rand() % 4;

        INFO  << "time1=" << tuple._data.time1
	      << "  value1=" << tuple._data.value1;
        batchInput1( &tuple );
    }

    // Send a batch of tuples and delay.
    //
    DEBUG << "call sendInput1...";
    sendInput1( SLEEP_TIME );

    return;
}

////////////////////////////////////////////////////////////////////////////////
//
// Return here after sending a packet and a delay.
//
void  Aurorajoin_TestMarshal::sentInput2()
{
    int32        timestamp;
    Time         current_time;

    current_time = Time::now();

    timestamp = (int32)( current_time - time0 ).to_secs();
    if ( timestamp < 0 ) timestamp = 0;

    for ( uint32  i = 0; i < BATCH_SIZE; i++ )
    {
        // This has to be in the loop scope so the constructor is rerun.
        Input2  tuple;

        tuple._data.time2 = timestamp;
        tuple._data.value2 = rand() % 4;

        INFO  << "time2=" << tuple._data.time2
	      << "  value2=" << tuple._data.value2;
        batchInput2( &tuple );
    }

    // Send a batch of tuples and delay.
    //
    DEBUG << "call sendInput2...";
    sendInput2( SLEEP_TIME );

    return;
}

////////////////////////////////////////////////////////////////////////////////
//
int  main( int  argc, const char  *argv[] )
{
    string  ip   = "127.0.0.1";     // client host ip.
    int     port = 15000;           // client (and input) port.

    int32              status;
    Aurorajoin_TestMarshal   marshal;        // Client and I/O stream state.

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

        // Send the first batch of tuples.
        // Queue up the next round with a delay.
        DEBUG  << "marshal.sentInput1()";
        marshal.sentInput1();
        DEBUG  << "marshal.sentInput2()";
        marshal.sentInput2();

        DEBUG  << "run the client event loop...";
        // Run the client event loop.  Return only on an exception.
        marshal.runClient();
    }

    return( status );
}
