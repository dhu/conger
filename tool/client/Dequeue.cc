/*
 * Dequeue.cpp
 *
 *  Created on: Oct 7, 2011
 *      Author: jj
 */

#include "Dequeue.h"

using namespace Borealis;

Status handleOutput(ptr<StreamEvent> event);

Dequeue::Dequeue()
{

}
void Dequeue::start()
{
    medusaClient = ptr < MedusaClient > (new MedusaClient(InetAddress()));
    Status status;
    status = medusaClient->set_data_handler(
            InetAddress(
                    Util::form_endpoint("127.0.0.1:25000",
                            DEFAULT_MONITOR_PORT)), wrap(&handleOutput));
    DEBUG << status;

    //status = medusaClient->subscribe("aggregate");
    DEBUG << status;
    medusaClient->run();
}

Status handleOutput(ptr<StreamEvent> event)
{
    DEBUG << event->_stream.as_string();
    if (event->_stream == Name("aggregate"))
    {
        uint32 offset = 0;
        int32 index;

        offset = 0;
        index = 0;

        // For each tuple that was received
        for (index = 0; index < event->_inserted_count; index++)
        {
            //offset += HEADER_SIZE;
            AggregateTuple *tuple =
                    (AggregateTuple *) &event->_bin_tuples[offset];
            offset += sizeof(AggregateTuple);
            INFO << tuple->currenttime << " " << tuple->maxprice;
        }

    }
    else
    {
        (DEBUG << (event->_stream.as_string()));
    }
    return Status(true);
}

Dequeue::~Dequeue()
{

}

int main()
{
    Dequeue dequeue;
    dequeue.start();
}
