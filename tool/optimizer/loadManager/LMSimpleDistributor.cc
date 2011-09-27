#include "LMSimpleDistributor.h"
#include "LMParams.h"
#include "LMStatisticsManager.h"
#include "LMNodeManager.h"

BOREALIS_NAMESPACE_BEGIN

//#define DEBUG_ONE_WAY

LMSimpleDistributor* LMSimpleDistributor::_singleton = 0;


//------------------------------------------------------------
// Simulator Initialization Functions
//------------------------------------------------------------

void LMSimpleDistributor::pairwise_distribution()
{
    _score_function_type =
        LMParams::distribution_method.dynamic_distribution_score_function_type;
    bool move = true;

    while (move)
    {
        IdPairVect node_pairs;
        node_pairs.clear();
        LMToolFunctions::get_imbalanced_node_pairs(
                LMParams::distribution_param.load_balancing_threshold,
                0.0,
                node_pairs);

        move =  false;

        for (unsigned int i = 0; i < node_pairs.size(); ++i)
        {
                if (one_way_load_balancing(node_pairs[i]))
                    move = true;
        }
    }
}

bool LMSimpleDistributor::one_way_load_balancing(const IdPair& node_pair)
{

    const IdType& ns_id = LMToolFunctions::get_more_loaded_node_id(node_pair);
    const IdType& nr_id = LMToolFunctions::get_less_loaded_node_id(node_pair);

    // Move load no more than (Ls - Lr) / 2
    double amount =
        (LMStatisticsManager::get_singleton()->get_node_average_load(ns_id) -
        LMStatisticsManager::get_singleton()->get_node_average_load(nr_id)) / 2.0;

#ifdef DEBUG_ONE_WAY
    cout << "Balancing load between node pair: " << node_pair << endl;
    cout << *(LMNodeManager::get_singleton()) << endl;
    LMStatisticsManager::get_singleton()->output_node_load_statistics(cout);
    LMStatisticsManager::get_singleton()->output_box_load_statistics(cout);
#endif

    bool move = false;
    while (amount > LMParams::distribution_param.load_selection_threshold)
    {
        // gather all candidate boxes
        DescendingIdSet candidate_box_sets;
        order_candidate_boxes_by_score(ns_id, nr_id, amount, candidate_box_sets);


        // select the best box set to move.
        while (!candidate_box_sets.empty())
        {
            // consider the box with largest score
            const IdSet& box_ids = candidate_box_sets.begin()->second;

            // move the selected boxes
            LMNodeManager::get_singleton()->move_boxes_between_nodes(ns_id, nr_id, box_ids);
            amount -= LMStatisticsManager::get_singleton()->get_boxes_average_load(box_ids);
            #ifdef DEBUG_ONE_WAY
                cout << "MOVE: " << ns_id << " -> " << nr_id << ": " << box_ids << endl;
                LMStatisticsManager::get_singleton()->output_node_load_statistics(cout);
            #endif

            move = true;
            break;
        }

        // end the process when no boxes can be moved
        if (candidate_box_sets.empty())
            break;
    }

    // If the load of the nodes are still not balanced, allow the sender
    // to move box with larger load (Lr will be larger than Ls then)
    // But keep (Lr - Ls) less than the load balancing threshold

    amount =
        (LMStatisticsManager::get_singleton()->get_node_average_load(ns_id) -
        LMStatisticsManager::get_singleton()->get_node_average_load(nr_id))/ 2.0;
    if ( amount > LMParams::distribution_param.load_balancing_threshold / 2.0)
    {
        amount += LMParams::distribution_param.load_balancing_threshold / 2.0;
    }

    DescendingIdSet candidate_box_sets;
    order_candidate_boxes_by_score(ns_id, nr_id, amount, candidate_box_sets);


    // select the best box set to move.
    if (!candidate_box_sets.empty())
    {
        // consider the box with largest score
        const IdSet& box_ids = candidate_box_sets.begin()->second;

        // move the selected boxes
        LMNodeManager::get_singleton()->move_boxes_between_nodes(ns_id, nr_id, box_ids);
            amount -= LMStatisticsManager::get_singleton()->get_boxes_average_load(box_ids);
    #ifdef DEBUG_ONE_WAY
    cout << " *** Move extra box:" << endl;
    cout << "MOVE: " << ns_id << " -> " << nr_id << ": " << box_ids << endl;
    LMStatisticsManager::get_singleton()->output_node_load_statistics(cout);
    #endif
            move = true;
    }


#ifdef DEBUG_ONE_WAY
    cout << "After oneWayLoadBalancing between node pair: " << node_pair << endl;
    cout << *(LMNodeManager::get_singleton()) << endl;
    LMStatisticsManager::get_singleton()->output_node_load_statistics(cout);
    LMStatisticsManager::get_singleton()->output_box_load_statistics(cout);
#endif

    return move;
}

void LMSimpleDistributor::order_candidate_boxes_by_score(
                const IdType& sender_id,
                const IdType& receiver_id,
                double max_load,
                DescendingIdSet& candidate_box_sets)
{
    const IdSet sender_box_ids = LMNodeManager::get_singleton()->get_lmnode(sender_id).get_boxes();
    for (IdSet::const_iterator it = sender_box_ids.begin();
              it != sender_box_ids.end();
              ++it)
    {
        const IdType& box_id = *it;
        if (LMStatisticsManager::get_singleton()->get_box_average_load(box_id) > max_load)
            continue;

        if (LMStatisticsManager::get_singleton()->get_box_average_load(box_id) < 0.0001)
            continue;

        IdSet box_ids;
        box_ids.insert(box_id);
        double score = get_score(sender_id, receiver_id, box_ids);
        candidate_box_sets.insert(pair<double, IdSet>(score, box_ids));
    }
}

double LMSimpleDistributor::get_score(
        const IdType& sender_id,
        const IdType& receiver_id,
        const IdSet& box_ids)
{
    double load_variance_reduction =
        LMToolFunctions::get_migration_load_variance_reduction(
                sender_id, receiver_id, box_ids);
    double correlation_based_score =
        LMToolFunctions::get_migration_correlation_based_score(
                sender_id, receiver_id, box_ids);
    double load_correlation_increase =
        LMToolFunctions::get_migration_correlation_increase(
                sender_id, receiver_id, box_ids);
    double load =
        LMStatisticsManager::get_singleton()->get_boxes_average_load(box_ids);

    switch(_score_function_type)
    {
        case SF_LOAD:
                return load;
        case SF_REDUCED_LOAD_VARIANCE:
                return load_variance_reduction;
        case SF_CORRELATION_BASED:
                return correlation_based_score;
        case SF_INCREASED_CORRELATION:
                return load_correlation_increase;
        default:
                return load;
    }
}

BOREALIS_NAMESPACE_END
