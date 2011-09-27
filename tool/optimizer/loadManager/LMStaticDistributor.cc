#include "LMStaticDistributor.h"
#include "LMParams.h"
#include "LMToolFunctions.h"
#include "BorealisToolFunctions.h"
#include "LMStatisticsManager.h"
#include "LMNodeManager.h"
#include "LMIDMatrix.h"
#include "LMQueryNetwork.h"
#include "LMCorrelationBasedLoadDistributor.h"

#include <cmath>


BOREALIS_NAMESPACE_BEGIN

#define PRINT_NODE_LOAD_COEFFICIENT_MATRIX
//#define DEBUG_RESILIENT

LMStaticDistributor* LMStaticDistributor::_singleton = 0;

LMStaticDistributor::LMStaticDistributor():_fs_sequence_loaded(false) {}


void LMStaticDistributor::determine_reference_arcs(
        LMArcMap& reference_arcs)
{
    reference_arcs = LMQueryNetwork::get_singleton()->get_input_arcs();
}

///////////////////////////////////////////////////////////
void LMStaticDistributor::write_distribution_plans()
{
    NOTICE << "write_distribution_plans ... ";

    // create a super graph
    _super_graph = new LMSuperGraph(LMQueryNetwork::get_singleton()->get_boxes());

    // cluster operators into super operators
//    if (LMParams::distribution_method.operator_clustering_active) {
//        LMToolFunctions::cluster_boxes(*_super_graph);
//    }

    string method_name;
    method_name = "Resilient";
    resilient_operator_distribution(0);
    BorealisToolFunctions::write_plan(method_name);
    print_node_load_coefficient_matrix(method_name);

    method_name = "Resilient-Lower-Bound-Avg";
    resilient_operator_distribution(1);
    BorealisToolFunctions::write_plan(method_name);
    print_node_load_coefficient_matrix(method_name);

    method_name = "Resilient-Lower-Bound-Min";
    resilient_operator_distribution(2);
    BorealisToolFunctions::write_plan(method_name);
    print_node_load_coefficient_matrix(method_name);

    method_name = "Random";
    random_box_distribution();
    BorealisToolFunctions::write_plan(method_name);
    print_node_load_coefficient_matrix(method_name);

    method_name = "LLF-Load-Balancing";
//    method_name = "Load-Balancing";
    largest_load_first_load_balancing();
    BorealisToolFunctions::write_plan(method_name);
    print_node_load_coefficient_matrix(method_name);

    method_name = "Max-Rate-Load-Balancing";
//    method_name = "Load-Balancing";
    max_rate_load_balancing();
    BorealisToolFunctions::write_plan(method_name);
    print_node_load_coefficient_matrix(method_name);

    method_name = "Random-Load-Balancing";
    random_load_balancing();
    BorealisToolFunctions::write_plan(method_name);
    print_node_load_coefficient_matrix(method_name);

    method_name = "Connected-Load-Balancing";
    connected_load_balacing();
    BorealisToolFunctions::write_plan(method_name);
    print_node_load_coefficient_matrix(method_name);

    method_name = "Correlation-Based";
    LMParams::distribution_method.initial_distribution_score_function_type =
        SF_CORRELATION_BASED;
    LMCorrelationBasedLoadDistributor::get_singleton()->
        global_load_distribution();
    BorealisToolFunctions::write_plan(method_name);
    print_node_load_coefficient_matrix(method_name);

    for (int i = 1; 
         i <= LMParams::distribution_param.number_of_random_static_distributions; 
         ++i) 
    {
        set_random_load();

        char method_name_buf[64];
        sprintf(method_name_buf, "Random_r%d", i);
        random_box_distribution();
        BorealisToolFunctions::write_plan(method_name_buf);
        print_node_load_coefficient_matrix(method_name_buf);

        sprintf(method_name_buf, "LLF-Load-Balancing_r%d", i);
//        sprintf(method_name_buf, "Load-Balancing_r%d", i);
        largest_load_first_load_balancing();
        BorealisToolFunctions::write_plan(method_name_buf);
        print_node_load_coefficient_matrix(method_name_buf);

        sprintf(method_name_buf, "Max-Rate-Load-Balancing_r%d", i);
        max_rate_load_balancing();
        BorealisToolFunctions::write_plan(method_name_buf);
        print_node_load_coefficient_matrix(method_name_buf);
 
        sprintf(method_name_buf, "Random-Load-Balancing_r%d", i);
        random_load_balancing();
        BorealisToolFunctions::write_plan(method_name_buf);
        print_node_load_coefficient_matrix(method_name_buf);
 
        sprintf(method_name_buf, "Connected-Load-Balancing_r%d", i);
        connected_load_balacing();
        BorealisToolFunctions::write_plan(method_name_buf);
        print_node_load_coefficient_matrix(method_name_buf);

        sprintf(method_name_buf, "Correlation-Based_r%d", i);
        LMParams::distribution_method.initial_distribution_score_function_type 
            = SF_CORRELATION_BASED;
        LMCorrelationBasedLoadDistributor::get_singleton()->
            global_load_distribution();
        BorealisToolFunctions::write_plan(method_name_buf);
        print_node_load_coefficient_matrix(method_name_buf);
    }
    
    // write opertor load coefficient matrix to file
    // determins the reference arcs. now use input arcs
    LMArcMap reference_arcs;
    determine_reference_arcs(reference_arcs);
    // Comupte operator load coefficient matrix
    LMIDMatrix operator_load_coefficient_matrix(reference_arcs.size());
    LMToolFunctions::fill_load_coefficient_matrix(
            reference_arcs, 
            operator_load_coefficient_matrix);
    ofstream ofs;
    string filename =
          LMParams::runtime_param.output_dir + "data/" +
                 LMParams::runtime_param.output_prefix + "H";
    ofs.open(filename.c_str(), ios::out);
    if (!ofs)
    {
        WARN << "can not open file " << filename;
        return;
    }
    for (LMIDMatrix::const_iterator 
            it = operator_load_coefficient_matrix.begin(); 
            it != operator_load_coefficient_matrix.end(); 
            ++it)
    {
        for (unsigned int i = 0; i < it->second.size(); ++i) 
        {
            ofs << setw(5) << it->second[i] << " ";
        }
        ofs << endl;
    }
    ofs.close();

    // write connection matrix to file
    filename = LMParams::runtime_param.output_dir + "data/" +
                 LMParams::runtime_param.output_prefix + "CM";
    ofs.open(filename.c_str(), ios::out);
    if (!ofs)
    {
        WARN << "can not open file " << filename;
        return;
    }
    IdVect box_ids;
    for (LMIDMatrix::const_iterator 
            it = operator_load_coefficient_matrix.begin(); 
            it != operator_load_coefficient_matrix.end(); 
            ++it)
    {
        box_ids.push_back(it->first);
    }
    for (unsigned int i = 0; i < box_ids.size(); ++i)
    {
        for (unsigned int j = 0; j < box_ids.size(); ++j)
        {
            if (LMQueryNetwork::get_singleton()->is_connected(
                        box_ids[i], box_ids[j]))
            {
                ofs << 1 << " ";
            }
            else
            {
                ofs << 0 << " ";
            }
        }
        ofs << endl;
    }

    ofs.close();

    delete _super_graph;
    NOTICE << "write_distribution_plans finished.";

    if (LMParams::runtime_param.status_filename != "")
    {
        ofs.open(LMParams::runtime_param.status_filename.c_str(), ios::out);
        if (ofs)
        {
            ofs << "experimentFinished" << endl;
        }
        else
        {
            NOTICE << "Can not open status file : " << LMParams::runtime_param.status_filename.c_str();
        }
        ofs.close();
    }
    exit(0);
}

void LMStaticDistributor::resilient_operator_distribution(int lower_bound_mode)
{
    // determins the reference arcs
    // now use input arcs
    LMArcMap reference_arcs;
    determine_reference_arcs(reference_arcs);
    // Comupte operator load coefficient matrix
    LMIDMatrix operator_load_coefficient_matrix(reference_arcs.size());
    LMToolFunctions::fill_load_coefficient_matrix(
            reference_arcs, 
            operator_load_coefficient_matrix);

//    cout << "operator_load_coefficient_matrix: " << endl;
//    cout << operator_load_coefficient_matrix << endl;

    
    // determins the stream rates to test feasible set size
    vector<double> row_sum;
    operator_load_coefficient_matrix.get_row_sum(row_sum);
    ofstream srfs;
    string stream_rate_filename =
          LMParams::runtime_param.output_dir + "xml/" +
                 LMParams::runtime_param.output_prefix + "stream_rate_multiplier";
    srfs.open(stream_rate_filename.c_str(), ios::out);
    if (!srfs)
    {
        WARN << "can not open file " << stream_rate_filename;
        return;
    }
    ofstream srprodfs;
    string stream_rate_product_filename =
          LMParams::runtime_param.output_dir + "data/" +
                 LMParams::runtime_param.output_prefix + "ideal_fss";
    srprodfs.open(stream_rate_product_filename.c_str(), ios::out);
    if (!srprodfs)
    {
        WARN << "can not open file " << stream_rate_product_filename;
        return;
    }

    LMArcMap::const_iterator refarc_it = reference_arcs.begin();
#ifdef DEBUG_RESILIENT
    cout << "Maximum stream rates: " << endl;
#endif
    double ideal_fss = 1;
    for (unsigned int i = 0; i < row_sum.size(); ++i)
    {
        double rate = 
             (double)LMNodeManager::get_singleton()->get_lmnodes().size() /  
                row_sum[i] ;
        srfs << get_stream_id(refarc_it->first) << "  " << rate << endl;
#ifdef DEBUG_RESILIENT
        cout << get_stream_id(refarc_it->first) << "  " << rate << endl;
#endif
        ideal_fss *= rate / (i+1);
        ++refarc_it;
    }
    srprodfs << ideal_fss << endl;
    srprodfs.close();
    srfs.close();
#ifdef DEBUG_RESILIENT
    cout << "Ideal Feasible Set Size: " << ideal_fss << endl;
    cout << endl << "lower bound: ";
#endif

    // Compute the lower bound for the stream rates
    vector<double> lower_bound;
    for (LMArcMap::const_iterator it = reference_arcs.begin();
            it != reference_arcs.end();
            ++it)
    {
        if (lower_bound_mode == 1)
        {
            lower_bound.push_back(
                LMStatisticsManager::get_singleton()->get_time_series(
                    LMStatisticsManager::STREAM_RATE, it->first).get_average());
        }
        else if (lower_bound_mode == 2)
        {
            lower_bound.push_back(
                LMStatisticsManager::get_singleton()->get_time_series(
                    LMStatisticsManager::STREAM_RATE, it->first).get_minimum());
        }
        else
        {
            lower_bound.push_back(0);
        }
#ifdef DEBUG_RESILIENT
        cout << lower_bound.back() << " ";
#endif
    }
#ifdef DEBUG_RESILIENT
    cout << endl;
    cout << "normalized bound: ";
#endif
    // normalized the lower bound
    for (unsigned int i = 0; i < lower_bound.size(); ++i)
    {
        lower_bound[i] = lower_bound[i] * row_sum[i] / 
            LMNodeManager::get_singleton()->get_lmnodes().size();
#ifdef DEBUG_RESILIENT
        cout << lower_bound[i] << " ";
#endif
    }
#ifdef DEBUG_RESILIENT
    cout << endl << endl;
#endif
    
    // Normalize opertor cload coefficient matrix
    operator_load_coefficient_matrix.normalize();

#ifdef DEBUG_RESILIENT
    cout << "Normalizded operator_load_coefficient_matrix: " << endl;
    cout << operator_load_coefficient_matrix << endl;
#endif


    // Add opertor distribution method
    vector<string> operator_distribution_methods;
    operator_distribution_methods.push_back("Resilient");
    //operator_distribution_methods.push_back("Max-Min-Plane-Distance");
    //operator_distribution_methods.push_back("Max-Min-Axis-Distance");


    // Distribute operators
    for (unsigned int i = 0; i < operator_distribution_methods.size(); ++i)
    {
        LMNodeManager::get_singleton()->clear_all_lmnodes();

        // Order the operators
        IdValuePairVect ordered_operator_index;
        order_operators(
                operator_load_coefficient_matrix,
                operator_distribution_methods[i],
                ordered_operator_index);

        // Distribute the operators
        map<IdType, IdSet> operator_sets;
        distribute_operators(
                operator_load_coefficient_matrix,
                lower_bound,
                ordered_operator_index,
                operator_distribution_methods[i],
                operator_sets);

        // Put the operators onto the nodes
        for (map<IdType, IdSet>::const_iterator it = operator_sets.begin();
                it != operator_sets.end();
                ++it)
        { 
            LMNodeManager::get_singleton()->add_boxes(it->first, it->second);
        }

    }
}




void LMStaticDistributor::distribute_operators(
            const LMIDMatrix& operator_load_coefficient_matrix,
            const vector<double>& lower_bound,
            const IdValuePairVect& ordered_operator_index,
            const string& distribution_method,
            map<IdType, IdSet>& operator_sets) 
{
    // number of input streams
    unsigned int d = operator_load_coefficient_matrix.get_dimension();

    // initialize node load coefficient matrix
    LMIDMatrix node_load_coefficient_matrix(d);
    for (LMNodeMap::const_iterator 
            it = LMNodeManager::get_singleton()->get_lmnodes().begin();
            it != LMNodeManager::get_singleton()->get_lmnodes().end();
            ++it)
    {
        node_load_coefficient_matrix.add_new_entry(it->first);
    }

    // initialize initialized number of connected edges matrix
    IdPairValueMap number_of_connected_edges_matrix;
    for (LMNodeMap::const_iterator 
            it = LMNodeManager::get_singleton()->get_lmnodes().begin();
            it != LMNodeManager::get_singleton()->get_lmnodes().end();
            ++it)
    {
        const IdType node_id = it->first;
        for (unsigned int j = 0; j < ordered_operator_index.size(); ++j)
        {
            const IdType& operator_id = ordered_operator_index[j].first;
            number_of_connected_edges_matrix[IdPair(node_id, operator_id)] = 0;
        }
    }


    // assign each operators
    for (unsigned int j = 0; j < ordered_operator_index.size(); ++j)
    {
        const IdType& operator_id = ordered_operator_index[j].first;
        LMIDMatrix::const_iterator it = 
            operator_load_coefficient_matrix.find(operator_id);
        assert(it != operator_load_coefficient_matrix.end());
        const vector<double>& v = it->second;

        LMIDMatrix P_add_v = node_load_coefficient_matrix;
        P_add_v.add(v);
        LMIDMatrix P_add_v_related = P_add_v;
        P_add_v_related.remove_unrelated_columns(v);


        IdValuePairVect minimizing_metric;
        get_minimizing_metric(
                P_add_v, 
                P_add_v_related,
                lower_bound,
                number_of_connected_edges_matrix,
                operator_id,
                distribution_method,
                minimizing_metric);

        double minimum = 1000000;
        int num = 0;
        IdType node_id;
        for (unsigned int i = 0; i < minimizing_metric.size(); ++i)
        {
            if (minimizing_metric[i].second < minimum) 
            {
                minimum = minimizing_metric[i].second;
                node_id = minimizing_metric[i].first;
            }
            if (minimizing_metric[i].second == minimum) 
            {
                ++num;
                if (drand48() < 1.0 / (double)num ) 
                {
                    node_id = minimizing_metric[i].first;
                }
            }
        }

        // assign operator to the selected node
        node_load_coefficient_matrix.add(node_id, v);
        operator_sets[node_id].insert(operator_id);

        // update number of connected edges matrix
        // for all operators connected to the assinged operator, increase
        // their nubmer of connected edges. (Since the value for the operators
        // that are already assigend is never used, it does not matter what
        // their values is.
        LMBoxVect connected_boxes;
        LMQueryNetwork::get_singleton()->get_box(operator_id)->
            get_connected_boxes(connected_boxes);

        for (unsigned int k = 0; k < connected_boxes.size(); ++k)
        {
            ++number_of_connected_edges_matrix[
                IdPair(node_id, connected_boxes[k]->get_id())];
        }
    }
}

void LMStaticDistributor::order_operators(
            const LMIDMatrix& operator_load_coefficient_matrix,
            const string& distribution_method,
            IdValuePairVect& ordered_operator_index)
{
    if (distribution_method == "Resilient" || 
        distribution_method == "Max-Min-Plane-Distance" ||
        distribution_method == "Max-Min-Axis-Distance")
    {
        LMToolFunctions::order_id_matrix(
            operator_load_coefficient_matrix,
            VOM_NORM2,
            "descending",
            ordered_operator_index);
    }
    if (distribution_method == "Random")
    {
        LMToolFunctions::order_id_matrix(
            operator_load_coefficient_matrix,
            VOM_RAND,
            "descending",
            ordered_operator_index);
    }
}

void LMStaticDistributor::get_minimizing_metric(
        const LMIDMatrix& P_add_v,
        const LMIDMatrix& P_add_v_related,
        const vector<double>& lower_bound,
        const IdPairValueMap& number_of_connected_edges_matrix,
        const IdType& operator_id,
        const string& distribution_method,
        IdValuePairVect& minimizing_metric) 
{
    bool match = false;
    unsigned int n = P_add_v.size();
    LMIDMatrix::const_iterator it_related = P_add_v_related.begin();
    for (LMIDMatrix::const_iterator it = P_add_v.begin();
            it != P_add_v.end();
            ++it) {
        double value = 0;
        if (distribution_method == "Random")
        {
            value = drand48();
            match = true;
        }
        if (distribution_method == "Resilient")
        {
            const IdType& node_id = it->first;
            // for nodes with hyperplane above the ideal hyper plane, we give it
            // a lower value in addition to the inserve distance
            double lower = -10000.0;
            // consider the number of connected edges too
            IdPairValueMap::const_iterator pit = 
                number_of_connected_edges_matrix.find(
                        IdPair(node_id, operator_id));
            assert(pit != number_of_connected_edges_matrix.end());
            lower -= pit->second * 10000.0;
            double prod = 0;
            for (unsigned int i = 0; i < it->second.size(); ++i) 
            {
                value += it->second[i] * it->second[i];
                prod += it->second[i] * lower_bound[i];
                if (it->second[i] > 1.0 / (double)n) {
                    lower = 0;
                }
            }
            value = sqrt(value) / (1.0 - prod)  + lower;
            match = true;
        }
        if (distribution_method == "Max-Min-Plane-Distance")
        {
            for (unsigned int i = 0; i < it->second.size(); ++i) 
            {
                value += it->second[i] * it->second[i];
            }
            value = sqrt(value);
            match = true;
        }
        if (distribution_method == "Max-Min-Axis-Distance")
        {
            value = *max_element(
                    it_related->second.begin(), 
                    it_related->second.end()); 
            match = true;
        }
        ++it_related;
        minimizing_metric.push_back(IdValuePair(it->first, value)); 
    }

    if (match == false) 
    {
        cerr << "LMStaticDistributor::get_minimizing_metric: "
             << "invalid distribution_method "
             << distribution_method
             << endl;
        exit(1);
    }
}





////////////////////////////////////////////////////////////////////////

void LMStaticDistributor::distribute_boxes(
        const AscendingSuperBoxSets& super_box_sets)
{
    LMNodeManager::get_singleton()->clear_all_lmnodes();
    LMNodeMap::const_iterator nit =
        LMNodeManager::get_singleton()->get_lmnodes().begin();
    for (AscendingSuperBoxSets::const_iterator it = super_box_sets.begin();
            it != super_box_sets.end();
            ++it)
    {
        const IdType& node_id = nit->first;
        const LMSuperBoxSet& sbox_set = it->second;
        for (LMSuperBoxSet::const_iterator sbit = sbox_set.begin();
                sbit != sbox_set.end();
                ++sbit)
        {
            LMSuperBox* sbox = *sbit;
            LMNodeManager::get_singleton()->add_boxes(node_id, sbox->get_boxes());
        }
        ++nit;
    }
}


void LMStaticDistributor::random_box_distribution()
{
    // initialize empty super box set
    AscendingSuperBoxSets boxsets;
    for (unsigned int i = 0;
            i < LMNodeManager::get_singleton()->get_lmnodes().size();
            ++i) {
        boxsets.insert(
           boxsets.begin(), pair<double, LMSuperBoxSet>(0.0, LMSuperBoxSet()));
    }


    // order super boxes randomly
    DescendingSuperBoxes super_boxes;
    for (LMSuperBoxMap::const_iterator 
            it = _super_graph->get_super_boxes().begin();
            it != _super_graph->get_super_boxes().end();
            ++it)
    {
        super_boxes.insert(
           super_boxes.end(),
           pair<double, LMSuperBox*>(drand48(), it->second));
    }

    // distribute all the super boxes one by one
    while (!super_boxes.empty())
    {
        // put the first operator to the group with the least number of
        // operators
        DescendingSuperBoxes::iterator it = super_boxes.begin();

        boxsets.begin()->second.insert(it->second);
        AscendingSuperBoxSets::value_type tmp = 
            AscendingSuperBoxSets::value_type (
                    boxsets.begin()->first + 1.0,
                    boxsets.begin()->second);
        boxsets.erase(boxsets.begin());
        boxsets.insert(tmp);

        super_boxes.erase(it);
    }


    distribute_boxes(boxsets);
}

void LMStaticDistributor::random_load_balancing()
{
    greedy_load_balancing(false);
}

void LMStaticDistributor::largest_load_first_load_balancing()
{
    greedy_load_balancing(true);
}

void LMStaticDistributor::max_rate_load_balancing()
{
    greedy_load_balancing(true);
}


void LMStaticDistributor::greedy_load_balancing(bool order_by_load)
{
    // initialize empty super box set
    AscendingSuperBoxSets boxsets;
    for (unsigned int i = 0;
            i < LMNodeManager::get_singleton()->get_lmnodes().size();
            ++i) {
        boxsets.insert(
           boxsets.begin(), pair<double, LMSuperBoxSet>(0.0, LMSuperBoxSet()));
    }


    // order super boxes 
    DescendingSuperBoxes super_boxes;
    if (order_by_load)
    {
        for (LMSuperBoxMap::const_iterator 
                it = _super_graph->get_super_boxes().begin();
                it != _super_graph->get_super_boxes().end();
                ++it)
        {
            super_boxes.insert(
               super_boxes.end(),
               pair<double, LMSuperBox*>(
                   LMStatisticsManager::get_singleton()->get_boxes_average_load(
                       it->second->get_boxes()),
                   it->second));
        }
    } 
    else
    {
        for (LMSuperBoxMap::const_iterator 
                it = _super_graph->get_super_boxes().begin();
                it != _super_graph->get_super_boxes().end();
                ++it)
        {
            super_boxes.insert(
               super_boxes.end(),
               pair<double, LMSuperBox*>(drand48(), it->second));
        }
    }
        

    // distribute all the super boxes one by one
    while (!super_boxes.empty())
    {
        // put the first operator to the group with the least load 
        DescendingSuperBoxes::iterator it = super_boxes.begin();
        double sbox_load = 
               LMStatisticsManager::get_singleton()->get_boxes_average_load(
                       it->second->get_boxes());

        boxsets.begin()->second.insert(it->second);
        AscendingSuperBoxSets::value_type tmp = 
            AscendingSuperBoxSets::value_type (
                    boxsets.begin()->first + sbox_load,
                    boxsets.begin()->second);
        boxsets.erase(boxsets.begin());
        boxsets.insert(tmp);

        super_boxes.erase(it);
    }


    distribute_boxes(boxsets);
}

void LMStaticDistributor::connected_load_balacing()
{
    // initialize empty super box set
    AscendingSuperBoxSets boxsets;
    for (unsigned int i = 0;
            i < LMNodeManager::get_singleton()->get_lmnodes().size();
            ++i) {
        boxsets.insert(
           boxsets.begin(), pair<double, LMSuperBoxSet>(0.0, LMSuperBoxSet()));
    }


    // order super boxes by load
    DescendingSuperBoxes super_boxes;
    for (LMSuperBoxMap::const_iterator 
            it = _super_graph->get_super_boxes().begin();
            it != _super_graph->get_super_boxes().end();
            ++it)
    {
        double sbox_load = 
            LMStatisticsManager::get_singleton()->get_boxes_average_load(
                       it->second->get_boxes());
        super_boxes.insert(
           super_boxes.end(),
           pair<double, LMSuperBox*>(sbox_load, it->second));
    }

    // compate system average load
    double system_avg_load = 
        LMStatisticsManager::get_singleton()->get_system_total_load() / 
        (double)(LMNodeManager::get_singleton()->get_lmnodes().size());

    // distribute all the super boxes one by one
    while (!super_boxes.empty())
    {
        // get the node with least load
        LMSuperBoxSet& boxset = boxsets.begin()->second;
        double boxset_load = boxsets.begin()->first;

        vector<LMSuperBox*> expanding_list;
        expanding_list.clear();
        expanding_list.push_back(super_boxes.begin()->second);
        bool first = true;
        while (!expanding_list.empty())
        {
            LMSuperBox* sbox = expanding_list[0];
            double sbox_load = 
                LMStatisticsManager::get_singleton()->get_boxes_average_load(
                       sbox->get_boxes());
            double new_boxset_load = boxset_load + sbox_load;  
            if (first || new_boxset_load <= system_avg_load) 
            {
                first = false;
                // put the super box on the node 
                boxset.insert(sbox);
                // update load
                boxset_load = boxset_load + sbox_load;
                // erase sbox from candidate box list
                for (DescendingSuperBoxes::iterator it = super_boxes.begin();
                        it != super_boxes.end();
                        ++it) 
                {
                    if (it->second == sbox) 
                    {
                        super_boxes.erase(it);
                        break;
                    }
                }

                // add boxes connected to the sbox to to the expanding list
                LMSuperBoxMap connected_sboxes;
                sbox->get_connected_super_boxes(connected_sboxes);
                for (DescendingSuperBoxes::const_iterator 
                        it = super_boxes.begin();
                        it != super_boxes.end();
                        ++it)
                {
                    LMSuperBox* candidate_sbox = it->second;
                    if (connected_sboxes.find(candidate_sbox->get_id()) != 
                            connected_sboxes.end()) 
                    {
                        // the candidate_sbox is connected to sbox, 
                        // so add it to the expanding list
                        expanding_list.push_back(candidate_sbox);
                    }
                }
            }
            // erase sbox from the expanding list
            expanding_list.erase(expanding_list.begin());
        }

        // update node load in the AscendingSuperBoxSets


        AscendingSuperBoxSets::value_type tmp = 
            AscendingSuperBoxSets::value_type (
                    boxset_load,
                    boxsets.begin()->second);
        boxsets.erase(boxsets.begin());
        boxsets.insert(tmp);
    }


    distribute_boxes(boxsets);
}

void LMStaticDistributor::set_random_load()
{
    if (!_fs_sequence_loaded)
    {
        unsigned int dimenstion = 
            LMQueryNetwork::get_singleton()->get_input_arcs().size(); 
        load_fs_sequence(dimenstion);
    }
    int i = 0;
    // set random stream rate for all input streams
    for (LMArcMap::const_iterator 
            it = LMQueryNetwork::get_singleton()->get_input_arcs().begin();
            it != LMQueryNetwork::get_singleton()->get_input_arcs().end();
            ++it) 
    {
        // generate random mean
        double mean;
        if (_fs_sequence_loaded)
        {
            mean = _fs_sequence[i].back();
            _fs_sequence[i].pop_back();
        }
        else
        {
            mean = drand48();
        }

        // generate random bound
        vector<double> noise;
        double avg_noise = 0;
        for (int i = 0; i < LMParams::distribution_param.number_of_samples; ++i) 
        {
            noise.push_back(drand48());
            avg_noise += noise.back();
        }
        avg_noise /= noise.size();

        double variance = drand48();

        // set all values to be the random rate
        for (int i = 0; i < LMParams::distribution_param.number_of_samples; ++i) 
        {
            double rate = mean + variance * (noise[i] - avg_noise);
            LMStatisticsManager::get_singleton()->update_arc_rate(
                    it->first,
                    rate);
        }
    }

    // update load
    LMStatisticsManager::get_singleton()->update_statistics(
            LMParams::distribution_param.number_of_samples);
}

void LMStaticDistributor::print_node_load_coefficient_matrix(
            const string& distribution_method)
{
#ifdef PRINT_NODE_LOAD_COEFFICIENT_MATRIX
    // determins the reference arcs
    // now use input arcs
    LMArcMap reference_arcs;
    determine_reference_arcs(reference_arcs);
    // Comupte operator load coefficient matrix
    LMIDMatrix operator_load_coefficient_matrix(reference_arcs.size());
    LMToolFunctions::fill_load_coefficient_matrix(
            reference_arcs, 
            operator_load_coefficient_matrix);
    operator_load_coefficient_matrix.normalize();

    unsigned int d = operator_load_coefficient_matrix.get_dimension();

    LMIDMatrix node_load_coefficient_matrix(d);
    for (LMNodeMap::const_iterator 
            it = LMNodeManager::get_singleton()->get_lmnodes().begin();
            it != LMNodeManager::get_singleton()->get_lmnodes().end();
            ++it)
    {
        node_load_coefficient_matrix.add_new_entry(it->first);
        const LMNode& lmnode = it->second;
        for (IdSet::const_iterator bit = lmnode.get_boxes().begin();
                bit != lmnode.get_boxes().end();
                ++bit) 
        {
            const vector<double>& v = 
                operator_load_coefficient_matrix.find(*bit)->second;
            node_load_coefficient_matrix.add(it->first, v);
        }
    }

    cout << distribution_method << endl;
    cout << node_load_coefficient_matrix;
//    cout << *(LMNodeManager::get_singleton());
    LMStatisticsManager::get_singleton()->output_statistics(
                    cout, LMStatisticsManager::NODE_LOAD);
    cout << endl;

    ofstream ofs;
    string filename =
          LMParams::runtime_param.output_dir + "data/" +
                 LMParams::runtime_param.output_prefix + 
                 distribution_method +
                 "_P";
    ofs.open(filename.c_str(), ios::out);
    if (!ofs)
    {
        WARN << "can not open file " << filename;
        return;
    }
    for (LMIDMatrix::const_iterator 
            it = node_load_coefficient_matrix.begin(); 
            it != node_load_coefficient_matrix.end(); 
            ++it)
    {
        for (unsigned int i = 0; i < it->second.size(); ++i) 
        {
            ofs << setw(5) << it->second[i] << " ";
        }
        ofs << endl;
    }
    ofs.close();
#endif

}

void LMStaticDistributor::load_fs_sequence(unsigned int dimension)
{
    // read data size 
    vector<int> data_size;
    ifstream ifs;
    string fn = LMParams::runtime_param.load_manager_path +
        "fs_sequence/fs_sequence_size";
    ifs.open(fn.c_str(), ios::in);
    if (!ifs) {
        cerr << "Can not open fs sequence size file " << fn << endl;
        return;
    }
    unsigned int count = 0;
    while (!ifs.eof() && count < 15)
    {
        ++count;
        double a;
        ifs >> a;
        data_size.push_back((int)a);
    }

    ifs.close();

    if (dimension > count) 
    {
        cerr << "LMStaticDistributor: dimension higher than " << count 
             << "is not supported." << endl;
        return;
    }

    fstream of;
    ostringstream filename;  
    filename << LMParams::runtime_param.load_manager_path  
             << "fs_sequence/fs_sequence" 
             << dimension << + ".dat";
    of.open(filename.str().c_str(), ios::in);
    if (!of)
    {
            cerr << "Can not open fs sequence file " << filename.str() << endl;
            return;
    }
    for (unsigned int i = 0; i < dimension; ++i)
    {
        _fs_sequence.push_back(vector<double>());
        for (int j = 0; j < data_size[dimension - 1]; ++j) 
        {
            double a;
            of >> a;
            _fs_sequence[i].push_back(a);
        }
    }
    of.close();

    _fs_sequence_loaded = true;
}


BOREALIS_NAMESPACE_END
