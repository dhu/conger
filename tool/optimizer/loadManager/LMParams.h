#ifndef LMParams_H
#define LMParams_H

#include "LMCommon.h"
#include "LMParamTypes.h"

BOREALIS_NAMESPACE_BEGIN

/**
 * A structure defining parameters for load distributors methods
 *
 * @author Ying Xing (yx@cs.brown.edu)
 * @version 2.0 03/15/2004
 */
class LoadDistributionMethod
{

  // Attribute(s)
public:

    /**
     * Is true if warm up load distribution is activate
     */
    bool warmup_load_distribution_active;

    /**
     * Is true if initial load distribution is activate
     */
    bool initial_load_distribution_active;

    /**
     * Is true if dynamic load distribution is activate
     */
    bool dynamic_load_distribution_active;

    /**
     * Is true if dynamic load distribution is activate
     */
    bool static_load_distribution_active;

    /**
     * Is true if operator clustering is active
     */
    bool operator_clustering_active;

    /**
     * The type of the score function
     */
    ScoreFunctionType initial_distribution_score_function_type;

    /**
     * The type of the score function
     */
    ScoreFunctionType dynamic_distribution_score_function_type;

    /**
     * Pairwise load distribution method
     */
    PairwiseLoadDistributionMethod pairwise_method;

    /**
     * Activate pairwise refine correlation step if is parameter is true
     */
    bool pairwise_refine_correlation;

    //////////////
    // Not used yet
    //////////////

    /**
     * For bandwidth Based method
     */
    bool selective_exchange;

    /**
     * Balance load between neighbors first
     */
    bool neighboring_nodes_first;

    /**
     * Distibute bandwidth uniformly between node pairs
     */
    bool uniform_bandwidth_allocation;

    /**
     * Use adaptive Bandwidth headroom
     */
    bool adaptive_bandwidth;


  // Method(s)
public:

    /**
     * Constructs a default InputBoxParams object.
     */
    LoadDistributionMethod();

    /**
     * Ouput the LoadDistributionMethodParams object to a output stream
     */
    friend ostream& operator<<(ostream& os, const LoadDistributionMethod& ob);

    /**
     * Read in parameters from a istream
     */
    friend istream& operator>>(istream& is, LoadDistributionMethod& ob);

};
/**
 * A structure defining parameters for load distributors.
 *
 * @author Ying Xing (yx@cs.brown.edu)
 * @version 2.0 03/15/2004
 */
class LoadDistributionParams
{

  // Attribute(s)
public:
    /**
     * The smaple period for each statistics value
     */
    double initial_warm_up_period;

    /**
     * The smaple period for each statistics value
     */
    double statistics_sample_period;

    /**
     * The the number of smaples in each statistics TimeSeries
     */
    int number_of_samples;

    /**
     * The overloaded threshold
     */
    double overloaded_threshold;

    /**
     * The load balancing threshold
     */
    double load_balancing_threshold;

    /**
     * The score threshold for selective exchange algoithm 
     */
    double selective_exchange_score_theshold;

    ///////////////////////////////////////
    // Not used yet

    /**
     * The load migration threshold (is used to ensure that operators
     * with small load is not moved very frequently
     */
    double load_selection_threshold;

    /**
     * The threadhold for the activation of bandwidth reduction step.
     */
    double bandwidth_reduction_activation_threshold;

    /**
     * Bandwidth Evaluation Coefficeitn of std
     */
    double divergent_coefficient;

    /**
     * the maximum size of a cluster when clustering operators
     */
    double maximum_cluster_size;

    /**
     * The number of random operator distributions
     */
    int number_of_random_static_distributions;

  // Method(s)
public:

    /**
     * Constructs a default InputBoxParams object.
     */
    LoadDistributionParams();

    /**
     * Ouput the LoadDistributionMethodParams object to a output stream
     */
    friend ostream& operator<<(ostream& os, const LoadDistributionParams& ob);

    /**
     * Read in parameters from a istream
     */
    friend istream& operator>>(istream& is, LoadDistributionParams& ob);

};

/**
 * Timing Params
 *
 * @author Ying Xing (yx@cs.brown.edu)
 * @version 2.0 03/15/2004
 */
class DistributionTimeParams
{

  // Attribute(s)
public:

    /**
     * The delay time between staring load manager and load distribution
     */
    double load_distribution_starting_delay;


    /**
     * The load distribution interval for dyanmic load distribution
     */
    double load_distribution_interval;


  // Method(s)
public:

    /**
     * Constructs a default InputBoxParams object.
     */
    DistributionTimeParams();

    /**
     * Ouput the LoadDistributionMethodParams object to a output stream
     */
    friend ostream& operator<<(ostream& os, const DistributionTimeParams& ob);

    /**
     * Read in parameters from a istream
     */
    friend istream& operator>>(istream& is, DistributionTimeParams& ob);

};

/**
 * Parameters determined at runtime (not configuration parameter)
 */
class RunTimeParams
{
public:
    RunTimeParams();

    /**
     * The time interval for collecting initial statistics
     */
    double warm_up_time;

    /*
     * Is true if box migration is active
     */
    bool move_box_active;

    /*
     * Is true if box migration is active
     */
    bool start_output_to_files;

    /**
     * The directory that constains fs sequence
     */
    string load_manager_path;

    /**
     * The file name that reports status
     */
    string status_filename;

    /**
     * The root directory for ouput files
     */
    string output_dir;

    /**
     * The prefix for ouput files
     */
    string output_prefix;

    /**
     * Number of boxes to move
     */
    int number_of_boxes_to_move;

    /**
     * Ouput the RunTimeParams object to a output stream
     */
    friend ostream& operator<<(ostream& os, const RunTimeParams& ob);

};


/**
 * The following parameters determines whether
 */
class DisplayParams
{
public:
    DisplayParams();

    bool show_average_statistics;
    bool show_basic_info;
    bool show_query_network;
    bool show_box_location;
    bool show_gathered_statistics;
    bool show_lmstatistics_manager;
    bool show_node_load;
    bool show_box_load;
    bool show_stream_rate;
    bool show_selectivity;
    bool show_box_cost;

    /**
     * Ouput the DisplayParams object to a output stream
     */
    friend ostream& operator<<(ostream& os, const DisplayParams& ob);

    /**
     * Read in parameters from a istream
     */
    friend istream& operator>>(istream& is, DisplayParams& ob);
};

/**
 * The following parameters determines whether
 */
class OutputStatisticsParams
{
public:
    OutputStatisticsParams();

    bool output_node_load;
    bool output_box_load;
    bool output_stream_rate;
    bool output_selectivity;
    bool output_box_cost;

    /**
     * Ouput the DisplayParams object to a output stream
     */
    friend ostream& operator<<(ostream& os, const OutputStatisticsParams& ob);

    /**
     * Read in parameters from a istream
     */
    friend istream& operator>>(istream& is, OutputStatisticsParams& ob);
};



/**
 * A structure defining all the simulation parameters.
 */
class LMParams
{

  // Attribute(s)
public:
    /**
     * The parameters for the load distribution method.
     */
    static LoadDistributionMethod distribution_method;

    /**
     * The parameters for the load distributor.
     */
    static LoadDistributionParams distribution_param;

    /**
     * The parameters for the distribution time.
     */
    static DistributionTimeParams time_param;

    /**
     * The parameters for display setting.
     */
    static DisplayParams display_param;

    /**
     * The parameters for output setting.
     */
    static OutputStatisticsParams output_statistics_params;

    /**
     * The parameters for runtime setting
     */
    static RunTimeParams runtime_param;

  // Method(s)
public:

    /**
     * Ouput the LoadDistributionMethodParams object to a output stream
     */
    static ostream& output_params(ostream& os);

    /**
     * Read parameters from a input stream
     */
    static istream& read_params(istream& is);
};

BOREALIS_NAMESPACE_END
#endif


