
#include "LMCommon.h"

#include "LMIDMatrix.h"
#include "LMQueryNetwork.h"
#include "LMStatisticsManager.h"
#include "LMNodeManager.h"
#include "LMStaticDistributor.h"
#include "LMCorrelationBasedLoadDistributor.h"

using namespace std;
using namespace Borealis;
using namespace NMSTL;
int main(int argc, const char* argv[])
{
    if (argc == 2)
    {
       srand48(atoi(argv[1]));
    }

    // creat query graph
    LMBox* b1 = new LMBox("b1", "delay");
    LMBox* b2 = new LMBox("b2", "delay");
    LMBox* b3 = new LMBox("b3", "delay");
    LMBox* b4 = new LMBox("b4", "delay");
    LMBox* b5 = new LMBox("b5", "delay");
    LMBox* b6 = new LMBox("b6", "delay");

    LMArc* a1 = new LMArc("  ->b1",  0, 0, b1, 0, 10);
    LMArc* a2 = new LMArc("b1->b2", b1, 0, b2, 0, 10);
    LMArc* a3 = new LMArc("b2->  ", b2, 0,  0, 0, 10);
    LMArc* a4 = new LMArc("  ->b3",  0, 0, b3, 0, 10);
    LMArc* a5 = new LMArc("b3->b4", b3, 0, b4, 0, 10);
    LMArc* a6 = new LMArc("b4->  ", b4, 0,  0, 0, 10);
    LMArc* a7 = new LMArc("b1->b4", b1, 0, b4, 0, 10);
    LMArc* a8 = new LMArc("  ->b5", 0, 0,  b5, 0, 10);
    LMArc* a9 = new LMArc("b5->b6", b5, 0, b6, 0, 10);
    LMArc* a10 = new LMArc("b6->  ", b6, 0, 0, 0, 10);

    LMQueryNetwork::get_singleton()->add_box(b1);
    LMQueryNetwork::get_singleton()->add_box(b2);
    LMQueryNetwork::get_singleton()->add_box(b3);
    LMQueryNetwork::get_singleton()->add_box(b4);
    LMQueryNetwork::get_singleton()->add_box(b5);
    LMQueryNetwork::get_singleton()->add_box(b6);

    LMQueryNetwork::get_singleton()->add_arc(a1);
    LMQueryNetwork::get_singleton()->add_arc(a2);
    LMQueryNetwork::get_singleton()->add_arc(a3);
    LMQueryNetwork::get_singleton()->add_arc(a4);
    LMQueryNetwork::get_singleton()->add_arc(a5);
    LMQueryNetwork::get_singleton()->add_arc(a6);
    LMQueryNetwork::get_singleton()->add_arc(a7);
    LMQueryNetwork::get_singleton()->add_arc(a8);
    LMQueryNetwork::get_singleton()->add_arc(a9);
    LMQueryNetwork::get_singleton()->add_arc(a10);

    // create statistics 
    for (int i = 0; i < LMParams::distribution_param.number_of_samples; ++i)
    {
        LMStatisticsManager::get_singleton()->update_box_cost("b1", 0.1);
        LMStatisticsManager::get_singleton()->update_box_cost("b2", 0.3);
        LMStatisticsManager::get_singleton()->update_box_cost("b3", 0.2);
        LMStatisticsManager::get_singleton()->update_box_cost("b4", 0.1);
        LMStatisticsManager::get_singleton()->update_box_cost("b5", 0.1);
        LMStatisticsManager::get_singleton()->update_box_cost("b6", 0.1);
    
        LMStatisticsManager::get_singleton()->update_box_selectivity(
                get_selectivity_id("b1", 0), 0.5);
        LMStatisticsManager::get_singleton()->update_box_selectivity(
                get_selectivity_id("b2", 0), 0.5);
        LMStatisticsManager::get_singleton()->update_box_selectivity(
                get_selectivity_id("b3", 0), 0.8);
        LMStatisticsManager::get_singleton()->update_box_selectivity(
                get_selectivity_id("b4", 0), 0.5);
        LMStatisticsManager::get_singleton()->update_box_selectivity(
                get_selectivity_id("b5", 0), 0.8);
        LMStatisticsManager::get_singleton()->update_box_selectivity(
                get_selectivity_id("b6", 0), 0.5);

        LMStatisticsManager::get_singleton()->update_arc_rate("  ->b1", drand48() * 10);
        LMStatisticsManager::get_singleton()->update_arc_rate("  ->b3", drand48() * 10);
        LMStatisticsManager::get_singleton()->update_arc_rate("  ->b5", drand48() * 10);
    }
    LMNodeManager::get_singleton()->add_lmnode("n1");
    LMNodeManager::get_singleton()->add_lmnode("n2");
    LMNodeManager::get_singleton()->add_lmnode("n3");

    LMStatisticsManager::get_singleton()->update_statistics(
            LMParams::distribution_param.number_of_samples);
    
    
    cout << *LMStatisticsManager::get_singleton();

    cout << *LMQueryNetwork::get_singleton(); 
       LMParams::distribution_param.number_of_random_static_distributions = 0;
        LMStaticDistributor::get_singleton()->write_distribution_plans();
    cout << *LMNodeManager::get_singleton();
    LMParams::distribution_method.initial_distribution_score_function_type =
        SF_CORRELATION_BASED;

    /*
    LMNodeManager::get_singleton()->add_box("n1", "b1");
    LMNodeManager::get_singleton()->add_box("n1", "b2");
    LMNodeManager::get_singleton()->add_box("n1", "b3");
    LMNodeManager::get_singleton()->add_box("n1", "b4");
    LMNodeManager::get_singleton()->add_box("n1", "b5");
    LMNodeManager::get_singleton()->add_box("n1", "b6");

    LMCorrelationBasedLoadDistributor::get_singleton()->global_load_distribution();

    LMParams::distribution_method.pairwise_method = PLDM_ONE_WAY;
    LMParams::distribution_method.pairwise_method = PLDM_REDISTRIBUTE;
    LMParams::distribution_method.pairwise_method = PLDM_SELECTIVE_EXCHANGE;
    LMParams::distribution_method.initial_distribution_score_function_type = 
        SF_CORRELATION_BASED;
    LMParams::distribution_method.dynamic_distribution_score_function_type = 
        SF_CORRELATION_BASED;
    LMParams::distribution_method.pairwise_refine_correlation = true;
    LMCorrelationBasedLoadDistributor::get_singleton()->pairwise_load_distribution();
    cout << *LMNodeManager::get_singleton();
    */

}
