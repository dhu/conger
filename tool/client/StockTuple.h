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


struct StockFilterTuple: public TupleHeader
{
    char stock[6];
    int32 time;
    double price;
    int32 trade_volume;
    int32 trade_amount;
    /* bid 是买盘，ask 是卖盘, n 表示中性盘 */
    char bid_ask;
}__attribute__((__packed__));


struct StockJoinTuple: public TupleHeader
{
    int32 time;
    char stock0[6];
    double price0;
    char stock1[6];
    double price1;
}__attribute__((__packed__));

struct StockAggregateTuple: public TupleHeader
{
    int32 time;
    double price;
}__attribute__((__packed__));

struct StockAggregateFilterTuple: public TupleHeader
{
    int32 time;
    double price;
    int32 then;
}__attribute__((__packed__));


struct StockMapTuple: public TupleHeader
{
    char stock[6];
    int32 time;
    double usd;
}__attribute__((__packed__));


struct StockInputTuple: public TupleHeader
{
    char stock[6];
    int32 time;
    double price;
    int32 trade_volume;
    int32 trade_amount;
    /* bid 是买盘，ask 是卖盘, n 表示中性盘 */
    char bid_ask;
}__attribute__((__packed__));

#endif /* STOCK_TUPLE_H_ */
