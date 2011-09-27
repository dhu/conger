#include <iomanip>
#include "LMSuperBox.h"
#include "LMSuperEdge.h"

BOREALIS_NAMESPACE_BEGIN

void LMSuperBox::get_connected_super_boxes(LMSuperBoxMap& connected_boxes) const
{
    for (map<LMSuperBox*, LMSuperEdge*>::const_iterator
              sbeit = _super_edges.begin();
              sbeit != _super_edges.end();
              ++sbeit)
    {
        connected_boxes[sbeit->first->get_id()] = sbeit->first;
    }
}

ostream& operator<<(ostream& os, const LMSuperBox& super_box)
{
    os << "SBox(" << super_box.get_id() << "):{";
    os << "Boxes:";
    os << super_box.get_boxes();
    os << "SEdges:";
    for (map<LMSuperBox*, LMSuperEdge*>::const_iterator
                beit = super_box.get_connected_super_boxes_and_super_edges().begin();
              beit != super_box.get_connected_super_boxes_and_super_edges().end();
              ++beit)
    {
        os << "(" << beit->first->get_id() << ", " << beit->second->get_id() << ")";
    }
    os << "}";
    return os;
}

ostream& operator<<(ostream& os, const LMSuperBoxSet& super_boxes)
{
    for (LMSuperBoxSet::const_iterator sbit = super_boxes.begin();
              sbit != super_boxes.end();
              ++sbit)
        os << *(*sbit) << endl;
    return os;
}

ostream& operator<<(ostream& os, const LMSuperBoxMap& super_boxes)
{
    for (LMSuperBoxMap::const_iterator sbit = super_boxes.begin();
              sbit != super_boxes.end();
              ++sbit)
        os << *(sbit->second) << endl;
    return os;
}

ostream& operator<<(ostream& os, const AscendingSuperBoxes& ordered_super_boxes)
{
    for (AscendingSuperBoxes::const_iterator sbit = ordered_super_boxes.begin();
              sbit != ordered_super_boxes.end();
              ++sbit)
    {
        os << "(" << sbit->first << ")  " << *(sbit->second) << endl;
    }
    return os;
}

ostream& operator<<(ostream& os, const DescendingSuperBoxes& des_super_boxes)
{
    for (DescendingSuperBoxes::const_iterator sbit = des_super_boxes.begin();
              sbit != des_super_boxes.end();
              ++sbit)
    {
        os << "(" << sbit->first << ")  " << *(sbit->second) << endl;
    }
    return os;
}

ostream& operator<<(ostream& os, const AscendingSuperBoxSets& super_box_sets)
{
    for (AscendingSuperBoxSets::const_iterator it = super_box_sets.begin();
              it != super_box_sets.end();
              ++it)
    {
        os << "(" << it->first << ")  " << endl << it->second << endl;
    }
    return os;
}

BOREALIS_NAMESPACE_END


