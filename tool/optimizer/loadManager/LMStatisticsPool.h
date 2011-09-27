#ifndef LMStatisticsPool_H
#define LMStatisticsPool_H

#include "LMCommon.h"
#include "TimeSeries.h"
#include <string>
#include <map>

BOREALIS_NAMESPACE_BEGIN

using namespace StatisticsTools;

typedef string StatisticsName;
typedef map<IdType, TimeSeries> StatisticsMap;

class LMStatisticsPool:public map<StatisticsName, StatisticsMap>
{
public:
    LMStatisticsPool(int ts_sample_size): _ts_sample_size(ts_sample_size) {}

    /**
     * Return all statistics with given name
     * If the given statisticsName does not exist, add it to the statistics pool
     * @param name given StatisticsName
     */
    inline StatisticsMap& get_statistics(StatisticsName name);

    /**
     * Return statistics with given name and ID
     * If the TimeSeries with given id does not exist, add one with fixed number of samples
     */
    inline TimeSeries& get_statistics(StatisticsName name, IdType id);

    /**
     * Print the names of all statistics
     */
    void print_statistics_names();

    /**
     * ouput statistics of a given name
     */
    void output_time_series_statistics(ostream& os, const string& statistics_name);

    /**
     * Output average statistics
     */
    void output_average_statistics(ostream& os, const string& statistics_name);

    /**
     * Output all average statistics
     */
    void output_average_statistics(ostream& os);

    /**
     * Print all the statistics
     */
    friend ostream& operator<<(ostream& os, const LMStatisticsPool& pool);

private:
    int _ts_sample_size;
};

/**
 * Output a StatisticsMap
 */
ostream& operator<<(ostream& os, const StatisticsMap& sm);


StatisticsMap& LMStatisticsPool::get_statistics(StatisticsName name)
{
    return (*this)[name];
}


TimeSeries& LMStatisticsPool::get_statistics(StatisticsName name, IdType id)
{
    StatisticsMap& sm = (*this)[name];
    StatisticsMap::iterator sit = sm.find(id);
    if (sit != sm.end())
        return sit->second;
    else
    {
        sm[id] = TimeSeries(_ts_sample_size);
        return sm[id];
    }
}

BOREALIS_NAMESPACE_END
#endif
