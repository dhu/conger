#include <iostream>
#include <fstream>

#include "Filter_TestMarshal.h"

using namespace Borealis;

const uint32 SLEEP_TIME = 1000;          // Delay between injections.

Filter_TestMarshal marshal;

string ip   = "127.0.0.1";     // client host ip.

int port = 15000;           // client (and input) port.

////////////////////////////////////////////////////////////////////////////////
//
// Print the content of received tuples.
//
void Filter_TestMarshal::receivedOut(Schema *tuple)
{
    NOTICE << "Received a Tuple (seq_no=" << tuple->seq_no << ")";
}

int s = 0;

void send()
{
    Filter_TestMarshal::In t;
    t._data.seq_no = s++;
    marshal.batchIn(&t);
    marshal.sendIn(SLEEP_TIME);
    NOTICE << "sent tuple (seq_no=" << t._data.seq_no << ")";
}

////////////////////////////////////////////////////////////////////////////////
//
// Return here after sending a packet and a delay.
//
void Filter_TestMarshal::sentIn()
{
    send();
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
        NOTICE << "Could not deploy the network.";
    }
    else
    {
        NOTICE << "ready to start!";

        // Send the first batch of tuples. Queue up the next round with a delay.
        send();

        DEBUG << "run the client event loop...";
        // Run the client event loop.  Return only on an exception.
        marshal.runClient();
    }

    return(status);
}
