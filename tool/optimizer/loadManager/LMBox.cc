#include "LMBox.h"
#include "LMArc.h"

BOREALIS_NAMESPACE_BEGIN


void LMBox::get_arcs(LMArcMap& arcs) const
{
    arcs.insert(_input_arcs.begin(), _input_arcs.end());
    arcs.insert(_output_arcs.begin(), _output_arcs.end());
}

void LMBox::get_input_boxes(LMBoxVect& input_boxes) const
{
    for (LMArcMap::const_iterator ait = _input_arcs.begin();
            ait != _input_arcs.end();
            ++ait)
    {
        LMArc* arc = ait->second;
        if (arc->get_source_box() != 0)
            input_boxes.push_back(arc->get_source_box());
    }
}

void LMBox::get_output_boxes(LMBoxVect& output_boxes) const
{
    for (LMArcMap::const_iterator ait = _output_arcs.begin();
            ait != _output_arcs.end();
            ++ait)
    {
        LMArc* arc = ait->second;
        if (arc->get_destination_box() != 0)
            output_boxes.push_back(arc->get_destination_box());
    }
}

void LMBox::get_connected_boxes(LMBoxVect& boxes) const
{
    get_input_boxes(boxes);
    get_output_boxes(boxes);
}

bool LMBox::is_input_box() const
{
    // For each input arc of this box, check whether the arc has an input box.
    // If not, the box is an input box
    for (LMArcMap::const_iterator ait = _input_arcs.begin();
            ait != _input_arcs.end();
            ++ait)
    {
        LMArc* arc = ait->second;
        if (arc->get_source_box() == 0)
            return true;
    }
    return false;
}

bool LMBox::is_output_box() const
{
    // For each output arc of this box, check whether the arc has an output box.
    // If not, the box is an output box
    for (LMArcMap::const_iterator ait = _output_arcs.begin();
            ait != _output_arcs.end();
            ++ait)
    {
        LMArc* arc = ait->second;
        if (arc->get_destination_box() == 0)
            return true;
    }
    return false;
}

bool LMBox::is_io_box() const
{
    return (is_input_box() || is_output_box());
}

void LMBox::add_arc(const PortId& port, LMArc* arc)
{
    // From the source and destination box of the arc, we determine
    // whether the arc is an input arc to this box or an output arc to this box
    if (arc->get_source_box() == this)
    {
        // This is an output arc
        _output_ports.insert(port);
        _output_arcs[arc->get_id()] = arc;
        _output_port_arc_map[port][arc->get_id()] = arc;
    }
    else if (arc->get_destination_box() == this)
    {
        // This is an input arc
        _input_ports.insert(port);
        _input_arcs[arc->get_id()] = arc;
        _input_port_arc_map[port][arc->get_id()] = arc;
    }
    else
    {
        // The information does not match. This should not happen.
        cerr << "Adding wrong arc." << endl;
        exit(1);
    }
}

ostream& operator<<(ostream& os, const LMBox& box)
{
    // output id and type
    os << "(" << box._id << " (" << box._type << ")" << " | ";

    // output input arcs
    for (map<PortId, LMArcMap>::const_iterator 
            it = box._input_port_arc_map.begin();
            it != box._input_port_arc_map.end();
            ++it)
    {
        os << "i" << it->first << "<";
        bool first = true;
        for (LMArcMap::const_iterator ait = it->second.begin();
                ait != it->second.end();
                ++ait)
        {
            if (!first) os << " "; else first = false;
            os << ait->first;
        }
        os << ">";
    }
    os << " ";

    // output output arcs
    for (map<PortId, LMArcMap>::const_iterator 
            it = box._output_port_arc_map.begin();
            it != box._output_port_arc_map.end();
            ++it)
    {
        os << "o" << it->first << "<";
        bool first = true;
        for (LMArcMap::const_iterator ait = it->second.begin();
                ait != it->second.end();
                ++ait)
        {
            if (!first) os << " "; else first = false;
            os << ait->first;
        }
        os << ">";
    }
    os << ")";
    return os;
}

ostream& operator<<(ostream& os, const LMBoxVect& boxes)
{
    for (LMBoxVect::const_iterator bit = boxes.begin();
              bit != boxes.end();
              ++bit)
    {
        os << *bit << endl;
    }
    return os;
}

ostream& operator<<(ostream& os, const LMBoxMap& boxes)
{
    for (LMBoxMap::const_iterator bit = boxes.begin();
              bit != boxes.end();
              ++bit)
    {
        os << *(bit->second) << endl;
    }
    return os;
}

ostream& operator<<(ostream& os, const LMBoxSet& boxes)
{
    for (LMBoxSet::const_iterator bit = boxes.begin();
              bit != boxes.end();
              ++bit)
    {
        os << *bit << endl;
    }
    return os;
}

BOREALIS_NAMESPACE_END


