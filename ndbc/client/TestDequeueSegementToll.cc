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
#include "TrafficOutputTuple.h"
#include <time.h>

BOREALIS_NAMESPACE_BEGIN

void pretty_print(SegementTollTuple tuple)
{
    INFO << "way: " << tuple.way << ", dir: " << tuple.dir
            << ", seg: " << tuple.seg << ", toll: " << tuple.toll <<
            ", vol time: " << tuple.vol_time << ", avg speed time: "
            << tuple.avgspeed_time;
}

Status handleOutput(ptr<StreamEvent> event)
{
    if (event->_stream == Name("segementtoll"))
    {
        uint32 offset = 0;
        int32 index;

        offset = 0;
        index = 0;

        // For each tuple that was received
        for (index = 0; index < event->_inserted_count; index++)
        {
            //offset += HEADER_SIZE;
            SegementTollTuple *tuple =
                    (SegementTollTuple *) &event->_bin_tuples[offset];
            offset += sizeof(SegementTollTuple);
            pretty_print(*tuple);
        }

    }
    else
    {
        //(DEBUG << (event->_stream.as_string()));
    }
    return Status(true);
}

class TestDequeueSegementToll
{
public:
    TestDequeueSegementToll()
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

    ~TestDequeueSegementToll()
    {

    }
};

BOREALIS_NAMESPACE_END

int main()
{
    Borealis::TestDequeueSegementToll dequeue;
    dequeue.start();
}
