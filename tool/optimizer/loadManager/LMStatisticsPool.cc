#include "LMStatisticsPool.h"

BOREALIS_NAMESPACE_BEGIN

using namespace StatisticsTools;

void LMStatisticsPool::print_statistics_names()
{
    for (LMStatisticsPool::iterator it = begin();
              it != end();
              ++it)
    {
        cout << it->first << endl;
    }
}


ostream& operator<<(ostream& os, const LMStatisticsPool& pool)
{
    for (LMStatisticsPool::const_iterator nit = pool.begin();
              nit != pool.end();
              ++nit)
    {
        os << "-----------" << nit->first << "-----------" << endl;
        os << nit->second;
    }
    return os;
}

ostream& operator<<(ostream& os, const StatisticsMap& sm)
{
    for (StatisticsMap::const_iterator sit = sm.begin();
              sit != sm.end();
              ++sit)
    {
        os << "(" << sit->first << ") " << sit->second << endl;
    }
    return os;
}

void LMStatisticsPool::output_time_series_statistics(
        ostream& os, const string& statistics_name)
{
    os << "----------------- " << statistics_name << " ------------------" << endl;
    os << get_statistics(statistics_name);
    os << "---------------------------------------------" << endl;
}

void LMStatisticsPool::output_average_statistics(
        ostream& os, const string& statistics_name)
{


    os << "----------------- " << statistics_name << " ------------------" << endl;
    StatisticsMap& statistics = get_statistics(statistics_name);
    for (StatisticsMap::const_iterator it = statistics.begin();
            it != statistics.end();
            ++it) {
        os << it->first << "(" << (it->second).get_average() << ") ";
    }
    os << endl;
    os << "---------------------------------------------" << endl;
}


void LMStatisticsPool::output_average_statistics(
        ostream& os)
{
    for (LMStatisticsPool::const_iterator nit = begin();
              nit != end();
              ++nit)
    {
        os << "-----------" << nit->first << "-----------" << endl;
        const StatisticsMap& statistics = nit->second;
        for (StatisticsMap::const_iterator it = statistics.begin();
                it != statistics.end();
                ++it) {
            os << it->first << "(" << (it->second).get_average() << ") ";
        }
        os << endl;
        os << "---------------------------------------------" << endl;
    }
}

BOREALIS_NAMESPACE_END
