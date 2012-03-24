/*
 * StockTuple.h
 *
 *  Created on: Oct 8, 2011
 *      Author: jj
 */

#ifndef STOCK_TUPLE_H_
#define STOCK_TUPLE_H_

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

#endif /* STOCK_TUPLE_H_ */
