/*
 * Dequeue.h
 *
 *  Created on: Oct 7, 2011
 *      Author: jj
 */

#ifndef DEQUEUE_H_
#define DEQUEUE_H_

#include "common.h"
#include "MedusaClient.h"
#include "NMSTL/ptr"
#include "NMSTL/debug"

BOREALIS_NAMESPACE_BEGIN

class Dequeue {
public:
	struct AggregateTuple
	{
		int32 time;
		float maxprice;
	}__attribute__((__packed__));

	struct InputTuple
	{
		int32 time;
		float price;
	}__attribute__((__packed__));

public:
	Dequeue();

	void start();

	static Status handleOutput(ptr<StreamEvent> event);

	virtual ~Dequeue();

private:
	ptr<MedusaClient> medusaClient;

};

BOREALIS_NAMESPACE_END

#endif /* DEQUEUE_H_ */
