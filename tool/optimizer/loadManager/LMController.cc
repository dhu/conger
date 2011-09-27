#include "LMController.h"
#include "LMParams.h"
#include "LMSimpleDistributor.h"
#include "LMCorrelationBasedLoadDistributor.h"
#include "LMStatisticsManager.h"
#include "LMStaticDistributor.h"

BOREALIS_NAMESPACE_BEGIN

LMController* LMController::_singleton = 0;

LMController::LMController():
    _initial_distribution_needed(true),
    _starting_time(0.0)
{
}

void LMController::redistribute_load()
{
    if (_starting_time < 0.0001) 
    {
        if (LMStatisticsManager::get_singleton()->get_system_total_load() > 0) 
        {
            NOTICE << "Warm up period started";
            _starting_time = _current_time;
        }
        else 
        {
            NOTICE << "Waiting for input data ...";
            return;
        }
    }

    if (_current_time - _starting_time < LMParams::runtime_param.warm_up_time + 1)
    {
        LMStatisticsManager::get_singleton()->set_parameters(LDP_WARMUP);
        warm_up_load_distribution();
    }
    else if (_initial_distribution_needed)
    {
        LMStatisticsManager::get_singleton()->set_parameters(LDP_INITIAL);
        initial_load_distribution();
        _initial_distribution_needed = false;
    }
    else
    {
        LMStatisticsManager::get_singleton()->set_parameters(LDP_DYNAMIC);
        dynamic_load_distribution();
    }
}

void LMController::initial_load_distribution()
{
    if (LMParams::distribution_method.initial_load_distribution_active)
    {
        NOTICE << "Initial Operator Distribution ... ";
        // use correlation based load distributor to make initial load 
        // distribution
        LMCorrelationBasedLoadDistributor::get_singleton()->
            global_load_distribution();

    }
    else
    {
        if (LMParams::display_param.show_basic_info)
            NOTICE << "Initial load distribution is not active.";
    }

    if (LMParams::distribution_method.static_load_distribution_active)
    {
        LMStaticDistributor::get_singleton()->write_distribution_plans();
    }

}

void LMController::warm_up_load_distribution()
{
    if (LMParams::distribution_method.warmup_load_distribution_active)
    {
        NOTICE << "Pairwise Load Distribution ... ";
        LMCorrelationBasedLoadDistributor::get_singleton()->
            pairwise_load_distribution();
        //LMSimpleDistributor::get_singleton()->pairwise_distribution();
    }
    else
    {
        if (LMParams::display_param.show_basic_info)
            NOTICE << "No action in warm up period";
    }
}

void LMController::dynamic_load_distribution()
{
    if (LMParams::distribution_method.dynamic_load_distribution_active)
    {
//        LMSimpleDistributor::get_singleton()->pairwise_distribution();
        NOTICE << "Pairwise Load Distribution ... ";
        LMCorrelationBasedLoadDistributor::get_singleton()->
            pairwise_load_distribution();
    }
    else
    {
        if (LMParams::display_param.show_basic_info)
            NOTICE << "Dynamic load distribution is not active.";
    }
}

BOREALIS_NAMESPACE_END
