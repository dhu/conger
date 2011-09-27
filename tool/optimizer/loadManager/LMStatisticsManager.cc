#include "LMStatisticsManager.h"
#include "LMParams.h"
#include "LMQueryNetwork.h"
#include "TimeSeries.h"
#include "LMNodeManager.h"



BOREALIS_NAMESPACE_BEGIN

const string LMStatisticsManager::STREAM_RATE = "STREAM_RATE";
const string LMStatisticsManager::BOX_COST = "BOX_COST";
const string LMStatisticsManager::SELECTIVITY = "SELECTIVITY";
const string LMStatisticsManager::BOX_LOAD = "BOX_LOAD";
const string LMStatisticsManager::NODE_LOAD = "NODE_LOAD";


LMStatisticsManager* LMStatisticsManager::_singleton = 0;

LMStatisticsManager::LMStatisticsManager():
    _statistics_pool(LMParams::distribution_param.number_of_samples)
{
        _load_evaluation_interval_num = 
            LMParams::distribution_param.number_of_samples;
        _bandwidth_evaluation_interval_num = 
            LMParams::distribution_param.number_of_samples;
}

//-----------------------------------------------------
// Function that reads the statistics Timeseries
//-----------------------------------------------------
TimeSeries LMStatisticsManager::get_boxes_load_time_series(const IdSet& box_ids)
{
    TimeSeries ts(LMParams::distribution_param.number_of_samples);
    for (IdSet::const_iterator it = box_ids.begin();
            it != box_ids.end();
            ++it)
        ts += get_box_load_time_series(*it);

    return ts;
}


TimeSeries LMStatisticsManager::get_arcs_bandwidth_time_series(const IdSet& arc_ids)
{
    TimeSeries ts(LMParams::distribution_param.number_of_samples);
    for (IdSet::const_iterator ait = arc_ids.begin();
              ait != arc_ids.end();
              ++ait)
    {
        ts += get_arc_bandwidth_time_series(*ait);
    }
    return ts;
}

//-----------------------------------------------------
// Function that compute the statistics value
//-----------------------------------------------------
TimeSeries LMStatisticsManager::get_system_total_load_time_series()
{
    TimeSeries ts(LMParams::distribution_param.number_of_samples);
    const StatisticsMap& sm = _statistics_pool.get_statistics(NODE_LOAD);
    for (StatisticsMap::const_iterator it = sm.begin();
              it != sm.end();
              ++it)
    {
        ts += it->second;
    }
    return ts;
}

double LMStatisticsManager::get_system_total_load()
{
    return get_average_load(get_system_total_load_time_series());
}


double LMStatisticsManager::get_average_node_load()
{
    return get_system_total_load() / _statistics_pool.get_statistics(NODE_LOAD).size();
}

//-----------------------------------------------------
// Function that used to update the statistics
//-----------------------------------------------------

void LMStatisticsManager::update_statistics(int number_of_new_samples)
{
    delete_obsolete_statistics();
    update_ideal_stream_rate_and_box_load_statistics(number_of_new_samples);
    update_node_load_statistics();
}

void LMStatisticsManager::set_parameters(LoadDistributionPeriod period)
{
    switch (period)
    {
        case LDP_WARMUP:
            _load_evaluation_interval_num = 1;
            _bandwidth_evaluation_interval_num = 1;
            break;
        default:
            _load_evaluation_interval_num =
                LMParams::distribution_param.number_of_samples;
            _bandwidth_evaluation_interval_num =
                LMParams::distribution_param.number_of_samples;
    }
}

void LMStatisticsManager::delete_obsolete_statistics()
{
    DEBUG << "deleting obsolete statistics ... ";

    // get all the existing box name
    const LMBoxMap& boxes = LMQueryNetwork::get_singleton()->get_boxes();

    // delete statistics for all unsed boxes
    StatisticsMap& box_load = _statistics_pool.get_statistics(BOX_LOAD);
    StatisticsMap::iterator it = box_load.begin();
    while (it != box_load.end())
    {
        if (boxes.find(it->first) != boxes.end())
        {
            ++it;
            continue;
        }
        // delete box statistics
        StatisticsMap::iterator tmp = it;
        ++it;
        box_load.erase(tmp);
    }

    StatisticsMap& box_cost = _statistics_pool.get_statistics(BOX_COST);
    it = box_cost.begin();
    while (it != box_cost.end())
    {
        if (boxes.find(it->first) != boxes.end())
        {
            ++it;
            continue;
        }
        StatisticsMap::iterator tmp = it;
        ++it;
        box_cost.erase(tmp);
    }

    StatisticsMap& selectivities = _statistics_pool.get_statistics(SELECTIVITY);
    it = selectivities.begin();
    while (it != selectivities.end())
    {
        IdType box_id = get_box_id(it->first);
        if (boxes.find(box_id) != boxes.end())
        {
            ++it;
            continue;
        }
        StatisticsMap::iterator tmp = it;
        ++it;
        selectivities.erase(tmp);
    }

    // delete statitics for all unexisting arcs
    const LMArcMap& arcs = LMQueryNetwork::get_singleton()->get_arcs();
    StatisticsMap& stream_rate = _statistics_pool.get_statistics(STREAM_RATE);
    it = stream_rate.begin();
    while (it != stream_rate.end())
    {
        if (arcs.find(it->first) != arcs.end())
        {
            ++it;
            continue;
        }
        StatisticsMap::iterator tmp = it;
        ++it;
        stream_rate.erase(tmp);
    }
}

void LMStatisticsManager::update_ideal_stream_rate_and_box_load_statistics(
        int number_of_new_samples)
{
    // Set the inital checked arcs and going arcs to be the input arcs.
    LMArcMap checked_arcs(LMQueryNetwork::get_singleton()->get_input_arcs());
    LMArcMap going_arcs(checked_arcs);
    LMBoxSet updated_boxes;

    // Go along each going arc until the going arcs is empty
    while (!going_arcs.empty())
    {
        LMArc* arc = going_arcs.begin()->second;

        // update the output rate and load of the destination box of the going arc
        LMBox* box = arc->get_destination_box();

        // continue if no destination box exist
        if (box == 0)
        {
            going_arcs.erase(arc->get_id());
            continue;
        }

        // continue if the destination box has been updated
        if (updated_boxes.find(box) != updated_boxes.end())
        {
            going_arcs.erase(arc->get_id());
            continue;
        }

        // comput the total input rate of the destination box
        TimeSeries total_input_rate(LMParams::distribution_param.number_of_samples);
        bool all_input_arcs_rate_known = true;
        const LMArcMap& input_arcs = box->get_input_arcs();
        for (LMArcMap::const_iterator it = input_arcs.begin();
                it != input_arcs.end();
                ++it)
        {
            IdType input_arc_id = it->first;

            // check whether the rat of the arc is known
            if (checked_arcs.find(input_arc_id) == checked_arcs.end())
            {
                // The arc has not been checked yet, so erase  the going arc
                going_arcs.erase(arc->get_id());
                all_input_arcs_rate_known = false;
                break;
            }
            else
            {
                total_input_rate +=
                    _statistics_pool.get_statistics(STREAM_RATE, input_arc_id);
            }
        }

        // if any input arc rate is not known, then stop the going arc
        // and continue with the next arc
        if(!all_input_arcs_rate_known)
            continue;

        TimeSeries& box_load_statistics =
            _statistics_pool.get_statistics(BOX_LOAD, box->get_id());
        TimeSeries& box_cost_statistics =
            _statistics_pool.get_statistics(BOX_COST, box->get_id());
        for (int i = number_of_new_samples - 1; i > -1; --i)
        {
            box_load_statistics.slide_sample(
                    box_cost_statistics.get_sample(i) * total_input_rate.get_sample(i));
        }

        updated_boxes.insert(box);

        // update the ouput rate of the box
        // for each output port, compute data rate and update arc rate
        // statistics
        const map<PortId, LMArcMap>&  output_arcs = box->get_output_port_arc_map();
        for (map<PortId, LMArcMap>::const_iterator it = output_arcs.begin();
                it != output_arcs.end();
                ++it)
        {
            const PortId& port = it->first;
            const LMArcMap& port_arcs = it->second;

            // compute arc rate for the port
            TimeSeries& selectivity = _statistics_pool.get_statistics(
                    SELECTIVITY, get_selectivity_id(box->get_id(), port));

            TimeSeries arc_rate = dot_product(total_input_rate, selectivity);

            // update arc rate for each arc
            for (LMArcMap::const_iterator ait = port_arcs.begin();
                    ait != port_arcs.end();
                    ++ait)
            {
                const IdType& ouput_arc_id = ait->first;

                // update arc rate
                TimeSeries& arc_rate_statistics =
                    _statistics_pool.get_statistics(STREAM_RATE, ouput_arc_id);

                for (int i = number_of_new_samples - 1; i > -1; --i)
                {
                    arc_rate_statistics.slide_sample(arc_rate.get_sample(i));
                }

                // Add the arc to going arcs
                going_arcs.insert(*ait);

                // Add the arc to checked arcs
                checked_arcs.insert(*ait);
            }
        }

        // erase the arc being check from on going arc
        going_arcs.erase(arc->get_id());
    }

}

void LMStatisticsManager::update_node_load_statistics()
{
    StatisticsMap& node_load_statistics = _statistics_pool.get_statistics(NODE_LOAD);
    node_load_statistics.clear();

    for (LMNodeMap::const_iterator
              it = LMNodeManager::get_singleton()->get_lmnodes().begin();
            it != LMNodeManager::get_singleton()->get_lmnodes().end();
            ++it)
    {
        const LMNode& node = it->second;
        const IdSet& box_ids = node.get_boxes();

        TimeSeries& ts = _statistics_pool.get_statistics(NODE_LOAD, node.get_id());
        for (IdSet::const_iterator it = box_ids.begin();
                  it != box_ids.end();
                  ++it)
        {
            ts += _statistics_pool.get_statistics(BOX_LOAD, *it);
        }
    }
}

void LMStatisticsManager::update_node_load_statistics_for_adding_boxes(
        const IdType& node_id,
        const IdSet& box_ids)
{
    TimeSeries& ts =
        _statistics_pool.get_statistics(NODE_LOAD, node_id);

    for (IdSet::const_iterator it = box_ids.begin();
            it != box_ids.end();
            ++it)
    {
        const TimeSeries& box_load_ts =
            _statistics_pool.get_statistics(BOX_LOAD, *it);
        ts += box_load_ts;
    }
}

void LMStatisticsManager::update_node_load_statistics_for_box_migration(
        const IdType& sender_id,
        const IdType& receiver_id,
        const IdSet& box_ids)
{
    TimeSeries& sender_load_ts =
        _statistics_pool.get_statistics(NODE_LOAD, sender_id);
    TimeSeries& receiver_load_ts =
        _statistics_pool.get_statistics(NODE_LOAD, receiver_id);

    for (IdSet::const_iterator it = box_ids.begin();
            it != box_ids.end();
            ++it)
    {
        const TimeSeries& box_load_ts =
            _statistics_pool.get_statistics(BOX_LOAD, *it);
        sender_load_ts -= box_load_ts;
        receiver_load_ts += box_load_ts;
    }
}

void LMStatisticsManager::print_statistics_to_files()
{
    if (LMParams::runtime_param.output_dir == "")
        return;

    if (get_average_node_load() < 0.0000001)
        return;

    ofstream node_load_fs;
    ofstream box_load_fs;
    ofstream box_cost_fs;
    ofstream stream_rate_fs;
    ofstream box_selectivity_fs;

    int window_size = static_cast<int>(
            LMParams::time_param.load_distribution_interval /
            LMParams::distribution_param.statistics_sample_period);

    string filename = LMParams::runtime_param.output_dir + "/data/" +
        LMParams::runtime_param.output_prefix;

    static double first;
    if (first)
    {
        first = false;
        if (LMParams::output_statistics_params.output_node_load)
            node_load_fs.open((filename + "node_load.dat").c_str(), ios::out);
        if (LMParams::output_statistics_params.output_box_load)
            box_load_fs.open((filename + "box_load.dat").c_str(), ios::out);
        if (LMParams::output_statistics_params.output_box_cost)
            box_cost_fs.open((filename + "box_cost.dat").c_str(), ios::out);
        if (LMParams::output_statistics_params.output_stream_rate)
            stream_rate_fs.open((filename + "stream_rate.dat").c_str(), ios::out);
        if (LMParams::output_statistics_params.output_selectivity)
            box_selectivity_fs.open((filename + "selectivity.dat").c_str(), ios::out);
    }
    else
    {
        if (LMParams::output_statistics_params.output_node_load)
            node_load_fs.open((filename + "node_load.dat").c_str(), ios::app);
        if (LMParams::output_statistics_params.output_box_load)
            box_load_fs.open((filename + "box_load.dat").c_str(), ios::app);
        if (LMParams::output_statistics_params.output_box_cost)
            box_cost_fs.open((filename + "box_cost.dat").c_str(), ios::app);
        if (LMParams::output_statistics_params.output_stream_rate)
            stream_rate_fs.open((filename + "stream_rate.dat").c_str(), ios::app);
        if (LMParams::output_statistics_params.output_selectivity)
            box_selectivity_fs.open((filename + "selectivity.dat").c_str(), ios::app);
    }

    if (LMParams::output_statistics_params.output_node_load && node_load_fs)
    {
        StatisticsMap& sm = _statistics_pool.get_statistics(NODE_LOAD);
        for (int i = 0; i < window_size; ++i)
        {
            for (StatisticsMap::iterator it  = sm.begin();
                it != sm.end();
                ++it)
            {
                TimeSeries& ts = it->second;
                    node_load_fs << ts.get_sample(window_size - i - 1) << " ";
            }
            node_load_fs << endl;
        }
    }

    if (LMParams::output_statistics_params.output_box_load && box_load_fs)
    {
        StatisticsMap& sm = _statistics_pool.get_statistics(BOX_LOAD);
        for (int i = 0; i < window_size; ++i)
        {
            for (StatisticsMap::iterator it  = sm.begin();
                it != sm.end();
                ++it)
            {
            TimeSeries& ts = it->second;
                box_load_fs << ts.get_sample(window_size - i - 1) << " ";
            }
            box_load_fs << endl;
        }
    }

    if (LMParams::output_statistics_params.output_box_cost && box_cost_fs)
    {
        StatisticsMap& sm = _statistics_pool.get_statistics(BOX_COST);
        for (int i = 0; i < window_size; ++i)
        {
            for (StatisticsMap::iterator it  = sm.begin();
                it != sm.end();
                ++it)
            {
            TimeSeries& ts = it->second;
                box_cost_fs << ts.get_sample(window_size - i - 1) << " ";
            }
            box_cost_fs << endl;
        }
    }

    if (LMParams::output_statistics_params.output_stream_rate && stream_rate_fs)
    {
        StatisticsMap& sm = _statistics_pool.get_statistics(STREAM_RATE);
        for (int i = 0; i < window_size; ++i)
        {
            for (StatisticsMap::iterator it  = sm.begin();
                it != sm.end();
                ++it)
            {
            TimeSeries& ts = it->second;
                stream_rate_fs << ts.get_sample(window_size - i - 1) << " ";
            }
            stream_rate_fs << endl;
        }
    }

    if (LMParams::output_statistics_params.output_selectivity && box_selectivity_fs)
    {
        StatisticsMap& sm = _statistics_pool.get_statistics(SELECTIVITY);
        for (int i = 0; i < window_size; ++i)
        {
            for (StatisticsMap::iterator it  = sm.begin();
                it != sm.end();
                ++it)
            {
            TimeSeries& ts = it->second;
                box_selectivity_fs << ts.get_sample(window_size - i - 1) << " ";
            }
            box_selectivity_fs << endl;
        }
    }

    node_load_fs.close();
    box_load_fs.close();
    box_cost_fs.close();
    box_selectivity_fs.close();
    stream_rate_fs.close();
}



ostream& operator<<(ostream& os, const LMStatisticsManager& sm)
{
    os << "======================== LMStatisticsManager ========================" << endl;
    os << sm._statistics_pool;
    os << "=====================================================================" << endl;
    return os;
}

void LMStatisticsManager::output_statistics(
        ostream& os, 
        const string& statistics_name,
        bool output_average_statistics)
{
    if (output_average_statistics) 
    {
        _statistics_pool.output_average_statistics(os, statistics_name);
    }
    else
    {
        _statistics_pool.output_time_series_statistics(os, statistics_name);
    }
}

void LMStatisticsManager::output_average_statistics(ostream& os)
{
    os << "======================== LMStatisticsManager ========================" << endl;
    _statistics_pool.output_average_statistics(os);
    os << "=====================================================================" << endl;
}
        


BOREALIS_NAMESPACE_END
