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
#include "Tuple.h"

BOREALIS_NAMESPACE_BEGIN

class Dequeue {

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
