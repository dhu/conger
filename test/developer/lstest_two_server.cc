#include <iostream>
#include <fstream>

#include "Lstest_Two_ServerMarshal.h"

using namespace Borealis;

const uint32 SLEEP_TIME = 1000;          // Delay between injections.

Lstest_Two_ServerMarshal marshal;

string ip   = "127.0.0.1";     // client host ip.

int port = 15000;           // client (and input) port.

int batch_size = 5;      // send batch_size number of tuples a time


////////////////////////////////////////////////////////////////////////////////
//
// Print the content of received tuples.
//
void Lstest_Two_ServerMarshal::receivedCount(CountSchema *tuple)
{
    NOTICE << "CountSchema: (timestamp = " << tuple->timestamp << ", count = " << tuple->count << ", seq_no = " << tuple->seq_no << ")";
}

void Lstest_Two_ServerMarshal::receivedDistance(DistanceSchema *tuple)
{
    /*
    NOTICE << "Schema: (clientnum = " << tuple->clientnum
           << ", timestamp = " << tuple->timestamp
           << ", x = " << tuple->x
           << ", y = " << tuple->y
           << ", z = " << tuple->z
           << ", seq_no = " << tuple->seq_no << ")";
   */
    NOTICE << "DistanceSchema: (timestamp = " << tuple->timestamp
           << ", distance = " << tuple->distance
           << ", seq_no = " << tuple->seq_no << ")";
}

Lstest_Two_ServerMarshal::Input tuple;

int pos = 0;

int seq_no = 0;

void send_tuple()
{
//  tuple._data.timestamp = (Time::now().to_msecs()) - start_time;

  for (int i = 0; i < batch_size; ++i)
  {
    tuple._data.timestamp = (Time::now().to_msecs());
    pos = (pos + 100) % 3000;
    tuple._data.clientnum = 1;
    tuple._data.x = pos;
    tuple._data.y = pos;
    tuple._data.z = 0;
    tuple._data.seq_no = (++seq_no);
    marshal.batchInput(&tuple);

    tuple._data.clientnum = 2;
    tuple._data.x = 3000-pos;
    tuple._data.y = 3000-pos;
    tuple._data.z = 0;
    tuple._data.seq_no = (seq_no);
    marshal.batchInput(&tuple);
  }

  marshal.sendInput(SLEEP_TIME);
}

////////////////////////////////////////////////////////////////////////////////
//
// Return here after sending a packet and a delay.
//
void Lstest_Two_ServerMarshal::sentInput()
{
  send_tuple();
}

////////////////////////////////////////////////////////////////////////////////
//
int main(int argc, const char *argv[])
{
    if (argc == 2)
      batch_size = atoi(argv[1]);

    int32  status;

    // Run the front-end, open a client, subscribe to outputs and inputs.
    status = marshal.open();

    if (status)
    {
        WARN << "Could not deploy the network.";
    }
    else
    {

        // Send the first batch of tuples. Queue up the next round with a delay.
        send_tuple();

        DEBUG << "run the client event loop...";
        // Run the client event loop.  Return only on an exception.
        marshal.runClient();
    }

    return(status);
}


