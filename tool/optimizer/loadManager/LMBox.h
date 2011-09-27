#ifndef LMBox_H
#define LMBox_H

#include "LMCommon.h"
#include "LMArc.h"

BOREALIS_NAMESPACE_BEGIN

class LMBox;

typedef vector<LMBox*> LMBoxVect;

typedef map<IdType, LMBox*> LMBoxMap;

typedef set<LMBox*> LMBoxSet;

typedef set<PortId> PortSet;

/**
 * The LMBox class is a simplified class of an Borealis Box that stores some 
 * basic topology information of a Borealis Box 
 */
class LMBox
{
   // Method(s)
public:

    /**
     * Constructs a new LMBox with ID and type
     * @param id the identifer of the LMBox.
     * @param type the type of the LMBox
     */
    LMBox(const IdType& id, const string& type): _id(id), _type(type) {}

    /**
     * Returns the identifier of this LMBox.
     * @return the identifier of this LMBox.
     */
    const IdType& get_id() const { return _id;};

    /**
     * Returns the type of this LMBox.
     * @return the type of this LMBox.
     */
    const string& get_type() const { return _type;};

    /**
     * Returns the input ports of the LMBox
     * @return the input ports of the LMBox
     */
    const PortSet& get_input_ports() const { return _input_ports; }

    /**
     * Returns the output ports of the LMBox
     * @return the output ports of the LMBox
     */
    const PortSet& get_output_ports() const { return _output_ports; }

    /**
     * Returns the input arcs.
     * @return the input arcs.
     */
    const LMArcMap& get_input_arcs() const { return _input_arcs;}

    /**
     * Returns the output arcs.
     * @return the output arcs.
     */
    const LMArcMap& get_output_arcs() const { return _output_arcs;}

    /**
     * Returns the input port arc map.
     * @return the input port arc map.
     */
    const map<PortId, LMArcMap>& get_input_port_arc_map() const
    {
        return _input_port_arc_map;
    }

    /**
     * Returns the output port arc map.
     * @return the output port arc map.
     */
    const map<PortId, LMArcMap>& get_output_port_arc_map() const
    {
        return _output_port_arc_map;
    }

    /**
     * Returns all arcs.
     * @return all arcs.
     */
    void get_arcs(LMArcMap& arcs) const;

    /**
     * Returns the input boxes.
     * @param input_boxes return the input boxes.
     */
    void get_input_boxes(LMBoxVect& input_boxes) const;

    /**
     * Returns the output boxes.
     * @param output_boxes return the output boxes.
     */
    void get_output_boxes(LMBoxVect& output_boxes) const;

    /**
     * Returns all connected boxes
     * @param connected_boxes return the connected boxes
     */
    void get_connected_boxes(LMBoxVect& connected_boxes) const;

    /**
     * Returns true if the box is an input box
     * @return true if the box is an input box
     */
    bool is_input_box() const;

    /**
     * Returns true if the box is an output box
     * @return true if the box is an output box
     */
    bool is_output_box() const;

    /**
     * Returns true if the box is an I/O box
     * @return true if the box is an I/O box
     */
    bool is_io_box() const;

    /**
     * Add an LMArc that is connected to this LMBox
     * @param port the PortId that the LMArc is connected to
     * @param arc the connected LMArc
     */
    void add_arc(const PortId& port, LMArc* arc);

    /**
     * Output the LMBox to ostream
     * @param os output stream
     * @param box the LMBox to be output
     * @return os output stream
     */
    friend ostream& operator<<(ostream& os, const LMBox& box);

protected:

    /**
     * The identifier.
     */
    IdType _id;

    /**
     * Type of the box
     */
    string _type;

    /**
     * The input ports of the LMBox
     */
    PortSet _input_ports;

    /**
     * The input ports of the LMBox
     */
    PortSet _output_ports;

    /**
     * The input arcs
     */
    LMArcMap _input_arcs;

    /**
     * The input arcs
     */
    LMArcMap _output_arcs;

    /**
     * The map between port and the arc connected to the arc
     */
    map<PortId, LMArcMap> _input_port_arc_map;

    /**
     * The IDs of the input arcs
     */
    map<PortId, LMArcMap> _output_port_arc_map;


};

/**
 * Output a vector of LMBoxes to an output stream
 * @param os output stream
 * @param boxes a vector of LMBoxes 
 * @return os output stream
 */
ostream& operator<<(ostream& os, const LMBoxVect& boxes);

/**
 * Output a map of LMBox to an output stream
 * @param os output stream
 * @param boxes a map of LMBoxes 
 * @return os output stream
 */
ostream& operator<<(ostream& os, const LMBoxMap& boxes);

/**
 * Output a set of LMBox to an output stream
 * @param os output stream
 * @param boxes a set of LMBoxes 
 * @return os output stream
 */
ostream& operator<<(ostream& os, const LMBoxSet& boxes);

BOREALIS_NAMESPACE_END
#endif


