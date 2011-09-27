#ifndef LMSuperGraph_H
#define LMSuperGraph_H

#include "LMCommon.h"
#include "LMSuperBox.h"
#include "LMSuperEdge.h"
#include "LMBox.h"

BOREALIS_NAMESPACE_BEGIN

class LMQueryNetwork;

/**
 * A LMSuperGraph object holds all LMSuperBoxes and LMSuperEdges
 */
class LMSuperGraph
{

public:
    /**
     * Creates an empty LMSuperGraph
     */
    LMSuperGraph();

    /**
     * Creates a LMSuperGraph from a set of LMBoxes and each box
     * consists a single LMSuperBox
     */
    LMSuperGraph(const IdSet& boxes);

    /**
     * Creates a LMSuperGraph from a set of LMBoxes and each box
     * consists a single LMSuperBox
     */
    LMSuperGraph(const LMBoxMap& boxes);

    /**
     * Destroys a LMSuperGraph.
     */
    ~LMSuperGraph();

    /**
     * Returns the LMSuperBoxe that contains a given LMBox
     */
    LMSuperBox* get_parent_super_box(const IdType& box_id) const;

    /**
     * Returns the LMSuperBoxe that contains a given LMBox
     */
    LMSuperBox* get_parent_super_box(const IdType& box_id);

    /**
     * Returns the LMSuperBoxes
     */
    const LMSuperBoxMap& get_super_boxes() const {return _super_boxes;}

    /**
     * Returns the LMSuperEdges
     */
    const LMSuperEdgeMap& get_super_edges() const {return _super_edges;}

    /**
     * Returns the number of LMSuperBoxes
     */
    unsigned int get_number_of_super_boxes() const
    {
        return _super_boxes.size();
    }

    //-------------------------------------------------
    // Functions to maintain the super graph
    //-------------------------------------------------

    /**
     * Returns a new LMSuperBox
     */
    LMSuperBox* new_super_box()
    {
        ostringstream oss;
        oss << "sb" << ++_next_super_box_id;
        return new LMSuperBox(oss.str());
    }

    /**
     * Add a new LMSuperBox
     */
    void add_super_box(LMSuperBox* super_box);

    /**
     * Add an LMBox as a single LMSuperBox
     */
    void add_box(const IdType& box_id);

    /**
     * Delete LMSuperBox
     */
    void delete_super_box(LMSuperBox* super_box);

    /**
     * Merge LMSuperboxes and create a new LMSuperbox
     */
    LMSuperBox* merge_super_boxes(const LMSuperBoxPair& super_boxes);

    /**
     * Merge LMSuperboxes and move all boxes from the sender LMSuperbox to the receiver LMSuperBox
     */
    void merge_super_boxes(LMSuperBox* sender, LMSuperBox* receiver);

    /**
     * Merge LMSuperEdge (Merge the Two LMSuperboxes connected to it)
     */
    LMSuperBox* merge_super_edge(const LMSuperEdge* super_edge)
    {
        return merge_super_boxes(super_edge->get_super_boxes());
    }

    /**
     * Move Box from one LMSuperBox to the other.
     * Remove the sender LMSuperBox if it is empty
     */
    void move_box(LMSuperBox* sender, LMSuperBox* receiver, const IdType& box_id);

    /**
     * Output the LMSuperGraph
     */
    friend ostream& operator<<(ostream& os, const LMSuperGraph& super_graph);

private:
    /**
     * Delete a LMSuperEdge
     */
    void delete_super_edge(const LMSuperEdge* super_edge);

private:

    /**
     * A map of LMSuperBoxes
     */
    LMSuperBoxMap _super_boxes;

    /**
     * A map of LMSuperEdges
     */
    LMSuperEdgeMap _super_edges;

    /**
     * The next ID for the new LMSuperBox
     */
    unsigned int _next_super_box_id;

    /**
     * The next ID for the new LMSuperEdge
     */
    unsigned int _next_super_edge_id;

    /**
     * A index from LMBox to LMSuperBox
     */
    map<IdType, LMSuperBox*> _box_to_super_box_index;

    /**
     * A index from LMArc to LMSuperEdge
     */
    map<IdType, LMSuperEdge*> _arc_to_super_edge_index;

};



BOREALIS_NAMESPACE_END
#endif


