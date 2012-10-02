/*
 * TrafficInputTuple.h
 *
 *  Created on: Sep. 21, 2012
 *      Author: hellojinjie
 */

#ifndef TRAFFIC_OUTPUT_TUPLE_H_
#define TRAFFIC_OUTPUT_TUPLE_H_

#include "TupleHeader.h"

using namespace Borealis;

/* 这里的 float 类型一定要 double */
struct StatisticsTuple: public TupleHeader
{
    int time;
    int volume;
    int min_speed;
    int max_speed;
    int avg_speed;
}__attribute__((__packed__));

struct SegementTollTuple: public TupleHeader
{
    int way;
    int dir;
    int seg;
    int toll;
}__attribute__((__packed__));

#endif /* TRAFFIC_OUTPUT_TUPLE_H_ */
