/**
 * Enqueue.cc
 *
 *  Created on: Sep 30, 2011
 *  Author: hellojinjie
 *  Email: hellojinjie%gmail.com
 */

#include "Enqueue.h"
#include "TupleHeader.h"
#include "NMSTL/net"
#include "NMSTL/callback"
#include <fstream>
#include <errno.h>
#include <cstring>

using namespace Borealis;
using namespace std;

Enqueue::Enqueue()
{

}

void Enqueue::loadData()
{
    ifstream stream("stock.csv", ifstream::in);
    if (!stream.good())
    {
        perror("stock.csv");
        exit(errno);
    }
    while (stream.good())
    {
        StockInputTuple tuple;
        char other[100];
        char* current;
        stream.getline(other, 100);
        current = other;
        if (strlen(current) == 0)
        {
            break;
        }
        char tmpchar[50];
        /* parse time */
        int firstsegmentlength = strchr(current, ',') - current;
        strncpy(tmpchar, current, firstsegmentlength);
        current = strchr(current, ',') + 1;
        tmpchar[firstsegmentlength] = '\0';
        tuple.time = atoi(tmpchar);

        /* parse price */
        int secondsegmentlength = strchr(current, ',') - current;
        strncpy(tmpchar, current, secondsegmentlength);
        current = strchr(current, ',') + 1;
        tmpchar[secondsegmentlength] = '\0';
        tuple.price = atof(tmpchar);

        /* parse volume */
        int thirdsegmentlength = strchr(current, ',') - current;
        strncpy(tmpchar, current, thirdsegmentlength);
        current = strchr(current, ',') + 1;
        tmpchar[thirdsegmentlength] = '\0';
        tuple.trade_volume = atoi(tmpchar);

        /* parse amount */
        int fouthsegmentlength = strchr(current, ',') - current;
        strncpy(tmpchar, current, fouthsegmentlength);
        current = strchr(current, ',') + 1;
        tmpchar[fouthsegmentlength] = '\0';
        tuple.trade_amount = atoi(tmpchar);

        /* parse bid-ask */
        tuple.bid_ask = current[0];

        cout << "tuple.price: " << tuple.price << endl;
        cout << "tuple.trade_volume: " << tuple.trade_volume << endl;
        cout << "tuple.trade_amount: " << tuple.trade_amount << endl;
        cout << "tuple.bid_ask: " << tuple.bid_ask << endl;
        cout << endl;

        data.push_back(tuple);
    }
    INFO << "data loaded...";
}

void Enqueue::start()
{
    /* load data from csv file */
    this->loadData();
    medusaClient = ptr<MedusaClient>(new MedusaClient(InetAddress()));
    Status status = medusaClient->set_data_path(676110,
            Util::get_host_address("127.0.0.1"), 15000);
    if (!status)
    {
        WARN << "set data path error...";
        exit(errno);
    }
    DEBUG << "set_data_path status: " << status;

    this->sendPacket();

    medusaClient->run();
    INFO << "data sent...";
}

void Enqueue::sendPacket()
{
    if (!data.empty())
    {
        eventPacket.reset();
        eventPacket = ptr<StreamEvent>(new StreamEvent("inputstream"));
        eventPacket->_inject = true;
        StockInputTuple tuple = data.front();
        data.pop_front();
        StockInputTuple* pTuple = &tuple;
        string stringData(reinterpret_cast<const char *>(pTuple),
                sizeof(StockInputTuple));
        DEBUG << pTuple->time;
        DEBUG << "data size:" << stringData.size();
        eventPacket->insert_bin(stringData);
        medusaClient->fast_post_event(eventPacket);
        DEBUG << "sending " << tuple.time << " " << tuple.price;
    }

    (new CallbackTimer(medusaClient->get_loop(),
            wrap(this, &Enqueue::sendPacket)))->arm(
            Time::now() + Time::msecs(1));

}

Enqueue::~Enqueue()
{

}

int main(int argc, char** argv)
{
    if (argc == 2)
    {
        using namespace std;
        ptr<MedusaClient> client = ptr<MedusaClient>(
                new MedusaClient(InetAddress("127.0.0.1", 15000)));

        string what(argv[1]);
        cout << "We are trying box: " << what << endl;

        if (what == "map")
        {
            client->add_conger_string("map");
        }
        else if (what == "filter")
        {
            client->add_conger_string("filter");
        }
        else if (what == "filter_map")
        {
            client->add_conger_string("filter_map");
        }
    }
    Enqueue enqueue;
    enqueue.start();
}
