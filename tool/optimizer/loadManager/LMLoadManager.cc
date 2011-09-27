#include "LMLoadManager.h"
#include "LMQueryNetwork.h"
#include "LMStatisticsManager.h"
#include "LMNodeManager.h"
#include "LMPerformanceMonitor.h"
#include "LMController.h"
#include "LMStaticDistributor.h"

BOREALIS_NAMESPACE_BEGIN

void LMLoadManager::update()
{
    if (!update_topology()) {return;}
    update_statistics();
    LMStatisticsManager::get_singleton()->print_statistics_to_files();
}


void LMLoadManager::redistribute_load()
{
    NOTICE << "^^^^^^^^^^^ LMLoadManager::redistribute_load ^^^^^^^^^^^^^" ;

    if (LMNodeManager::get_singleton()->get_lmnodes().empty() ||
            LMQueryNetwork::get_singleton()->get_boxes().empty() )
    {
        if (LMParams::display_param.show_basic_info)
            NOTICE << "Empty Query Network.";
        return;
    }

    // Record boxes location before migration
    _box_node_map_before_redistribition =
        LMNodeManager::get_singleton()->get_box_node_map();

    // redistribute load
    LMController::get_singleton()->set_current_time(get_current_time());
    LMController::get_singleton()->redistribute_load();

    // move load
    move_boxes();
    NOTICE << "^^^^^^^^^ LMLoadManager::redistribute_load finished ^^^^^^^^^^" ;
}

void LMLoadManager::move_boxes()
{
    // record load migration statistics
    double amount_of_load_moved = 0;

    // for each node, determin the boxes to be moved to it
    const LMNodeMap& lmnodes = LMNodeManager::get_singleton()->get_lmnodes();
    for (LMNodeMap::const_iterator it = lmnodes.begin();
            it != lmnodes.end();
            ++it)
    {

        const IdType& receiver_id = it->first;

        // The map between the sender node ID and the boxes it needs to move
        // to a receiver node
        map<IdType, IdSet> sender_box_map;

        // fill in the above map
        const IdSet& box_ids = it->second.get_boxes();
        for (IdSet::const_iterator bit = box_ids.begin();
                bit != box_ids.end();
                ++bit)
        {
            const IdType& box_id = *bit;

            const IdType& sender_id = _box_node_map_before_redistribition[box_id];
            if (sender_id != "" &&
                    sender_id != receiver_id)
            {
                sender_box_map[sender_id].insert(box_id);
                amount_of_load_moved +=
                    LMStatisticsManager::get_singleton()->get_box_average_load(box_id);
            }
        }

        // move boxes
        for (map<IdType, IdSet>::const_iterator mit = sender_box_map.begin();
                mit != sender_box_map.end();
                ++mit)
        {
            move_boxes_between_nodes(mit->first, it->first, mit->second);
        }
    }


    LoadMigrationStatistics::amount_of_load_moved.add_sample(amount_of_load_moved);
}


BOREALIS_NAMESPACE_END
