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
		InputTuple tuple;
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
	Status status = medusaClient->set_data_path(64000, "127.0.0.1", 15000);
	if (!status)
	{
		WARN << "set data path error...";
		exit(errno);
	}
	DEBUG << "set_data_path status: " << status;
	while (!data.empty())
	{
		eventPacket.reset();
		eventPacket = ptr<StreamEvent>(new StreamEvent("packet"));
		eventPacket->_inject = true;
		InputTuple tuple = data.front();
		data.pop_back();
		eventPacket->insert_bin(string((const char *) &tuple), sizeof(tuple));
		medusaClient->fast_post_event(eventPacket);
		DEBUG << "sending " << tuple.time << " ...";
	}
	INFO << "data sent...";
}

Enqueue::~Enqueue()
{

}

int main()
{
	Enqueue enqueue;
	enqueue.start();
}
