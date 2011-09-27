#ifndef LMToolFunctions_H
#define LMToolFunctions_H

#include "LMCommon.h"
#include "LMParamTypes.h"
#include "LMSuperGraph.h"
#include "LMIDMatrix.h"

BOREALIS_NAMESPACE_BEGIN

/**
 * The LMNodeManager manage all LMNodes
 */
class LMToolFunctions
{
public:

    /**
     * Returns the node pair to balance load
     * @params load_balancing_threshold the load different of each node pair must be larger than this threshold
     * @params sender_min_load the load of the sender node must be larger than this threshold
     * @params lbnodes the given LMNodes
     * @params node_pairs returns the result node pairs
     */
    static void get_imbalanced_node_pairs(
        double load_balancing_threshold,
        double sender_min_load,
        IdPairVect& node_pairs,
        bool use_divergent_load = false);

    /**
     * Returns the more loaded node from a node pair
     */
    static const IdType& get_more_loaded_node_id(
            const IdPair& node_pair, bool use_divergent_load = false);

    /**
     * Returns the less loaded node from a node pair
     */
    static const IdType& get_less_loaded_node_id(
            const IdPair& node_pair, bool use_divergent_load = false);

    /**
     * Returns the load variance reduction if the given boxes are moved
     * from the sender to the receiever
     */
    static double get_migration_load_variance_reduction(
        const IdType& sender_id,
        const IdType& receiver_id,
        const IdSet& box_ids);

    /**
     * Returns the load variance reduction if the given boxes are moved
     * from the sender to the receiever
     */
    static double get_migration_correlation_increase(
        const IdType& sender_id,
        const IdType& receiver_id,
        const IdSet& box_ids);

    /**
     * Returns the load variance reduction if the given boxes are moved
     * from the sender to the receiever
     */
    static double get_migration_correlation_based_score(
        const IdType& sender_id,
        const IdType& receiver_id,
        const IdSet& box_ids);



    ////////////////////////////////////////////////////////
    // For static load distribution
    ////////////////////////////////////////////////////////

    /**
     * cluster the operators
     */
    static void cluster_boxes(
        LMSuperGraph& super_graph);

    /**
     * Fill in operator load coefficient matrix
     */
    static void fill_load_coefficient_matrix(
            const LMArcMap& reference_arcs,
            LMIDMatrix& operator_load_coefficient_matrix);

    /**
     * Order the ID matrix according some method
     * @param id_matrix the LMIDMatrix
     * @param order_method the ordering method
     * @param order either "ascending" or "descending"
     * @param ordered_index the index of ids the ordered list
     */
    static void order_id_matrix(
            const LMIDMatrix& id_matrix,
            VectorOrderingMethod order_method,
            const string& order,
            IdValuePairVect& ordered_id_index);




    ////////////////////////////////////////////////////////
    // Obsolete
    ////////////////////////////////////////////////////////

    /**
     * Returns the connected query graphs
     */
    static void get_connected_boxes(
        vector<IdSet>& connected_boxes);

    /**
     * Returns the connected query graphs
     */
    static void get_connected_super_box_sets(
        const LMSuperBoxMap& sboxes,
        vector<LMSuperBoxMap>& connected_sbox_sets);

    /**
     * Returns the connected query graphs
     */
    static void get_box_groups(
        const IdSet& boxes,
        vector<IdSet>& box_groups);

    /**
     * Return boxes in box set vector
     */
    static void get_box_sets(
        const IdSet& boxes,
        DescendingIdSet& box_set_vect);
};



BOREALIS_NAMESPACE_END
#endif
