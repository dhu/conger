#include "LMNode.h"
#include "LMCommon.h"

BOREALIS_NAMESPACE_BEGIN

void LMNode::add_box(IdType box_id)
{
    _boxes.insert(box_id);
}

void LMNode::add_boxes(const IdSet& box_ids)
{
    for (IdSet::const_iterator it = box_ids.begin();
              it != box_ids.end();
              ++it)
    {
        _boxes.insert(*it);
    }
}

void LMNode::delete_box(IdType box_id)
{
    _boxes.erase(box_id);
}

void LMNode::delete_boxes(const IdSet& box_ids)
{
    for (IdSet::const_iterator it = box_ids.begin();
              it != box_ids.end();
              ++it)
    {
        _boxes.erase(*it);
    }
}

void LMNode::take_snapshot()
{
    _boxes_snapshot = _boxes;
}

ostream& operator<<(ostream& os, const LMNode& lmnode)
{
    os << "LMNode(" << lmnode.get_id() << "){";
    os << lmnode._boxes;
    os << "}";
    return os;
}

ostream& operator<<(ostream& os, const LMNodeVect& lmnodes)
{
    for (unsigned int i = 0; i < lmnodes.size(); ++i)
    {
        os << lmnodes[i] << endl;
    }
    return os;
}


ostream& operator<<(ostream& os, const LMNodeMap& lmnodes)
{
    for (LMNodeMap::const_iterator it = lmnodes.begin();
            it != lmnodes.end();
            ++it)
    {
        os << it->second << endl;
    }
    return os;
}

BOREALIS_NAMESPACE_END
