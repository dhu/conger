#include <iostream>
#include <fstream>

#include "Mitre_DemoMarshal.h"
#include "Diagram.h"

using namespace Borealis;

const uint32 SLEEP_TIME = 10;          // Delay between injections.

const Time time0 = Time::now() - Time::msecs(100);

Mitre_DemoMarshal marshal;

string ip   = "127.0.0.1";     // client host ip.

int    port = DEFAULT_BOREALIS_PORT;  // client (and input) port.

string file_name = "mitre_demo.dat";

ifstream is;

int timestamp = -1;


////////////////////////////////////////////////////////////////////////////////
//
// Print the content of received tuples.
//
void Mitre_DemoMarshal::receivedAnalysisoutput(AppElement *tuple)
{
//    NOTICE << "Received Analysis Tuple (timestamp=" << tuple->timestamp
//           << ", id=" << tuple->id
//           << ", x=" << tuple->x
//           << ", y=" << tuple->y
//           << ", color=" << tuple->color << ")";
}

void Mitre_DemoMarshal::receivedEnemytanksoutput(AppElement *tuple)
{
    NOTICE << "Received Enemytank Tuple (timestamp=" << tuple->timestamp
           << ", id=" << tuple->id
           << ", x=" << tuple->x
           << ", y=" << tuple->y
           << ", color=" << tuple->color << ")";
}

void Mitre_DemoMarshal::receivedEnemyaircraftoutput(AppElement *tuple)
{
//    NOTICE << "Received Enemyaircraft Tuple (timestamp=" << tuple->timestamp
//           << ", id=" << tuple->id
//           << ", x=" << tuple->x
//           << ", y=" << tuple->y
//           << ", color=" << tuple->color << ")";
}

void Mitre_DemoMarshal::receivedAcrossthelineoutput(AppElement *tuple)
{
//    NOTICE << "Received Acrosstheline Tuple (timestamp=" << tuple->timestamp
//           << ", id=" << tuple->id
//           << ", x=" << tuple->x
//           << ", y=" << tuple->y
//           << ", color=" << tuple->color << ")";
}

void Mitre_DemoMarshal::receivedVideooutput(ImageData *tuple)
{
//    NOTICE << "Received Video Tuple (timestamp=" << tuple->timestamp
//           << ", frame_index=" << tuple->frame_index
//           << ", row_index_begin=" << tuple->row_index_begin
//           << ", row_index_end=" << tuple->row_index_end << ")";
}

void send_tuple()
{
  if (timestamp >= 24000)
  {
    is.seekg(0);
    timestamp = -1;
  }
  char b;  // byte
  is >> b;
  if (b == 'e')
  {
    Mitre_DemoMarshal::Elementinput e;
    is.read((char*)(&e._data), sizeof(e._data));
    marshal.batchElementinput(&e);
    marshal.sendElementinput(SLEEP_TIME);
    if (e._data.timestamp > timestamp)
    {
      timestamp = e._data.timestamp;
      NOTICE << "current timestamp = " << timestamp;
    }
  }
  else
  {
    Mitre_DemoMarshal::Videoinput v;
    is.read((char*)(&v._data), sizeof(v._data));
    marshal.batchVideoinput(&v);
    marshal.sendVideoinput(SLEEP_TIME);
    if (v._data.timestamp > timestamp)
    {
      timestamp = v._data.timestamp;
      NOTICE << "current timestamp = " << timestamp;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
//
// Return here after sending a packet and a delay.
//
void Mitre_DemoMarshal::sentElementinput()
{
  send_tuple();
}

void Mitre_DemoMarshal::sentVideoinput()
{
  send_tuple();
}

////////////////////////////////////////////////////////////////////////////////
//
int main(int argc, const char *argv[])
{
    int32  status;


    // Run the front-end, open a client, subscribe to outputs and inputs.
    status = marshal.open();

    if (status)
    {
        WARN << "Could not deploy the network.";
    }
    else
    {
        DEBUG << "time0 = " << time0;

        is.open(file_name.c_str(), ios::in);

        // Send the first batch of tuples. Queue up the next round with a delay.
        send_tuple();

        DEBUG << "run the client event loop...";
        // Run the client event loop.  Return only on an exception.
        marshal.runClient();
    }

    return(status);
}
