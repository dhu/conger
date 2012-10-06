/**
 * DataProvider.cc
 *
 *  Created on: Sep 20, 2012
 *      Author: hellojinjie
 */

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>
#include <ctime>
#include "DataProvider.h"
#include <errno.h>
#include <iostream>
#include "NMSTL/debug"
#include <vector>

namespace ndbc
{

using namespace std;
using namespace NMSTL;

DataProvider::DataProvider(string filename): buffer_size(10000), fetch_size(1000)
{
    stream = new ifstream(filename.c_str(), ifstream::in);
    if (!stream->good())
    {
        perror(filename.c_str());
        exit(errno);
    }

    this->start_time = time(NULL);
}

list<TrafficTuple> DataProvider::get_data()
{
    time_t current = time(NULL);
    if (this->buffer.size() < this->buffer_size)
    {
        this->fetch();
    }

    list<TrafficTuple> result;

    while (!this->buffer.empty())
    {
        TrafficTuple tuple = this->buffer.front();
        if (tuple.m_iTime <= current - start_time)
        {
            this->buffer.pop_front();
            result.push_back(tuple);
        }
        else
        {
            break;
        }
    }
    return result;
}

void DataProvider::fetch()
{
    while (this->buffer.size() < this->buffer_size)
    {
        for (unsigned int i = 0; i <= this->fetch_size; i++)
        {
            if (this->stream->good())
            {
                char line[500];
                this->stream->getline(line, 500, '\n');
                string one_line(line);
                try
                {
                    TrafficTuple tuple = this->parse(string(one_line));
                    this->buffer.push_back(tuple);
                }
                catch (exception& e)
                {
                    NOTICE << "can not parse line: " << one_line;
                }
            }
            else
            {
                break;
            }
        }
        if (not this->stream->good())
        {
            break;
        }
    }
}

TrafficTuple DataProvider::parse(string one_line)
{
    using namespace boost::algorithm;
    using boost::lexical_cast;

    vector<string> result;
    split(result, one_line, is_any_of(","), token_compress_on);

    TrafficTuple tuple;
    tuple.m_iType = lexical_cast<int>(result[0]);
    tuple.m_iTime = lexical_cast<int>(result[1]);
    tuple.m_iVid = lexical_cast<int>(result[2]);
    tuple.m_iSpeed = lexical_cast<int>(result[3]);
    tuple.m_iXway = lexical_cast<int>(result[4]);
    tuple.m_iLane = lexical_cast<int>(result[5]);
    tuple.m_iDir = lexical_cast<int>(result[6]);
    tuple.m_iSeg = lexical_cast<int>(result[7]);
    tuple.m_iPos = lexical_cast<int>(result[8]);
    tuple.m_iQid = lexical_cast<int>(result[9]);
    tuple.m_iSinit = lexical_cast<int>(result[10]);
    tuple.m_iSend = lexical_cast<int>(result[11]);
    tuple.m_iDow = lexical_cast<int>(result[12]);
    tuple.m_iTod = lexical_cast<int>(result[13]);
    tuple.m_iDay = lexical_cast<int>(result[14]);

    return tuple;
}

DataProvider::~DataProvider()
{
    stream->close();
}

} /* namespace ndbc */
