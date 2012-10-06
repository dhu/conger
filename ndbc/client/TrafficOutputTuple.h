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
    int time;
    int way;
    int dir;
    int seg;
    int vol_time;
    int avgspeed_time;
    int toll;
}__attribute__((__packed__));

struct CarTollTuple: public TupleHeader
{
    int car_id;
    int toll;
    int way;
    int dir;
    int seg;
}__attribute__((__packed__));

struct CurActiveCarsTuple: public TupleHeader
{
    int car_id;
    int way;
    int dir;
    int seg;
    int time;
    int car_count;
}__attribute__((__packed__));

#endif /* TRAFFIC_OUTPUT_TUPLE_H_ */
