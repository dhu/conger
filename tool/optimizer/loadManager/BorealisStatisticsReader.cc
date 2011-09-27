#include "BorealisStatisticsReader.h"
#include "LMCommon.h"
#include "LMStatisticsManager.h"
#include "LMQueryNetwork.h"
#include "LMNodeManager.h"
#include "LMParams.h"
#include <Statistics.h>
#include <TSStats.h>
#include <set>

//#define OUTPUT_LATENCY

BOREALIS_NAMESPACE_BEGIN

bool BorealisStatisticsReader::read_statistics(
        map<string, Statistics> & nodes_statistics,
        double precision,
        int window_size)
{
#ifdef OUTPUT_LATENCY
    // record output statistics
    vector<double> tuple_num_per_window;
    vector<double> latency_per_window;
    vector<double> latency_sqr_per_window;
    static double tuple_num;
    static double latency;
    static double latency_sqr;

    for (int i = 0; i < window_size; ++i)
    {
        tuple_num_per_window.push_back(0.0);
        latency_per_window.push_back(0.0);
        latency_sqr_per_window.push_back(0.0);
    }
#endif


    // update statistics for each node
    for (map<string, Statistics>::iterator it = nodes_statistics.begin();
            it != nodes_statistics.end();
            ++it)
    {
        Statistics& stats = it->second;
        if (stats.empty())
            continue;
        const IdType& node_id = it->first;

        if (LMParams::display_param.show_gathered_statistics)
        {
            NOTICE << "\nStatistics received from node " << it->first
                    << "\n" << stats.as_string();
        }

        // update box cost statistics
        try
        {
            const TSStats::IDTimeSeries& box_cost_tss =
                    stats.copy_tsstats("Cost", window_size, precision, 0)._tsstats;
            for (TSStats::IDTimeSeries::const_iterator it = box_cost_tss.begin();
                      it != box_cost_tss.end();
                      ++it)
            {
                for (int i = window_size - 1; i > -1; --i)
                {
                    LMStatisticsManager::get_singleton()->update_box_cost(
                            it->first, it->second.get_sample(i));
                }
            }
        }
        catch (...)
        {
            return false;
        }

        // update box port selectivity
        try
        {
            const TSStats::IDTimeSeries& box_selectivity_tss =
                    stats.copy_tsstats("Selectivity", window_size, precision, 0)._tsstats;;
            for (TSStats::IDTimeSeries::const_iterator it = box_selectivity_tss.begin();
                      it != box_selectivity_tss.end();
                      ++it)
            {
                for (int i = window_size - 1; i > -1; --i)
                {
                    LMStatisticsManager::get_singleton()->update_box_selectivity(
                            it->first, it->second.get_sample(i));
                }
            }
        }
        catch (...)
        {
            return false;
        }


        // get all the boxes on a node
        const IdSet& boxes =
            LMNodeManager::get_singleton()->get_lmnode(node_id).get_boxes();
        // update input arc stream rate
        try
        {
            const TSStats::IDTimeSeries& stream_rate_tss =
                    stats.copy_tsstats("StreamRate", window_size, precision, 0)._tsstats;;

            for (TSStats::IDTimeSeries::const_iterator
                 it = stream_rate_tss.begin();
                 it != stream_rate_tss.end();
                 ++it)
            {
                const string& stream_name = it->first;

                // get arc ids from stream name
                try
                {
                    const IdSet& arc_ids =
                        LMQueryNetwork::get_singleton()->get_arc_ids(stream_name);

                    for (IdSet::const_iterator ait = arc_ids.begin();
                            ait != arc_ids.end();
                            ++ait)
                    {
                        const IdType& arc_id = *ait;

                        // check whether the arc is a input arc
                        if (LMQueryNetwork::get_singleton()->is_input_arc(arc_id))
                        {
                            LMBox* dest_box =
                                LMQueryNetwork::get_singleton()->get_arc(arc_id)->
                                get_destination_box();
                            // ignore an stream that do not have output box set
                            // yet
                            if (dest_box == 0)
                                continue;

                            // check whether the destination box of the arc is
                            // in the given node, only update arc rate if so
                            if (boxes.find(dest_box->get_id()) != boxes.end())
                            {
                                for (int i = window_size - 1; i > -1; --i)
                                {
                                    LMStatisticsManager::get_singleton()->update_arc_rate(
                                            *ait, it->second.get_sample(i));
                                }
                            }
                        }
                    }
                }
                catch(...)
                {
                }
            }
        }
        catch (...)
        {
            return false;
        }


#ifdef OUTPUT_LATENCY
        // update output latency
        try
        {
            const TSStats::IDTimeSeries& tss = stats.copy_tsstats(
                    "TupleNum", window_size, precision, 0)._tsstats;;

            for (TSStats::IDTimeSeries::const_iterator it = tss.begin();
                      it != tss.end();
                      ++it)
            {
                if (LMQueryNetwork::get_singleton()->is_output_stream(it->first))
                {
                    for (int i = 0; i < window_size; ++i)
                    {
                        tuple_num_per_window[i] += it->second.get_sample(window_size - i - 1);
                    }
                }
            }
        }
        catch (...)
        {
        }

        try
        {
            const TSStats::IDTimeSeries& tss =
                    stats.copy_tsstats("OutLatency", window_size, precision, 0)._tsstats;;

            for (TSStats::IDTimeSeries::const_iterator it = tss.begin();
                      it != tss.end();
                      ++it)
            {
                if (LMQueryNetwork::get_singleton()->is_output_stream(it->first))
                {
                    for (int i = 0; i < window_size; ++i)
                    {
                        latency_per_window[i] += it->second.get_sample(window_size - i - 1);
                    }
                }
            }
        }
        catch (...)
        {
        }

        try
        {
            const TSStats::IDTimeSeries& tss =
                    stats.copy_tsstats("OutLatencySqr", window_size, precision, 0)._tsstats;;

            for (TSStats::IDTimeSeries::const_iterator it = tss.begin();
                      it != tss.end();
                      ++it)
            {
                if (LMQueryNetwork::get_singleton()->is_output_stream(it->first))
                {
                    for (int i = 0; i < window_size; ++i)
                    {
                        latency_sqr_per_window[i] += it->second.get_sample(window_size - i - 1);
                    }
                }
            }
        }
        catch (...)
        {
        }
#endif
    }


#ifdef OUTPUT_LATENCY
    if (LMParams::runtime_param.output_dir == "")
    {
        return true;
    }

    ofstream fs;
    string latency_filename = LMParams::runtime_param.output_dir + "/data/" +
    LMParams::runtime_param.output_prefix;
    latency_filename += "latency.DAT";

    NOTICE << "HERE ^^^^^^^^^^^^^^^^^^^^^^^^^^";

    static double first;
    if (first)
    {
        first = false;
        fs.open(latency_filename.c_str(), ios::out);
    }
    else
    {
        fs.open(latency_filename.c_str(), ios::app);
    }

    if (!fs)
    {
        WARN << "can not open output latency file : " << latency_filename;
    }

    for (int i = 0; i < window_size; ++i)
    {
        if (tuple_num_per_window[i] < 0.5)
            continue;

        tuple_num += tuple_num_per_window[i];
        latency += latency_per_window[i];
        latency_sqr += latency_sqr_per_window[i];

        double average_latency_perwindow =
            latency_per_window[i] / tuple_num_per_window[i];
        double standard_deviation_perwindow =
            sqrt( latency_sqr_per_window[i] / tuple_num_per_window[i] -
              (average_latency_perwindow * average_latency_perwindow));

        double average_latency =
            latency / tuple_num;
        double standard_deviation =
            sqrt(latency_sqr / tuple_num - average_latency * average_latency );


        fs << setw(15) << average_latency_perwindow << " ";
        fs << setw(15) << standard_deviation_perwindow << " ";
        fs << setw(15) << average_latency << " ";
        fs << setw(15) << standard_deviation << " ";
        fs << setw(5) << tuple_num_per_window[i] << " ";
        fs << setw(10) << tuple_num << " ";
        fs << "\n";

    }

    fs.close();
#endif


    return true;
}

BOREALIS_NAMESPACE_END
