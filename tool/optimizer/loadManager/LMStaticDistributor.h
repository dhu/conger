#ifndef LMStaticDistributor_H
#define LMStaticDistributor_H

#include "LMCommon.h"
#include "LMParamTypes.h"
#include "LMToolFunctions.h"

BOREALIS_NAMESPACE_BEGIN


/**
 * The LMStaticDistributor implements a load distributor that
 * make static load distribution plan
 */
class LMStaticDistributor
{
  // Method(s)
public:
    /**
     * Returns the singleton of the CentralizedLoadDistributor
     */
    static LMStaticDistributor* get_singleton()
    {
        if (_singleton == 0)
          _singleton = new LMStaticDistributor();
        return _singleton;
    }

    /**
     * Write different distribution plans to file
     */
    void write_distribution_plans();

private:
    /**
     * The handle of the LMStaticDistributor
     */
    static LMStaticDistributor* _singleton;

    /**
     * private constructor
     */
    LMStaticDistributor();

    /**
     * The vector that stores fs_sequence data (halton sequence in ideal
     * feasilbe set)
     */
    vector< vector<double> > _fs_sequence;

    /**
     * is true is fs_sequence is loaded successfully
     */
    bool _fs_sequence_loaded;


    /**
     * The handle of the super graph
     */
    LMSuperGraph* _super_graph;

    /////////////////////////////////////////////////
    // Operator distribution methods
    /////////////////////////////////////////////////

    /**
     * Resilient Operator Distribution
     * @param lower_bound_mode 0: 0 
     *                         1: avg
     *                         2: min
     */
    void resilient_operator_distribution(int lower_bound_mode = 0);

    /**
     * Distribute boxes randomly
     */
    void random_box_distribution();

    /**
     * Largest load first load balancing algorithm
     */
    void largest_load_first_load_balancing();

    /**
     * Balance load using maximum stream rate / box load
     */
    void max_rate_load_balancing();

    /**
     * Random load balancing algorithm
     */
    void random_load_balancing();

    /**
     * Connected load balancing algorithm
     */
    void connected_load_balacing();

    /////////////////////////////////////////////////
    // Tool methods
    /////////////////////////////////////////////////
    
    /**
     * Determines the reference streams as variables for operator distribution
     * @param reference_arcs returns the ids of the reference arcs
     */
    void determine_reference_arcs(LMArcMap& reference_arcs);

    /**
     * Order the boxes according to the operator distribution method
     * @param operator_load_coefficient_matrix the given 
     *                operator load coefficient matrix
     * @param distribution_method
     * @param ordered_operator_index return the result
     */
    void order_operators(
            const LMIDMatrix& operator_load_coefficient_matrix,
            const string& distribution_method,
            IdValuePairVect& ordered_operator_index);

    /**
     * Distribute the boxes
     * @param operator_load_coefficient_matrix the given 
     *                operator load coefficient matrix
     * @param distribution_method
     * @param operator_sets return the result
     */
    void distribute_operators(
            const LMIDMatrix& operator_load_coefficient_matrix,
            const vector<double>& lower_bound,
            const IdValuePairVect& ordered_operator_index,
            const string& distribution_method,
            map<IdType, IdSet>& operator_sets); 

    /**
     * Comput the minimizing metric for a given operator distribution method
     */
    
    void get_minimizing_metric(
            const LMIDMatrix& P_add_v,
            const LMIDMatrix& P_add_v_related,
            const vector<double>& lower_bound,
            const IdPairValueMap& number_of_connected_edges_matrix,
            const IdType& operator_id,
            const string& distribution_method,
            IdValuePairVect& minimizing_metric); 


    /**
     * Set randome load for the opertors.
     * It set random stream rate for all input stream rates and then compute the
     * load of the operators. 
     * This is for the load balancing approach
     * After this function, all data in the LMStatisticsManager are not valid
     * anymore
     */
    void set_random_load();

    /**
     * A load balancing algorithm. Order the opeators first and then distribute
     * the operators to the nodes one by one
     * @param order_by_load  true : order the operators by load
     *                       false: order the operators randomly
     */
    void greedy_load_balancing(bool order_by_load);

    /**
     * Distribute boxes to the nodes
     */
    void distribute_boxes(const AscendingSuperBoxSets& super_box_sets);


    /**
     * Print the node load coefficient matrix
     */
    void print_node_load_coefficient_matrix(const string& distribution_method);

    /**
     * load fs_sequence data
     */
    void load_fs_sequence(unsigned int dimention);

};

BOREALIS_NAMESPACE_END
#endif

