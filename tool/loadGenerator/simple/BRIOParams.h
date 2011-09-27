#ifndef BRIOParams_H
#define BRIOParams_H

#include <common.h>

BOREALIS_NAMESPACE_BEGIN

/**
 * A structure defining parameters for load distributors methods
 */
struct BRIOParams
{

public:
    /**
     * Returns the singleton handle 
     */
    static BRIOParams* get_singleton();

    /**
     * Number of fields
     */
    int number_of_fields;

    /**
     */
    double batch_interval;

    /**
     * Uniform input data probability
     */
    double uniform_input_distr_prob;

    /**
     * Random walk input data probability
     */
    double random_walk_input_distr_prob;

    /**
     * Constant input rate probability
     */
    double const_input_rate_prob;

    /**
     * Trace input rate probability
     */
    double trace_input_rate_prob;

    /**
     * Is true if TCP trace data is used
     */
    bool use_tcp_trace;

    /**
     * Input rate multiplier lower bound
     */
    static double rate_multiplier;

    /**
     * Input rate multiplier lower bound
     */
    double input_rate_multiplier_lower_bound;

    /**
     * Input rate multiplier lower bound
     */
    double input_rate_multiplier_upper_bound;

    /**
     * Input rate multiplier lower bound
     */
    int batch_size_lower_bound;

    /**
     * Input rate multiplier lower bound
     */
    int batch_size_upper_bound;

    /**
     * Stream rate mode
     * 0: normal
     * 1: increasing input rate // need to be implemented
     * 2: random rate
     */
    int stream_rate_mode;

    /**
     * The interval that the batch size increase
     */
    int batch_size_increasing_interval;

    /**
     * Random rate duration (sec)
     */
    double random_rate_duration;

    /**
     * Random rate duration (sec)
     */
    double idle_duration;

    /**
     * The name of the rate multiplier file
     */
    static string rate_multiplier_file_name;

    /**
     * The path of the BRInputGenerator
     */
    static string input_generator_path;

private:
    /**
     * Set default parameters
     */
    BRIOParams();
};

/**
 * Ouput the parameters
 */
ostream& operator<<(ostream& os, const BRIOParams& ob);

/**
 * Read in parameters from a istream
 */
istream& operator>>(istream& is, BRIOParams& ob);

BOREALIS_NAMESPACE_END
#endif


