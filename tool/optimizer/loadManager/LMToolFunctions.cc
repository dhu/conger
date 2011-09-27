#include "LMToolFunctions.h"
#include "LMNodeManager.h"
#include "LMStatisticsManager.h"

#include <cmath>


BOREALIS_NAMESPACE_BEGIN

using namespace StatisticsTools;

void LMToolFunctions::get_imbalanced_node_pairs(
                double load_balancing_threshold,
                double sender_min_load,
                IdPairVect& node_pairs,
                bool use_divergent_load)
{
    const LMNodeMap& lmnodes = LMNodeManager::get_singleton()->get_lmnodes();
    const IdSet& ignore_nodes = LMNodeManager::get_singleton()->get_ignore_nodes();
    DescendingIds nodes;

    for (LMNodeMap::const_iterator it = lmnodes.begin();
            it != lmnodes.end();
            ++it)
    {
        if (ignore_nodes.find(it->first) != ignore_nodes.end())
        {
            continue;
        }

        if (use_divergent_load)
        {
            nodes.insert(pair<double, IdType>(
                        LMStatisticsManager::get_singleton()->
                            get_node_divergent_load(it->first),
                        it->first));
        }
        else
        {
            nodes.insert(pair<double, IdType>(
                        LMStatisticsManager::get_singleton()->
                            get_node_average_load(it->first),
                        it->first));
        }
    }

    if (nodes.size() < 2)
        return;


    DescendingIds::iterator sit = nodes.end();
    DescendingIds::iterator rit = nodes.begin();
    --sit;

    while(sit != rit)
    {
        double sn_load = sit->first;
        double rn_load = rit->first;

        if (sn_load < sender_min_load)
            return;

        if (sn_load - rn_load < load_balancing_threshold)
            return;

        node_pairs.push_back(IdPair(sit->second, rit->second));

        --sit;
        if (sit == rit)
            return;
        ++rit;
    }
}

const IdType& LMToolFunctions::get_more_loaded_node_id(const IdPair& node_pair, bool use_divergent_load)
{
    if (use_divergent_load)
    {
        if (LMStatisticsManager::get_singleton()->get_node_divergent_load(node_pair.first) >
                LMStatisticsManager::get_singleton()->get_node_divergent_load(node_pair.second))
        {
            return node_pair.first;
        }
        else
            return node_pair.second;
    }
    else
    {
        if (LMStatisticsManager::get_singleton()->get_node_average_load(node_pair.first) >
                LMStatisticsManager::get_singleton()->get_node_average_load(node_pair.second))
        {
            return node_pair.first;
        }
        else
            return node_pair.second;
    }
}

const IdType& LMToolFunctions::get_less_loaded_node_id(const IdPair& node_pair, bool use_divergent_load)
{
    if (use_divergent_load)
    {
        if (LMStatisticsManager::get_singleton()->get_node_divergent_load(node_pair.first) >
                LMStatisticsManager::get_singleton()->get_node_divergent_load(node_pair.second))
        {
            return node_pair.second;
        }
        else
            return node_pair.first;
    }
    else
    {
        if (LMStatisticsManager::get_singleton()->get_node_average_load(node_pair.first) >
                LMStatisticsManager::get_singleton()->get_node_average_load(node_pair.second))
        {
            return node_pair.second;
        }
        else
            return node_pair.first;
    }
}

double LMToolFunctions::get_migration_load_variance_reduction(
                const IdType& sender_id,
                const IdType& receiver_id,
                const IdSet& box_ids)
{
    TimeSeries ts = LMStatisticsManager::get_singleton()->get_boxes_load_time_series(box_ids);

    double variacne_reduction  =
        2 * (covariance(LMStatisticsManager::get_singleton()->get_node_load_time_series(sender_id)-ts, ts) -
                covariance(LMStatisticsManager::get_singleton()->get_node_load_time_series(receiver_id), ts));

    return variacne_reduction;
}

double LMToolFunctions::get_migration_correlation_based_score(
                const IdType& sender_id,
                const IdType& receiver_id,
                const IdSet& box_ids)
{
    TimeSeries ts = LMStatisticsManager::get_singleton()->get_boxes_load_time_series(box_ids);

    double variacne_reduction  =
        2 * (correlation(LMStatisticsManager::get_singleton()->get_node_load_time_series(sender_id)-ts, ts) -
                correlation(LMStatisticsManager::get_singleton()->get_node_load_time_series(receiver_id), ts));

    return variacne_reduction;
}

double LMToolFunctions::get_migration_correlation_increase(
                const IdType& sender_id,
                const IdType& receiver_id,
                const IdSet& box_ids)
{
    TimeSeries ts = LMStatisticsManager::get_singleton()->get_boxes_load_time_series(box_ids);

    double correlation_increase  =
                  correlation(LMStatisticsManager::get_singleton()->get_node_load_time_series(sender_id)-ts,
                                          LMStatisticsManager::get_singleton()->get_node_load_time_series(receiver_id)+ts)
              + correlation(LMStatisticsManager::get_singleton()->get_node_load_time_series(sender_id),
                                          LMStatisticsManager::get_singleton()->get_node_load_time_series(receiver_id));

    return correlation_increase;

}


void LMToolFunctions::get_connected_boxes(
                vector<IdSet>& connected_boxes)
{
    LMBoxMap boxes(LMQueryNetwork::get_singleton()->get_boxes());
    while (!boxes.empty())
    {
        IdSet connected_query_graph;
        LMBoxVect expanding_boxes;
        expanding_boxes.push_back(boxes.begin()->second);
        while (!expanding_boxes.empty())
        {
            LMBox* box = expanding_boxes.back();
            expanding_boxes.pop_back();
            connected_query_graph.insert(box->get_id()); 
            boxes.erase(box->get_id());
            LMBoxVect connected_boxes;
            box->get_connected_boxes(connected_boxes);
            for (unsigned int i = 0; i < connected_boxes.size(); ++i)
            {
                LMBox* connected_box = connected_boxes[i];
                if (connected_query_graph.find(connected_box->get_id()) ==
                            connected_query_graph.end())
                {
                    expanding_boxes.push_back(connected_box);
                }
            }
        }
        connected_boxes.push_back(connected_query_graph);
        connected_query_graph.clear();
    }
}

void LMToolFunctions::get_connected_super_box_sets(
                const LMSuperBoxMap& sboxes,
                vector<LMSuperBoxMap>& connected_sbox_sets)
{
    LMSuperBoxMap boxes(sboxes);
    while (!boxes.empty())
    {
        LMSuperBoxMap connected_query_graph;
        LMSuperBoxVect expanding_boxes;
        expanding_boxes.push_back(boxes.begin()->second);
        while (!expanding_boxes.empty())
        {
            LMSuperBox* box = expanding_boxes.back();
            expanding_boxes.pop_back();
            connected_query_graph[box->get_id()] = box;
            boxes.erase(box->get_id());
            LMSuperBoxMap connected_boxes;
            box->get_connected_super_boxes(connected_boxes);
            for (LMSuperBoxMap::const_iterator it = connected_boxes.begin();
                    it != connected_boxes.end();
                    ++it)
            {
                LMSuperBox* connected_box = it->second;
                if (connected_query_graph.find(connected_box->get_id()) ==
                            connected_query_graph.end())
                {
                    expanding_boxes.push_back(connected_box);
                }
            }
        }
        connected_sbox_sets.push_back(connected_query_graph);
        connected_query_graph.clear();
    }
}


void LMToolFunctions::get_box_groups(
                const IdSet& boxes,
                vector<IdSet>& box_groups)
{
      IdSet boxids(boxes);
      while (!boxids.empty())
      {
          box_groups.push_back(IdSet());
          IdSet& box_group = box_groups.back();
          IdSet expanding_boxes;
          expanding_boxes.insert(*(boxes.begin()));
          while (!expanding_boxes.empty())
          {
              const IdType& box_id = *expanding_boxes.begin();
              LMBox* box = LMQueryNetwork::get_singleton()->get_box(box_id);
              box_group.insert(box_id);
              boxids.erase(box_id);
              expanding_boxes.erase(expanding_boxes.begin());

              LMBoxVect input_boxes;
              box->get_input_boxes(input_boxes);
              if (input_boxes.size() == 1)
              {
                  LMBox* input_box = input_boxes.front();
                  if (input_box->get_type() != "filter")
                      expanding_boxes.insert(input_box->get_id());
              }

              if (box->get_type() != "filter")
              {
                  LMBoxVect output_boxes;
                  box->get_output_boxes(output_boxes);
                  for (unsigned int i = 0; i < output_boxes.size(); ++i)
                  {
                      LMBox* output_box = output_boxes[i];
                      if (output_box->get_input_arcs().size() == 1)
                      {
                          expanding_boxes.insert(output_box->get_id());
                      }
                  }
              }
          }
      }
}


void LMToolFunctions::cluster_boxes(
                LMSuperGraph& super_graph)
{
    const LMBoxMap& boxes = LMQueryNetwork::get_singleton()->get_boxes();
    for (LMBoxMap::const_iterator it = boxes.begin();
            it != boxes.end();
            ++it)
    {
        LMBox* box = it->second;
        LMSuperBox* sbox = super_graph.get_parent_super_box(box->get_id());
        //cout << "check box " << *box << endl;

        if (box->get_type() == "filter")
        {
            // merge it with up stream box
            // get its uptream super box
            LMBox* upstream_box = 0;
            if (!(box->get_input_arcs().empty()))
            {
                LMArc* input_arc = box->get_input_arcs().begin()->second;
                upstream_box = input_arc->get_source_box();
            }
            if (upstream_box != 0)
            {
                LMSuperBox* upstream_sbox =
                    super_graph.get_parent_super_box(upstream_box->get_id());

                if (upstream_sbox == sbox)
                    continue;

                if (upstream_sbox->get_boxes().size() + sbox->get_boxes().size() <
                        LMParams::distribution_param.maximum_cluster_size)
                {
                        // merge the two super box
                        super_graph.merge_super_boxes(LMSuperBoxPair(sbox, upstream_sbox));
                        continue;
                }
            }

            // If the sbox only contain one box, then try to merge the filter
            // box with its down stream box
            LMBoxVect output_boxes;
            box->get_output_boxes(output_boxes);
            for (unsigned int i = 0; i < output_boxes.size(); ++i)
            {
                LMBox* downstream_box = output_boxes[i];
                LMSuperBox* downstream_sbox =
                    super_graph.get_parent_super_box(downstream_box->get_id());

                if (downstream_sbox->get_boxes().size() + sbox->get_boxes().size() <
                        LMParams::distribution_param.maximum_cluster_size)
                {
                        // merge the two super box
                        super_graph.merge_super_boxes(LMSuperBoxPair(sbox, downstream_sbox));
                        break;
                }
            }
        }

        if (box->get_type() == "union")
        {
            // merge it with down stream box
            // get its uptream super box
            LMBoxVect output_boxes;
            box->get_output_boxes(output_boxes);
            bool merged = false;
            for (unsigned int i = 0; i < output_boxes.size(); ++i)
            {
                LMBox* downstream_box = output_boxes[i];
                LMSuperBox* downstream_sbox =
                    super_graph.get_parent_super_box(downstream_box->get_id());

                if (downstream_sbox->get_boxes().size() + sbox->get_boxes().size() <
                        LMParams::distribution_param.maximum_cluster_size)
                {
                        // merge the two super box
                        super_graph.merge_super_boxes(LMSuperBoxPair(sbox, downstream_sbox));
                        merged = true;
                        break;
                }
            }
            if (merged)
                continue;


            // If the sbox only contain one box, then try to merge the union
            // box with one of its upstream box
            LMBoxVect input_boxes;
            box->get_input_boxes(input_boxes);
            for (unsigned int i = 0; i < input_boxes.size(); ++i)
            {
                LMBox* upstream_box = input_boxes[i];
                LMSuperBox* upstream_sbox =
                    super_graph.get_parent_super_box(upstream_box->get_id());

                if (upstream_sbox->get_boxes().size() + sbox->get_boxes().size() <
                        LMParams::distribution_param.maximum_cluster_size)
                {
                        // merge the two super box
                        super_graph.merge_super_boxes(LMSuperBoxPair(sbox, upstream_sbox));
                        break;
                }
            }
        }
    }
}


void LMToolFunctions::fill_load_coefficient_matrix(
            const LMArcMap& reference_arcs,
            LMIDMatrix& operator_load_coefficient_matrix)
{
    unsigned int d = reference_arcs.size();

    LMIDMatrix stream_rate_matrix(d);

    // initialize the matrices
    for (LMBoxMap::const_iterator 
            it = LMQueryNetwork::get_singleton()->get_boxes().begin();
            it != LMQueryNetwork::get_singleton()->get_boxes().end();
            ++it) 
    {
        operator_load_coefficient_matrix.add_new_entry(it->first);
    }
    for (LMArcMap::const_iterator 
            it = LMQueryNetwork::get_singleton()->get_arcs().begin();
            it != LMQueryNetwork::get_singleton()->get_arcs().end();
            ++it) 
    {
        stream_rate_matrix.add_new_entry(it->first);
    }



    d = 0;
    for (LMArcMap::const_iterator ref_arc_it = reference_arcs.begin();
            ref_arc_it != reference_arcs.end();
            ++ref_arc_it)
    {
        // Set the input arc rate to be 1
        stream_rate_matrix[ref_arc_it->first][d] += 1;

        LMArcVect arcs_to_follow;  
        arcs_to_follow.push_back(ref_arc_it->second);

        // Visit all arcs using depth first search
        while (!arcs_to_follow.empty())
        {
            // check whether the arc is an output arc, if so, go to next arc
            const IdType& arc_id = arcs_to_follow.back()->get_id();
            LMBox* box = arcs_to_follow.back()->get_destination_box();
            arcs_to_follow.pop_back();
            if (box == 0)
            {
                continue;
            }

            const IdType& box_id = box->get_id();

            // update coefficient for the box
            double coefficient = 
                stream_rate_matrix[arc_id][d] * 
                LMStatisticsManager::get_singleton()->get_time_series(
                        LMStatisticsManager::BOX_COST,
                        box_id).get_average();
            operator_load_coefficient_matrix[box_id][d] += coefficient;

            // update output stream rate and add the arcs to the to check list
            for (map<PortId, LMArcMap>::const_iterator 
                    it = box->get_output_port_arc_map().begin();
                    it != box->get_output_port_arc_map().end();
                    ++it) 
            {
                double stream_rate = 
                    stream_rate_matrix[arc_id][d] * 
                    LMStatisticsManager::get_singleton()->get_time_series(
                            LMStatisticsManager::SELECTIVITY,
                            get_selectivity_id(box_id, it->first)).get_average();
                for (LMArcMap::const_iterator ait = it->second.begin();
                        ait != it->second.end();
                        ++ait) {
                    stream_rate_matrix[ait->first][d] += stream_rate;
                    arcs_to_follow.push_back(ait->second);
                }
            }
        }

        ++d;
    }
}


void LMToolFunctions::order_id_matrix(
            const LMIDMatrix& id_matrix,
            VectorOrderingMethod order_method,
            const string& order,
            IdValuePairVect& ordered_id_index)
{
    for (LMIDMatrix::const_iterator it = id_matrix.begin();
            it != id_matrix.end();
            ++it) 
    {
        double value  = 0;
        switch (order_method)
        {
            case VOM_RAND:
                value = drand48();
            case VOM_NORM1: 
                {
                    vector<double>::const_iterator vit = 
                        max_element((it->second).begin(), (it->second).end()); 
                    value = *vit;
                    break;
                }
            case VOM_NORM2:
                for (unsigned int i = 0; i < it->second.size(); ++i)
                {
                    value += it->second[i] * it->second[i];
                }
                value = sqrt(value);
                break;
            case VOM_SUM:
                value = accumulate(it->second.begin(), it->second.end(), 0.0);
                break;
            default:
                cerr << "Invalid Order Method in LMToolFunctions::order_id_matrix"
                     << endl;
                exit(1);
        }
        ordered_id_index.push_back(IdValuePair(it->first, value));
    }
    if (order == "ascending")
    {
        sort(ordered_id_index.begin(), ordered_id_index.end(), less_than);
    }
    else if (order == "descending")
    {
        sort(ordered_id_index.begin(), ordered_id_index.end(), greater_than);
    }
    else
    {
        cerr << "LMToolFunctions::order_id_matrix: invalid order parameter" 
             << endl;
        exit(1);
    }
}

void LMToolFunctions::get_box_sets(
        const IdSet& boxes,
        DescendingIdSet& box_sets)
{
    for (IdSet::const_iterator it = boxes.begin();
            it != boxes.end();
            ++it)
    {
        IdSet box_set;
        box_set.clear();
        box_set.insert(*it);
        box_sets.insert(DescendingIdSet::value_type(0.0, box_set));
    }
}

BOREALIS_NAMESPACE_END
