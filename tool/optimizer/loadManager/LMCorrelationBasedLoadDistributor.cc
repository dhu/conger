#include "LMNodeManager.h"
#include "LMCorrelationBasedLoadDistributor.h"
#include "TimeSeries.h"
#include "LMToolFunctions.h"
#include "LMStatisticsManager.h"
//#include "SuperGraph.h"

#define DEBUG_GLOBAL
#define DEBUG_PAIRWISE
#define DEBUG_LOAD_BALANCING
//#define DEBUG_REDISTRIBUE
//#define DEBUG_ONE_WAY
//#define DEBUG_SELECTIVE_EXCHANGE
//#define DEBUG_PAIRWISE_IMPROVE
//#define DEBUG_PAIRWISE_REFINE
//#define DEBUG_SCORE_COMPUTATION
//#define DEBUG_MAX_SCORE_BOX_SELECTION
//
//#define DEBUG_BOX_SLECTION
//#define DEBUG_SHOW_CORRELATION_MATRIX
//#define DEBUG_TIME
//#define DEBUG_COMPUTE_CORRELATION_MATRIX
//#define DEBUG_BANDWIDTH_AWARE_CLUSTER_EXCHANGE

BOREALIS_NAMESPACE_BEGIN
using namespace StatisticsTools;

LMCorrelationBasedLoadDistributor* 
         LMCorrelationBasedLoadDistributor::_singleton = 0;

LMCorrelationBasedLoadDistributor::LMCorrelationBasedLoadDistributor()
{
    _pairwise_method = LMParams::distribution_method.pairwise_method;
    _score_function_type =
        LMParams::distribution_method.initial_distribution_score_function_type;
}


//------------------------------
// Global level load distribution algorithms
//------------------------------

void LMCorrelationBasedLoadDistributor::global_load_distribution()
{

#ifdef DEBUG_GLOBAL
#ifdef DEBUG_SHOW_CORRELATION_MATRIX
    print_correlation_matrix()
    LMStatisticsManager::get_singleton()->output_statistics( cout,
          LMStatisticsManager::BOX_LOAD);
#endif
#endif
#ifdef DEBUG_GLOBAL
    cout << "Before global mapping" << endl;
    cout << *LMNodeManager::get_singleton();
    LMStatisticsManager::get_singleton()->output_statistics( cout,
          LMStatisticsManager::NODE_LOAD);
#endif

    _score_function_type = 
        LMParams::distribution_method.initial_distribution_score_function_type;

    IdVect node_ids;
    for (LMNodeMap::const_iterator 
            it = LMNodeManager::get_singleton()->get_lmnodes().begin();
            it != LMNodeManager::get_singleton()->get_lmnodes().end();
            ++it)
    {
        node_ids.push_back(it->first);
    }
 
    redistribute_boxes(node_ids);


    _pairwise_method = PLDM_ONE_WAY;
    load_balancing();

    pairwise_improve_redistribution();


#ifdef DEBUG_GLOBAL
    cout << "After global mapping " << endl;
    cout << *LMNodeManager::get_singleton();
    LMStatisticsManager::get_singleton()->output_statistics( cout,
          LMStatisticsManager::NODE_LOAD);
#endif
}

//------------------------------
// Pair wise load distribution algorithms
//------------------------------
void LMCorrelationBasedLoadDistributor::pairwise_load_distribution()
{
    load_balancing(true);

    if (LMParams::distribution_method.pairwise_method != PLDM_ONE_WAY &&
        LMParams::distribution_method.pairwise_refine_correlation)
    {
        pairwise_refine_correlation();
    }
}

//------------------------------
// Pairwise base Load distribution algorithms
//------------------------------
void LMCorrelationBasedLoadDistributor::load_balancing(bool fuzzy)
{
#ifdef DEBUG_LOAD_BALANCING
    cout << "^^^^^^^^^^^^^^^^ Before load balacing:" << endl;
    cout << *LMNodeManager::get_singleton();
    LMStatisticsManager::get_singleton()->output_statistics( cout,
          LMStatisticsManager::NODE_LOAD);
#endif

    double load_balancing_threshold =
        LMParams::distribution_param.load_balancing_threshold;
    if (!fuzzy) 
    {
        load_balancing_threshold = 
              LMParams::distribution_param.load_selection_threshold;
    }

    _score_function_type =
        LMParams::distribution_method.dynamic_distribution_score_function_type;

    // Obsolete: 
    // move opeators between node paires until no more operators can be moved.
    //bool move = true;
    //while (move)
    //{
    //    for (unsigned int i = 0; i < node_pairs.size(); ++i)
    //    {
    //        if (balance_load_between_node_pair(node_pairs[i]))
    //                move = true;
    //    }
    //    move =  false;
    //}
    
    // Only balance the load pairwisely one time
    IdPairVect node_pairs;
    node_pairs.clear();
    LMToolFunctions::get_imbalanced_node_pairs(
            LMParams::distribution_param.load_balancing_threshold,
            0.0,
            node_pairs);



    for (unsigned int i = 0; i < node_pairs.size(); ++i)
    {
        balance_load_between_node_pair(node_pairs[i]);
#ifdef DEBUG_LOAD_BALANCING
        cout << "After Balancing node between node pair: " 
             << node_pairs[i] << endl;
        cout << *LMNodeManager::get_singleton(); 
        LMStatisticsManager::get_singleton()->output_statistics( cout,
                 LMStatisticsManager::NODE_LOAD);
#endif
    }

#ifdef DEBUG_LOAD_BALANCING
    cout << "_________________After load balacing:" << endl;
    cout << *LMNodeManager::get_singleton();
    LMStatisticsManager::get_singleton()->output_statistics( cout,
          LMStatisticsManager::NODE_LOAD);
#endif
}

//------------------------------
// Pair wise load distribution base algorithm
//------------------------------
bool LMCorrelationBasedLoadDistributor::balance_load_between_node_pair(
        const IdPair& node_pair)
{

#ifdef DEBUG_PAIRWISE
    cout << "^^^  before balanceLoadBetweenNodePair ^^^^^^^^^^^^^^^^^^" << endl;
#endif

    // remember the old distribution plan 
    LMNodeManager::get_singleton()->take_snapshot(node_pair.first);
    LMNodeManager::get_singleton()->take_snapshot(node_pair.second);

    // remember the old maximum divergent load. (Used to determine whether
    // accept the new plan)
    double old_max_load = max( 
            LMStatisticsManager::get_singleton()->get_node_divergent_load(
                        node_pair.first), 
            LMStatisticsManager::get_singleton()->get_node_divergent_load(
                        node_pair.second)); 

    bool move = false;
    switch (_pairwise_method) {
        case PLDM_ONE_WAY:
            move = one_way_box_redistribution(node_pair);
        break;
        case PLDM_REDISTRIBUTE: 
        {
            IdVect node_ids;
            node_ids.push_back(node_pair.first);
            node_ids.push_back(node_pair.second);

            // redistribute boxes
            redistribute_boxes(node_ids);
            one_way_box_redistribution(node_pair);

            break;
        }
        case PLDM_SELECTIVE_EXCHANGE:
            selective_exchange(node_pair);
            one_way_box_redistribution(node_pair);
            break;
        default: 
            cerr << "Wrong pairwise load distribution method: " 
                 << _pairwise_method << endl;
            exit(1);
    }

    // use max node divergent_load to determine whether boxes are moved
    double new_max_load = max(
            LMStatisticsManager::get_singleton()->get_node_divergent_load(
                node_pair.first), 
            LMStatisticsManager::get_singleton()->get_node_divergent_load(
                node_pair.second)); 
    if (new_max_load != old_max_load)
    {
        move = true;
    }
    else
    {
        move = false;
        // reverse to the old distribution plan
        LMNodeManager::get_singleton()->copy_snapshot(IdPair(node_pair.first, node_pair.second));
    }


#ifdef DEBUG_PAIRWISE
    cout << "^^^  after balanceLoadBetweenNodePair ^^^^^^^^^^^^^^^^^^" << endl;
#endif
    return move;
}

//------------------------------
// Pair wise load distribution algorithms
//------------------------------
bool LMCorrelationBasedLoadDistributor::one_way_box_redistribution(
        const IdPair& node_pair)
{
#ifdef DEBUG_ONE_WAY
     cout << "Before Oneway load balacing between node pair: " << node_pair << endl;
     cout << *(LMNodeManager::get_singleton()) << endl;
     LMStatisticsManager::get_singleton()->output_statistics( cout,
            LMStatisticsManager::NODE_LOAD);
     LMStatisticsManager::get_singleton()->output_statistics( cout,
            LMStatisticsManager::BOX_LOAD);
#endif

    const IdType& ns_id = LMToolFunctions::get_more_loaded_node_id(node_pair);
    const IdType& nr_id = LMToolFunctions::get_less_loaded_node_id(node_pair);

    // Compute the amout of load to be moved ((Ls - Lr) / 2)
    double amount =
        (LMStatisticsManager::get_singleton()->get_node_average_load(ns_id) -
        LMStatisticsManager::get_singleton()->get_node_average_load(nr_id)) / 2.0;

    bool move = false;
    while (amount > LMParams::distribution_param.load_selection_threshold)
    {
        // get the box with the maximum score
        double max_score;
        IdSet::const_iterator max_score_box_pos = get_max_score_box_pos(
            LMNodeManager::get_singleton()->get_lmnode(ns_id).get_boxes(),
            LMNodeManager::get_singleton()->get_lmnode(nr_id).get_boxes(),
            amount, 
            LMNodeManager::get_singleton()->get_lmnode(ns_id).get_boxes(),
            max_score);
        if (max_score < -10)
            break;
        const IdType& max_score_box_id = *max_score_box_pos;

        // move the selected boxes
        IdSet box_ids;
        box_ids.insert(max_score_box_id);
        LMNodeManager::get_singleton()->move_boxes_between_nodes(
                ns_id, nr_id, box_ids);

        // update load to be moved
        amount -= LMStatisticsManager::get_singleton()->get_boxes_average_load(
                box_ids);

        move = true;

#ifdef DEBUG_ONE_WAY
        cout << "MOVE: " << ns_id << " -> " << nr_id << ": " << box_ids << endl;
        LMStatisticsManager::get_singleton()->output_statistics( cout,
            LMStatisticsManager::NODE_LOAD);
#endif
    }

    // If the load of the nodes are still not balanced, allow the sender
    // to move box with larger load (Lr will be larger than Ls then)
    // But keep (Lr - Ls) less than the load balancing threshold

    amount =
        (LMStatisticsManager::get_singleton()->get_node_average_load(ns_id) -
        LMStatisticsManager::get_singleton()->get_node_average_load(nr_id))/ 2.0;
    if ( amount > LMParams::distribution_param.load_balancing_threshold)
    {
        amount *= 2.0;
    }

    // get the box with the maximum score
    double max_score;
    IdSet::const_iterator max_score_box_pos = get_max_score_box_pos(
        LMNodeManager::get_singleton()->get_lmnode(ns_id).get_boxes(),
        LMNodeManager::get_singleton()->get_lmnode(nr_id).get_boxes(),
        amount, 
        LMNodeManager::get_singleton()->get_lmnode(ns_id).get_boxes(),
        max_score);

    if (max_score > 0)
    {
        const IdType& max_score_box_id = *max_score_box_pos;
 
 
        // move the selected boxes
        IdSet box_ids;
        box_ids.insert(max_score_box_id);
        LMNodeManager::get_singleton()->move_boxes_between_nodes(
                ns_id, nr_id, box_ids);

        move = true;

#ifdef DEBUG_ONE_WAY
        cout << " *** Move extra box:" << endl;
        cout << "MOVE: " << ns_id << " -> " << nr_id << ": " << box_ids << endl;
        LMStatisticsManager::get_singleton()->output_statistics( cout,
            LMStatisticsManager::NODE_LOAD);
#endif
    }


#ifdef DEBUG_ONE_WAY
    cout << "After oneWayLoadBalancing between node pair: " << node_pair << endl;
    cout << *(LMNodeManager::get_singleton()) << endl;
    LMStatisticsManager::get_singleton()->output_statistics( cout,
            LMStatisticsManager::NODE_LOAD);
#endif

    return move;
}

//------------------------------
// Box redistribiton, both for global algorithm and Pairwise algorithm
//------------------------------
void LMCorrelationBasedLoadDistributor::redistribute_boxes(const IdVect& node_ids)
{

#ifdef DEBUG_REDISTRIBUE
    cout << "Before redistribute load" << endl;
    cout << *(LMNodeManager::get_singleton()) << endl;
#endif

    AscendingIdSet ordered_node_box_sets;  // the box sets of the nodes
    IdSet candidate_boxes;
    for (unsigned int i = 0; i < node_ids.size(); ++i) {
        // initialize node box sets
        ordered_node_box_sets.insert(AscendingIdSet::value_type(0, IdSet()));
 
        // add all box sets of a node to the candidate boxes list
        const IdSet& node_boxes = 
            LMNodeManager::get_singleton()->get_lmnode(node_ids[i]).get_boxes();
        candidate_boxes.insert(node_boxes.begin(), node_boxes.end());
    }

#ifdef DEBUG_TIME
    Timeval tm1;
    gettimeofday(&tm1, 0);
    cout << "number of boxes: " << candidate_boxes.size() << endl;
#endif

    while (!candidate_boxes.empty()) 
    {
        // get the min load node index
        IdSet min_load_box_set(ordered_node_box_sets.begin()->second);
       
        // record its load
        double min_node_load = ordered_node_box_sets.begin()->first;
       
        // get the box with the largest score
        double max_score;
        IdSet::const_iterator max_score_box_pos = get_max_score_box_pos(
                ordered_node_box_sets,
                min_load_box_set,
                -1,
                candidate_boxes,
                max_score);
        const IdType& max_score_box_id = *max_score_box_pos;
       
       
        // put the opeator set with the largest score to the leaset loaded node
        min_load_box_set.insert(max_score_box_id);
        min_node_load += 
            LMStatisticsManager::get_singleton()->get_box_average_load(
                    max_score_box_id);
       
        // update the load of the node
        ordered_node_box_sets.erase(ordered_node_box_sets.begin());
        ordered_node_box_sets.insert(
                AscendingIdSet::value_type(min_node_load, min_load_box_set));
       
        // erase the box set from the candidate list
        candidate_boxes.erase(max_score_box_pos);

#ifdef DEBUG_REDISTRIBUE
    cout << ordered_node_box_sets << endl;
#endif

    }
 
    // move boxes
    // clear all nodes first
    for (unsigned int i = 0; i < node_ids.size(); ++i)
    {
        LMNodeManager::get_singleton()->clear_lmnode(node_ids[i]);
    }
    // add boxes to them
    int i = 0;
    for (AscendingIdSet::const_iterator it = ordered_node_box_sets.begin();
            it != ordered_node_box_sets.end();
            ++it)
    {
        LMNodeManager::get_singleton()->add_boxes(node_ids[i], it->second);
        ++i;
    }

#ifdef DEBUG_TIME
    gettimeofday(&tm2, 0);
    tm2 -= tm1;
    cout << "Computation Time: " << tm2.double_value() << endl;
    exit(1);
#endif
#ifdef DEBUG_REDISTRIBUE
    cout << "After redistribute load" << endl;
    cout << *(LMNodeManager::get_singleton()) << endl;
#endif
}

bool LMCorrelationBasedLoadDistributor::selective_exchange(
        const IdPair& node_pair)
{
#ifdef DEBUG_SELECTIVE_EXCHANGE
    cout << "*** Before selectiveExchange:" << endl;
    cout << *(LMNodeManager::get_singleton());
#endif

    IdType ns_id = node_pair.first;
    IdType nr_id = node_pair.second;
 
    int count = 
        LMNodeManager::get_singleton()->get_lmnode(ns_id).get_boxes().size() +
        LMNodeManager::get_singleton()->get_lmnode(nr_id).get_boxes().size();
 
    bool move = false;
    while (--count > 0) {
 
        if (LMStatisticsManager::get_singleton()->get_node_average_load(ns_id) <
            LMStatisticsManager::get_singleton()->get_node_average_load(nr_id))
        {
            IdType tmp = ns_id;
            ns_id = nr_id;
            nr_id = tmp;
        }

        // select the box with maximum load from the sender node
        double max_score;
        IdSet::const_iterator max_score_box_pos = get_max_score_box_pos(
            LMNodeManager::get_singleton()->get_lmnode(ns_id).get_boxes(),
            LMNodeManager::get_singleton()->get_lmnode(nr_id).get_boxes(),
            -1, 
            LMNodeManager::get_singleton()->get_lmnode(ns_id).get_boxes(),
            max_score);

#ifdef DEBUG_SELECTIVE_EXCHANGE
    cout << "Max score box: " << *max_score_box_pos 
         << " score " << max_score << endl;
#endif


        // check whether the largest score is greater than the threshold
        if (max_score < 
                LMParams::distribution_param.selective_exchange_score_theshold)
            break;


        // move the box with the largest score to the reciever node
        IdSet box_ids;
        box_ids.insert(*max_score_box_pos);
        LMNodeManager::get_singleton()->move_boxes_between_nodes(
                ns_id, nr_id, box_ids);

        move = true;

#ifdef DEBUG_SELECTIVE_EXCHANGE
        cout << "MOVE: " << ns_id << " -> " << nr_id << ": " << box_ids << endl;
        LMStatisticsManager::get_singleton()->output_statistics( cout,
              LMStatisticsManager::NODE_LOAD);
#endif
    }

#ifdef DEBUG_SELECTIVE_EXCHANGE
    cout << "*** After selectiveExchange:" << endl;
    cout << *(LMNodeManager::get_singleton());
#endif
    return move;
}


//------------------------------
// Correlation Refinement Algorithms
//------------------------------

//------------------------------
// For the pairwise algorithm
//------------------------------
void LMCorrelationBasedLoadDistributor::pairwise_refine_correlation()
{

#ifdef DEBUG_PAIRWISE_REFINE
    cout << "Before pairwise refine" << endl;
    cout << *(LMNodeManager::get_singleton());
    LMStatisticsManager::get_singleton()->output_statistics( cout,
            LMStatisticsManager::NODE_LOAD);
#endif

    typedef map<double, IdType> OrderedLMNodes;
    OrderedLMNodes nodes;
 
    // order the nodes by divergent load
    const LMNodeMap& lmnodes = LMNodeManager::get_singleton()->get_lmnodes();
    for (LMNodeMap::const_iterator it = lmnodes.begin();
            it != lmnodes.end();
            ++it)
    {
        double divergent_load = 
            LMStatisticsManager::get_singleton()->get_node_divergent_load(
                    it->first);
        nodes[divergent_load] = it->first;
    }
 
    while(!nodes.empty()) {
        OrderedLMNodes::iterator sit = nodes.end();
        --sit;
       
        if (sit->first < 1.0)
          break;
        double old_divergent_load = sit->first;
        const LMNode& sn = 
            LMNodeManager::get_singleton()->get_lmnode(sit->second);
        IdType sender_id = sn.get_id();
        nodes.erase(sit);
       
        // find min correlation node
        double min_cor = 2.0;
        OrderedLMNodes::iterator rit;
        for (OrderedLMNodes::iterator it = nodes.begin();
             it != nodes.end();
             ++it) 
        {
            double cor = correlation(
                 LMStatisticsManager::get_singleton()->
                    get_node_load_time_series(sender_id),
                 LMStatisticsManager::get_singleton()->
                    get_node_load_time_series(it->second));
            if (cor < min_cor) 
            {
                min_cor = cor;
                rit = it;
            }
        } 


        if (min_cor > 0.8)
          continue;
       
        IdType receiver_id = rit->second;
       
        IdPair node_pair;
        node_pair.first = sender_id;
        node_pair.second = receiver_id;
        LMNodeManager::get_singleton()->take_snapshot(sender_id);
        LMNodeManager::get_singleton()->take_snapshot(receiver_id);
        balance_load_between_node_pair(node_pair);
       
#ifdef DEBUG_PAIRWISE_REFINE
        cout << "find node pair: " << sender_id << " "
             << receiver_id << ": "
             << min_cor << endl;
        cout << "old divergent_load: " << old_divergent_load 
             << "  max new divergent_load: " 
             << max(
                   LMStatisticsManager::get_singleton()->get_node_divergent_load(
                     sender_id), 
                   LMStatisticsManager::get_singleton()->get_node_divergent_load(
                     receiver_id))
             << endl;

#endif
       
        if (max(LMStatisticsManager::get_singleton()->
                get_node_divergent_load(sender_id),
               (LMStatisticsManager::get_singleton()->
                get_node_divergent_load(receiver_id))) < 
                old_divergent_load)
        {
            nodes.erase(rit);
        }
        else
        {
            LMNodeManager::get_singleton()->copy_snapshot(IdPair(sender_id, receiver_id));
        }
    }
#ifdef DEBUG_PAIRWISE_REFINE
    cout << "After pairwise refine" << endl;
    cout << *(LMNodeManager::get_singleton());
    LMStatisticsManager::get_singleton()->output_statistics( cout,
          LMStatisticsManager::NODE_LOAD);
#endif
}

//------------------------------
// For the global algorithm
//------------------------------
void LMCorrelationBasedLoadDistributor::pairwise_improve_redistribution()
{
#ifdef DEBUG_PAIRWISE_IMPROVE
    cout << "before pairwise improve" << endl;
    print_node_correlation_matrix();
    LMStatisticsManager::get_singleton()->output_statistics( cout,
          LMStatisticsManager::NODE_LOAD);
    cout << *LMNodeManager::get_singleton();
#endif

    // The threshold used
    double threshold = 0.8;
 
    const LMNodeMap& lmnodes = LMNodeManager::get_singleton()->get_lmnodes();
    unsigned int number_of_nodes = lmnodes.size();
    int count = number_of_nodes * number_of_nodes / 2;
 
    // initialize candidate node pairs (node pairs with correlation less than
    // threshold in ascending order)
    multimap<double, IdPair> candidate_node_pairs;
    LMNodeMap::const_iterator it_i = lmnodes.begin();
    for (unsigned int i = 0; i < number_of_nodes - 1; ++i)
    {
        // move the inner loop to the correct place
        LMNodeMap::const_iterator it_j = lmnodes.begin();
        for (unsigned int k = 0; k <= i; ++k)
        {
            ++it_j;
        }
        for (unsigned int j = i + 1; j < number_of_nodes; ++j)
        {
            double cor = correlation(
                 LMStatisticsManager::get_singleton()->
                    get_node_load_time_series(it_i->first),
                 LMStatisticsManager::get_singleton()->
                    get_node_load_time_series(it_j->first));
            // add a node pair to candidate nodes
            if (cor < threshold)
            {
                candidate_node_pairs.insert(
                    pair<double, IdPair>(cor, IdPair(it_i->first, it_j->first)));
            }
            ++it_j;
        }
        ++it_i;
    }
 

    while (!candidate_node_pairs.empty() && --count > 0) {
        // Try to improve the correlation between the node pair with minimum
        // load correlation
        IdType n1_id = candidate_node_pairs.begin()->second.first;
        IdType n2_id = candidate_node_pairs.begin()->second.second;

#ifdef DEBUG_PAIRWISE_IMPROVE
        cout << "Redistribute boxes between node pair: " << n1_id << "  " << n2_id << endl;
#endif

        IdVect node_pair;
        node_pair.push_back(n1_id);
        node_pair.push_back(n2_id);
 
        // remember old information
        double old_cor = candidate_node_pairs.begin()->first;

        // erase the node pair from candidate list
        candidate_node_pairs.erase(candidate_node_pairs.begin());

 
        LMNodeManager::get_singleton()->take_snapshot(n1_id);
        LMNodeManager::get_singleton()->take_snapshot(n2_id);



 
        // redistribute load
        redistribute_boxes(node_pair);
 
        // check th new information
        double new_cor = correlation(
                 LMStatisticsManager::get_singleton()->
                    get_node_load_time_series(n1_id),
                 LMStatisticsManager::get_singleton()->
                    get_node_load_time_series(n2_id));
 
        if (old_cor < new_cor) {

#ifdef DEBUG_PAIRWISE_IMPROVE
            cout << "Correlation improved: " 
                 << "old_cor "  << old_cor << "  new_cor  " << new_cor << endl;
#endif
            // new one is better, so we keep the changes
            // first remove pairs in the candidate_node_pairs 
            // that include this pair
            multimap<double, IdPair>::iterator it = candidate_node_pairs.begin();
            for (unsigned int i = 0; i < candidate_node_pairs.size(); ++i)
            {
                if ((it->second.first) == n1_id ||
                    (it->second.second) == n1_id ||
                    (it->second.first) == n2_id ||
                    (it->second.second) == n2_id)
                {
                    multimap<double, IdPair>::iterator tmp = it;
                    ++it;
                    candidate_node_pairs.erase(tmp);
                }
                else
                {
                    ++it;
                }
            }
            
            // update candidate_node_pairs
            for (LMNodeMap::const_iterator it = lmnodes.begin();
                    it != lmnodes.end();
                    ++it)
            {
                if (it->first < n1_id && 
                    it->first != n2_id)
                {
                    double cor = correlation(
                        LMStatisticsManager::get_singleton()->
                            get_node_load_time_series(it->first),
                        LMStatisticsManager::get_singleton()->
                            get_node_load_time_series(n1_id)); 
                    if (cor < threshold)
                    {
                        candidate_node_pairs.insert(pair<double, IdPair>(
                                cor, IdPair(it->first, n1_id)));
                    }
                }

                if (it->first < n2_id && 
                    it->first != n1_id)
                {
                    double cor = correlation(
                        LMStatisticsManager::get_singleton()->
                            get_node_load_time_series(it->first),
                        LMStatisticsManager::get_singleton()->
                            get_node_load_time_series(n2_id));
                    if (cor < threshold)
                    {
                        candidate_node_pairs.insert(pair<double, IdPair>(
                                cor, IdPair(it->first, n2_id)));
                    }
                }
            }

        }
        else 
        {

#ifdef DEBUG_PAIRWISE_IMPROVE
            cout << "Keep old plan: " 
                 << "old_cor "  << old_cor << "  new_cor  " << new_cor << endl;
#endif
            // new plan is not better, so we switch back to the former one
              LMNodeManager::get_singleton()->copy_snapshot(IdPair(n1_id, n2_id));
        }
  }
#ifdef DEBUG_PAIRWISE_IMPROVE
    cout << "after pairwise improve" << endl;
    print_node_correlation_matrix();
    cout << *LMNodeManager::get_singleton();
#endif

}



////////////////////////////////////////////////////////////////////////
// Tool functions 
////////////////////////////////////////////////////////////////////////
IdSet::const_iterator LMCorrelationBasedLoadDistributor::get_max_score_box_pos(
        const AscendingIdSet& all_node_box_sets,
        const IdSet& receiver_box_set,
        double max_load,
        const IdSet& candidate_boxes,
        double& max_score) const
{
    max_score = -1000;
    IdSet::const_iterator max_score_box_iterator = candidate_boxes.begin();
    for (IdSet::const_iterator it = candidate_boxes.begin();
              it != candidate_boxes.end();
              ++it)
    {
        double box_load = 
            LMStatisticsManager::get_singleton()->get_box_average_load(*it);
        // skip operators with laod larger than max_load
        if ((max_load > 0 && box_load > max_load) || 
             box_load < LMParams::distribution_param.load_selection_threshold)
        {
            continue;
        }

        double score = get_score(all_node_box_sets, receiver_box_set, *it);
        if (score > max_score)
        {
            max_score = score;
            max_score_box_iterator = it;
        }
    }

    return max_score_box_iterator;
}

double LMCorrelationBasedLoadDistributor::get_score(
        const AscendingIdSet& all_node_box_sets,
        const IdSet& receiver_boxes,
        const IdType& box_id) const
{
    switch(_score_function_type)
    {
        case SF_LOAD:
            return 
                LMStatisticsManager::get_singleton()->get_box_average_load(
                        box_id);
        case SF_CORRELATION_BASED:
            { 
                // compute correlation based score
                double avg_correltion = 0;
                for (AscendingIdSet::const_iterator 
                        it = all_node_box_sets.begin();
                        it != all_node_box_sets.end();
                        ++it)
                { 
                    // delete the box_id from node set if it exist
                    if (it->second.count(box_id))
                    {
                        IdSet boxes(it->second);
                        boxes.erase(box_id);
                        avg_correltion += 
                            get_load_correlation(boxes, box_id);
                    }
                    else
                    {
                        avg_correltion += 
                            get_load_correlation(it->second, box_id);
                    }
                }
                avg_correltion /= all_node_box_sets.size();

                double correlation_based_score = avg_correltion -
                    get_load_correlation(receiver_boxes, box_id);

                // round the correlation based score 
                // For boxes with the same rounded correlation based score, 
                // the opertor with larger load has larger score
                return ((int)(correlation_based_score * 10)) / 10.0 + 
                    LMStatisticsManager::get_singleton()->get_box_average_load(
                            box_id) / 100.0;
             }
        case SF_RANDOM:
            return drand48();
        default:
            return drand48();
    }
}


IdSet::const_iterator LMCorrelationBasedLoadDistributor::get_max_score_box_pos(
        const IdSet& sender_box_set,
        const IdSet& receiver_box_set,
        double max_load,
        const IdSet& candidate_boxes,
        double& max_score) const
{
    max_score = -1000;
    IdSet::const_iterator max_score_box_iterator = candidate_boxes.begin();
    for (IdSet::const_iterator it = candidate_boxes.begin();
              it != candidate_boxes.end();
              ++it)
    {
        double box_load = 
            LMStatisticsManager::get_singleton()->get_box_average_load(*it);
        // skip operators with laod larger than max_load
        if ((max_load > 0 && box_load > max_load) || 
             box_load < LMParams::distribution_param.load_selection_threshold)
        {
            continue;
        }


        double score = get_score(sender_box_set, receiver_box_set, *it);

#ifdef DEBUG_MAX_SCORE_BOX_SELECTION
        cout << "box " << *it << " score: " << score << endl; 
#endif
        if (score > max_score)
        {
            max_score = score;
            max_score_box_iterator = it;
        }
    }

#ifdef DEBUG_MAX_SCORE_BOX_SELECTION
    cout << "max score box " << *max_score_box_iterator << "  " 
         << max_score << endl;
#endif

    return max_score_box_iterator;
}

double LMCorrelationBasedLoadDistributor::get_score(
        const IdSet& sender_box_set,
        const IdSet& receiver_box_set,
        const IdType& box_id) const
{
    switch(_score_function_type)
    {
        case SF_LOAD:
            return 
                LMStatisticsManager::get_singleton()->get_box_average_load(
                        box_id);
        case SF_CORRELATION_BASED:
            { 
                // erase the box from the sender side if it in included 
                // in the sender side
                IdSet boxes(sender_box_set);
                boxes.erase(box_id);

                // compute correlation based score
                double correlation_based_score = 
                    (get_load_correlation(boxes, box_id)-
                    get_load_correlation(receiver_box_set, box_id)) / 2.0;

#ifdef DEBUG_SCORE_COMPUTATION
                cout << box_id << "  sender " << boxes << "  correlation: "
                    <<  get_load_correlation(boxes, box_id) << endl;
                cout << box_id << "  reciever " << receiver_box_set << "  correlation: "
                    <<  get_load_correlation(receiver_box_set, box_id) << endl;
                cout << "score: " << correlation_based_score << endl;
#endif 

                // round the correlation based score 
                // For boxes with the same rounded correlation based score, 
                // the opertor with larger load has larger score
                return ((int)(correlation_based_score * 20)) / 20.0 + 
                    LMStatisticsManager::get_singleton()->get_box_average_load(
                            box_id) / 1000.0;
             }
        case SF_RANDOM:
            return drand48();
        default:
            return drand48();
    }
}


double LMCorrelationBasedLoadDistributor::get_load_correlation(
    const IdSet& box_set,
    const IdType& box_id) const
{
     
    const TimeSeries& box_ts = 
        LMStatisticsManager::get_singleton()->get_box_load_time_series(box_id);

    TimeSeries boxes_ts = 
        LMStatisticsManager::get_singleton()->get_boxes_load_time_series(box_set);

    if (box_set.count(box_id)) 
    {
        boxes_ts -= box_ts;
    }

    return correlation(boxes_ts, box_ts);
}

void LMCorrelationBasedLoadDistributor::print_node_correlation_matrix() 
{
    cout << "------------- Node Correlation Matrix --------------" << endl;
    for (LMNodeMap::const_iterator 
            it_i = LMNodeManager::get_singleton()->get_lmnodes().begin();
           it_i != LMNodeManager::get_singleton()->get_lmnodes().end();
           ++it_i)
    {
        for (LMNodeMap::const_iterator 
                it_j = LMNodeManager::get_singleton()->get_lmnodes().begin();
               it_j != LMNodeManager::get_singleton()->get_lmnodes().end();
               ++it_j)
        {
            cout << setw(5) << setprecision (2) << correlation(
               LMStatisticsManager::get_singleton()->
                    get_node_load_time_series(it_i->first),
               LMStatisticsManager::get_singleton()->
                    get_node_load_time_series(it_j->first)
                    );
        }
        cout << endl;
    }
    cout << "----------------------------------------------------" << endl;
}

/*
//------------------------------
// Tool functions for load distribution
// -----------------------------
void CorrelationBasedLoadDistributor::print_correlation_matrix() 
{
    // TODO
  AuroraBoxMap::const_iterator it_i, it_j;
  cout << setw(7) << "";
  for (it_i = boxes.begin(); it_i != boxes.end(); ++it_i){
    cout << "(" << setw(3) << it_i->second->getId() << ") ";
  }
  cout << endl;
  for (it_i = boxes.begin(); it_i != boxes.end(); ++it_i){
    cout << "(" << setw(3) << it_i->second->getId() << ") "; 
    for (it_j = boxes.begin(); it_j != boxes.end(); ++it_j) {
      cout << setw(6) << LDFunctions::getDiscreteCorrelation(it_i->second->getLoadTimeSeries(), it_j->second->getLoadTimeSeries());
    }
    cout << endl;
  }
}

OrderedSuperBoxes::iterator LMCorrelationBasedLoadDistributor::select_best_super_box(
    const LMNode& node_ids,
    const IdType& destination_node_id,
    ordered_super_boxes& candidate_boxes)
{
  bool first = true;
  double max_score;
  OrderedSuperBoxes::iterator selected_box_pos;

#ifdef DEBUG_BOX_SLECTION
  vector<double> scores;
#endif

  map<double, OrderedSuperBoxes::iterator> good_box_its;
  for (OrderedSuperBoxes::iterator bit = candidate_boxes.begin();
       bit != candidate_boxes.end();
       ++bit) {
    double score = compute_score(node_ids, destination_node_id, bit);
    if (first) {
      max_score = score;
      good_box_its[bit->first] = bit;
      first = false;

#ifdef DEBUG_BOX_SLECTION
      scores.push_back(score);
#endif

    }
    else if (score > max_score + 0.0001) {
      max_score = score;
      good_box_its.clear();
      good_box_its[bit->first] = bit;

#ifdef DEBUG_BOX_SLECTION
      scores.clear();
      scores.push_back(score);
#endif

    }
    else if (score > max_score - 0.0001) {
      good_box_its[bit->first] = bit;

#ifdef DEBUG_BOX_SLECTION
      scores.push_back(score);
#endif
    }
  }

#ifdef DEBUG_BOX_SLECTION
  cout << "Max Score: " << max_score << "(";
  for (int i = 0; i < scores.size(); ++i)
    cout << scores[i] << " ";
  cout << ")" << endl;
  cout << "good boxes: ";
  for (map<double, OrderedSuperBoxes::iterator>::iterator
        it = good_box_its.begin();
       it != good_box_its.end();
       ++it)
    cout << it->second->second->get_id()
         << "(" << it->first << ")  ";
  cout << endl;
#endif

  map<double, OrderedSuperBoxes::iterator>::iterator
    it = good_box_its.end();
  --it;
#ifdef DEBUG_BOX_SLECTION
  cout << "Box " << it->second->second->get_id() << " --> Node "
       << destination_lbnode.server->get_id() << endl;
#endif
  return it->second;
}

double LMCorrelationBasedLoadDistributor::compute_score(
    const LMNode& node_ids,
    const IdType& destination_node_id,
    OrderedSuperBoxes::iterator& bit)
{
  switch (_box_selection_method) {
    case BSM_BALANCING:
      return bit->first;
    case BSM_RANDOM:
      return static_cast<double>(rand());
    case BSM_CORRELATION_BASED:
      {
        super_box* sbox = bit->second;
        double avg_cor = 0;
        for (int i = 0; i < node_ids.size(); ++i) {
          const LMNode& lbnode = _lbnode_manager.get_lmnode(node_ids[i]);
          if (lbnode.get_super_boxes().find(sbox->get_id()) != lbnode.get_super_boxes().end()) {
            avg_cor += get_discrete_correlation(
                lbnode.get_load_time_series() - _statistics_manager.get_super_box_load_time_series(sbox),
                _statistics_manager.get_super_box_load_time_series(sbox));
          }
          else
            avg_cor += get_discrete_correlation(lbnode.get_load_time_series(), _statistics_manager.get_super_box_load_time_series(sbox));
        }

        avg_cor /= node_ids.size();
        return avg_cor - get_discrete_correlation(
          _statistics_manager.get_super_box_load_time_series(sbox),
          _lbnode_manager.get_lmnode(destination_node_id).get_load_time_series());
      }
    default:
      cerr << "Wrong Parameter for global distribution method" << endl;
      exit(1);
  }
}


void LMCorrelationBasedLoadDistributor::compute_correlation_matrix(
    const super_box_map& super_boxes,
    super_box_pair_correlations& correlation_matrix ) const
{
  correlation_matrix.clear();
  for (SuperBoxMap::const_iterator iti = super_boxes.begin();
       iti != super_boxes.end();
       ++iti) {
    for (SuperBoxMap::const_iterator itj = super_boxes.begin();
         itj != super_boxes.end();
         ++itj) {
      if (correlation_matrix.find(super_box_pair(iti->second, itj->second))
          != correlation_matrix.end())
        continue;

      correlation_matrix[super_box_pair(iti->second, itj->second)] =
        get_discrete_correlation(
            _statistics_manager.get_super_box_load_time_series(iti->second),
            _statistics_manager.get_super_box_load_time_series(itj->second));
    }
  }

#ifdef DEBUG_COMPUTE_CORRELATION_MATRIX
  for (SuperBoxMap::const_iterator iti = super_boxes.begin();
       iti != super_boxes.end();
       ++iti) {
    for (SuperBoxMap::const_iterator itj = super_boxes.begin();
         itj != super_boxes.end();
         ++itj) {
      cout << setw(5) << correlation_matrix[super_box_pair(iti->second, itj->second)];
    }
    cout << endl;
  }
#endif
}

SuperBoxPairCorrelations::const_iterator LMCorrelationBasedLoadDistributor::get_largest_correlation_pair(
        const super_box_pair_correlations& correlation_matrix,
        const super_box_pair_correlations& checked_pairs) const
{
  double max_cor = -100;
  SuperBoxPairCorrelations::const_iterator max_cor_pair = correlation_matrix.end();

  for (SuperBoxPairCorrelations::const_iterator it = correlation_matrix.begin();
      it != correlation_matrix.end();
      ++it) {
    if(it->first.first == it->first.second)
      continue;
    if (checked_pairs.find(it->first) != checked_pairs.end())
      continue;

    if (max_cor < it->second) {
      max_cor = it->second;
      max_cor_pair = it;
    }
  }
  return max_cor_pair;
}

void LMCorrelationBasedLoadDistributor::get_adjacent_super_box_correlations(
    const super_box_pair& sbox_pair,
    super_box_pair_correlations& candidate_boxes,
    double threshold)
{
  super_box* sbox = sbox_pair.first;
  for (map<super_box*, super_edge*>::const_iterator it = sbox->get_connected_super_boxes_and_super_edges().begin();
      it != sbox->get_connected_super_boxes_and_super_edges().end();
      ++it) {
    double cor = get_discrete_correlation(
        _statistics_manager.get_super_box_load_time_series(sbox),
        _statistics_manager.get_super_box_load_time_series(it->first));
    if (cor <= threshold)
      candidate_boxes[super_box_pair(sbox, it->first)] = cor;
  }

  sbox = sbox_pair.second;
  for (map<super_box*, super_edge*>::const_iterator it = sbox->get_connected_super_boxes_and_super_edges().begin();
      it != sbox->get_connected_super_boxes_and_super_edges().end();
      ++it) {
    double cor = get_discrete_correlation(
        _statistics_manager.get_super_box_load_time_series(sbox),
        _statistics_manager.get_super_box_load_time_series(it->first));
    if (cor < threshold)
      candidate_boxes[super_box_pair(sbox, it->first)] = cor;
  }
}

SuperBoxPairCorrelations::const_iterator LMCorrelationBasedLoadDistributor::get_max_score_candidate_pair(
    const super_box_pair& sbox_pair,
    const super_box_pair_correlations& candidate_pairs)
{
  double max_score = -100;
  SuperBoxPairCorrelations::const_iterator max_score_pair = 0;
  for (SuperBoxPairCorrelations::const_iterator it = candidate_pairs.begin();
      it != candidate_pairs.end();
      ++it) {
    double score =
      _statistics_manager.get_super_box_average_load(it->first.first) +
      _statistics_manager.get_super_box_average_load(it->first.second);

    if (score > max_score) {
      max_score = score;
      max_score_pair = it;
    }
  }
  return max_score_pair;
}

void LMCorrelationBasedLoadDistributor::remove_checke_pair(
    const super_box_pair& sbox_pair,
    super_box_pair_correlations& checked_pair) {
  SuperBoxPairCorrelations::iterator it = checked_pair.begin();
  while (it != checked_pair.end()) {
    if (it->first.first == sbox_pair.first || it->first.second == sbox_pair.first ||
        it->first.first == sbox_pair.second || it->first.second == sbox_pair.second) {
      SuperBoxPairCorrelations::iterator tmp = it;
      ++it;
      checked_pair.erase(tmp);
    }
  }
}



*/

/*
////////////////////////////////////////////////////////////////////////
//  Obsolete
////////////////////////////////////////////////////////////////////////


void LMCorrelationBasedLoadDistributor::order_candidate_boxes_by_score(
        const IdSet& sender_boxes,
        const IdSet& receiver_boxes,
        double max_load,
        DescendingIdSet& ordered_candidate_box_sets) const
{
    DescendingIdSet candidate_box_sets(ordered_candidate_box_sets);
    ordered_candidate_box_sets.clear();
    for (DescendingIdSet::const_iterator it = candidate_box_sets.begin();
              it != candidate_box_sets.end();
              ++it)
    {
        const IdSet& box_set = it->second;
        double box_set_avg_load =
           LMStatisticsManager::get_singleton()->get_boxes_average_load(box_set);
        if (max_load >= 0 && 
                (box_set_avg_load > max_load || 
                 box_set_avg_load < 
                   LMParams::distribution_param.load_selection_threshold))
            continue;

        double score = get_score(sender_boxes, receiver_boxes, box_set);
        ordered_candidate_box_sets.insert(pair<double, IdSet>(score, box_set));
    }
}

void LMCorrelationBasedLoadDistributor::order_candidate_boxes_by_score(
        const AscendingIdSet& other_nodes_boxes,
        const IdSet& receiver_boxes,
        double max_load,
        DescendingIdSet& ordered_candidate_box_sets) const
{
    DescendingIdSet candidate_box_sets(ordered_candidate_box_sets);
    ordered_candidate_box_sets.clear();
    for (DescendingIdSet::const_iterator it = candidate_box_sets.begin();
              it != candidate_box_sets.end();
              ++it)
    {
        const IdSet& box_set = it->second;
        double box_set_avg_load =
           LMStatisticsManager::get_singleton()->get_boxes_average_load(box_set);
        if (max_load >= 0 && 
                (box_set_avg_load > max_load || 
                 box_set_avg_load < 
                   LMParams::distribution_param.load_selection_threshold))
            continue;

        double score = get_score(other_nodes_boxes, receiver_boxes, box_set);
        ordered_candidate_box_sets.insert(pair<double, IdSet>(score, box_set));
    }
}

double LMCorrelationBasedLoadDistributor::get_score(
        const IdSet& sender_boxes,
        const IdSet& receiver_boxes,
        const IdSet& box_set) const
{
    IdSet new_sender_box_set = sender_boxes;
    for (IdSet::const_iterator it = box_set.begin();
            it != box_set.end();
            ++it)
    {
        new_sender_box_set.erase(*it);
    }


    TimeSeries ts = LMStatisticsManager::get_singleton()->
            get_boxes_load_time_series(box_set);
    double correlation_based_score =
        (correlation(
             LMStatisticsManager::get_singleton()->
                 get_boxes_load_time_series(sender_boxes),
             ts) -
         correlation(
             LMStatisticsManager::get_singleton()->
                 get_boxes_load_time_series(receiver_boxes),
             ts)) / 2; 

    double load =
        LMStatisticsManager::get_singleton()->get_boxes_average_load(box_set);

    switch(_score_function_type)
    {
        case SF_LOAD:
                return load;
        case SF_CORRELATION_BASED:
                return ((int)(correlation_based_score * 20)) / 20.0 + load / 100.0;
        default:
                return load;
    }
}

double LMCorrelationBasedLoadDistributor::get_score(
        const AscendingIdSet& other_nodes_boxes,
        const IdSet& receiver_boxes,
        const IdSet& box_set) const
{
    TimeSeries ts = LMStatisticsManager::get_singleton()->
            get_boxes_load_time_series(box_set);

    double avg_correltion = 0;
    if (other_nodes_boxes.empty())
    {
        avg_correltion = 0;
    }
    else 
    {
        for (AscendingIdSet::const_iterator it = other_nodes_boxes.begin();
                it != other_nodes_boxes.end();
                ++it)
        { 
            avg_correltion += correlation(
                LMStatisticsManager::get_singleton()->
                    get_boxes_load_time_series(it->second),
                ts);
        }
        avg_correltion /= other_nodes_boxes.size();
    }

    double correlation_based_score = avg_correltion -
        correlation(
            ts,
            LMStatisticsManager::get_singleton()->
                get_boxes_load_time_series(receiver_boxes));

    double load =
        LMStatisticsManager::get_singleton()->get_boxes_average_load(box_set);

    switch(_score_function_type)
    {
        case SF_LOAD:
                return load;
        case SF_CORRELATION_BASED:
                return ((int)(correlation_based_score * 20)) / 20.0 + load / 100.0;
        default:
                return load;
    }
}
*/

BOREALIS_NAMESPACE_END
