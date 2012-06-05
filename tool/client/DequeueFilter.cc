/*
 * DequeueFilter.cpp
 *
 *  Created on: May 15, 2011
 *      Author: hellojinjie
 */

#include "common.h"
#include "MedusaClient.h"
#include "NMSTL/ptr"
#include "NMSTL/debug"
#include "StockTuple.h"
#include <time.h>



BOREALIS_NAMESPACE_BEGIN

void pretty_print(StockFilterTuple tuple)
{
    struct tm* current_time;

    current_time = localtime((const time_t*) &tuple.time);

    char stock[7];
    char time_string[100];
    char price_string[100];

    // sprintf(time_string, "%d:%d:%d", current_time->tm_hour, current_time->tm_min, current_time->tm_sec);
    strftime(time_string, 100, "%H:%M:%S", current_time);
    sprintf(price_string, "%2.2f", tuple.price);

    memcpy(stock, tuple.stock, 6);
    stock[6] = '\0';

    INFO << "stock: " << stock << " 当前时间: " << time_string << ", 价格: " << price_string;
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
            StockFilterTuple *tuple =
                    (StockFilterTuple *) &event->_bin_tuples[offset];
            offset += sizeof(StockFilterTuple);
            pretty_print(*tuple);
        }

    }
    else
    {
        (DEBUG << (event->_stream.as_string()));
    }
    return Status(true);
}

class DequeueFilter
{
public:
    DequeueFilter()
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

    ~DequeueFilter()
    {

    }
};

BOREALIS_NAMESPACE_END

int main()
{
    Borealis::DequeueFilter dequeue;
    dequeue.start();
}
