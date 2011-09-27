#include <iostream>
#include <fstream>

#include "Fail_DemoMarshal.h"

using namespace Borealis;

const uint32 SLEEP_TIME_BEFORE_TUPLES = 10;          // initial delay

const uint32 SLEEP_TIME_BETWEEN_TUPLES = 100;          // Delay between injections.

fail_demo_marshal marshal;

string ip   = "127.0.0.1";     // client host ip.

int port = 32000;           // client (and input) port.

Fail_DemoMarshal::input tuple;

string crash_ip   = "127.0.0.1";     // ip address of the node to crash

int crash_port = 32000;           // port of the node to crash

string visualizer_ip   = "127.0.0.1";     // ip address of the visualizer

int visualizer_port = 50000;           // port of the visulaizer

string command = "xterm -T borealis2@127.0.0.1:32000 -geometry 44x2+280+30 -e sh -l -c \"LD_LIBRARY_PATH=$LD_LIBRARY_PATH ../../src/src/borealis -d 127.0.0.1:32000;sleep 10\" &";

string query2 =
"<query  name=\"borealis://brown.cs.edu/ha_demo/query2\">"
"<box name=\"borealis://brown.cs.edu/ha_demo/union2\" type=\"union\">"
" <input  port=\"1\" stream=\"borealis://brown.cs.edu/ha_demo/s1\"/>"
" <output  port=\"1\" stream=\"borealis://brown.cs.edu/ha_demo/output1\"/>"
"</box>"
"</query>";

#define QUERY2_NAME "borealis://brown.cs.edu/ha_demo/query2"

#define OUTPUT1 "borealis://brown.cs.edu/ha_demo/output1"

////////////////////////////////////////////////////////////////////////////////
//
// Print the content of received tuples.
//
void Fail_DemoMarshal::received_output_1(Schema *tuple)
{
//    NOTICE << "output 1: (sequence number = " << tuple->seq_no << ", " << tuple->timestamp << ")";
}

void Fail_DemoMarshal::received_output_2(Schema *tuple)
{
//    NOTICE << "output 1: (sequence number = " << tuple->seq_no << ", " << tuple->timestamp << ")";
}

void send_tuple()
{
  tuple._data.seq_no++;
  tuple._data.timestamp = (Time::now().to_msecs());
  marshal.batch_input(&tuple);
  marshal.send_input(SLEEP_TIME_BETWEEN_TUPLES+(tuple._data.seq_no % 100));
  if (tuple._data.seq_no == 100)
  {
    MedusaClient client(InetAddress(crash_ip, crash_port));client.crash();
    NOTICE << "crash node " << crash_ip << ":" << crash_port;
  }
  else if (tuple._data.seq_no == 200)
  {
    NOTICE << "restart the failed node";
    NOTICE << command;
    system( command.c_str() );
  }
  else if (tuple._data.seq_no == 210)
  {
    MedusaClient client(InetAddress(crash_ip, crash_port));
    NOTICE << "adding to " << crash_ip << ":" << crash_port << " query " << query2;
    client.add_query_xml(query2);

    string visualizer(visualizer_ip + ":" + visualizer_port);

    StreamDef stream_def(OUTPUT1);

    Subscription sub(stream_def, "tcp", visualizer, "MyTCPSink", Subscription::FAST);
    RPC<void> r = client.subscribe(sub, Subscription::ADD);
    NOTICE << "Done subscribing to output " << OUTPUT1;

    r = client.start_query(QUERY2_NAME);
    NOTICE << "started query " << QUERY2_NAME;

  }
}

////////////////////////////////////////////////////////////////////////////////
//
// Return here after sending a packet and a delay.
//
void Fail_DemoMarshal::sent_input()
{
  send_tuple();
}

////////////////////////////////////////////////////////////////////////////////
//
int main(int argc, const char *argv[])
{

    int32  status;

    // Run the front-end, open a client, subscribe to outputs and inputs.
    status = marshal.open(ip, port);

    if (status)
    {
        WARN << "Could not deploy the network.";
    }
    else
    {

        // Send the first batch of tuples. Queue up the next round with a delay.
        sleep(SLEEP_TIME_BEFORE_TUPLES);
        send_tuple();

        DEBUG << "run the client event loop...";
        // Run the client event loop.  Return only on an exception.
        marshal.run_client();
    }

    return(status);
}


