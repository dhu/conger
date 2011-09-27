#ifndef NetworkTrafficQueryParams_H
#define NetworkTrafficQueryParams_H

#include <common.h>

BOREALIS_NAMESPACE_BEGIN

/**
 * A structure defining parameters for query generation
 */
class NetworkTrafficQueryParams
{

public:
    /**
     * Returns the singleton handle 
     */
    static NetworkTrafficQueryParams* get_singleton();

    /**
     * Number of queries
     */
    unsigned int number_of_queries;

    /**
     * Number of group by values
     * 1: group by source
     * 2: group by source_destination_pair
     */
    unsigned int number_of_group_by_values;

    /**
     * Number of count aggregate boxes in a query
     */
    unsigned int number_of_count_box_per_query;

    /**
     * Number of average aggregate boxes in a query
     */
    unsigned int number_of_avg_box_per_query;

    /**
     * Number of average aggregate boxes in a query
     */
    unsigned int timeout;

    /**
     * The initial window value 
     */
    unsigned int initial_window_size;

    /**
     * The step between the window-size value of different aggregate operators
     */
    unsigned int window_size_step;

    /**
     * The initial advance value 
     */
    unsigned int initial_advance;

    /**
     * The step between the advance value of different aggregate operators
     */
    unsigned int advance_step;

    /**
     * The number of count sliding aggregates
     */
    unsigned int number_of_count_sliding_window_boxes;

    /**
     * The number of average sliding aggregates
     */
    unsigned int number_of_avg_sliding_window_boxes;

    /**
     * The initial sliding window size 
     */
    unsigned int initial_sliding_window_size;

    /**
     * The sliding window step 
     */
    unsigned int sliding_window_size_step;


private:
    /**
     * Create a NetworkTrafficQueryParams object with default parameters
     */
    NetworkTrafficQueryParams();

    
};

/**
 * Ouput the parameters
 */
ostream& operator<<(ostream& os, const NetworkTrafficQueryParams& ob);

/**
 * Read in parameters from a istream
 */
istream& operator>>(istream& is, NetworkTrafficQueryParams& ob);

BOREALIS_NAMESPACE_END
#endif


