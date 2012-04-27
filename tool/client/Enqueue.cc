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
        stream.getline(other, 100);
        if (strlen(other) == 0)
        {
                break;
        }
        char tmpchar[50];
        /* parse time */
        int firstcommapostion = strchr(other, ',') - other;
        strncpy(tmpchar, other, firstcommapostion);
        tmpchar[firstcommapostion] = '\0';
        tuple.time = atoi(tmpchar);

        /* parse price */
        int secondcommapostion = strchr(other + firstcommapostion + 1, ',')
            - other - firstcommapostion - 1;
        strncpy(tmpchar, other + firstcommapostion + 1, secondcommapostion);
        tmpchar[secondcommapostion] = '\0';
        tuple.price = atof(tmpchar);
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
        string stringData(reinterpret_cast<const char *>(pTuple), sizeof(StockInputTuple));
        DEBUG << pTuple->time;
        DEBUG << stringData;
        DEBUG << stringData.size();
        eventPacket->insert_bin(stringData);
        medusaClient->fast_post_event(eventPacket);
        DEBUG << "sending " << tuple.time << " " << tuple.price;
    }

    (new CallbackTimer(medusaClient->get_loop(),
                    wrap(this, &Enqueue::sendPacket)))->arm(Time::now() + Time::msecs(1));

}

Enqueue::~Enqueue()
{

}

int main()
{
	Enqueue enqueue;
	enqueue.start();
}
