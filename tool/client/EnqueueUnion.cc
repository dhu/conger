/**
 * EnqueueUnion.cc
 *
 *  Created on: May 16, 2012
 *  Author: hellojinjie
 *  Email: hellojinjie%gmail.com
 */

#include "TupleHeader.h"
#include "NMSTL/net"
#include "NMSTL/callback"
#include "StockTuple.h"
#include "MedusaClient.h"
#include <fstream>
#include <list>
#include <errno.h>
#include <cstring>

BOREALIS_NAMESPACE_BEGIN

class EnqueueUnion
{

public:
    ptr<MedusaClient> medusaClient;
    ptr<StreamEvent> eventPacket;
    list<StockInputTuple> data000;
    list<StockInputTuple> data529;

    EnqueueUnion()
    {

    }

    void loadData(list<StockInputTuple> &data, char* stock)
    {
        char filename[100] = {'\0'};
        strcat(filename, "stock");
        strcat(filename, stock);
        strcat(filename, ".csv");
        ifstream stream(filename, ifstream::in);
        if (!stream.good())
        {
            perror(filename);
            exit(errno);
        }
        while (stream.good())
        {
            StockInputTuple tuple;
            memcpy(tuple.stock, stock, 6);
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
        INFO << "data loaded..." << stock;
    }

    void start()
    {
        /* load data from csv file */
        this->loadData(this->data000, "600000");
        this->loadData(this->data529, "600529");
        medusaClient = ptr<MedusaClient>(new MedusaClient(InetAddress()));
        Status status = medusaClient->set_data_path(676110,
                Util::get_host_address("127.0.0.1"), 15000);
        if (!status)
        {
            WARN << "set data path error...";
            exit(errno);
        }
        DEBUG << "set_data_path status: " << status;

        this->sendPacket000();
        this->sendPacket529();

        medusaClient->run();
        INFO << "data sent...";
    }

    void sendPacket000()
    {
        if (!data000.empty())
        {
            eventPacket.reset();
            eventPacket = ptr<StreamEvent>(new StreamEvent("inputstream000"));
            eventPacket->_inject = true;
            StockInputTuple tuple = data000.front();
            data000.pop_front();
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
                        wrap(this, &EnqueueUnion::sendPacket000)))->arm(
                Time::now() + Time::msecs(1));

    }

    void sendPacket529()
    {
        if (!data529.empty())
        {
            eventPacket.reset();
            eventPacket = ptr<StreamEvent>(new StreamEvent("inputstream529"));
            eventPacket->_inject = true;
            StockInputTuple tuple = data529.front();
            data529.pop_front();
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
                        wrap(this, &EnqueueUnion::sendPacket529)))->arm(
                Time::now() + Time::msecs(1));

    }

    ~EnqueueUnion()
    {

    }
};

BOREALIS_NAMESPACE_END

int main(int argc, char** argv)
{
    using namespace Borealis;
    using namespace std;

    ptr<MedusaClient> client = ptr<MedusaClient>(
            new MedusaClient(InetAddress("127.0.0.1", 15000)));

    client->add_conger_string("union");
    Borealis::EnqueueUnion enqueue;
    enqueue.start();
}
