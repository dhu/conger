#include "DenseMarshal.h"

using namespace Borealis;

#define  FRAME_WIDTH   640
#define  FRAME_HEIGHT  480
#define  FRAME_DEPTH     3

const uint32  SLEEP_TIME  = 100;           // Delay between injections.
const uint32  BATCH_SIZE  =   1;           // Number of input tuples per batch.
const uint32  BATCH_COUNT =   3;           // Number batches to send.

const Time  time0 = Time::now() - Time::msecs( 100 );

      DenseMarshal   marshal;        // Client and I/O stream state.

      uint32  result = 0;
      uint32  total  = BATCH_COUNT * BATCH_SIZE;
      uint32  remain = BATCH_COUNT;

      uint8   expect = 0;
      uint8   value[FRAME_WIDTH][FRAME_HEIGHT][FRAME_DEPTH];


////////////////////////////////////////////////////////////////////////////////
//
// Print the content of received tuples.
//
void  DenseMarshal::receivedResult(PacketTuple  *tuple)
{
//..............................................................................


    NOTICE << "receivedResult:  time="
           << tuple->time << " value=" << (int)*(tuple->value);

    expect += 1;

    if (*(tuple->value) != expect)
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
void  DenseMarshal::sentPacket()
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
        value[0][0][0] += 1;

        // This has to be in the loop scope so the constructor is rerun.
        Packet  tuple;

        tuple._data.time  = timestamp;
        tuple._data.value = (uint8 *)&(value[0][0][0]);
        
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

        // Use a constant array each frame.
        for (uint32  i = 0; i < FRAME_WIDTH; i++)
        {   for (uint32  j = 0; j < FRAME_HEIGHT; j++)
            {   for (uint32  k = 0; k < FRAME_DEPTH; k++)
                {   value[i][j][k] = k;
        }   }   }

        // Send the first batch of tuples.  Queue up the next round with a delay.
        marshal.sentPacket();

        DEBUG  << "run the client event loop...";
        // Run the client event loop.  Return only on an exception.
        marshal.runClient();
    }

    NOTICE << "validate_array:  Done";

    return  result;
}
