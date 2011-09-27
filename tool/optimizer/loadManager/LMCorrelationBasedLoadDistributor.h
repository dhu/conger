#ifndef LMCorrelationBasedLoadDistributor_H
#define LMCorrelationBasedLoadDistributor_H

#include "LMCommon.h"
#include "LMToolFunctions.h"

BOREALIS_NAMESPACE_BEGIN

//typedef pair<LMSuperBox*, LMSuperBox*> LMSuperBoxPair;
//typedef map<pair<LMSuperBox*, LMSuperBox*>, double> LMSuperBoxPairCorrelations;

/**
 * The LMCorrelationBasedLoadDistributor class implements
 * a load distributor that distribute operators using correaltion based
 * algorithms.
 *
 * Load balancing algorithms is also implemented for comparison
 *
 */

class LMCorrelationBasedLoadDistributor {
  // Method(s)
  public:
    /**
     * Returns the singleton of the CentralizedLoadDistributor
     */
    static LMCorrelationBasedLoadDistributor* get_singleton()
    {
        if (_singleton == 0)
          _singleton = new LMCorrelationBasedLoadDistributor();
        return _singleton;
    }

    /**
     * The global load distribution algorithm
     */
    void global_load_distribution();

    /**
     * The pair-wise load distributon algorithm
     */
    void pairwise_load_distribution();

  private:
    /**
     * Constructs a simulator.
     */
    LMCorrelationBasedLoadDistributor();

    ///////////////////////////////////////////////////////
    // Algorithms
    ///////////////////////////////////////////////////////
    /**
     * This load balancing is use for the global mapping
     * if fuzzy is true, then use load balancing threshold
     */
    void load_balancing(bool fuzzy = false);

    /**
     * Pairwise load distribution
     */
    bool balance_load_between_node_pair(const IdPair& node_pair);

    /**
     * One way load distributon
     */
    bool one_way_box_redistribution(const IdPair& node_pair);

    /**
     * this method redistribute all boxes
     * can be use both for global and pairwise distribution
     */
    void redistribute_boxes(const IdVect& node_ids);

    /**
     * Selective Operator Exchange
     */
    bool selective_exchange(const IdPair& node_pair);

    ///////////////////////////////////////////////////////
    // Tool functions
    ///////////////////////////////////////////////////////
    /**
     * Returns the position(iterator) of the box with the maximum score in the 
     * candidate_boxes
     * If max_load is possitive, then the load of the box must be 
     * less than max_load. 
     * This function is used for box redistribution
     * @param all_node_box_sets the box sets of all nodes
     * @param receiver_boxes the boxes on the receiver node
     * @param max_load maximum average load of the box
     * @param candidate_box_sets returns the candidate box sets
     * @param max_score returns the maximum score
     */
    IdSet::const_iterator get_max_score_box_pos(
        const AscendingIdSet& all_node_box_sets,
        const IdSet& receiver_box_set,
        double max_load,
        const IdSet& candidate_boxes,
        double& max_score) const;

    /**
     * Returns the score of the given box set 
     * This function is used for box redistribution
     * @param all_node_box_sets the box sets of all nodes
     * @param receiver_boxes the boxes on the receiver node
     * @param box_id ID of the given box
     */
    double get_score(
        const AscendingIdSet& all_node_box_sets,
        const IdSet& receiver_boxes,
        const IdType& box_id) const;

    /**
     * Returns the position(iterator) of the box with the maximum score in the 
     * candidate_boxes
     * If max_load is possitive, then the load of the box must be 
     * less than max_load. 
     * This function is used for box migration between a node pair
     * @param all_node_box_sets the box sets of all nodes
     * @param receiver_boxes the boxes on the receiver node
     * @param max_load maximum average load of the box
     * @param candidate_box_sets returns the candidate box sets
     * @param max_score returns the maximum score
     */
    IdSet::const_iterator get_max_score_box_pos(
        const IdSet& sender_box_set,
        const IdSet& receiver_box_set,
        double max_load,
        const IdSet& candidate_boxes,
        double& max_score) const;

    /**
     * Returns the score of the given box set 
     * This function is used for box migration between a node pair
     * @param all_node_box_sets the box sets of all nodes
     * @param receiver_boxes the boxes on the receiver node
     * @param box_id ID of the given box
     */
    double get_score(
        const IdSet& sender_box_set,
        const IdSet& receiver_box_set,
        const IdType& box_id) const;

    /**
     * Returns the load correlation between a box_set and a box
     * If the box is contained in the box_set, then remove the load time serires
     * of the box from the total load time series of the box_set when computing
     * the correlation
     * @param box_set the given box_set (usually the boxes on a node)
     * @param box_id ID of the given box
     */
    double get_load_correlation(
        const IdSet& box_set,
        const IdType& box_id) const;

    /**
     * Pairwisely refine the correlation between two nodes
     */
    void pairwise_refine_correlation();

    /**
     * Pairwise Redistribute load if the correlation between two nodes
     * is less then zero
     */
    void pairwise_improve_redistribution();




    ///////////////////////////////////////////////////////
    // Tool functions
    ///////////////////////////////////////////////////////
    /**
     * Print the correlation matrix of the nodes
     */
    void print_node_correlation_matrix();

    ///////////////////////////////////////////////////////
    // TODO
    ///////////////////////////////////////////////////////



    /**
     * Select the superbox with largest score
     * @parame node_ids all nodes considered
     * @params destination_node_index the destination node index
     * @params candidate_boxes all boxes to be considered
     */
//    OrderedSuperBoxes::iterator select_best_super_box(
//        const IdVect& node_ids,
//        const IdType& destination_node_id,
//        ordered_super_boxes& candidate_boxes);

    /**
     * Compute the score of a SuperBox
     * @parame node_ids all nodes considered
     * @params destination_node_index the destination node index
     * @params candidate_boxes all boxes to be considered
     */
//    double compute_score(
//        const IdVect& node_ids,
//        const IdType& destination_node_id,
//        OrderedSuperBoxes::iterator& bit);

    /**
     * Compute the correlation matrix of a set of SuperBoxes
     * @params super_boxes the given SuperBoxes
     * @params correlation_matrix returns the correlation matrix
     */
//    void compute_correlation_matrix(
//        const super_box_map& super_boxes,
//        super_box_pair_correlations& correlation_matrix ) const;

    /**
     * Returns the node pair with largest correlation
     * Returns null if all pairs has been checked
     * @params correlation_matrix the given CorrelationMatrix
     *
     */
//    SuperBoxPairCorrelations::const_iterator get_largest_correlation_pair(
//        const super_box_pair_correlations& correlation_matrix,
//        const super_box_pair_correlations& checked_pairs) const;

    /**
     * Put SupBox pairs that are adjacetn to the given SuperBox pair and
     * whose correlation with one of the given SuperBox is larger than the given threshold
     */
//    void get_adjacent_super_box_correlations(const super_box_pair& sbox_pair, super_box_pair_correlations& candidate_boxes, double threshold);

    /**
     * Returns the candidate pair with largest score
     */
//    SuperBoxPairCorrelations::const_iterator get_max_score_candidate_pair(
//        const super_box_pair& sbox_pair,
//        const super_box_pair_correlations& candidate_pairs);

    /**
     * Remove any pair that conatains any superbox in the given pair
     */
//    void CorrelationBasedLoadDistributor::remove_checke_pair(
//        const super_box_pair& sbox_pair,
//        super_box_pair_correlations& checked_pair);
    
    /**
     * Print the correlation matrix of the system
     */
//    void print_correlation_matrix();


////////////////////////////////////////////////////////////////////////////
//  Obsolete
////////////////////////////////////////////////////////////////////////////

    /**
     * Returns the candidate boxes for box migration and order
     * the box sets by their score
     * The average load of the candidate boxes must be smaller than
     * a given threshold (max_load)
     * @param max_load maximum average load of the box
     * @param candidate_box_sets returns the candidate box sets
     */
//    void order_candidate_boxes_by_score(
//        const IdSet& sender_boxes,
//        const IdSet& receiver_boxes,
//        double max_load,
//        DescendingIdSet& ordered_candidate_box_sets) const;

    /**
     * Returns the candidate boxes for box migration and order
     * the box sets by their score
     * The average load of the candidate boxes must be smaller than
     * a given threshold (max_load)
     * @param sender_id ID of the sender node
     * @param receiver_id ID of the receiver node
     * @param max_load maximum average load of the box
     * @param candidate_box_sets returns the candidate box sets
     */
 //   void order_candidate_boxes_by_score(
 //       const AscendingIdSet& other_nodes_boxes,
 //       const IdSet& receiver_boxes,
 //       double max_load,
 //       DescendingIdSet& ordered_candidate_box_sets) const;

    /**
     * Returns the score of the given box set
     */
 //   double get_score(
 //       const IdSet& sender_boxes,
 //       const IdSet& receiver_boxes,
 //       const IdSet& box_set) const;
 //
    /**
     * Returns the score of the given box set
     */
 //   double get_score(
 //       const AscendingIdSet& other_nodes_boxes,
 //       const IdSet& receiver_boxes,
 //       const IdSet& box_set) const;
 //
////////////////////////////////////////////////////////////////////////////
//  End obsolete
////////////////////////////////////////////////////////////////////////////

  // Attribute(s)
  private:
    /**
     * The handle of the LMSimpleDistributor
     */
    static LMCorrelationBasedLoadDistributor* _singleton;

    /**
     * pair wise distribution method
     */
    PairwiseLoadDistributionMethod _pairwise_method;

    /**
     * Score function type
     */
    ScoreFunctionType _score_function_type;

};

BOREALIS_NAMESPACE_END
#endif

