#include "PassMarshal.h"

#include  "util.h"

using namespace Borealis;

#define  FRAME_WIDTH   320
#define  FRAME_HEIGHT    1
#define  FRAME_DEPTH     1

const uint32  SLEEP_TIME  = 100;           // Delay between injections.
const uint32  BATCH_SIZE  =   1;           // Number of input tuples per batch.
const uint32  BATCH_COUNT =   5;           // Number batches to send.

const Time  time0 = Time::now() - Time::msecs( 100 );

      PassMarshal   marshal;        // Client and I/O stream state.

      uint32  result = 0;
      uint32  total  = BATCH_COUNT * BATCH_SIZE;
      uint32  remain = BATCH_COUNT * BATCH_SIZE;

      uint8   expect = 0;
      uint8   value[FRAME_WIDTH];   //[FRAME_HEIGHT][FRAME_DEPTH];


////////////////////////////////////////////////////////////////////////////////
//
// Print the content of received tuples.
//
void  PassMarshal::receivedFrame_Out(frame  *tuple)
{
//..............................................................................


    expect += 1;

    NOTICE << "receivedFrame_Out:  expect=" << (uint32)expect
                               << " value=" << (uint32)*(tuple->array)
                               <<   " mid=" << (uint32)tuple->array[FRAME_WIDTH / 2];


    if (*(tuple->array) != expect)
    {   NOTICE << "receivedFrame_Out:  Failed:  Expected value is " << (uint32)expect;
        result = 2;
    }

    if ((tuple->array[FRAME_WIDTH / 2]) != expect)
    {   NOTICE << "receivedFrame_Out:  Failed:  Expected value is " << (uint32)expect;
        result = 2;
    }


    total--;

    if (total == 0)
    {   NOTICE << "receivedFrame_Out:  Terminating ...";
        marshal.terminateClient();
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Return here after sending a packet and a delay.
//
void  PassMarshal::sentFrame_In()
{
    int32        timestamp;
    Time         current_time;
//..............................................................................


    current_time = Time::now();

    timestamp = (int32)( current_time - time0 ).to_secs();
    if ( timestamp < 0 ) timestamp = 0;
    DEBUG  << "in sent at timestamp = " << timestamp << "  current_time = " << current_time;

    for (uint32  i = 0; i < BATCH_SIZE; i++)
    {   
        //value[0][0][0] += 1;
        value[0] += 1;
        value[FRAME_WIDTH / 2] += 1;

        // This has to be in the loop scope so the constructor is rerun.
        Frame_In  tuple;

        tuple._data.array = (uint8 *)&(value[0]);   //[0][0]);
        
        // DEBUG  << "time=" << tuple._data.time << "  value=" << tuple._data.value;
        batchFrame_In(&tuple);
    }

    // Send a batch of tuples and delay if not done.
    //
    remain--;

    if (remain)
    {   DEBUG << "call sendFrame_In...";
        sendFrame_In(SLEEP_TIME);
    }
    else
    {   DEBUG << "call sendFrame_In one last time.";
        sendFrame_In(0);
        //sendFrame_In(SLEEP_TIME);
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


    // DO not run the front-end, but still open a client,
    // subscribe to outputs and inputs.
    //
    //marshal.openInputOutput();
    result = marshal.open();

    if (result)
    {   WARN << "Could not deploy the network.";
        result = 1;
    }
    else
    {   DEBUG  << "time0 = " << time0;

        // Use a constant array each frame.
        //
        for (uint32  i = 0; i < FRAME_WIDTH / 2; i++)
        {   value[i] = i;
        }

        for (uint32  i = FRAME_WIDTH / 2; i < FRAME_WIDTH; i++)
        {   value[i] = i - (FRAME_WIDTH / 2);
        }

        // Send the first batch of tuples.  Queue up the next round with a delay.
        //
        marshal.sentFrame_In();

        DEBUG  << "run the client event loop...";

        // Run the client event loop.  Return only on an exception.
        marshal.runClient();

        NOTICE << "validate_pass:  Done";
    }

    return  result;
}
