/*
 * DequeueMap.cpp
 *
 *  Created on: May 16, 2011
 *      Author: hellojinjie
 */

#include "common.h"
#include "MedusaClient.h"
#include "NMSTL/ptr"
#include "NMSTL/debug"
#include "StockTuple.h"
#include <time.h>


BOREALIS_NAMESPACE_BEGIN

void pretty_print(StockAggregateFilterTuple tuple)
{
    struct tm* current_time;

    current_time = localtime((const time_t*) &tuple.time);

    char time_string[100];
    char price_string[100];
    char time_then_string[100];

    strftime(time_string, 100, "%H:%M:%S", current_time);
    sprintf(price_string, "%2.2f", tuple.price);

    current_time = localtime((const time_t*) &tuple.then);
    strftime(time_then_string, 100, "%H:%M:%S", current_time);

    INFO << "当前时间: " << time_string << ", 过去十分钟最高价: " << price_string
            << ", 当时: " << time_then_string;
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
            StockAggregateFilterTuple *tuple =
                    (StockAggregateFilterTuple *) &event->_bin_tuples[offset];
            offset += sizeof(StockAggregateFilterTuple);
            pretty_print(*tuple);
        }

    }
    else
    {
        (DEBUG << (event->_stream.as_string()));
    }
    return Status(true);
}

class DequeueAggregateJoin
{
public:
    DequeueAggregateJoin()
    {

    }

    ptr<MedusaClient> medusaClient;

    void start()
    {
        medusaClient = ptr<MedusaClient>(new MedusaClient(InetAddress()));
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

    ~DequeueAggregateJoin()
    {

    }
};

BOREALIS_NAMESPACE_END

int main()
{
    Borealis::DequeueAggregateJoin dequeue;
    dequeue.start();
}
