#include "LMNodeManager.h"
#include "LMStatisticsManager.h"

BOREALIS_NAMESPACE_BEGIN

LMNodeManager* LMNodeManager::_singleton = 0;

void LMNodeManager::add_box(const IdType& node_id, const IdType& box_id)
{
    // add boxes to lmnode
    _lmnodes[node_id].add_box(box_id);

    // update box to lmnode index
    _box_to_lmnode_map[box_id] = node_id;

    // update node statistics
    IdSet box_ids;
    box_ids.insert(box_id);
    LMStatisticsManager::get_singleton()->update_node_load_statistics_for_adding_boxes(node_id, box_ids);

}

void LMNodeManager::add_boxes(const IdType& node_id, const IdSet& box_ids)
{
    // add boxes to lmnode
    _lmnodes[node_id].add_boxes(box_ids);

    // update box to lmnode index
    for (IdSet::const_iterator it = box_ids.begin();
            it != box_ids.end();
            ++it)
    {
        _box_to_lmnode_map[*it] = node_id;
    }

    // update node statistics
    LMStatisticsManager::get_singleton()->update_node_load_statistics_for_adding_boxes(node_id, box_ids);
}

void LMNodeManager::move_boxes_between_nodes(
        const IdType& sender_id,
        const IdType& receiver_id,
        const IdSet& box_ids)
{
    // move boxes
    _lmnodes[sender_id].delete_boxes(box_ids);
    _lmnodes[receiver_id].add_boxes(box_ids);

    // update box to lmnode index
    for (IdSet::const_iterator it = box_ids.begin();
            it != box_ids.end();
            ++it)
    {
        _box_to_lmnode_map[*it] = receiver_id;
    }

    // update node statistics
    LMStatisticsManager::get_singleton()->update_node_load_statistics_for_box_migration(sender_id, receiver_id, box_ids);
}

void LMNodeManager::clear()
{
    // clear statistics
    LMStatisticsManager::get_singleton()->clear_all_node_load_statistics();

    // clear all lmnodes
    _lmnodes.clear();

    // clear box to node index
    _box_to_lmnode_map.clear();
}

void LMNodeManager::clear_all_lmnodes()
{

    // clear lmnodes
    for (LMNodeMap::iterator it = _lmnodes.begin();
            it != _lmnodes.end();
            ++it)
    {
        it->second.clear();
    }

    // clear box to node index
    _box_to_lmnode_map.clear();

    // clear statistics
    LMStatisticsManager::get_singleton()->clear_all_node_load_statistics();
}

void LMNodeManager::clear_lmnode(IdType node_id)
{
    LMNode& lmnode = _lmnodes[node_id];

    // update box to node index
    for (IdSet::const_iterator it = lmnode.get_boxes().begin();
            it != lmnode.get_boxes().end();
            ++it)
    {
        _box_to_lmnode_map.erase(*it);
    }

    // clear lmnode
    lmnode.clear();

    // clear statistics
    LMStatisticsManager::get_singleton()->clear_node_load_statistics(node_id);
}


void LMNodeManager::copy_snapshot(IdPair node_ids)
{
    // clear box
    clear_lmnode(node_ids.first);
    clear_lmnode(node_ids.second);

    // add snapshot boxes back to the node
    add_boxes(node_ids.first, _lmnodes[node_ids.first].get_snapshot_boxes());
    add_boxes(node_ids.second, _lmnodes[node_ids.second].get_snapshot_boxes());
}

void LMNodeManager::print_state()
{
    cout << "LMNodes:" << endl;
    cout << _lmnodes;
    cout << "Ignore nodes" << endl;
    cout << _ignore_nodes << endl;
    cout << "box to node index" << endl;
    cout << _box_to_lmnode_map << endl;
}


ostream& operator<<(ostream& os, const LMNodeManager& lmnode_manager)
{
    os << "========================= LMNodeManager =============================" << endl;
    os << lmnode_manager._lmnodes;
    os << "=====================================================================" << endl;
    return os;
}


BOREALIS_NAMESPACE_END
