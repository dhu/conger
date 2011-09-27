#include "Two_ChainsMarshal.h"

using namespace Borealis;

const uint32  SLEEP_TIME = 100;          // Delay between injections.
const uint32  BATCH_SIZE  = 5;          // Number of input tuples per batch.
const uint32  BATCH_COUNT = 3;           // Number batches to send.

const Time  time0 = Time::now() - Time::msecs( 100 );

      Two_ChainsMarshal   marshal;        // Client and I/O stream state.

      int  value0  = 0;
      int  value1  = 0;
      int  expect0 = 0;
      int  expect1 = 0;
      int32   result = 0;
      int32   total0  = BATCH_COUNT * BATCH_SIZE;
      int32   total1  = BATCH_COUNT * BATCH_SIZE;
      int32   remain0 = BATCH_COUNT;
      int32   remain1 = BATCH_COUNT;


////////////////////////////////////////////////////////////////////////////////
//
// Print the content of received tuples.
//
void  Two_ChainsMarshal::receivedSo_B2_P1(intstuple  *tuple)
{
//..............................................................................


    NOTICE << "receivedSo_B2_P1:  " << " f1=" << tuple->f1;

    expect0 += 1;

    if (tuple->f1 != expect0)
    {   NOTICE << "receivedSo_B2_P1:  Failed:  Expected value is " << expect0;
        result = 2;
    }

    total0--;

    if (total0 == 0)
    {   NOTICE << "receivedSo_B2_P1:  NOT Terminating ...";
        //marshal.terminateClient();
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Return here after sending a packet and a delay.
//
void  Two_ChainsMarshal::sentSi_B1_P1()
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
        value0 += 1;

        // This has to be in the loop scope so the constructor is rerun.
        Si_B1_P1  tuple;

        tuple._data.f1 = value0;
        
        DEBUG  << "  value0=" << tuple._data.f1;
        batchSi_B1_P1(&tuple);
    }

    // Send a batch of tuples and delay if not done.
    //
    DEBUG << "call sendSi_B3_P1...";

    remain0--;

    if (remain0)
    {   sendSi_B1_P1(SLEEP_TIME);
    }
    else
    {   sendSi_B1_P1(0);
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Print the content of received tuples.
//
void  Two_ChainsMarshal::receivedSo_B4_P1(intstuple  *tuple)
{
//..............................................................................


    NOTICE << "receivedSo_B4_P1:  " << " f1=" << tuple->f1;

    expect1 += 1;

    if (tuple->f1 != expect1)
    {   NOTICE << "receivedSo_B4_P1:  Failed:  Expected value is " << expect1;
        result = 2;
    }

    total1--;

    if (total1 == 0)
    {   NOTICE << "receivedSo_B4_P1:  Terminating ...";
        marshal.terminateClient();
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Return here after sending a packet and a delay.
//
void  Two_ChainsMarshal::sentSi_B3_P1()
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
        value1 += 1;

        // This has to be in the loop scope so the constructor is rerun.
        Si_B3_P1  tuple;

        tuple._data.f1 = value1;
        
        DEBUG  << "  value1=" << tuple._data.f1;
        batchSi_B3_P1(&tuple);
    }

    // Send a batch of tuples and delay if not done.
    //
    DEBUG << "call sendSi_B3_P1...";

    remain1--;

    if (remain1)
    {   sendSi_B3_P1(SLEEP_TIME);
    }
    else
    {   sendSi_B3_P1(0);
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
        marshal.sentSi_B1_P1();
        marshal.sentSi_B3_P1();

        // Run the client event loop.  Return if terminated or an exception.
        DEBUG  << "run the client event loop...";
        marshal.runClient();
    }

    NOTICE << "validate_two_chains:  Done";

    return(result);
}
