#include <iomanip>
#include "LMSuperGraph.h"
#include "LMQueryNetwork.h"

//#define DEBUG_MOVE_AURORA_BOX

BOREALIS_NAMESPACE_BEGIN

LMSuperGraph::LMSuperGraph():
    _next_super_box_id(0),
    _next_super_edge_id(0)
{
}

LMSuperGraph::LMSuperGraph(const IdSet& boxes):
    _next_super_box_id(0),
    _next_super_edge_id(0)
{
    for (IdSet::const_iterator it = boxes.begin();
            it != boxes.end();
            ++it)
    {
        add_box(*it);
    }
}

LMSuperGraph::LMSuperGraph(const LMBoxMap& boxes):
    _next_super_box_id(0),
    _next_super_edge_id(0)
{
    for (LMBoxMap::const_iterator it = boxes.begin();
            it != boxes.end();
            ++it)
    {
        add_box(it->first);
    }
}

LMSuperGraph::~LMSuperGraph()
{
    for (LMSuperBoxMap::iterator sbit = _super_boxes.begin();
              sbit != _super_boxes.end();
              ++sbit)
    {
        delete sbit->second;
    }
    for (LMSuperEdgeMap::iterator seit = _super_edges.begin();
              seit != _super_edges.end();
              ++seit)
    {
        delete seit->second;
    }
}

LMSuperBox* LMSuperGraph::get_parent_super_box(const IdType& box_id) const
{
    map<IdType, LMSuperBox*>::const_iterator sit =
        _box_to_super_box_index.find(box_id);
    if (sit == _box_to_super_box_index.end())
        return 0;
    else
        return sit->second;
}

LMSuperBox* LMSuperGraph::get_parent_super_box(const IdType& box_id)
{
    map<IdType, LMSuperBox*>::iterator sit =
        _box_to_super_box_index.find(box_id);
    if (sit == _box_to_super_box_index.end())
        return 0;
    else
        return sit->second;
}

void LMSuperGraph::add_super_box(LMSuperBox* super_box)
{
    // add the LMSuperBox to repository
    _super_boxes[super_box->get_id()] = super_box;

    if (super_box->get_boxes().empty())
        return;

    for (IdSet::const_iterator bit = super_box->get_boxes().begin();
              bit != super_box->get_boxes().end();
              ++bit)
    {
        // update the Box to LMSuperBox index
        _box_to_super_box_index[*bit] = super_box;

        // for the arcs connected to this LMSuperBox,
        // update LMSuperEdge it is in or create new LMSuperEdge
        LMBox* box = LMQueryNetwork::get_singleton()->get_boxes().find(*bit)->second;
        LMArcMap arcs;
        box->get_arcs(arcs);
        for (LMArcMap::iterator it = arcs.begin();
                it != arcs.end();
                ++it)
        {
            // find the other box connected to the arc
            LMArc* arc = it->second;
            LMBox* other_box;
            if (arc->get_source_box() != box)
                other_box = arc->get_source_box();
            else
                other_box = arc->get_destination_box();

            if (other_box == 0)
                continue;

            // update/create LMSuperEdge if the other box belongs to a LMSuperBox
            LMSuperBoxMap::iterator sit =
                _box_to_super_box_index.find(other_box->get_id());
            if (sit != _box_to_super_box_index.end())
            {
                LMSuperBox* other_super_box = sit->second;
                if (other_super_box == super_box)
                    continue;
                map<LMSuperBox*, LMSuperEdge*>::const_iterator
                    beit = super_box->get_connected_super_boxes_and_super_edges().find(other_super_box);

                if (beit != super_box->get_connected_super_boxes_and_super_edges().end())
                {
                    // There is an existing LMSuperEdge already. Update it
                    LMSuperEdge* s_edge = beit->second;
                    s_edge->add_arc(arc->get_id());

                    // Update Arc to LMSuperEdges index
                    _arc_to_super_edge_index[arc->get_id()] = s_edge;
                }
                else
                {
                    // There is no existing LMSuperEdge. Create one
                    ostringstream oss;
                    oss << "se" << _next_super_edge_id++;
                    LMSuperEdge* s_edge = new LMSuperEdge(
                            oss.str(),
                            super_box,
                            other_super_box);

                    // Add the arcs into the LMSuperEdge

                    const IdSet& s_boxes = super_box->get_boxes();
                    const IdSet& d_boxes = other_super_box->get_boxes();

                    for (IdSet::const_iterator bit = s_boxes.begin();
                              bit != s_boxes.end();
                              ++bit)
                    {
                        LMBox* box = LMQueryNetwork::get_singleton()->get_boxes().find(*bit)->second;
                        LMArcMap arcs;
                        box->get_arcs(arcs);
                        for (LMArcMap::iterator it = arcs.begin();
                                it != arcs.end();
                                ++it)
                        {
                            LMArc* arc = it->second;
                            if (arc == 0)
                                continue;
                            if (arc->get_source_box() == 0)
                                continue;
                            if (arc->get_destination_box() == 0)
                                continue;
                            if (d_boxes.find(arc->get_source_box()->get_id()) != d_boxes.end() ||
                                    d_boxes.find(arc->get_destination_box()->get_id()) != d_boxes.end())
                            {
                                s_edge->add_arc(arc->get_id());
                            }
                        }
                    }

                    // Update LMSuperEdges
                    _super_edges[s_edge->get_id()] = s_edge;

                    // Update LMArc to LMSuperEdges index
                    _arc_to_super_edge_index[arc->get_id()] = s_edge;

                    // update LMSuperEdges for the LMSuperBoxes
                    super_box->add_super_edge(other_super_box, s_edge);
                    other_super_box->add_super_edge(super_box, s_edge);
                }
            }
        }
    }
}

void LMSuperGraph::add_box(const IdType& box_id)
{
    LMSuperBox* super_box = new_super_box();
    super_box->add_box(box_id);
    add_super_box(super_box);
}

void LMSuperGraph::delete_super_box(LMSuperBox* super_box)
{
    // delete the LMSuperBox from the LMSuperBox List
    _super_boxes.erase(super_box->get_id());

    // delete the indices related to this LMSuperBox
    for (IdSet::const_iterator bit = super_box->get_boxes().begin();
              bit != super_box->get_boxes().end();
              ++bit)
    {
        _box_to_super_box_index.erase(*bit);
    }

    // delete the LMSuperEdges connected to this LMSuperBox from
    // the LMSuperEdges list, other LMSuperBoxes, and indices
    for (map<LMSuperBox*, LMSuperEdge*>::const_iterator
                beit = super_box->get_connected_super_boxes_and_super_edges().begin();
              beit != super_box->get_connected_super_boxes_and_super_edges().end();
              ++beit)
    {
        delete_super_edge(beit->second);
    }

    delete super_box;
}

void LMSuperGraph::delete_super_edge(const LMSuperEdge* super_edge)
{
      super_edge->get_super_boxes().first->delete_super_edge(super_edge->get_super_boxes().second);
      super_edge->get_super_boxes().second->delete_super_edge(super_edge->get_super_boxes().first);

        _super_edges.erase(super_edge->get_id());
        for (IdSet::const_iterator ait = super_edge->get_arcs().begin();
                  ait != super_edge->get_arcs().end();
                  ++ait)
        {
            _arc_to_super_edge_index.erase(*ait);
        }
        delete super_edge;
}

LMSuperBox* LMSuperGraph::merge_super_boxes(const LMSuperBoxPair& super_boxes)
{
    // create a new LMSuperbox
    LMSuperBox* sbox = new_super_box();

    // add the boxes to the new LMSuperBox
    sbox->add_boxes(super_boxes.first->get_boxes());
    sbox->add_boxes(super_boxes.second->get_boxes());

    // delete the old LMSuperBoxes
    delete_super_box(super_boxes.first);
    delete_super_box(super_boxes.second);

    // add the new LMSuperbox
    add_super_box(sbox);

    return sbox;
}

void LMSuperGraph::merge_super_boxes(LMSuperBox* sender, LMSuperBox* reciever)
{
    // move all boxes from the sender to reciever
    while (sender->get_boxes().size() > 0)
    {
        move_box(sender, reciever, *(sender->get_boxes().begin()));
    }

    // delete sender
    delete_super_box(sender);
}


void LMSuperGraph::move_box(LMSuperBox* sender, LMSuperBox* receiver, const IdType& box_id)
{
#ifdef DEBUG_MOVE_AURORA_BOX
    cout << "**** before moving box " << box_id << " from SuperBox "
              << sender->get_id() << " to SuperBox " << receiver->get_id() << endl;
    cout << *this;
#endif

    // move the box from sender LMSuperBox to receiver LMSuperBox
    // first delete it from sender
    sender->delete_box(box_id);

    receiver->add_box(box_id);

    // update the Boxes to LMSuperBox index
    _box_to_super_box_index[box_id] = receiver;

    // update information for each Arc
    LMBox* box = LMQueryNetwork::get_singleton()->get_boxes().find(box_id)->second;
    LMArcMap arcs;
    box->get_arcs(arcs);
    for (LMArcMap::iterator it = arcs.begin();
            it != arcs.end();
            ++it)
    {
        IdType arc_id = it->first;

        // if the arc is contained in an existing LMSuperEdge, then removed it.
        map<IdType, LMSuperEdge*>::iterator old_arc_it = _arc_to_super_edge_index.find(arc_id);
        if (old_arc_it != _arc_to_super_edge_index.end())
        {
            LMSuperEdge* old_super_edge = old_arc_it->second;
            old_super_edge->delete_arc(arc_id);
            _arc_to_super_edge_index.erase(arc_id);
            if (old_super_edge->get_arcs().empty())
            {
                // remove the LMSuperEdge from the graph.
                delete_super_edge(old_super_edge);
            }
        }

        // find the other box connected to the arc
        LMArc* arc = it->second;
        LMBox* other_box;
        if (arc->get_source_box() != box)
            other_box = arc->get_source_box();
        else
            other_box = arc->get_destination_box();

        // if the other LMBox is not contained in a LMSuperBox, then ignore this arc
        map<IdType, LMSuperBox*>::iterator other_sbox_it =
            _box_to_super_box_index.find(other_box->get_id());
        if (other_sbox_it == _box_to_super_box_index.end())
            continue;

        // find the other LMSuperBox this arc is connected to
        LMSuperBox* other_super_box = other_sbox_it->second;

        // Add the arc to proper LMSuperEdge
        // Do nothing for internal arc
        if (other_super_box == receiver)
              continue;

        map<LMSuperBox*, LMSuperEdge*>::const_iterator
            beit = receiver->get_connected_super_boxes_and_super_edges().find(other_super_box);

        if (beit != receiver->get_connected_super_boxes_and_super_edges().end())
        {
            // There is an existing LMSuperEdge already. Update it
            LMSuperEdge* s_edge = beit->second;
            s_edge->add_arc(arc_id);

            // Update LMArc to LMSuperEdges index
            _arc_to_super_edge_index[arc_id] = s_edge;
        }
        else
        {
            // There is no existing LMSuperEdge. Create one
            ostringstream oss;
            oss << "se" << _next_super_edge_id++;
            LMSuperEdge* s_edge = new LMSuperEdge(
                    oss.str(),
                    receiver,
                    other_super_box);

            // Add the arc to this LMSuperEdge
            s_edge->add_arc(arc_id);

            // Update LMSuperEdges
            _super_edges[s_edge->get_id()] = s_edge;

            // Update LMArc to LMSuperEdges index
            _arc_to_super_edge_index[arc_id] = s_edge;

            // update LMSuperEdges for the LMSuperBoxes
            receiver->add_super_edge(other_super_box, s_edge);
            other_super_box->add_super_edge(receiver, s_edge);
        }
    }

#ifdef DEBUG_MOVE_AURORA_BOX
    cout << "**** after moving box " << box_id << " from LMSuperBox "
              << sender->get_id() << " to LMSuperBox " << receiver->get_id() << endl;
    cout << *this;
#endif
}

ostream& operator<<(ostream& os, const LMSuperGraph& super_graph)
{
    os << "============================ LMSuperGraph =======================" << endl;
    os << "----------- LMSuperBoxes --------------" << endl;
    os << super_graph.get_super_boxes();
    os << "----------- LMSuperEdges --------------" << endl;
    os << super_graph.get_super_edges();
    os << "------ LMBox LMSuperBox Index -----" << endl;
    for (map<IdType, LMSuperBox*>::const_iterator
                sit = super_graph._box_to_super_box_index.begin();
              sit != super_graph._box_to_super_box_index.end();
              ++sit)
    {
        os << "(" << sit->first << ", " << sit->second->get_id() << ")";
    }
    os << endl;
    os << "----- LMArc LMSuperEdge Index -----" << endl;
    for (map<IdType, LMSuperEdge*>::const_iterator
                eit = super_graph._arc_to_super_edge_index.begin();
              eit != super_graph._arc_to_super_edge_index.end();
              ++eit)
    {
        os << "(" << eit->first << ", " << eit->second->get_id() << ")";
    }
    os << endl;
    os << "===============================================================" << endl;
    return os;
}

BOREALIS_NAMESPACE_END


