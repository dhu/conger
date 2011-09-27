#ifndef LMNode_H
#define LMNode_H

#include "LMCommon.h"

BOREALIS_NAMESPACE_BEGIN

/**
 * The data structure that stores the intermediate information
 * for operator distribution. Before operators are moved from
 * one node to the other, we need this data structure to store
 * the staistics of a Node as if some LMBox are moved from
 * one node to the others.
 */
struct LMNode
{

    /**
     * Creates empty LMNode
     * Just for the map container
     */
    LMNode(){}

    /**
     * Construct the LMNode using given Node
     * @param id the id of this LMNode, which is the same as the id of the node this LMNode is related to
     * @param super_boxes the LMSuperBoxes on given Node
     * @param staistics_manager the StatisticsManager
     */
    LMNode(const IdType& id):_id(id) {}

    /**
     * Returns the ID of the node this LMNode is related to
     */
    const IdType& get_id() const { return _id; }

    /**
     * Returns the LMBoxes on this LMNode
     * @param boxes reurns the LMBoxes on this LMNode
     */
    const IdSet& get_boxes() const { return _boxes; }

    /**
     * Adds an box
     * @param box_id ID of the box
     */
    void add_box(IdType box_id);

    /**
     * Adds a set of boxes
     * @param box_ids IDs of the boxes
     */
    void add_boxes(const IdSet& box_ids);

    /**
     * Delete an box
     * @param box_id ID of the box
     */
    void delete_box(IdType box_id);

    /**
     * Delete a set of boxes
     * @param box_ids IDs of the boxes
     */
    void delete_boxes(const IdSet& box_ids);

    /**
     * Clear all boxes form this LMNode
     */
    void clear() { _boxes.clear(); }

    /**
     * Keep a copy (snapshot) of the current boxes on the LMNode.
     */
    void take_snapshot();

    /**
     * Copy the snapshot to current value
     */
    const IdSet& get_snapshot_boxes() {return _boxes_snapshot; }

    /**
     * Output the LMNode to an ostream
     */
    friend ostream& operator<<(ostream& os, const LMNode& lmnode);


private:
    /**
     * The id of this LMNode, which is the same as the id of the Node this LMNode is related to
     */
    IdType _id;

    /**
     *
     */
    IdSet _boxes;

    /**
     * The data structures that are used to store the
     * former boxes on the node
     */
    IdSet _boxes_snapshot;
};

typedef vector<LMNode> LMNodeVect;
typedef map<IdType, LMNode> LMNodeMap;

ostream& operator<<(ostream& os, const LMNodeVect& lmnodes);
ostream& operator<<(ostream& os, const LMNodeMap& lmnodes);

/*
struct LMNodeRefLess: public binary_function<const LMNode*, const LMNode*, bool>
{
  bool operator()(const LMNode* lbr1, const LMNode* lbr2) const
  {
    return lbr1->getAverageLoad() < lbr2->getAverageLoad();
  }
};
*/


BOREALIS_NAMESPACE_END
#endif
