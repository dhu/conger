#ifndef LMSuperEdge_H
#define LMSuperEdge_H

#include "LMCommon.h"
#include <functional>
#include <map>


BOREALIS_NAMESPACE_BEGIN

class LMSuperBox;

/**
 * The LMSuperEdge class is a group of AuroraArcs
 */
class LMSuperEdge
{

public:
    /**
     * Construct an empty LMSuperEdge
     */
    LMSuperEdge(const IdType& id, LMSuperBox* box1, LMSuperBox* box2);

    /**
     * Destruct the LMSuperEdge
     */
    ~LMSuperEdge(){}

    /**
     * Returns the ID of this LMSuperEdge.
     * @return the ID of this LMSuperEdge.
     */
    const IdType& get_id() const { return _id; }

    /**
     * Returns all arcs
     */
    const IdSet& get_arcs() const {return _arcs; }

    /**
     * Returns the LMSuperBoxes connected the LMSuperEdge
     */
    const pair<LMSuperBox*, LMSuperBox*>& get_super_boxes() const {return _super_boxes;}

    /**
     * Add an AuroraArc to this LMSuperEdge
     */
    void add_arc(const IdType& arc_id) {_arcs.insert(arc_id); }

    /**
     * Erase an AuroraArc to this LMSuperEdge
     */
    void delete_arc(const IdType& arc_id) {_arcs.erase(arc_id);}


private:
    /**
     * The identifier of this LMSuperEdge
     */
    IdType _id;

    /**
     * A map of Aurora arcs
     */
    IdSet _arcs;

    /**
     * The LMSuperBoxes connected by the LMSuperEdge
     */
    pair<LMSuperBox*, LMSuperBox*> _super_boxes;

};

typedef map<IdType, LMSuperEdge*> LMSuperEdgeMap;
typedef multimap<double, LMSuperEdge*> AscendingSuperEdges;
typedef multimap<double, LMSuperEdge*, greater<double> > DescendingSuperEdges;

ostream& operator<<(ostream& os, const LMSuperEdge& super_edge);
ostream& operator<<(ostream& os, const LMSuperEdgeMap& super_edges);
ostream& operator<<(ostream& os, const AscendingSuperEdges& asc_super_edges);
ostream& operator<<(ostream& os, const DescendingSuperEdges& des_super_edges);


BOREALIS_NAMESPACE_END
#endif


