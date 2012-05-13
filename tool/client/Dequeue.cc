/*
 * Dequeue.cpp
 *
 *  Created on: Oct 7, 2011
 *      Author: jj
 */

#include "Dequeue.h"
#include <time.h>

using namespace Borealis;

void pretty_print(AggregateTuple tuple);
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

    //status = medusaClient->subscribe("outputstream");
    DEBUG << status;
    medusaClient->run();
}

Status handleOutput(ptr<StreamEvent> event)
{
    if (event->_stream == Name("outputstream"))
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
            pretty_print(*tuple);
        }

    }
    else
    {
        (DEBUG << (event->_stream.as_string()));
    }
    return Status(true);
}

void pretty_print(AggregateTuple tuple)
{
    struct tm* current_time;

    current_time = localtime((const time_t*)&tuple.time);

    char time_string[100];
    char price_string[100];

    // sprintf(time_string, "%d:%d:%d", current_time->tm_hour, current_time->tm_min, current_time->tm_sec);
    strftime(time_string, 100, "%H:%M:%S", current_time);
    sprintf(price_string, "%2.2f", tuple.price);

    INFO << "当前时间: " << time_string << ", 过去十分钟最高价: " << price_string;
}

Dequeue::~Dequeue()
{

}

int main()
{
    Dequeue dequeue;
    dequeue.start();
}
