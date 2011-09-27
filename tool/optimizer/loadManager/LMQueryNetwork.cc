#include "LMQueryNetwork.h"

BOREALIS_NAMESPACE_BEGIN

LMQueryNetwork* LMQueryNetwork::_singleton = 0;


LMQueryNetwork::~LMQueryNetwork()
{
    clear();
}

void LMQueryNetwork::clear()
{
    // delelte all LMBoxes
    for (LMBoxMap::iterator it = _box_map.begin();
            it != _box_map.end();
            ++it)
        delete it->second;

    // delete all LMArcs
    for (LMArcMap::iterator it = _arc_map.begin();
            it != _arc_map.end();
            ++it)
        delete it->second;

    // clear all data structure
    _box_map.clear();
    _arc_map.clear();
    _input_arcs.clear();
    _output_arcs.clear();
    _stream_id_arc_id_map.clear();
}

LMBox* LMQueryNetwork::get_box(const IdType& id) const
{
    LMBoxMap::const_iterator it = _box_map.find(id);
    if (it != _box_map.end())
        return(it->second);

    return 0;
}

LMArc* LMQueryNetwork::get_arc(const IdType& id) const
{
    LMArcMap::const_iterator it = _arc_map.find(id);
    if (it != _arc_map.end())
        return(it->second);

    return 0;
}

void LMQueryNetwork::add_arc(LMArc* arc)
{
    // add the arc to the arc map
    _arc_map[arc->get_id()] = arc;

    // check whether the arc is I/O arc. If so, update I/O arc map.
    if (arc->get_source_box() == 0)
        _input_arcs[arc->get_id()] = arc;

    if (arc->get_destination_box() == 0)
        _output_arcs[arc->get_id()] = arc;

    // Update the source and destination LMBox of this LMArc
    if (arc->get_source_box() != 0)
        arc->get_source_box()->add_arc(arc->get_source_port(), arc);

    if (arc->get_destination_box() != 0)
        arc->get_destination_box()->add_arc(arc->get_destination_port(), arc);
}

unsigned int LMQueryNetwork::get_arc_tuple_size(const IdType& id) const
{
    LMArcMap::const_iterator it = _arc_map.find(id);
    if (it != _arc_map.end())
        return it->second->get_tuple_size() ;

    return 0;
}

const IdSet& LMQueryNetwork::get_arc_ids(const IdType& stream_id) const
        throw(Exception)
{
    map<IdType, IdSet>::const_iterator it = _stream_id_arc_id_map.find(stream_id);
    if (it == _stream_id_arc_id_map.end())
        throw INVALID_ID;
    return it->second;
}

bool LMQueryNetwork::is_output_stream(const IdType& stream_name) const
{
    try
    {
        // get all arcs of the given stream
        const IdSet& arc_ids = get_arc_ids(stream_name);

        // returns true if the given stream has an output arc
        for (IdSet::const_iterator ait = arc_ids.begin();
                ait != arc_ids.end();
                ++ait)
        {
            if (is_output_arc(*ait)) 
            { 
                return true; 
            }
        }

    } catch(...)
    {
        return false;
    }


    // returns false if the given stream has no output arc
    return false;
}


bool LMQueryNetwork::is_connected(
        const IdType& box1_id, const IdType& box2_id) const
{
    // obtain the boxes from id
    LMBox* box1 = get_box(box1_id);
    LMBox* box2 = get_box(box2_id);
     
    // Returns false if the first box_id is not valid
    if (box1 == 0)
    {
        cerr << "Wrong box name: " << box1_id << endl;
        return false;
    }
    
    // check whether the second box is connected to the first box
    // if so, return true, otherwise, return false
    LMBoxVect connected_boxes; 
    box1->get_connected_boxes(connected_boxes);
    if (find(connected_boxes.begin(), connected_boxes.end(), box2) !=
        connected_boxes.end())
        return true;
    else
        return false;
}

ostream& operator<<(ostream& os, const LMQueryNetwork& query_network)
{
    os << "========================== LMQueryNetwork ===========================" << endl;
    os << "-------------- boxes ------------- " << endl;
    os << query_network._box_map;
    os << "-------------- arcs ------------- " << endl;
    os << query_network._arc_map;
    os << "=====================================================================" << endl;
    return os;
}

BOREALIS_NAMESPACE_END
