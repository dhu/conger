/*
 * DequeueJoin.cpp
 *
 *  Created on: May 17, 2011
 *      Author: hellojinjie
 */

#include "common.h"
#include "MedusaClient.h"
#include "NMSTL/ptr"
#include "NMSTL/debug"
#include "StockTuple.h"
#include <time.h>



BOREALIS_NAMESPACE_BEGIN

void pretty_print(StockJoinTuple tuple)
{
    struct tm* current_time;

    current_time = localtime((const time_t*) &tuple.time);

    char time_string[100];
    strftime(time_string, 100, "%H:%M:%S", current_time);

    char stock0[7] = {'\0'};
    char stock1[7] = {'\0'};
    char price_string0[100];
    char price_string1[100];

    sprintf(price_string0, "%2.2f", tuple.price0);
    sprintf(price_string1, "%2.2f", tuple.price1);

    memcpy(stock0, tuple.stock0, 6);
    memcpy(stock1, tuple.stock1, 6);

    INFO << "当前时间: " << time_string << ", "
            << stock0 << " 价格: " << price_string0 << ", "
            << stock1 << " 价格: " << price_string1 ;
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
            StockJoinTuple *tuple =
                    (StockJoinTuple *) &event->_bin_tuples[offset];
            offset += sizeof(StockJoinTuple);
            pretty_print(*tuple);
        }

    }
    else
    {
        (DEBUG << (event->_stream.as_string()));
    }
    return Status(true);
}

class DequeueJoin
{
public:
    DequeueJoin()
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

    ~DequeueJoin()
    {

    }
};

BOREALIS_NAMESPACE_END

int main()
{
    Borealis::DequeueJoin dequeue;
    dequeue.start();
}
