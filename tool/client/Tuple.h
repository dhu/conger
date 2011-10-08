/*
 * Tuple.h
 *
 *  Created on: Oct 8, 2011
 *      Author: jj
 */

#ifndef TUPLE_H_
#define TUPLE_H_

#include "TupleHeader.h"

using namespace Borealis;

struct AggregateTuple
{
	int32 time;
	float maxprice;
}__attribute__((__packed__));

struct InputTuple: public TupleHeader
{
	int32 time;
	float price;
}__attribute__((__packed__));

#endif /* TUPLE_H_ */
