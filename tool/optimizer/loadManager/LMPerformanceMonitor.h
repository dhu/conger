#ifndef LMPerformanceMonitor_H
#define LMPerformanceMonitor_H

#include "LMCommon.h"
#include "TimeSeries.h"

BOREALIS_NAMESPACE_BEGIN

using namespace StatisticsTools;

struct LoadMigrationStatistics
{
    static TimeSeries amount_of_load_moved;
};

struct GlobalClusteringPerformanceMetric
{
    static double average_super_box_avg_load;
    static double maximum_super_box_avg_load;
};

struct GlobalDistributionPerformanceMetric
{
    static double average_node_avg_load;
    static double std_of_node_avg_load;
    static double average_node_load_std;
    static double maximum_node_load_std;
    static double average_node_load_correlation;
    static double cutting_super_edge_over_total_super_edge_bandwidth_ratio;
};

struct BandwidthReductionMetric
{
    static double bandwidth_reduction_ratio;
    static double bandwidth_reduction_ratio_after_load_balancing;

    static StatisticsTools::TimeSeries amount_of_load_moved;
    static StatisticsTools::TimeSeries average_load;
    static StatisticsTools::TimeSeries max_load;
    static double number_of_load_migrations;
    static StatisticsTools::TimeSeries end_to_end_latency;
    static StatisticsTools::TimeSeries latency_sim_time;

    static map<IdType, StatisticsTools::TimeSeries> nodes_load;
};


BOREALIS_NAMESPACE_END

#endif
