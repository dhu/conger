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

/* 这里的类型一定要 double */
struct AggregateTuple: public TupleHeader
{
	int32 time;
	double price;
 }__attribute__((__packed__));

struct StockInputTuple: public TupleHeader
{
	int32 time;
	double price;
} __attribute__((__packed__));

#endif /* STOCK_TUPLE_H_ */
