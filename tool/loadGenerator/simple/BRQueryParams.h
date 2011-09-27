#ifndef BRQueryParams_H
#define BRQueryParams_H

#include <common.h>

BOREALIS_NAMESPACE_BEGIN

/**
 * A structure defining parameters for query generation
 */
struct BRQueryParams
{

public:
    /**
     * Number of fields
     */
    int number_of_fields;

    /**
     * Number of box
     */
    int number_of_boxes;

    /**
     * Maximum number of boxes per connected query tree
     * (union operator can connect query trees, this parameter is only
     * the limit for each query tree)
     */
    int max_number_of_boxes_per_query_tree;

    /**
     * The probability of stop generating box from a chain
     */
    double prob_stop;

    /**
     * The probability of resue an existing stream as input stream
     */
    double prob_reuse_stream;

    /**
     * The proability of generating a delay box
     */
    double prob_box_delay;

    /**
     * The proability of generating a filter box
     */
    double prob_box_filter;

    /**
     * The proability of generating a delay box
     */
    double prob_box_union;

    /**
     * The minimum delay of a delay box
     */
    double delay_min;

    /**
     * The minimum delay of a delay box
     */
    double delay_max;

    /**
     * The percentage of selectivities less than one
     */
    double delay_selectivity_percentage;

    /**
     * The minimum value for delay box selectivity
     */
    double delay_selectivity_min;

    /**
     * The maximum value for delay box selectivity
     */
    double delay_selectivity_max;

    /**
     * The minimum value for a filter value
     */
    int filter_min;

    /**
     * The maximum value for a filter value
     */
    int filter_max;

    /**
     * Set default parameters
     */
    BRQueryParams();
};

/**
 * Ouput the parameters
 */
ostream& operator<<(ostream& os, const BRQueryParams& ob);

/**
 * Read in parameters from a istream
 */
istream& operator>>(istream& is, BRQueryParams& ob);

BOREALIS_NAMESPACE_END
#endif


