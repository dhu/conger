#include "DeployMarshal.h"
#include "HeadClient.h"

using namespace Borealis;

const uint32  SLEEP_TIME  = 400;          // Delay between injections.
const uint32  BATCH_SIZE  =  20;          // Number of input tuples per batch.
const uint32  BATCH_COUNT =  20;          // Number batches to send.

const Time  time0 = Time::now() - Time::msecs( 100 );

      DeployMarshal   marshal;        // Client and I/O stream state.

      double  value  = 0.0;
      double  expect = 0.0;

      int32   result = 0;
      int32   total  = BATCH_COUNT * BATCH_SIZE;
      int32   remain = BATCH_COUNT;


////////////////////////////////////////////////////////////////////////////////
//
//  valid_deploy is launched and deploys the first query.
//
//  After a wait validate_add is launched and deploys the additional query.
//  validate_add receives tuples m through the end.
//
//  After all tuples pass through valid_add it terminates.
//  After all tuples pass through valid_deploy it terminates.
//  
//..............................................................................



////////////////////////////////////////////////////////////////////////////////
//
// Print the content of received tuples.
//
void  DeployMarshal::receivedResult(PacketTuple  *tuple)
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
void  DeployMarshal::sentPacket()
{
    int32        timestamp;
    Time         current_time;
//..............................................................................


    current_time = Time::now();

    timestamp = (int32)( current_time - time0 ).to_secs();
    if ( timestamp < 0 ) timestamp = 0;
    DEBUG  << "timestamp = " << timestamp << "  current_time = " << current_time;
    
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
    DEBUG << "call sendPacket...";

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
    HeadClient  *head;
    int32        result = 0;
    string       xml;
    Status       status = true;
    RPC<void>    rpc;
//
//  Return:  1 - Could not deploy the network.
//           2 - An invalid tuple value was received.
//..............................................................................


    // Run the front-end, open a client, subscribe to outputs and inputs.
    //result = marshal.open();
    marshal.openInputOutput();

    xml = Util::full_path("Deploy.xml");

    if (xml == "")
    {   status = "Deploy.xml was not found";
    }
    else
    {   head = (HeadClient *)new HeadClient(InetAddress(
                                     Util::form_endpoint("localhost",
                                                         DEFAULT_HEAD_PORT)));

        rpc = head->deploy_xml_file(xml);
        status = rpc.stat();

        DEBUG << "Added Deploy.xml to Borealis front end:  " << status;
    }

    if (status)
    {   xml = Util::full_path("DeployDeploy.xml");

        if (xml == "")
        {   status = "DeployDeploy.xml was not found";
        }
    }

    if (status)
    {   rpc    = head->deploy_xml_file(xml);
        status = rpc.stat();

        DEBUG << "Added DeployDeploy.xml to Borealis front end:  " << status;
    }

    if (status)
    {   DEBUG  << "time0 = " << time0;

        // Send the first batch of tuples.  Queue up the next round with a delay.
        marshal.sentPacket();

        DEBUG  << "run the client event loop...";
        // Run the client event loop.  Return only on an exception.
        marshal.runClient();
    }
    else
    {   WARN << "Could not deploy the network.";
        result = 1;
    }
 
    NOTICE << "validate_deploy:  Done";
    NOTICE << "result = " << result;

    return(result);
}
