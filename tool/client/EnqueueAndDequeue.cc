/*
 * EnqueueAndDequeue.cc
 * 将发送数据和接收数据放在一个程序里
 *
 *  Created on: Oct 13, 2011
 *      Author: jj
 */


#include "common.h"
#include "NMSTL/ptr"
#include "NMSTL/debug"
#include "MedusaClient.h"
#include "StockTuple.h"
#include <fstream>

using namespace Borealis;

void run();
Status handleOutput(ptr<StreamEvent> event);
void loadData();

static ptr<MedusaClient> medusaClient =
		ptr<MedusaClient>(new MedusaClient(InetAddress()));
static ptr<StreamEvent> eventPacket;
static list<InputTuple> data;

int main()
{
	run();
	return 0;
}

void run()
{
	Status status;
	status = medusaClient->set_data_path(640000,
			Util::get_host_address("127.0.1.1"), 15000);
	DEBUG << status;
	status = medusaClient->set_data_handler(
			InetAddress(Util::form_endpoint("127.0.0.1:25000",25000)),
			wrap(&handleOutput));
	DEBUG << status;

	loadData();
	status = medusaClient->set_data_path(640000,
			Util::get_host_address("127.0.0.1"), 15000);
	if (!status)
	{
		WARN << "set data path error...";
		exit(errno);
	}
	DEBUG << "set_data_path status: " << status;
	while (!data.empty())
	{
		eventPacket.reset();
		eventPacket = ptr<StreamEvent>(new StreamEvent("aggregate"));
		eventPacket->_inject = true;
		InputTuple tuple = data.front();
		data.pop_front();
		eventPacket->insert_bin(string((const char *) &tuple), sizeof(tuple));
		medusaClient->fast_post_event(eventPacket);
		DEBUG << "sending " << tuple.time << " " << tuple.price;
	}
	medusaClient->run();
}

void loadData()
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

Status handleOutput(ptr<StreamEvent> event)
{
	DEBUG << event->_stream.as_string();
	if (event->_stream == Name("aggregate"))
	{
		uint32 offset = 0;
		int32 index;

		offset = 2;
		index = 2;

		// For each tuple that was received
		for (index = 0; index < event->_inserted_count; index++)
		{
			offset += HEADER_SIZE;
			AggregateTuple *tuple =
					(AggregateTuple *) &event->_bin_tuples[offset];
			offset += sizeof(AggregateTuple);
			INFO << tuple->time << " " << tuple->maxprice;
		}

	}
	else
	{
		(DEBUG << (event->_stream.as_string()));
	}
	return true;
}
