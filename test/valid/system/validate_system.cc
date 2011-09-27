#include "SystemMarshal.h"

using namespace Borealis;

const uint32  SLEEP_TIME = 100;          // Delay between injections.
const uint32  BATCH_SIZE  = 20;          // Number of input tuples per batch.
const uint32  BATCH_COUNT = 3;           // Number batches to send.

const Time  time0 = Time::now() - Time::msecs( 100 );

      SystemMarshal   marshal;        // Client and I/O stream state.

      double  value  = 0.0;
      double  expect = 0.0;
      int32   result = 0;
      int32   total  = BATCH_COUNT * BATCH_SIZE;
      int32   remain = BATCH_COUNT;


////////////////////////////////////////////////////////////////////////////////
//
// Print the content of received tuples.
//
void  SystemMarshal::receivedResult(PacketTuple  *tuple)
{
//..............................................................................


    NOTICE << "receivedResult:  time="
           << tuple->time << " value=" << tuple->value;

    expect += 1.0;

    if (tuple->value != expect)
    {   NOTICE << "receivedResult:  Failed:  Expected value is " << expect;
        result = 2;
    }

    total--;

    if (total == 0)
    {   NOTICE << "receivedResult:  Terminating ...";
        marshal.terminateClient();
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Return here after sending a packet and a delay.
//
void  SystemMarshal::sentPacket()
{
    int32        timestamp;
    Time         current_time;
//..............................................................................


    current_time = Time::now();

    timestamp = (int32)( current_time - time0 ).to_secs();
    if ( timestamp < 0 ) timestamp = 0;
    //DEBUG  << "timestamp = " << timestamp << "  current_time = " << current_time;
    
    for (uint32  i = 0; i < BATCH_SIZE; i++)
    {   
        value += 1.0;

        // This has to be in the loop scope so the constructor is rerun.
        Packet  tuple;

        tuple._data.time  = timestamp;
        tuple._data.value = value;
        
        // DEBUG  << "time=" << tuple._data.time << "  value=" << tuple._data.value;
        batchPacket(&tuple);
    }

    // Send a batch of tuples and delay if not done.
    //
    //DEBUG << "call sendPacket...";

    remain--;

    if (remain)
    {   sendPacket(SLEEP_TIME);
    }
    else
    {   sendPacket(0);
    }

    return;
}




////////////////////////////////////////////////////////////////////////////////
//
int  main(int  argc, const char  *argv[])
{
//  Return:  1 - Could not deploy the network.
//           2 - An invalid tuple value was received.
//..............................................................................


    // Run the front-end, open a client, subscribe to outputs and inputs.
    result = marshal.open();

    if (result)
    {   WARN << "Could not deploy the network.";
        result = 1;
    }
    else
    {   DEBUG  << "time0 = " << time0;

        // Send the first batch of tuples.  Queue up the next round with a delay.
        marshal.sentPacket();

        DEBUG  << "run the client event loop...";
        // Run the client event loop.  Return only on an exception.
        marshal.runClient();
    }

    NOTICE << "validate_system:  Done";

    return(result);
}
