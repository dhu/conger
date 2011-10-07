/**
 * Enqueue.h
 *
 *  Created on: Sep 30, 2011
 *  Author: hellojinjie
 *  Email: hellojinjie%gmail.com
 */

#ifndef ENQUEUE_H_
#define ENQUEUE_H_

#include "common.h"
#include "NMSTL/debug"
#include "NMSTL/ptr"
#include "MedusaClient.h"
#include <list>

BOREALIS_NAMESPACE_BEGIN

class Enqueue
{
public:
	struct AggregateTuple
	{
		int32 time;
		float maxprice;
	}__attribute__((__packed__));

	struct InputTuple : public TupleHeader
	{
		int32 time;
		float price;
	}__attribute__((__packed__));

public:
	Enqueue();

	void start();
	void loadData();

	virtual ~Enqueue();

private:
	ptr<MedusaClient> medusaClient;
	ptr<StreamEvent> eventPacket;
	list<InputTuple> data;
};

BOREALIS_NAMESPACE_END

#endif /* ENQUEUE_H_ */
