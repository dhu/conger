#include "LMSuperEdge.h"
#include "LMSuperBox.h"
#include <iomanip>

BOREALIS_NAMESPACE_BEGIN

LMSuperEdge::LMSuperEdge(
        const IdType& id,
        LMSuperBox* box1,
        LMSuperBox* box2):
    _id(id)
{
    _super_boxes.first = box1;
    _super_boxes.second = box2;
}


ostream& operator<<(ostream& os, const LMSuperEdge& super_edge)
{
    os << "SEdge(" << super_edge.get_id() << "):";
    os << "Arcs{";
    bool first = true;
    for (IdSet::const_iterator it = super_edge.get_arcs().begin();
              it != super_edge.get_arcs().end();
              ++it)
    {
        if (first)
            first = false;
        else
            os << " ";
        os << *it;
    }
    os << "}";
    os << "SBoxes{"
          << super_edge.get_super_boxes().first->get_id() << ", "
          << super_edge.get_super_boxes().second->get_id();
    os << "}";
    os << endl;
    return os;
}

ostream& operator<<(ostream& os, const LMSuperEdgeMap& super_edges)
{
    for (LMSuperEdgeMap::const_iterator
                seit = super_edges.begin();
              seit != super_edges.end();
              ++seit)
    {
        os << *(seit->second);
    }
    return os;
}

ostream& operator<<(ostream& os, const AscendingSuperEdges& asc_super_edges)
{
    for (AscendingSuperEdges::const_iterator
                seit = asc_super_edges.begin();
              seit != asc_super_edges.end();
              ++seit)
    {
        os << "(" << seit->first << ") " << *(seit->second);
    }
    return os;
}

ostream& operator<<(ostream& os, const DescendingSuperEdges& des_super_edges)
{
    for (DescendingSuperEdges::const_iterator
                seit = des_super_edges.begin();
              seit != des_super_edges.end();
              ++seit)
    {
        os << "(" << seit->first << ") " << *(seit->second);
    }
    return os;
}

BOREALIS_NAMESPACE_END


