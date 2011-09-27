#ifndef LMSimpleDistributor_H
#define LMSimpleDistributor_H

#include "LMCommon.h"
#include "LMParamTypes.h"
#include "LMToolFunctions.h"

BOREALIS_NAMESPACE_BEGIN


/**
 * The LMSimpleDistributor class implements a load distributor that balance
 * the average load of the nodes.
 */
class LMSimpleDistributor
{
public:
    /**
     * Returns the singleton of the CentralizedLoadDistributor
     */
    static LMSimpleDistributor* get_singleton()
    {
        if (_singleton == 0)
          _singleton = new LMSimpleDistributor();
        return _singleton;
    }

    /**
     * Global operator distribution
     */
    void global_distribution() {}

    /**
     * Dynamic operator distribution
     */
    void pairwise_distribution();


private:
    /**
     * Creates the LMSimpleDistributor
     */
    LMSimpleDistributor():_score_function_type(SF_LOAD) {}

    /**
     * Balance the load between two nodes and only
     * move operators from the more loaded node to less loaded node
     * @param node_pair The IDs of the given node pair
     */
    bool one_way_load_balancing(const IdPair& node_pair);

    // TODO
    void selective_exchange(){}

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
    void order_candidate_boxes_by_score(
        const IdType& sender_id,
        const IdType& receiver_id,
        double max_load,
        DescendingIdSet& candidate_box_sets);

    /**
     * Returns the score of the given box set
     * @param sender_id ID of the sender node
     * @param receiver_id ID of the receiver node
     * @param box_ids IDs of the boxes
     * @return the score of the given box set
     */
    double get_score(
        const IdType& sender_id,
        const IdType& receiver_id,
        const IdSet& box_ids);


  // Attribute(s)
private:
    /**
     * The handle of the LMSimpleDistributor
     */
    static LMSimpleDistributor* _singleton;

    /**
     * The score function type
     */
    ScoreFunctionType _score_function_type;


};

BOREALIS_NAMESPACE_END
#endif

