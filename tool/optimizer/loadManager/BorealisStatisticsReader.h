/*
 * @(#)BorealisStatisticsReader.h
 */

#ifndef BorealisStatisticsReader_H
#define BorealisStatisticsReader_H


#include <common.h>
#include <rpc_QueryProcessor.h>

BOREALIS_NAMESPACE_BEGIN


/**
 * An BorealisStatisticsReader class reads the statistics
 * collected from Borealis and fill them in the LMStatisticsManager
 */
struct BorealisStatisticsReader
{

public:
    /**
     * Read Statistics
     * Returns true if statistics are valid
     * @param nodes_statistics the statistics collected from the nodes
     * @param precision the precision of the statistics copied
     * @param window_size the window_size of the statistics copied
     */
    static bool read_statistics(
        map<string, Statistics>& nodes_statistics,
        double precision,
        int window_size);
};


BOREALIS_NAMESPACE_END
#endif


