/**
 * DataProvider.h
 *
 *  Created on: Sep 20, 2012
 *      Author: hellojinjie
 */

#ifndef DATAPROVIDER_H_
#define DATAPROVIDER_H_

#include <ctime>
#include <fstream>
#include <list>
#include <string>

namespace ndbc
{

using namespace std;

typedef struct TrafficTupleStruct
{
    int     m_iType;   // Type:
                       //   . 0: position report
                       //   . 2: account balance request
                       //   . 3: daily expenditure request
                       //   . 4: travel time request
    int     m_iTime;   // 0...10799 (second), timestamp position report emitted
    int     m_iVid;    // 0...MAXINT, vehicle identifier
    int     m_iSpeed;  // 0...100, speed of the vehicle
    int     m_iXway;   // 0...L-1, express way
    int     m_iLane;   // 0...4, lane
    int     m_iDir;    // 0..1, direction
    int     m_iSeg;    // 0...99, segment
    int     m_iPos;    // 0...527999, position of the vehicle
    int     m_iQid;    // query identifier
    int     m_iSinit;  // start segment
    int     m_iSend;   // end segment
    int     m_iDow;    // 1..7, day of week
    int     m_iTod;    // 1...1440, minute number in the day
    int     m_iDay;    // 1..69, 1: yesterday, 69: 10 weeks ago
} TrafficTuple;


class DataProvider
{
public:
    DataProvider(std::string filename);

    list<TrafficTuple> get_data();

    virtual ~DataProvider();

private:
    void fetch();
    TrafficTuple parse(string one_line);

    list<TrafficTuple> buffer;
    unsigned int buffer_size;
    unsigned int fetch_size;
    ifstream* stream;

    time_t start_time;
};

} /* namespace ndbc */
#endif /* DATAPROVIDER_H_ */
