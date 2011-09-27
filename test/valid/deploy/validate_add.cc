#include "AddMarshal.h"
#include "HeadClient.h"

using namespace Borealis;

// These must matsh the settings in:   validate_deploy.cc
//
const uint32  BATCH_SIZE  =  20;          // Number of input tuples per batch.
const uint32  BATCH_COUNT =  20;          // Number batches to send.

const Time  time0 = Time::now() - Time::msecs( 100 );

      AddMarshal   marshal;        // Client and I/O stream state.

      double  value  = 0.0;
      double  expect = 0.0;
      int32   result = 0;
      int32   total  = BATCH_COUNT * BATCH_SIZE;


////////////////////////////////////////////////////////////////////////////////
//
// Print the content of received tuples.
//
void  AddMarshal::receivedResult(PacketTuple  *tuple)
{
//..............................................................................


    NOTICE << "receivedResult:  time="
           << tuple->time << " value=" << tuple->value;

    if (expect == 0.0)
    {   expect = tuple->value;
    }
    else
    {   expect += 1.0;

        if (tuple->value != expect)
        {   NOTICE << "receivedResult:  Failed:  Expected value is " << expect;
            result = 2;
        }

        if (expect == (double)total)
        {   NOTICE << "receivedResult:  Terminating ...";
            marshal.terminateClient();
        }
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
    marshal.openInputOutput();

    // Extend the previous deployment.
    //
    xml = Util::full_path( "Add.xml" ); 

    if (xml == "")
    {   status = "Add.xml was not found";
    }
    else
    {   head = (HeadClient *)new HeadClient(InetAddress(
                                     Util::form_endpoint("localhost",
                                                         DEFAULT_HEAD_PORT)));
        rpc    = head->deploy_xml_file( xml );
        status = rpc.stat();

        DEBUG << "Added Add.xml to Borealis Head:  " << status;
    }

    if (status)
    {   xml = Util::full_path( "AddDeploy.xml" ); 

        if (xml == "")
        {   status = "AddDeploy.xml was not found";
        }
    }

    if (status)
    {   rpc    = head->deploy_xml_file(xml);
        status = rpc.stat();

        DEBUG << "Added AddDeploy.xml to Borealis front end:  " << status;
    }

    if (status)
    {   DEBUG  << "time0 = " << time0;
        DEBUG  << "run the client event loop...";

        // Run the client event loop.  Return only on an exception.
        marshal.runClient();
    }
    else
    {   WARN << "Could not deploy the network.";
        result = 1;
    }
 
    NOTICE << "validate_add:  Done";

    return(result);
}
