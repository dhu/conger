/*
 * Dequeue.cpp
 *
 *  Created on: Oct 7, 2011
 *      Author: jj
 */

#include "Dequeue.h"

using namespace Borealis;

Dequeue::Dequeue()
{

}
void Dequeue::start()
{
	medusaClient = ptr<MedusaClient>(new MedusaClient(InetAddress()));
	Status status = medusaClient->set_data_handler(
			InetAddress("127.0.0.1:25000"), wrap(&handleOutput));
	medusaClient->run();
}

Status Dequeue::handleOutput(ptr<StreamEvent> event)
{
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
	return Status(true);
}

Dequeue::~Dequeue()
{

}

int main()
{
	Dequeue dequeue;
	dequeue.start();
}
