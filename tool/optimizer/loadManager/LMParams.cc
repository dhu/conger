#include <sstream>
#include "LMParams.h"

BOREALIS_NAMESPACE_BEGIN

LoadDistributionMethod::LoadDistributionMethod()
{
    warmup_load_distribution_active = false;
    initial_load_distribution_active = true;
    dynamic_load_distribution_active = true;
    static_load_distribution_active = false;
    operator_clustering_active = true;

    initial_distribution_score_function_type = SF_REDUCED_LOAD_VARIANCE;
    dynamic_distribution_score_function_type = SF_REDUCED_LOAD_VARIANCE;

    pairwise_method = PLDM_SELECTIVE_EXCHANGE;
    pairwise_refine_correlation = true;

    selective_exchange = false;
    neighboring_nodes_first = false;
    uniform_bandwidth_allocation = false;
    adaptive_bandwidth = false;
}

ostream& operator<<(ostream& os, const LoadDistributionMethod& ob)
{
    os << "%LoadDistributionMethod" << endl;
    os << "warmupLoadDistributionActive: " << ob.warmup_load_distribution_active << endl;
    os << "initialLoadDistributionActive: " << ob.initial_load_distribution_active << endl;
    os << "dynamicLoadDistributionActive: " << ob.dynamic_load_distribution_active << endl;
    os << "staticLoadDistributionActive: " << ob.static_load_distribution_active << endl;
    os << "initialDistributionScoreFunctionType: " << (int)ob.initial_distribution_score_function_type << endl;
    os << "dynamicDistributionScoreFunctionType: " << (int)ob.dynamic_distribution_score_function_type << endl;
    os << "pairwise_method: " << ob.pairwise_method << endl;
    os << "pairwise_refine_correlation: " << ob.pairwise_refine_correlation << endl;
    os << "selectiveExchange: " << ob.selective_exchange << endl;
    os << "neighboringNodesFirst: " << ob.neighboring_nodes_first << endl;
    os << "uniformBandwidthAllocation: " << ob.uniform_bandwidth_allocation << endl;
    os << "adaptiveBandwidth: " << ob.adaptive_bandwidth << endl;
    return os;
};

istream& operator>>(
        istream& is,
        LoadDistributionMethod& ob)
{
    char buf[1024];
    bool start = false;
    while(!is.eof())
    {
        is.getline(buf, sizeof(buf));
        istringstream line(buf);
        line >> buf;

        if (!start)
        {
            if (strcmp(buf, "%LoadDistributionMethod") == 0)
                start = true;
            continue;
        }

        if (strcmp(buf, "")== 0)
            break;
        else if (strcmp(buf, "warmupLoadDistributionActive:") == 0)
              line >> ob.warmup_load_distribution_active;
        else if (strcmp(buf, "initialLoadDistributionActive:") == 0)
              line >> ob.initial_load_distribution_active;
        else if (strcmp(buf, "dynamicLoadDistributionActive:") == 0)
              line >> ob.dynamic_load_distribution_active;
        else if (strcmp(buf, "staticLoadDistributionActive:") == 0)
              line >> ob.static_load_distribution_active;
        else if (strcmp(buf, "operatorClusteringActive:") == 0)
              line >> ob.operator_clustering_active;
        else if (strcmp(buf, "initialDistributionScoreFunctionType:") == 0)
              line >> (int&)ob.initial_distribution_score_function_type;
        else if (strcmp(buf, "dynamicDistributionScoreFunctionType:") == 0)
              line >> (int&)ob.dynamic_distribution_score_function_type;
        else if (strcmp(buf, "pairwise_method:") == 0)
              line >> (int&)ob.pairwise_method;
        else if (strcmp(buf, "pairwise_refine_correlation:") == 0)
              line >> ob.pairwise_refine_correlation;
        else if (strcmp(buf, "selectiveExchange:") == 0)
              line >> ob.selective_exchange;
        else if (strcmp(buf, "neighboringNodesFirst:") == 0)
              line >> ob.neighboring_nodes_first;
        else if (strcmp(buf, "uniformBandwidthAllocation:") == 0)
              line >> ob.uniform_bandwidth_allocation;
        else if (strcmp(buf, "adaptiveBandwidth:") == 0)
              line >> ob.adaptive_bandwidth;
        else
              cout << "Unknown parameter for LoadDistributionMethod: " << buf << endl;
    }
    return is;
};

LoadDistributionParams::LoadDistributionParams()
{
    initial_warm_up_period = 10;
    statistics_sample_period = 1.0;
    number_of_samples = 10;
    overloaded_threshold = 0.90;
    load_balancing_threshold = 0.2;
    load_selection_threshold = 0.005;
    selective_exchange_score_theshold = 0.2;
    bandwidth_reduction_activation_threshold = 0.8;
    divergent_coefficient = 1.0;
    maximum_cluster_size = 10;
    number_of_random_static_distributions = 0;
}

ostream& operator<<(ostream& os, const LoadDistributionParams& ob)
{
    os << "%LoadDistributionParams" << endl;
    os << "initial_warm_up_period: " << ob.initial_warm_up_period << endl;
    os << "statisticsSamplePeriod: " << ob.statistics_sample_period << endl;
    os << "numberOfSamples: " << ob.number_of_samples << endl;
    os << "overloadedThreshold: " << ob.overloaded_threshold << endl;
    os << "loadBalancingThreshold: " << ob.load_balancing_threshold << endl;
    os << "loadSelectionThreshold: " << ob.load_selection_threshold << endl;
    os << "selective_exchange_score_theshold: " << ob.selective_exchange_score_theshold << endl;
    os << "bandwidthReductionActivationThreshold: " << ob.bandwidth_reduction_activation_threshold << endl;
    os << "divergentCoefficient: " << ob.divergent_coefficient << endl;
    os << "maximumClusterSize: " << ob.maximum_cluster_size << endl;
    os << "number_of_random_static_distributions: " << ob.number_of_random_static_distributions << endl;
    return os;
};

istream& operator>>(
        istream& is,
        LoadDistributionParams& ob)
{
    char buf[1024];
    bool start = false;
    while(!is.eof())
    {
        is.getline(buf, sizeof(buf));
        istringstream line(buf);
        line >> buf;

        if (!start)
        {
            if (strcmp(buf, "%LoadDistributionParams") == 0)
                start = true;
            continue;
        }

        if (strcmp(buf, "")== 0)
            break;
        else if (strcmp(buf, "initial_warm_up_period:") == 0)
              line >> ob.initial_warm_up_period;
        else if (strcmp(buf, "statisticsSamplePeriod:") == 0)
              line >> ob.statistics_sample_period;
        else if (strcmp(buf, "numberOfSamples:") == 0)
              line >> ob.number_of_samples;
        else if (strcmp(buf, "overloadedThreshold:") == 0)
              line >> ob.overloaded_threshold;
        else if (strcmp(buf, "loadBalancingThreshold:") == 0)
              line >> ob.load_balancing_threshold;
        else if (strcmp(buf, "loadSelectionThreshold:") == 0)
              line >> ob.load_selection_threshold;
        else if (strcmp(buf, "selective_exchange_score_theshold:") == 0)
              line >> ob.selective_exchange_score_theshold;
        else if (strcmp(buf, "bandwidthReductionActivationThreshold:") == 0)
              line >> ob.bandwidth_reduction_activation_threshold;
        else if (strcmp(buf, "divergentCoefficient:") == 0)
              line >> ob.divergent_coefficient;
        else if (strcmp(buf, "maximumClusterSize:") == 0)
              line >> ob.maximum_cluster_size;
        else if (strcmp(buf, "number_of_random_static_distributions:") == 0)
              line >> ob.number_of_random_static_distributions;
        else
              cout << "Unknown parameter for LoadDistributionParams: " << buf << endl;
    }
    return is;
};


DistributionTimeParams::DistributionTimeParams()
{
    load_distribution_starting_delay = 10;
    load_distribution_interval = 10;
}

ostream& operator<<(ostream& os, const DistributionTimeParams& ob)
{
    os << "%DistributionTimeParams" << endl;
    os << "loadDistributionStartingDelay: " << ob.load_distribution_starting_delay << endl;
    os << "loadDistributionInterval: " << ob.load_distribution_interval << endl;
    return os;
};

istream& operator>>(
        istream& is,
        DistributionTimeParams& ob)
{
    char buf[1024];
    bool start = false;
    while(!is.eof())
    {
        is.getline(buf, sizeof(buf));
        istringstream line(buf);
        line >> buf;

        if (!start)
        {
            if (strcmp(buf, "%DistributionTimeParams") == 0)
                start = true;
            continue;
        }

        if (strcmp(buf, "")== 0)
            break;
        else if (strcmp(buf, "loadDistributionStartingDelay:") == 0)
              line >> ob.load_distribution_starting_delay;
        else if (strcmp(buf, "loadDistributionInterval:") == 0)
              line >> ob.load_distribution_interval;
        else
              cout << "Unknown parameter for DistributionTimeParams: " << buf << endl;
    }
    return is;
};

RunTimeParams::RunTimeParams()
{
    move_box_active = true;
    warm_up_time = 10;
    start_output_to_files = false;
}

ostream& operator<<(ostream& os, const RunTimeParams& ob)
{
    os << "%RunTimeParams" << endl;
    os << "move_box_active: " << ob.move_box_active << endl;
    os << "warmUpTime: " << ob.warm_up_time << endl;
    os << "status_filename: " << ob.status_filename << endl;
    os << "load_manager_path: " << ob.load_manager_path << endl;
    os << "output_dir: " << ob.output_dir << endl;
    os << "output_prefix: " << ob.output_prefix << endl;
    return os;
};

DisplayParams::DisplayParams()
{
    show_average_statistics = true;
    show_basic_info = true;
    show_query_network = false;
    show_box_location = true;
    show_gathered_statistics = false;
    show_lmstatistics_manager = false;
    show_node_load = true;
    show_box_load = false;
    show_stream_rate = false;
    show_selectivity = false;
    show_box_cost = false;
}

ostream& operator<<(ostream& os, const DisplayParams& ob)
{
    os << "%DisplayParams" << endl;
    os << "show_average_statistics: " << ob.show_average_statistics << endl;
    os << "show_basic_info: " << ob.show_basic_info << endl;
    os << "show_query_network: " << ob.show_query_network << endl;
    os << "show_box_location: " << ob.show_box_location << endl;
    os << "show_gathered_statistics: " << ob.show_gathered_statistics << endl;
    os << "show_lmstatistics_manager: " << ob.show_lmstatistics_manager << endl;
    os << "show_node_load: " << ob.show_node_load << endl;
    os << "show_box_load: " << ob.show_box_load << endl;
    os << "show_stream_rate: " << ob.show_stream_rate << endl;
    os << "show_selectivity: " << ob.show_selectivity << endl;
    os << "show_box_cost: " << ob.show_box_cost << endl;
    return os;
};

istream& operator>>(
        istream& is,
        DisplayParams& ob)
{
    char buf[1024];
    bool start = false;
    while(!is.eof())
    {
        is.getline(buf, sizeof(buf));
        istringstream line(buf);
        line >> buf;

        if (!start)
        {
            if (strcmp(buf, "%DisplayParams") == 0)
                start = true;
            continue;
        }

        if (strcmp(buf, "")== 0)
            break;
        else if (strcmp(buf, "show_average_statistics:") == 0)
              line >> ob.show_average_statistics;
        else if (strcmp(buf, "show_basic_info:") == 0)
              line >> ob.show_basic_info;
        else if (strcmp(buf, "show_query_network:") == 0)
              line >> ob.show_query_network;
        else if (strcmp(buf, "show_box_location:") == 0)
              line >> ob.show_box_location;
        else if (strcmp(buf, "show_gathered_statistics:") == 0)
              line >> ob.show_gathered_statistics;
        else if (strcmp(buf, "show_lmstatistics_manager:") == 0)
              line >> ob.show_lmstatistics_manager;
        else if (strcmp(buf, "show_node_load:") == 0)
              line >> ob.show_node_load;
        else if (strcmp(buf, "show_box_load:") == 0)
              line >> ob.show_box_load;
        else if (strcmp(buf, "show_stream_rate:") == 0)
              line >> ob.show_stream_rate;
        else if (strcmp(buf, "show_selectivity:") == 0)
              line >> ob.show_selectivity;
        else if (strcmp(buf, "show_box_cost:") == 0)
              line >> ob.show_box_cost;
        else
              cout << "Unknown parameter for DistributionTimeParams: " << buf << endl;
    }
    return is;
};

OutputStatisticsParams::OutputStatisticsParams()
{
    output_node_load = true;
    output_box_load = false;
    output_stream_rate = true;
    output_selectivity = false;
    output_box_cost = false;
}

ostream& operator<<(ostream& os, const OutputStatisticsParams& ob)
{
    os << "%OutputStatisticsParams" << endl;
    os << "output_node_load: " << ob.output_node_load << endl;
    os << "output_box_load: " << ob.output_box_load << endl;
    os << "output_stream_rate: " << ob.output_stream_rate << endl;
    os << "output_selectivity: " << ob.output_selectivity << endl;
    os << "output_box_cost: " << ob.output_box_cost << endl;
    return os;
};

istream& operator>>(
        istream& is,
        OutputStatisticsParams& ob)
{
    char buf[1024];
    bool start = false;
    while(!is.eof())
    {
        is.getline(buf, sizeof(buf));
        istringstream line(buf);
        line >> buf;

        if (!start)
        {
            if (strcmp(buf, "%OutputStatisticsParams") == 0)
                start = true;
            continue;
}

        if (strcmp(buf, "")== 0)
            break;
        if (strcmp(buf, "output_node_load:") == 0)
              line >> ob.output_node_load;
        else if (strcmp(buf, "output_box_load:") == 0)
              line >> ob.output_box_load;
        else if (strcmp(buf, "output_stream_rate:") == 0)
              line >> ob.output_stream_rate;
        else if (strcmp(buf, "output_selectivity:") == 0)
              line >> ob.output_selectivity;
        else if (strcmp(buf, "output_box_cost:") == 0)
              line >> ob.output_box_cost;
        else
              cout << "Unknown parameter for OutputStatisticsParams: " << buf << endl;
}
    return is;
};

LoadDistributionMethod LMParams::distribution_method;
LoadDistributionParams LMParams::distribution_param;
DistributionTimeParams LMParams::time_param;
DisplayParams LMParams::display_param;
OutputStatisticsParams LMParams::output_statistics_params;
RunTimeParams LMParams::runtime_param;

ostream& LMParams::output_params(ostream& os)
{
    os << LMParams::distribution_method << endl;
    os << LMParams::distribution_param << endl;
    os << LMParams::time_param << endl;
    os << LMParams::display_param << endl;
    os << LMParams::output_statistics_params << endl;
    os << LMParams::runtime_param << endl;
    return os;
}

istream& LMParams::read_params(istream& is)
{
    is >> LMParams::distribution_method;
    is >> LMParams::distribution_param;
    is >> LMParams::time_param;
    is >> LMParams::display_param;
    is >> LMParams::output_statistics_params;
    return is;
};

BOREALIS_NAMESPACE_END
