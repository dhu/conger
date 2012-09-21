/*
 * TrafficInputTuple.h
 *
 *  Created on: Sep. 21, 2012
 *      Author: hellojinjie
 */

#ifndef TRAFFIC_INPUT_TUPLE_H_
#define TRAFFIC_INPUT_TUPLE_H_

#include "TupleHeader.h"

using namespace Borealis;

/* 这里的 float 类型一定要 double */
struct PositionReportTuple: public TupleHeader
{
    int m_iTime; // 0...10799 (second), timestamp position report emitted
    int m_iVid; // 0...MAXINT, vehicle identifier
    int m_iSpeed; // 0...100, speed of the vehicle
    int m_iXway; // 0...L-1, express way
    int m_iLane; // 0...4, lane
    int m_iDir; // 0..1, direction
    int m_iSeg; // 0...99, segment
    int m_iPos; // 0...527999, position of the vehicleF
}__attribute__((__packed__));

#endif /* TRAFFIC_INPUT_TUPLE_H_ */
