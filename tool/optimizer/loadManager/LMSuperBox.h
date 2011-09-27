#ifndef LMSuperBox_H
#define LMSuperBox_H

#include "LMCommon.h"
#include <vector>
#include <map>

BOREALIS_NAMESPACE_BEGIN

class LMSuperEdge;

/**
 * The LMSuperBox class is a group of AuroraBoxes
 */
class LMSuperBox
{

public:
    /**
     * Construct an empty LMSuperBox
     */
    LMSuperBox(const IdType& id):_id(id){}

    /**
     * Destruct the LMSuperBox
     */
    ~LMSuperBox(){}

    /**
     * Returns the ID of this LMSuperBox.
     * @return the ID of this LMSuperBox.
     */
    const IdType& get_id() const { return _id; }

    /**
     * Returns all boxes
     */
    const IdSet& get_boxes() const {return _boxes; }

    /**
     * Returns the LMSuperEdges connected to this LMSuperBox
     */
    const map<LMSuperBox*, LMSuperEdge*>& get_connected_super_boxes_and_super_edges()
        const { return _super_edges; }

    /**
     * Add the SuperBoxes connected to this LMSuperBox to the given set
     */
    void get_connected_super_boxes(map<IdType, LMSuperBox*>& connected_boxes) const;

    /**
     * Add an AuroraBox to this LMSuperBox
     */
    void add_box(const IdType& box_id) {_boxes.insert(box_id);}

    /**
     * Add an set of AuroraBoxes to this LMSuperBox
     */
    void add_boxes(const IdSet& boxes) {_boxes.insert(boxes.begin(), boxes.end());}

    /**
     * Erase an AuroraBox to this LMSuperBox
     */
    void delete_box(const IdType& box_id) {_boxes.erase(box_id);}

    /**
     * Add LMSuperEdge
     */
    void add_super_edge(LMSuperBox* sbox, LMSuperEdge* sedge)
    {
        _super_edges[sbox] = sedge;
    }

    /**
     * Delete Super Edge
     */
    void delete_super_edge(LMSuperBox* sbox) { _super_edges.erase(sbox); }

private:
    /**
     * The identifier if this LMSuperBox
     */
    IdType _id;


    /**
     * A set of Aurora boxes
     */
    IdSet _boxes;

    /**
     * All the LMSuperEdges connected to this box
     */
    map<LMSuperBox*, LMSuperEdge*> _super_edges;

};

typedef map<IdType, LMSuperBox*> LMSuperBoxMap;
typedef std::set<LMSuperBox*> LMSuperBoxSet;
typedef vector<LMSuperBox*> LMSuperBoxVect;
typedef pair<LMSuperBox*, LMSuperBox*> LMSuperBoxPair;
typedef multimap<double, LMSuperBox*> AscendingSuperBoxes;
typedef multimap<double, LMSuperBox*, greater<double> > DescendingSuperBoxes;
typedef multimap<double, LMSuperBoxSet> AscendingSuperBoxSets;

ostream& operator<<(ostream& os, const LMSuperBox& super_box);
ostream& operator<<(ostream& os, const LMSuperBoxMap& super_boxes);
ostream& operator<<(ostream& os, const LMSuperBoxSet& super_boxes);
ostream& operator<<(ostream& os, const AscendingSuperBoxes& );
ostream& operator<<(ostream& os, const DescendingSuperBoxes& des_super_boxes);
ostream& operator<<(ostream& os, const AscendingSuperBoxSets& super_box_sets);

BOREALIS_NAMESPACE_END
#endif


