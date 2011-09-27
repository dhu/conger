#ifndef LMNodeManager_H
#define LMNodeManager_H

#include "LMCommon.h"
#include "LMNode.h"

BOREALIS_NAMESPACE_BEGIN

/**
 * The LMNodeManager manage all LMNodes
 */
class LMNodeManager
{
public:
    static const int INVALID_NODE_ID = -1;

    /**
     * Returns the singleton of the LMNodeManager
     */
    static LMNodeManager* get_singleton()
    {
        if (_singleton == 0)
          _singleton = new LMNodeManager();
        return _singleton;
    }

    /**
     * Returns the LMNodes
     */
    const LMNodeMap& get_lmnodes() const { return _lmnodes; }

    /**
     * Returns the LMNode with given ID
     */
    const LMNode& get_lmnode(IdType lmnode_id) const
    {
        return _lmnodes.find(lmnode_id)->second;
    }

    /**
     * Returns the map between box to node mapping
     */
    const map<IdType, IdType>& get_box_node_map() const
    {
        return _box_to_lmnode_map;
    }

    /**
     * Returns the ID of the LMNode that an LMBox is in
     */
    const IdType& get_box_host_lmnode_id(IdType box_id) const
    {
        return _box_to_lmnode_map.find(box_id)->second;
    }

    /**
     * Add a LMNode
     */
    void add_lmnode(const IdType& id)
    {
        _lmnodes[id] = LMNode(id);
    }

    /**
     * Add boxes to a LMNode
     */
    void add_box(const IdType& node_id, const IdType& box_id);

    /**
     * Add boxes to a LMNode
     */
    void add_boxes(const IdType& node_id, const IdSet& box_ids);

    /**
     * Move a set of box from one LMNode to another
     */
    void move_boxes_between_nodes(
        const IdType& sender_id, const IdType& receiver_id, const IdSet& box_ids);

    /**
     * Clear all LMNodes
     */
    void clear();

    /**
     * Clear boxes from all LMNodes
     */
    void clear_all_lmnodes();

    /**
     * Clear boxes from given LMNode
     * @param id the id of the LMNode to be cleared
     */
    void clear_lmnode(IdType node_id);

    /**
     * Let a given LMNode take a snapshot
     * @param id the id of the given LMNode
     */
    void take_snapshot(IdType node_id) { _lmnodes[node_id].take_snapshot(); }

    /**
     * Copy the snapshot of a pair of machiens
     * @param node_ids the ID of the node pair
     */
    void copy_snapshot(IdPair node_ids);

    /**
     * Returns the IDs of the nodes to be ignored in load distribution
     */
    const IdSet& get_ignore_nodes() const { return _ignore_nodes; }

    /**
     * Add a node to be ignored in load distribution
     */
    void add_ignore_node(const IdType& node_id) {_ignore_nodes.insert(node_id);}

    /**
     * Add a node to be ignored in load distribution
     */
    void delete_ignore_node(const IdType& node_id) {_ignore_nodes.erase(node_id); }

    /**
     * Add a node to be ignored in load distribution
     */
    void clear_ignore_nodes() { _ignore_nodes.clear(); }

    /**
     * Print the state of the LMNodeMagager
     */
    void print_state();


    /**
     * Output the LMNodeManager to an ostream
     */
    friend ostream& operator<<(ostream& os, const LMNodeManager& lmnode_manager);

private:

    /**
     * Creates the LMNodeManager
     */
    LMNodeManager() {}

    /**
     * The pointer to the LMNodeManager
     */
    static LMNodeManager* _singleton;

    /*
     * LMNodes
     */
    LMNodeMap _lmnodes;

    /*
     * Igmore the nodes in this set in load distribution
     */
    IdSet _ignore_nodes;

    /*
     * A map from LMBox to its host LMNode
     */
    map<IdType, IdType> _box_to_lmnode_map;
};



BOREALIS_NAMESPACE_END
#endif
